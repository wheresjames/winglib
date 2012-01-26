/*------------------------------------------------------------------
// vfw1.hpp
//
// Copyright (c) 1997
// Robert Umbehant
// winglib@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted for commercial and
// non-commercial purposes, provided that the following
// conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to
//   endorse or promote products derived from this software without
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#if defined( OEX_WIN64 )
#	define CVFWCAP_GetWindowLong( h )		GetWindowLongPtr( h, 0 )
#	define CVFWCAP_SetWindowLong( h, ptr )	SetWindowLongPtr( h, 0, (LONG_PTR)ptr )
#else
#	define CVFWCAP_GetWindowLong( h )		GetWindowLong( h, GWL_USERDATA )
#	define CVFWCAP_SetWindowLong( h, ptr )	SetWindowLong( h, GWL_USERDATA, (LONG)ptr )
#endif


//==================================================================
// CVfwCap
//
/// Encapsulates the windows VFW capture API
/**
	This class manages a single VFW capture device instance.
*/
//==================================================================
class CVfwCap
{
public:

	enum
	{
		/// Invalid driver value
		eInvalidDriver = 0xffff
	};

	enum
	{
		/// Maximum number of capture drivers
		eMaxDrivers	= 32
	};

	enum
	{
		eDlgNone = 0,
		eDlgVideoSource = 1,
		eDlgVideoFormat = 2,
		eDlgVideoDisplay = 3,
		eDlgVideoCompression = 4
	};

	enum
	{
		eVfwQuitMsg = ( WM_USER + 101 )
	};

	/// Driver information structure
	struct SDriverInfo
	{
		/// Driver index
		DWORD	index;

		/// Driver name
		TCHAR	szName[ oexSTRSIZE ];

		/// Driver version string
		TCHAR	szVer[ oexSTRSIZE ];
	};

	/// Holds data during a video frame callback
	struct SFrameCallbackInfo
	{
		/// Video header information
		LPVIDEOHDR		pVHdr;

		/// Class pointer
		CVfwCap			*pCap;
	};

public:

	/// Constructor
	CVfwCap()
	{_STT();
		m_hWnd = NULL;

		m_hGrab = NULL;

		m_bConnected = FALSE;
		m_dwCurrentDriver = eInvalidDriver;
		m_dwNumDrivers = 0;

		m_pVideoData = NULL;
		m_dwVideoDataSize = 0;
		m_pVideoFormat = NULL;

		m_pAudioData = NULL;
		m_dwAudioDataSize = 0;
		m_pAudioFormat = NULL;

		// Windows default filename
		*m_szCaptureFileName = 0;

		m_bCallbacksEnabled = FALSE;

		m_dwOnStatus = 0;
		m_dwOnError = 0;
		m_llFrame = 0;

		m_hFrameWnd = NULL;
		m_uFrameMsg = 0;

		m_hDrawDc = NULL;
		m_hDrawDib = NULL;

		m_uDevice = 0;
		m_uSource = 0;

		m_nWidth = 0;
		m_nHeight = 0;
		m_uFormat = 0;
		m_fFps = 0;

		m_hThread = INVALID_HANDLE_VALUE;
		m_hStop = CreateEvent( NULL, TRUE, FALSE, NULL );
		m_hSuccess = CreateEvent( NULL, TRUE, FALSE, NULL );
	}

	/// Destructor
	virtual ~CVfwCap()
	{_STT();
		Destroy();

		if ( INVALID_HANDLE_VALUE != m_hStop )
		{	CloseHandle( m_hStop );
			m_hStop = INVALID_HANDLE_VALUE;
		} // end if

		if ( INVALID_HANDLE_VALUE != m_hSuccess )
		{	CloseHandle( m_hSuccess );
			m_hStop = INVALID_HANDLE_VALUE;
		} // end if
	}

	//==============================================================
	// Init()
	//==============================================================
	/// Initializes capture system
	oexBOOL Init()
	{_STT();
		return oexTRUE;
	}


	//==============================================================
	// ShowDialogBox()
	//==============================================================
	/// Displays the specified dialog box
	/**
		\param [in] db	-	The dialog box to display.
							- CVfwCap::eDlgVideoSource		-	Video source
							- CVfwCap::eDlgVideoFormat		-	Video Format
							- CVfwCap::eDlgVideoDisplay		-	Video display
							- CVfwCap::eDlgVideoCompression	-	Video compression

		\return Returns non-zero if success.
	*/
	BOOL ShowDialogBox( DWORD db )
	{_STT();
		if ( !IsConnected() ) return FALSE;

		// What dialog box do we want to show
		switch( db )
		{
			case eDlgVideoSource:
				if ( m_cdc.fHasDlgVideoSource ) return capDlgVideoSource( m_hWnd );
				break;

			case eDlgVideoFormat:
				if ( m_cdc.fHasDlgVideoFormat ) return capDlgVideoFormat( m_hWnd );
				break;

			case eDlgVideoDisplay:
				if ( m_cdc.fHasDlgVideoDisplay ) return capDlgVideoDisplay( m_hWnd );
				break;

			case eDlgVideoCompression:
				return capDlgVideoCompression( m_hWnd );
				break;

			default: break;
		} // end switch

		return FALSE;
	}

	//==============================================================
	// LoadDriverInfo()
	//==============================================================
	/// Loads information about drivers into m_cdi
	/**
		\return Returns non-zero if success.
	*/
	BOOL LoadDriverInfo()
	{_STT();
		// Assume no drivers
		m_dwNumDrivers = 0;

		// Load driver info
		for ( DWORD i = 0; i < eMaxDrivers; i++ )
		{
			// Init structure
			m_vdi[ m_dwNumDrivers ].index = eInvalidDriver;
			*m_vdi[ m_dwNumDrivers ].szName = 0;
			*m_vdi[ m_dwNumDrivers ].szVer = 0;

			// Attempt to get video driver information
			if ( capGetDriverDescription(	i,
											m_vdi[ m_dwNumDrivers ].szName, sizeof( m_vdi[ m_dwNumDrivers ].szName ),
											m_vdi[ m_dwNumDrivers ].szVer, sizeof( m_vdi[ m_dwNumDrivers ].szVer ) ) )
				m_vdi[ m_dwNumDrivers++ ].index = i;

		} // end if

		// Return TRUE if we found any drivers
		return ( m_dwNumDrivers > 0 );
	}

	//==============================================================
	// DriverConnect()
	//==============================================================
	/// Connects to the specified device driver
	/**
		\param [in] hWnd	-	Window handle
		\param [in] i		-	The index of the driver to connect to.

		iii. capDriverConnect() does all kinds of stupid things when
			 the specified device is already open.

		\return Returns non-zero if success.
	*/
	static BOOL DriverConnect( HWND hWnd, DWORD i )
	{
		DWORD err = 0;
#if defined( _MSC_VER )
		__try {
#endif
			// Attempt to connect to the specified driver
			if ( !capDriverConnect( hWnd, i ) )
				return FALSE;

#if defined( _MSC_VER )
		} __except( err = GetExceptionCode() ) { return FALSE; }
#endif

		return TRUE;
	}


	//==============================================================
	// Connect()
	//==============================================================
	/// Connects to the specified device driver
	/**
		\param [in] i	-	The index of the driver to connect to.

		\return Returns non-zero if success.
	*/
	BOOL Connect( DWORD i )
	{_STT();
		if ( !IsWnd() )
			return FALSE;

		oexAutoLock ll( m_lock );
		if ( !ll.IsLocked() )
			return FALSE;

		// Disconnect from old driver
		Disconnect();

		// This basically checks driver existence
		TCHAR szName[ oexSTRSIZE ] = { 0 }, szVer[ oexSTRSIZE ] = { 0 };
		if ( !capGetDriverDescription( i, szName, oexSTRSIZE, szVer, oexSTRSIZE ) )
			return FALSE;

		// Attempt connection
		try	{
			if ( !DriverConnect( m_hWnd, i ) )
				return FALSE;
		} catch( ... ) { return FALSE; }

		// We're connected
		m_bConnected = oexTRUE;

		// Save the driver number
		m_dwCurrentDriver = i;

		// Get driver capabilities
		capDriverGetCaps( m_hWnd, &m_cdc, sizeof( m_cdc ) );

		// Initialize draw dib
//		m_hDrawDib = DrawDibOpen();

		return TRUE;
	}

	//==============================================================
	// Close()
	//==============================================================
	/// Closes any open driver and releases all resources
	void Close()
	{_STT();
		// Disconnect from driver
		Disconnect();

		// Funny things seem to happen when the capture driver is
		// already in use.  It seems we can open it again, it just
		// doesn't do anything and crashes a lot on shutdown.
		try
		{
			if ( m_hWnd && ::IsWindow( m_hWnd ) )
				::DestroyWindow( m_hWnd );
			m_hWnd = NULL;

			if ( m_hGrab != NULL )
			{	CloseHandle( m_hGrab );
				m_hGrab = NULL;
			} // end if

			// Release allocated buffers
			ReleaseVideoFormatData();
			ReleaseAudioFormatData();

		} // end try

		catch( ... )
		{	oexERROR( 0, "Assert while releasing capture window, check for misbehaving capture driver." );
		} // end catch

		// Reset vars
		m_dwCurrentDriver = eInvalidDriver;
		m_dwNumDrivers = 0;

		// Lose capture filename
		*m_szCaptureFileName = 0;

		m_dwOnStatus = 0;
		m_dwOnError = 0;
		m_llFrame = 0;
		m_nWidth = 0;
		m_nHeight = 0;

/*
		if ( m_hDrawDib != NULL )
		{	DrawDibClose( m_hDrawDib );
			m_hDrawDib = NULL;
		} // end if
*/
	}

	//==============================================================
	// Create()
	//==============================================================
	/// Creates a capture window
	/**
		\param [in] pTitle		-	Title for the capture window.
		\param [in] hwndParent	-	Handle of the parent window.
		\param [in] dwStyle		-	Window styles for the capture window.
		\param [in] x			-	Horizontal offset for the capture window.
		\param [in] y			-	Vertical offset for the capture window.
		\param [in] width		-	Width of the capture window.
		\param [in] height		-	Height of the capture window.
		\param [in] nID			-	Contorl id for the capture window.

		\return Returns non-zero if success.
	*/
	BOOL Create(	LPCTSTR pTitle = NULL, HWND hwndParent = NULL,
					DWORD dwStyle = WS_VISIBLE | WS_OVERLAPPEDWINDOW,
					long x = 10, long y = 10, long width = 320, long height = 240,
					int nID = 0 )
	{_STT();
		// Lose old window
		Close();

		// Attempt to create a capture window
//#if defined( oexDEBUG )
//		m_hWnd = capCreateCaptureWindow( pTitle, dwStyle, 0, 0, 320, 240, hwndParent, nID );
//#else
		m_hWnd = capCreateCaptureWindow( pTitle, 0, 0, 0, 1, 1, hwndParent, nID );
//#endif

		if ( !IsWnd() )
		{	oexERROR( GetLastError(), "Unable to create capture window" );
			return oexFALSE;
		} // end if

		m_nWidth = width;
		m_nHeight = height;

		return oexTRUE;
	}


	//==============================================================
	// GetSafeHwnd()
	//==============================================================
	/// Returns the capture window handle.
	HWND GetSafeHwnd() { return m_hWnd; }

	//==============================================================
	// IsWnd()
	//==============================================================
	/// Returns non-zero if there is a valid capture window
	BOOL IsWnd()
	{_STT();
		return ( m_hWnd && ::IsWindow( m_hWnd ) );
	}

	//==============================================================
	// IsConnected()
	//==============================================================
	/// Returns non-zero if there is an open capture device.
	BOOL IsConnected() { return m_bConnected; }

	//==============================================================
	// GetCurrentDriverInfo()
	//==============================================================
	/// Retrieves information about the current capture device
	/**
		\param [out] pcdi	-	Structure that receives information about the
								currently open capture driver.
		\return Returns non-zero if success.
	*/
	BOOL GetCurrentDriverInfo( SDriverInfo *pcdi )
	{_STT();
		if ( !IsConnected() ) return FALSE;
		return GetDriverInfo( m_dwCurrentDriver, pcdi );
	}

	//==============================================================
	// GetDriverInfo()
	//==============================================================
	/// Retrieves information about the specified capture device
	/**
		\param [in] i		-	Index of the capture driver to query.
		\param [out] pcdi	-	Structure that receives information about the
								currently open capture driver.

		\return Returns non-zero if success.
	*/
	BOOL GetDriverInfo( DWORD i, SDriverInfo *pcdi )
	{_STT();
		if ( !m_dwNumDrivers || !LoadDriverInfo() )
			return FALSE;

		if ( pcdi == NULL || i >= m_dwNumDrivers )
			return FALSE;

		memcpy( pcdi, &m_vdi[ i ], sizeof( SDriverInfo ) );

		return TRUE;
	}

	//==============================================================
	// GetNumDrivers()
	//==============================================================
	/// Retuns the number of installed capture devices.
	DWORD GetNumDrivers()
	{_STT();
		if ( !m_dwNumDrivers ) LoadDriverInfo(); return m_dwNumDrivers;
	}

	//==============================================================
	// GetStatus()
	//==============================================================
	/// Returns non-zero if a capture device is connected
	BOOL GetStatus()
	{_STT();
		return capGetStatus( m_hWnd, &m_cs, sizeof( m_cs ) );
	}

	//==============================================================
	// ShowVideoSourceDlg()
	//==============================================================
	/// Displays the video source dialog box
	/**
		\return Returns non-zero if success.
	*/
	BOOL ShowVideoSourceDlg()
	{_STT();
		return ShowDialogBox( eDlgVideoSource );
	}

	//==============================================================
	// ShowVideoFormatDlg()
	//==============================================================
	/// Displays the video format dialog box
	/**
		\return Returns non-zero if success.
	*/
	BOOL ShowVideoFormatDlg()
	{_STT();
		return ShowDialogBox( eDlgVideoFormat );
	}

	//==============================================================
	// ShowVideoDisplayDlg()
	//==============================================================
	/// Displays the video display dialog box
	/**
		\return Returns non-zero if success.
	*/
	BOOL ShowVideoDisplayDlg()
	{_STT();
		return ShowDialogBox( eDlgVideoDisplay );
	}

	//==============================================================
	// ShowVideoCompressionDlg()
	//==============================================================
	/// Displays the video compression dialog box
	/**
		\return Returns non-zero if success.
	*/
	BOOL ShowVideoCompressionDlg()
	{_STT();
		return ShowDialogBox( eDlgVideoCompression );
	}

	//==============================================================
	// Preview()
	//==============================================================
	/// Enables / disables preview mode
	/**
		\param [in] en		-	Set to non-zero to enable preview mode.  Zero to disable.

		\return Returns non-zero if success.
	*/
	BOOL Preview( BOOL en )
	{_STT();
		return capPreview( m_hWnd, en );
	};

	//==============================================================
	// SetPreviewRate()
	//==============================================================
	/// Sets the frame rate for preview mode
	/**
		\param [in] rate	-	Frame rate of video preview

		\return Returns non-zero if success.
	*/
	BOOL SetPreviewRate( DWORD rate = 30 )
	{_STT();
		return capPreviewRate( m_hWnd, rate );
	}

	//==============================================================
	// PreviewStretching()
	//==============================================================
	/// Enables / disables stretching of the preview video
	/**
		\param [in] en		-	Set to non-zero to enable stretching.  Zero to disable.

		\return Returns non-zero if success.
	*/
	BOOL PreviewStretching( BOOL en )
	{_STT();
		return capPreviewScale( m_hWnd, en );
	}

	//==============================================================
	// EnableOverlay()
	//==============================================================
	/// Enables / disables video overlay mode
	/**
		\param [in] en		-	Set to non-zero to enable overlay.  Zero to disable.

		\return Returns non-zero if success.
	*/
	BOOL EnableOverlay( BOOL en )
	{_STT();
		if ( !m_cdc.fHasOverlay ) return FALSE;
		return capOverlay( m_hWnd, en );
	}

	//==============================================================
	// SetCaptureFileName()
	//==============================================================
	/// Sets the capture filename
	/**
		\param [in] pName	-	The filename for capture

		\return Returns non-zero if success.
	*/
	BOOL SetCaptureFileName( LPCTSTR pName )
	{_STT();
		if ( pName == NULL ) return FALSE;
		if ( !capFileSetCaptureFile( m_hWnd, pName ) ) return FALSE;
		strcpy( m_szCaptureFileName, pName );
		return TRUE;
	}

	//==============================================================
	// GetCaptureFileName()
	//==============================================================
	/// Returns the currently set capture filename
	LPCTSTR GetCaptureFileName()
	{_STT();
		GetCaptureFileName( m_szCaptureFileName );
		return m_szCaptureFileName;
	}

	//==============================================================
	// GetCaptureFileName()
	//==============================================================
	/// Copies the currently set capture filename to the specified buffer
	/**
		\param [in] buf		-	Character buffer that receives the filename.

		\return Returns non-zero if success.
	*/
	BOOL GetCaptureFileName( LPSTR buf )
	{_STT();
		return capFileGetCaptureFile( m_hWnd, buf, MAX_PATH - 1 );
	}

	//==============================================================
	// FileAlloc()
	//==============================================================
	/// Pre allocates disk space for video capture.
	/**
		\param [in] size	-	The amount of video space to allocate.

		\return Returns non-zero if success.
	*/
	BOOL FileAlloc( DWORD size )
	{_STT();
		return capFileAlloc( m_hWnd, size );
	}

	//==============================================================
	// CaptureToFile()
	//==============================================================
	/// Starts capture of data to file
	/**
		\return Returns non-zero if success.
	*/
	BOOL CaptureToFile()
	{_STT();
		return capCaptureSequence( m_hWnd );
	}

	//==============================================================
	// SaveAs()
	//==============================================================
	/// Copies the contents of the capture file to another.
	/**
		\param [in] pName	-	New filename.

		If a larger amount of data was allocated with FileAlloc() than
		was actually used.   This function copies only the used data.

		\return Returns non-zero if success.
	*/
	BOOL SaveAs( LPCTSTR pName )
	{_STT();
		return capFileSaveAs( m_hWnd, pName );
	}

	//==============================================================
	// IsAudioFormatData()
	//==============================================================
	/// Returns non-zero if there is audio format data
	BOOL IsAudioFormatData() { return ( m_pAudioFormat != NULL ); }

	//==============================================================
	// SetAudioFormat()
	//==============================================================
	/// Sets the audio format of the capture device.
	/**
		\return Returns non-zero if success.
	*/
	BOOL SetAudioFormat()
	{_STT();
		if ( !IsAudioFormatData() ) return FALSE;
		return capSetAudioFormat( m_hWnd, m_pAudioFormat, sizeof( m_dwAudioDataSize ) );
	}

	//==============================================================
	// GetCaptureSetup()
	//==============================================================
	/// Retrieves capture setup data to m_cp
	/**
		\return Returns non-zero if success.
	*/
	BOOL GetCaptureSetup()
	{_STT();
		return capCaptureGetSetup( m_hWnd, &m_cp, sizeof( m_cp ) );
	}

	//==============================================================
	// SetCaptureSetup()
	//==============================================================
	/// Sets capture setup data in m_cp to the device
	/**
		\return Returns non-zero if success.
	*/
	BOOL SetCaptureSetup()
	{_STT();
		return capCaptureSetSetup( m_hWnd, &m_cp, sizeof( m_cp ) );
	}

	//==============================================================
	// SetCaptureAudio()
	//==============================================================
	/// Set audio capture flag in capture setup structure m_cp
	void SetCaptureAudio( BOOL ca )
	{_STT();
		GetCaptureSetup(); m_cp.fCaptureAudio = ca; SetCaptureSetup();
	}

	//==============================================================
	// SetYield()
	//==============================================================
	/// Sets yield bit in capture setup structure m_cp
	void SetYield( BOOL yield )
	{_STT();
		GetCaptureSetup(); m_cp.fYield = yield; SetCaptureSetup();
	}

	//==============================================================
	// CaptureSingleFrameOpen()
	//==============================================================
	/// Opens the capture file for single frame capture
	/**
		\return Returns non-zero if success.
	*/
	BOOL CaptureSingleFrameOpen()
	{_STT();
		return capCaptureSingleFrameOpen( m_hWnd );
	}

	//==============================================================
	// CaptureSingleFrameClose()
	//==============================================================
	/// Closes the capture file for single frame capture
	/**
		\return Returns non-zero if success.
	*/
	BOOL CaptureSingleFrameClose()
	{_STT();
		return capCaptureSingleFrameClose( m_hWnd );
	}

	//==============================================================
	// CaptureSingleFrame()
	//==============================================================
	/// Appends a single frame of video to the capture file
	/**
		\return Returns non-zero if success.
	*/
	BOOL CaptureSingleFrame()
	{_STT();
		return capCaptureSingleFrame( m_hWnd );
	}

	//==============================================================
	// CaptureSequence()
	//==============================================================
	/// Starts streaming video capture to a file.
	/**
		\return Returns non-zero if success.
	*/
	BOOL CaptureSequence()
	{_STT();
		return capCaptureSequence( m_hWnd );
	}

	//==============================================================
	// CaptureSequenceNoFile()
	//==============================================================
	/// Starts streaming video capture without writing to file.
	/**
		\return Returns non-zero if success.
	*/
	BOOL CaptureSequenceNoFile()
	{_STT();
		return capCaptureSequenceNoFile( m_hWnd );
	}

	//==============================================================
	// CaptureStop()
	//==============================================================
	/// Stops current video capture
	/**
		\return Returns non-zero if success.
	*/
	BOOL CaptureStop()
	{_STT();
		return capCaptureStop( m_hWnd );
	}

	//==============================================================
	// CaptureAbort()
	//==============================================================
	/// Aborts the current capture
	/**
		\return Returns non-zero if success.
	*/
	BOOL CaptureAbort()
	{_STT();
		return capCaptureAbort( m_hWnd );
	}

	//==============================================================
	// CopyToClipboard()
	//==============================================================
	/// Copies the current video frame and palette to the clipboard
	/**
		\return Returns non-zero if success.
	*/
	BOOL CopyToClipboard()
	{_STT();
		return capEditCopy( m_hWnd );
	}

	//==============================================================
	// SaveAsDIB()
	//==============================================================
	/// Saves the current frame of video as a DIB file
	/**
		\param [in] pFile	-	File name for DIB.

		\return Returns non-zero if success.
	*/
	BOOL SaveAsDIB( LPCTSTR pFile )
	{_STT();
		if ( pFile == NULL ) return FALSE;
		return capFileSaveDIB( m_hWnd, pFile );
	}

	//==============================================================
	// SetMCIDeviceName()
	//==============================================================
	/// Specifies the name of the MCI video device to use for capture.
	/**
		\param [in] pName	-	Name of the MCI device.

		\return Returns non-zero if success.
	*/
	BOOL SetMCIDeviceName( LPCTSTR pName )
	{_STT();
		return capSetMCIDeviceName( m_hWnd, pName );
	}

	//==============================================================
	// GetMCIDeviceName()
	//==============================================================
	/// Returns the name of the current MCI device being used for capture.
	/**
		\param [in] pName	-	Address of a buffer that receives the MCI device name.

		\return Returns non-zero if success.
	*/
	BOOL GetMCIDeviceName( LPSTR pName )
	{_STT();
		return capGetMCIDeviceName( m_hWnd, pName, MAX_PATH - 1 );
	}

	//==============================================================
	// SetUserData()
	//==============================================================
	/// Associates a four byte value with the current capture window.
	/**
		\param [in] data	-	Data to associate.

		\return Returns non-zero if success.
	*/
	BOOL SetUserData( long data )
	{_STT();
		return capSetUserData( m_hWnd, data );
	}

	//==============================================================
	// GetUserData()
	//==============================================================
	/// Returns the associated four byte value with the current capture window.
	/**
		\return Returns four byte user value.
	*/
	long GetUserData()
	{_STT();
		return (long)capGetUserData( m_hWnd );
	}

	//==============================================================
	// GrabFrame()
	//==============================================================
	/// Grabs a single frame of video from the current video device.
	/**
		\return Returns non-zero if success.
	*/
	BOOL GrabFrame()
	{_STT();
		BOOL bRet;

		try
		{
			// Grab frame
			bRet = capGrabFrame( m_hWnd );

		} // end try

		catch( ... )
		{	oexERROR( 0, "Assert in capGrabFrame()" );
			return FALSE;
		} // end catch

		return bRet;
	}

	//==============================================================
	// GrabFrameNoStop()
	//==============================================================
	/// Grabs a single frame of video from the current video device
	/// without stopping the capture.
	/**
		\return Returns non-zero if success.
	*/
	BOOL GrabFrameNoStop()
	{_STT();
		BOOL bRet;

		try
		{
			// Grab frame
			bRet = capGrabFrameNoStop( m_hWnd );

		} // end try

		catch( ... )
		{	oexERROR( 0, "Assert in capGrabFrameNoStop()" );;
			return FALSE;
		} // end catch

		return bRet;
	}

	//==============================================================
	// WaitGrab()
	//==============================================================
	/// Waits for a grab operation to complete
	/**
		\param [in] timeout	-	Maximum amount of time, in milli-seconds, to wait for
								a grab operation to complete.

		\return Returns non-zero if success.
	*/
	BOOL WaitGrab( DWORD timeout = 3000 )
	{_STT();
		return ( WaitForSingleObject( m_hGrab, timeout ) != WAIT_TIMEOUT );
	}

	//==============================================================
	// PaletteAuto()
	//==============================================================
	/// Requests the capture driver automatically create new palettes for video frames.
	/**
		\param [in] frames		-	Number of frames to sample.
		\param [in] colors		-	Number of colors in palette, cannot be more than 256.

		\return Returns non-zero if success.
	*/
	BOOL PaletteAuto( DWORD frames, DWORD colors )
	{_STT();
		return capPaletteAuto( m_hWnd, frames, colors );
	}

	//==============================================================
	// PaletteManual()
	//==============================================================
	/// Requests the capture driver manually sample video frames.
	/**
		\param [in] grab		-	Set to true for each frame included in palette.
		\param [in] colors		-	Number of colors in palette, cannot be more than 256.
									This value is only used on the first frame of the sequence.

		Call this function for each frame you want to include in the palette.

		\return Returns non-zero if success.
	*/
	BOOL PaletteManual( BOOL grab, DWORD colors )
	{_STT();
		return capPaletteManual( m_hWnd, grab, colors );
	}

	//==============================================================
	// LoadPalette()
	//==============================================================
	/// Loads a color palette from a file.
	/**
		\param [in] pFile	-	File name of the palette.

		\return Returns non-zero if success.
	*/
	BOOL LoadPalette( LPCTSTR pFile )
	{_STT();
		return capPaletteOpen( m_hWnd, pFile );
	}

	//==============================================================
	// GetPaletteFromClipboard()
	//==============================================================
	/// Loads a color palette from the clipboard
	/**
		\return Returns non-zero if success.
	*/
	BOOL GetPaletteFromClipboard()
	{_STT();
		return capPalettePaste( m_hWnd );
	}

	//==============================================================
	// SavePalette()
	//==============================================================
	/// Saves the color palette to a file
	/**
		\param [in] pFile	-	File name of the palette.

		\return Returns non-zero if success.
	*/
	BOOL SavePalette( LPCTSTR pFile )
	{_STT();
		return capPaletteSave( m_hWnd, pFile );
	}

	//==============================================================
	// SetScrollPos()
	//==============================================================
	/// Sets the offset of the upper left corner of the video capture window.
	/**
		\param [in] x	-	Horizontal offset of video capture window.
		\param [in] y	-	Vertical offset of video capture window.

		\return Returns non-zero if success.
	*/
	BOOL SetScrollPos( long x, long y )
	{_STT();
		POINT	pt = { x, y };
		return capSetScrollPos( m_hWnd, &pt );
	}

	//==============================================================
	// GetNumStatusCallbacks()
	//==============================================================
	/// Returns the nnumber of status callbacks since start of capture.
	DWORD GetNumStatusCallbacks() { return m_dwOnStatus; }

	//==============================================================
	// GetNumErrorCallbacks()
	//==============================================================
	/// Returns the number of error callbacks since start of capture.
	DWORD GetNumErrorCallbacks() { return m_dwOnError; }

	//==============================================================
	// GetNumFrameCallbacks()
	//==============================================================
	/// Returns the number of frame callbacks since the start of capture.
	DWORD GetNumFrameCallbacks() { return m_llFrame; }

	//==============================================================
	// CallbacksEnabled()
	//==============================================================
	/// Returns non-zero if callbacks are enabled.
	BOOL CallbacksEnabled() { return m_bCallbacksEnabled; }

	//==============================================================
	// SetPos()
	//==============================================================
	/// Sets the offset of the capture window with respect to it's parent window
	/**
		\param [in] x	-	Horizontal offset of video capture window.
		\param [in] y	-	Vertical offset of video capture window.

		\return Returns non-zero if success.
	*/
	BOOL SetPos( long x, long y )
	{_STT();
		return SetWindowPos( m_hWnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE );
	}

	//==============================================================
	// SetSize()
	//==============================================================
	/// Sets the size of the capture window
	/**
		\param [in] x	-	Horizontal size of video capture window.
		\param [in] y	-	Vertical size of video capture window.

		\return Returns non-zero if success.
	*/
	BOOL SetSize( long x, long y )
	{_STT();
		return SetWindowPos( m_hWnd, NULL, 0, 0, x, y, SWP_NOZORDER | SWP_NOMOVE );
	}

	//==============================================================
	// SetWindow()
	//==============================================================
	/// Sets the size of the capture window.
	/**
		\param [in] pRect	-	Rectangle defining new capture window position with
								respect to the parent window.

		\return Returns non-zero if success.
	*/
	BOOL SetWindow( LPRECT pRect )
	{_STT();
	 	if ( pRect == NULL ) return FALSE;
		return SetWindowPos(	m_hWnd, NULL,
								pRect->left,
								pRect->top,
								pRect->right - pRect->left,
								pRect->bottom - pRect->top,
								SWP_NOZORDER );
	}

	//==============================================================
	// ShowWindow()
	//==============================================================
	/// Sets the capture windows show state
	/**
		\param [in] uCmdShow	-	Flag specifying how the capture window is to be
									shown.

		See the Windows API function ::ShowWindow()

		\return Returns non-zero if success.
	*/
	BOOL ShowWindow( UINT uCmdShow = SW_SHOWNORMAL )
	{_STT();
		return ::ShowWindow( m_hWnd, uCmdShow );
	}

	/// Returns a list of available devices
	static oexINT GetDevices( oexUINT x_uType, oex::CPropertyBag *pList )
	{
		if ( !pList )
			return 0;

		// Get driver info
		TCHAR szName[ oexSTRSIZE ] = { 0 };
		TCHAR szVer[ oexSTRSIZE ] = { 0 };
		for ( DWORD i = 0; i < 32; i++ )
			if ( capGetDriverDescription( i, szName, oexSTRSIZE, szVer, oexSTRSIZE ) )
			{	CPropertyBag &r = (*pList)[ i ];
				r[ oexT( "name" ) ] = szName;
				r[ oexT( "ver" ) ] = szVer;
			} // end if

		return pList->Size();
	}



private:

	/// HDC that receives a copy of the video frame
	HDC			m_hDrawDc;

	/// Draw DIB handle used to decode video frames
	HDRAWDIB 	m_hDrawDib;

	/// Size of the decoded video
	RECT		m_rectDraw;

	/// Window that gets callback messages on video frame events
	HWND		m_hFrameWnd;

	/// Window message value used for video frame event callbacks
	UINT		m_uFrameMsg;

	/// Non-zero if video frame event callbacks are enabled
	BOOL		m_bCallbacksEnabled;

	/// Video status code
	DWORD		m_dwOnStatus;

	/// Video error code
	DWORD		m_dwOnError;

	/// Video frame code
	DWORD		m_llFrame;

protected:

	//==============================================================
	// StatusCallbackProc()
	//==============================================================
	/// Static function proxies video status callbacks
	static LRESULT PASCAL StatusCallbackProc(HWND hWnd, int nID, LPSTR lpStatusText)
	{_STT();

		// Retrieve our class pointer
		CVfwCap *pCap = (CVfwCap*)CVFWCAP_GetWindowLong( hWnd );
		if ( pCap == NULL ) return 0;

		return pCap->OnStatus( nID, lpStatusText );
	}

	//==============================================================
	// ErrorCallbackProc()
	//==============================================================
	/// Static function proxies video error callbacks
	static LRESULT PASCAL ErrorCallbackProc(HWND hWnd, int nErrID, LPSTR lpErrorText)
	{_STT();
		// Retrieve our class pointer
		CVfwCap *pCap = (CVfwCap*)CVFWCAP_GetWindowLong( hWnd );
		if ( pCap == NULL ) return 0;

		return pCap->OnError( nErrID, lpErrorText );
	}

	//==============================================================
	// FrameCallbackProc()
	//==============================================================
	/// Static function proxies video frame callbacks
	static LRESULT PASCAL FrameCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr)
	{_STT();
		// Retrieve our class pointer
		CVfwCap *pCap = (CVfwCap*)CVFWCAP_GetWindowLong( hWnd );
		if ( pCap == NULL ) return 0;

		return pCap->OnFrame( lpVHdr );
	}

	//==============================================================
	// StreamCallbackProc()
	//==============================================================
	/// Static function proxies video frame callbacks
	static LRESULT CALLBACK StreamCallbackProc(HWND hWnd, LPVIDEOHDR lpVHdr)
	{_STT();
		// Retrieve our class pointer
		CVfwCap *pCap = (CVfwCap*)CVFWCAP_GetWindowLong( hWnd );
		if ( pCap == NULL ) return 0;

		return pCap->OnStream( lpVHdr );
	}


	//==============================================================
	// OnStatus()
	//==============================================================
	/// Resolved video status callback
	virtual BOOL OnStatus( int nID, LPCTSTR pStatus )
	{_STT();
		// Count status updates
		m_dwOnStatus++;

		return TRUE;
	}

	//==============================================================
	// OnError()
	//==============================================================
	/// Resolved video error callback
	virtual BOOL OnError( int nID, LPCTSTR pStatus )
	{_STT();
		// Count errors
		m_dwOnError++;

		if ( nID != 0 && pStatus != NULL && *pStatus != 0 )
		{
			// Log error
	//		_Log( MB_ICONERROR, "Capture Driver", pStatus );

		} // end if

		return TRUE;
	}

	//==============================================================
	// OnFrame()
	//==============================================================
	/// Resolved video frame callback
	/**
		\param [in] pVHdr	-	Video header information.

		Video header structure

		\code

		typedef struct
		{
			LPSTR lpData;            // address of video buffer
			DWORD dwBufferLength;    // size, in bytes, of the
									 // data buffer
			DWORD dwBytesUsed;       // see below
			DWORD dwTimeCaptured;    // see below
			DWORD dwUser;            // user-specific data
			DWORD dwFlags;           // see below
			DWORD dwReserved[4];     // reserved; do not use
		} VIDEOHDR;

		\endcode

		- Members -

		<b>dwBytesUsed</b>:
		Number of bytes used in the data buffer.

		<b>dwTimeCaptured</b>:
		Time, in milliseconds, when the frame was captured relative to
		the first frame in the stream.
		Synchronization of audio and video in the resulting AVI file
		depends solely on this parameter, which should be derived
		directly from a counter (VSYNC_Count) incremented by a vertical
		sync interrupt. When using NTSC frame rates and frame-based
		interrupts, the returned value would be:

		<b>dwTimeCaptured</b> = VSYNC_Count * 1/29.97.

		The capture driver must maintain VSYNC_Count accurately by
		allowing recursive entry into the interrupt service routine
		(if only for the purpose of incrementing VSYNC_Count) and by not
		disabling interrupts for long periods of time.

		<b>dwFlags</b>:
		Flags giving information about the data buffer. The following
		flags are defined for this field: VHDR_DONE  Set by the device
		driver to indicate it is finished with the data buffer and it is
		returning the buffer to the application.

		VHDR_PREPARED	Set by the system to indicate the data buffer has
						been prepared with videoStreamPrepareHeader.
		VHDR_INQUEUE	Set by the system to indicate the data buffer is
						queued for playback.
		VHDR_KEYFRAME	Set by the device driver to indicate a key frame.
	*/
	virtual BOOL OnFrame( LPVIDEOHDR pVHdr )
	{_STT();
		// Do we have a callback function?
		if ( oexCHECK_PTR( pVHdr ) && oexCHECK_PTR( m_cbfOnFrame ) )
		{
			CCaptureTmpl::SFrameInfo si;
			oexZeroMemory( &si, sizeof( si ) );

			si.lSize = sizeof( si );
			si.pBuf = pVHdr->lpData;
			si.lImageSize = (long)( pVHdr->dwBytesUsed ? pVHdr->dwBytesUsed : pVHdr->dwBufferLength );
			si.lWidth = m_nWidth;
			si.lHeight = m_nHeight;
			si.llFrame = m_llFrame;

			// Make the callback
			m_cbfOnFrame( &si, m_pUser );

		} // end if

		// Count frames
		m_llFrame++;

		return TRUE;
	}

	//==============================================================
	// OnStream()
	//==============================================================
	/// Resolved video stream callback
	virtual BOOL OnStream( LPVIDEOHDR pVHdr )
	{_STT();
		// Do we have a callback function?
		if ( oexCHECK_PTR( pVHdr ) && oexCHECK_PTR( m_cbfOnFrame ) )
		{
			CCaptureTmpl::SFrameInfo si;
			oexZeroMemory( &si, sizeof( si ) );

			si.lSize = sizeof( si );
			si.pBuf = pVHdr->lpData;
			si.lImageSize = (long)( pVHdr->dwBytesUsed ? pVHdr->dwBytesUsed : pVHdr->dwBufferLength );
			si.lWidth = m_nWidth;
			si.lHeight = m_nHeight;
			si.llFrame = m_llFrame;

			// Make the callback
			m_cbfOnFrame( &si, m_pUser );

		} // end if

		// Count frames
		m_llFrame++;

		return TRUE;
	}

private:

	/// Window handle of capture window
	HWND				m_hWnd;

	/// Non-zero if capture device is connected
	BOOL				m_bConnected;

	/// Zero based index of current capture driver
	DWORD				m_dwCurrentDriver;

	/// Total number of cature drivers installed
	DWORD				m_dwNumDrivers;

	/// Information about installed capture drivers
	SDriverInfo			m_vdi[ eMaxDrivers ];

	/// Video format information
	LPBITMAPINFO		m_pVideoFormat;

	/// Pointer to custom video data
	BYTE				*m_pVideoData;

	/// Size of buffer in m_pVideoData
	DWORD				m_dwVideoDataSize;

	/// Currently set capture filename
	TCHAR				m_szCaptureFileName[ oexSTRSIZE ];

	/// Audio format information
	LPWAVEFORMATEX		m_pAudioFormat;

	/// Pointer to custom audio format data
	BYTE				*m_pAudioData;

	/// Size of buffer in m_pAudioData
	DWORD				m_dwAudioDataSize;

	/// Handle of grab event
	HANDLE				m_hGrab;

	/// Frame callback information
	SFrameCallbackInfo	m_fci;

	/// Lock to protect multiple capture driver threads
	static oexLock		m_lock;

public:

	//==============================================================
	// Reconnect()
	//==============================================================
	/// Attempts reconnect of current capture device
	/**
		\return Returns non-zero if success.
	*/
	BOOL Reconnect()
	{_STT();

		oexAutoLock ll( m_lock );
		if ( !ll.IsLocked() )
			return FALSE;

		// Disconnect from capture driver
		if ( !m_bConnected ) return FALSE;

		// Disconnect capture driver
		capDriverDisconnect( m_hWnd );

		// Attempt to connect to the specified driver
		m_bConnected = capDriverConnect( m_hWnd, m_dwCurrentDriver );
		if ( !m_bConnected ) { Disconnect(); return FALSE; }

		// Get driver capabilities
		capDriverGetCaps( m_hWnd, &m_cdc, sizeof( m_cdc ) );

		return TRUE;
	}

	//==============================================================
	// GetDriverIndex()
	//==============================================================
	/// Returns the index of the current capture device
	DWORD GetDriverIndex() { return m_dwCurrentDriver; }

	//==============================================================
	// SetFrameMessageTarget()
	//==============================================================
	/// Sets the window handle and message id for video event callbacks
	/**
		\param [in] hDC		-	DC that will receive each video frame.
		\param [in] pRect	-	Size of the DC in hDC.
		\param [in] hWnd	-	Window that receives the callback.
		\param [in] uMsg	-	Message ID for callbacks.

		\return Returns non-zero if success.
	*/
	BOOL SetFrameMessageTarget( HDC hDC, LPRECT pRect, HWND hWnd, UINT uMsg )
	{_STT();
		// Are we clearing the target?
		if ( hDC == NULL || pRect == NULL || hWnd == NULL )
		{	m_hDrawDc = NULL;
			m_hFrameWnd = NULL;
			m_uFrameMsg = 0;
		} // end if

		// Copy the destination rectangle
		CopyRect( &m_rectDraw, pRect );

		// Save information
		m_hDrawDc = hDC;
		m_hFrameWnd = hWnd;
		m_uFrameMsg = uMsg;

		// Get the bitmap information
		LPBITMAPINFO pbi;
		if ( !GetVideoFormat( &pbi ) || pbi == NULL ) return FALSE;

		// Set up the DrawDib
		return DrawDibBegin(	m_hDrawDib, hDC,
								( pRect->right - pRect->left ),
								( pRect->bottom - pRect->top ),
								(LPBITMAPINFOHEADER)pbi,
								pbi->bmiHeader.biWidth,
								pbi->bmiHeader.biHeight,
								0 );//DDF_DONTDRAW );
	}

	//==============================================================
	// SetCaptureWindowPos()
	//==============================================================
	/// Sets the Z-Order, position and size of the capture window.
	/**
		\param [in] hwndAfter	-	Puts the capture window after this window in the Z-Order.
		\param [in] x			-	Horizontal offset of upper left corner of capture window.
		\param [in] y			-	Veritical offset of upper left corner of capture window.
		\param [in] cx			-	Horizontal size of capture window.
		\param [in] cy			-	Vertical size of capture window.
		\param [in] flags		-	Flags to pass to SetWindowPos()

		\return Returns non-zero if success.
	*/
	BOOL SetCaptureWindowPos( HWND hwndAfter, int x, int y, int cx, int cy, UINT flags )
	{_STT();

		// Punt if no window
		if ( !::IsWindow( m_hWnd ) ) return FALSE;

		return SetWindowPos( m_hWnd, hwndAfter, x, y, cx, cy, flags );
	}

	//==============================================================
	// ReleaseVideoFormatData()
	//==============================================================
	/// Releases video format data
	void ReleaseVideoFormatData()
	{_STT();
		if ( m_pVideoData != NULL )
		{
			delete [] m_pVideoData;
			m_pVideoData = NULL;
		} // end if
		m_dwVideoDataSize = 0;
		m_pVideoFormat = NULL;
	}

	//==============================================================
	// ReleaseAudioFormatData()
	//==============================================================
	/// Releases audio format data
	void ReleaseAudioFormatData()
	{_STT();
		if ( m_pAudioData != NULL )
		{
			delete [] m_pAudioData;
			m_pAudioData = NULL;
		} // end if
		m_dwAudioDataSize = 0;
		m_pAudioFormat = NULL;
	}

	//==============================================================
	// GetAudioFormat()
	//==============================================================
	/// Gets the audio format information
	/**
		\param [out] ppwfx	-	Receives the audio format data.

		\return Returns non-zero if success.
	*/
	BOOL GetAudioFormat( LPWAVEFORMATEX *ppwfx = NULL )
	{_STT();
		if ( !IsConnected() ) return FALSE;

		// Lose the old audio data
		ReleaseAudioFormatData();

		// Get the size of the format buffer
		m_dwAudioDataSize = capGetAudioFormatSize( m_hWnd );
		if ( m_dwAudioDataSize == 0 ) return FALSE;

		// Allocate memory
		m_pAudioData = new BYTE[ m_dwAudioDataSize + 1 ];
		if ( m_pAudioData == NULL )
		{
			m_dwAudioDataSize = 0;
			return FALSE;
		} // end if

		// Set the pointer
		m_pAudioFormat = (LPWAVEFORMATEX)m_pAudioData;

		if ( !capGetAudioFormat( m_hWnd, m_pAudioFormat, m_dwAudioDataSize ) )
		{
			ReleaseAudioFormatData();
			return FALSE;
		} // end if

		// Does the caller want a copy of the pointer?
		if ( ppwfx != NULL ) *ppwfx = m_pAudioFormat;

		return TRUE;
	}

	//==============================================================
	// Disconnect()
	//==============================================================
	/// Disconnects current capture device
	void Disconnect()
	{_STT();
		if ( !IsWnd() ) return;

		oexAutoLock ll( m_lock );
		if ( !ll.IsLocked() )
			return;

		// Disconnect from capture driver
		if ( m_bConnected )
		{
			// No more callbacks
			DisableCallbacks();

			// Disconnect driver
			capDriverDisconnect( m_hWnd );

			// Not connected
			m_bConnected = FALSE;

		} // end if

		if ( m_hDrawDib )
		{	DrawDibClose( m_hDrawDib );
			m_hDrawDib = NULL;
		} // end if

	}

	//==============================================================
	// DisableCallbacks()
	//==============================================================
	/// Disables frame callbacks
	void DisableCallbacks()
	{_STT();

		BOOL bDelay = m_bCallbacksEnabled;

		m_bCallbacksEnabled = FALSE;

		capSetCallbackOnError( m_hWnd, NULL );
		capSetCallbackOnStatus( m_hWnd, NULL );
		capSetCallbackOnFrame( m_hWnd, NULL );
		capSetCallbackOnVideoStream( m_hWnd, NULL );

		if ( bDelay )
			Sleep( 1000 );
	}

	//==============================================================
	// EnableCallbacks()
	//==============================================================
	/// Enables frame callbacks
	/**
		\return Returns non-zero if success.
	*/
	BOOL EnableCallbacks()
	{_STT();
		if ( !IsWnd() )
			return FALSE;

		// Reset the number of callbacks
		m_dwOnStatus = 0;
		m_dwOnError = 0;
		m_llFrame = 0;

		// Set our class pointer
		SetLastError( 0 );

		// Give the window a pointer to the class
		if ( !CVFWCAP_SetWindowLong( GetSafeHwnd(), this ) && GetLastError() != 0 )
		{
			// Give up if we can't set the class pointer
			return FALSE;
		} // end if

		// Callbacks are enabled
		m_bCallbacksEnabled = TRUE;

		// Enable error callbacks
		capSetCallbackOnError( m_hWnd, CVfwCap::ErrorCallbackProc );

		// Enable status callbacks
		capSetCallbackOnStatus( m_hWnd, CVfwCap::StatusCallbackProc );

		// Enable frame callbacks
		capSetCallbackOnFrame( m_hWnd, CVfwCap::FrameCallbackProc );

		// Enable stream callbacks
		capSetCallbackOnVideoStream( m_hWnd, CVfwCap::StreamCallbackProc );

		return TRUE;
	}

	//==============================================================
	// AddInfoChunk()
	//==============================================================
	/// Adds information chunk to AVI file at current position
	/**
		\param [in] type	-	Value specifying info chunk type.
		\param [in] buf		-	Buffer containing info chunk data.
		\param [in] size	-	size of the buffer in buf.

		Use this function to add arbitrary data into the current
		AVI video stream.

		\return Returns non-zero if success.
	*/
	BOOL AddInfoChunk( FOURCC type, LPBYTE buf, DWORD size )
	{_STT();
		if ( buf == NULL || size == 0 ) return FALSE;

		CAPINFOCHUNK	cic;

		cic.fccInfoID = type;
		cic.lpData = buf;
		cic.cbData = size;

		return capFileSetInfoChunk( m_hWnd, &cic );
	}

	//==============================================================
	// GetVideoFormat()
	//==============================================================
	/// Returns information about the current video format
	/**
		\param [out] ppbmp	-	Structure to receive current video format information.
		\param [out] psize	-	Receives the size of the data copied to ppbmp.

		\return Returns non-zero if success.
	*/
	BOOL GetVideoFormat( LPBITMAPINFO *ppbmp = NULL, LPDWORD psize = NULL )
	{_STT();
		if ( ppbmp != NULL ) *ppbmp = NULL;
		if ( psize != NULL ) *psize = 0;
		if ( !IsConnected() ) return FALSE;

		// Lose the video data
		ReleaseVideoFormatData();

		// Get the size of the format buffer
		m_dwVideoDataSize = capGetVideoFormatSize( m_hWnd );
		if ( m_dwVideoDataSize == 0 ) return FALSE;

		// Allocate memory
		m_pVideoData = new BYTE[ m_dwVideoDataSize + 1 ];
		if ( m_pVideoData == NULL )
		{
			m_dwVideoDataSize = 0;
			return FALSE;
		} // end if

		// Set the pointer
		m_pVideoFormat = (BITMAPINFO*)m_pVideoData;

		if ( !capGetVideoFormat( m_hWnd, m_pVideoFormat, m_dwVideoDataSize ) )
		{
			ReleaseVideoFormatData();
			return FALSE;
		} // end if

		// For good measure
		m_pVideoData[ m_dwVideoDataSize ] = 0;

		// Does the caller want a copy of the pointer?
		if ( ppbmp != NULL ) *ppbmp = m_pVideoFormat;
		if ( psize != NULL ) *psize = m_dwVideoDataSize;

		return TRUE;
	}

	//==============================================================
	// SetVideoFormat()
	//==============================================================
	/// Sets the current video format
	/**
		\param [in] pbmp	-	Structure containing video format information.
		\param [in] size	-	Receives the size of the data copied to pbmp.

		\return Returns non-zero if success.
	*/
	BOOL SetVideoFormat( LPBITMAPINFO pbmp, DWORD size )
	{_STT();
		return capSetVideoFormat( m_hWnd, pbmp, size );
	}

private:

	/// Current driver capabilities
	CAPDRIVERCAPS		m_cdc;

	/// Current driver status ( call UpdateStatus() )
	CAPSTATUS			m_cs;

	/// Capture params
	CAPTUREPARMS		m_cp;

public:

	/// Set frame callback function
	void SetFrameCallback( CCaptureTmpl::cbf_OnFrame &f, oexPVOID u )
	{	m_cbfOnFrame = f; m_pUser = u; }

private:

	/// Frame callback function pointer
	CCaptureTmpl::cbf_OnFrame			m_cbfOnFrame;

	/// User data passed to callback function
	oexPVOID							m_pUser;

	/// Image width
	oexINT								m_nWidth;

	/// Image height
	oexINT								m_nHeight;

	/// Image format
	oexUINT								m_uFormat;

	/// Frames per second
	oexFLOAT							m_fFps;

	/// Device index
	oexUINT								m_uDevice;

	/// Source index
	oexUINT								m_uSource;

public:

	//==============================================================
	// Open()
	//==============================================================
	/// Opens the capture device and starts the capture
	BOOL Open( oexUINT x_uDevice, oexUINT x_uSource, oexINT x_nWidth, oexINT x_nHeight,
			   oexUINT x_uFormat, oexFLOAT x_fFps, CCaptureTmpl::cbf_OnFrame x_cbfOnFrame,
			   oexPVOID x_pUser, oexUINT x_uTimeout = oexDEFAULT_WAIT_TIMEOUT )
	{_STT();
		Destroy();

		// Save info
		m_uDevice = x_uDevice;
		m_uSource = x_uSource;
		m_nWidth = x_nWidth;
		m_nHeight = x_nHeight;
		m_uFormat = x_uFormat;
		m_fFps = x_fFps;
		m_cbfOnFrame = x_cbfOnFrame;
		m_pUser = x_pUser;

		// Reset stop event
		ResetEvent( m_hStop );
		ResetEvent( m_hSuccess );

		// Create a thread
		m_hThread = CreateThread(	(LPSECURITY_ATTRIBUTES)NULL, 0,
									&CVfwCap::_CaptureThread, (LPVOID)this,
									0, &m_dwThreadId );

		if ( INVALID_HANDLE_VALUE == m_hThread )
		{	oexERROR( GetLastError(), "Error creating capture thread" );
			return oexFALSE;
		} // end if

		HANDLE hEvents[] = { m_hSuccess, m_hThread };
		if ( WAIT_OBJECT_0 != ::WaitForMultipleObjects( 2, hEvents, FALSE, x_uTimeout ) )
		{	oexERROR( 0, "Failed to open capture device" );
			Destroy();
			return oexFALSE;
		} // end if

		return oexTRUE;
	}

	//==============================================================
	// Destroy()
	//==============================================================
	/// Closes any open driver and releases all resources
	void Destroy()
	{_STT();
		if ( INVALID_HANDLE_VALUE == m_hThread )
			return;

		HANDLE hThread = m_hThread;
		m_hThread = INVALID_HANDLE_VALUE;

		// Tell Thread To Stop
		SetEvent( m_hStop );

		// Post quit message to thread
		if ( m_dwThreadId )
		{	PostThreadMessage( m_dwThreadId, eVfwQuitMsg, 0, 0L );
			m_dwThreadId = 0;
		} // end if

		// Wait for thread to exit, if it's hung, kill it
		if ( WAIT_OBJECT_0 != ::WaitForSingleObject( hThread, oexDEFAULT_WAIT_TIMEOUT ) )
			TerminateThread( hThread, FALSE );
	}

	/// Returns non-zero if thread is running
	BOOL IsRunning()
	{_STT();
		if ( INVALID_HANDLE_VALUE == m_hThread ) return FALSE;
		if ( INVALID_HANDLE_VALUE == m_hStop ) return FALSE;
		return WAIT_OBJECT_0 != ::WaitForSingleObject( m_hStop, 0 );
	}

private:

	static DWORD WINAPI _CaptureThread( LPVOID pData )
	{_STT();
		CVfwCap *pVfwCap = (CVfwCap*)pData;
		if ( !pVfwCap )
			return -1;

		return pVfwCap->CaptureThread( pData );
	}

	static BOOL MessagePump()
	{_STT();
		MSG msg;

		// If there is a message
		while (	PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			if ( eVfwQuitMsg == msg.message )
				return oexFALSE;

			// Translate accelerators
	//		if ( !TranslateAccelerator( msg.hwnd, hAccel, &msg ) )
			{
				// Translate key strokes
				TranslateMessage( &msg );

				// Dispatch the message
				DispatchMessage( &msg );

			} // end if

		} // end if

		return oexTRUE;
	}


	DWORD WINAPI CaptureThread( LPVOID pData )
	{_STT();
		HRESULT hr = -1;
		if ( FAILED( hr = ::CoInitialize( NULL ) ) )
		{	oexERROR( hr, oexT( "::CoInitialize() failed" ) );
			return -1;
		} // end if

		// Pump messages
		MessagePump();

		CStr sTitle = oexFmt( oexT( "oex_vfw_%u" ), m_uDevice );
		if ( FindWindowEx( NULL, NULL, NULL, sTitle.Ptr() ) )
		{	oexERROR( -1, oexFmt( oexT( "Capture device is already open : %u" ), m_uDevice ) );
			Close(); return -2;
		} // end if

		// Create capture window
		BOOL bRes = Create( sTitle.Ptr(), NULL, WS_VISIBLE | WS_OVERLAPPEDWINDOW,
						    0, 0, m_nWidth, m_nHeight, 0 );
		if ( !bRes )
		{	oexERROR( GetLastError(), oexT( "Failed to open capture device" ) );
			Close(); return -3;
		} // end if

		// Attempt to connect to specified device
		if ( !Connect( m_uDevice ) )
		{	oexERROR( GetLastError(), CStr().Fmt( oexT( "Unable to open specified device : %d" ), (int)m_uDevice ) );
			Close(); return -4;
		} // end if

		// Enable video callbacks
		if ( !EnableCallbacks() )
		{	oexERROR( GetLastError(), oexT( "Video callback unsupported" ) );
			Close(); return -5;
		} // end if

		// Set frame rate
		if ( !SetPreviewRate( m_fFps ) )
		{	oexERROR( GetLastError(), CStr().Fmt( oexT( "Unable to set frame rate : %d" ), (int)m_fFps ) );
			Close(); return -6;
		} // end if

		// Let creator know we succeeded
		SetEvent( m_hSuccess );

		if ( !CaptureSequenceNoFile() )
//		if ( !GrabFrame() )
		{	oexERROR( GetLastError(), CStr().Fmt( oexT( "Failed to get image : %d" ), (int)m_fFps ) );
			Close(); return -7;
		} // end if

		DWORD dwDelay = (DWORD)( m_fFps / 2 );
		if ( 1 > dwDelay )
			dwDelay = 1;

		// If there is a message
		while (	MessagePump()
			    && WAIT_OBJECT_0 != ::WaitForSingleObject( m_hStop, dwDelay ) )
			;

		// Kill the capture stuff
		Close();

		// Uninitialize com
		::CoUninitialize();

		return 0;
	}

	/// Thread handle
	HANDLE							m_hThread;

	/// The thread id
	DWORD							m_dwThreadId;

	/// Stop event
	HANDLE							m_hStop;

	/// Event triggered when capture thread successfully starts
	HANDLE							m_hSuccess;

};

// Global lock for vfw capture devices
oexLock		CVfwCap::m_lock;


class CV4w1 : public CCaptureTmpl
{
public:


public:

	/// Default contructor
	CV4w1()
	{_STT();
		m_nWidth = 0;
		m_nHeight = 0;
		m_uFormat = 0;
		m_fFps = 0;
		m_bReady = FALSE;
	}

	/// Destructor
	~CV4w1()
	{_STT();
		Destroy();
	}

	/// Static callback
	static oexRESULT _OnFrame( CCaptureTmpl::SFrameInfo *pFi, LPVOID pUser )
	{_STT();
		CV4w1 *pThis = (CV4w1*)pUser;
		if ( !pThis ) return ERROR_INVALID_PARAMETER;
		return pThis->OnFrame( pFi, pUser );
	}

	/// Handles video frame callbacks
	oexRESULT OnFrame( CCaptureTmpl::SFrameInfo *pFi, LPVOID pUser )
	{_STT();
		m_pFi = pFi;
		m_bReady = oexTRUE;

		while ( m_bReady && m_cap.IsRunning() )
			Sleep( 15 );

		m_pFi = oexNULL;

		return 0;
	}

	/// Static callback
	static oexRESULT _OnStream( CCaptureTmpl::SFrameInfo *pFi, LPVOID pUser )
	{_STT();
		CV4w1 *pThis = (CV4w1*)pUser;
		if ( !pThis ) return ERROR_INVALID_PARAMETER;
		return pThis->OnFrame( pFi, pUser );
	}

	//==============================================================
	// Open()
	//==============================================================
	/**
		\param [in] x_uDevice	-	Device index
	*/
	virtual oexBOOL Open( oexUINT x_uType, oexUINT x_uDevice, oexUINT x_uSource, oexINT x_nWidth, oexINT x_nHeight, oexUINT x_uFormat, oexFLOAT x_fFps, oexBOOL x_bInit )
	{_STT();
		Destroy();

		// Save information
		m_nWidth = x_nWidth;
		m_nHeight = x_nHeight;
		m_uFormat = x_uFormat;
		m_fFps = x_fFps;

		// Create capture window
		BOOL bRes = m_cap.Open( x_uDevice, x_uSource, x_nWidth, x_nHeight,
			                    x_uFormat, x_fFps, CV4w1::_OnFrame, this );
		if ( !bRes )
		{	oexERROR( GetLastError(), oexT( "Failed to open capture device" ) );
			Destroy();
			return oexFALSE;
		} // end if

		// We're running
		m_sigRunning.Signal();

		return oexTRUE;
	}

	//==============================================================
	// Open()
	//==============================================================
	/**
		\param [in] x_pFile		- Video file to open
	*/
	virtual oexBOOL Open( oexUINT x_uType, oexCSTR x_pFile, oexINT x_nWidth, oexINT x_nHeight, oexUINT x_uFormat, oexFLOAT x_fFps, oexBOOL x_bInit )
	{_STT();

		return oexFALSE;
	}

	//==============================================================
	// Destroy()
	//==============================================================
	/// Closes any open capture device
	virtual oexBOOL Destroy()
	{_STT();
		// Close the capture device
		m_cap.Destroy();

		m_pFi = oexNULL;
		m_nWidth = 0;
		m_nHeight = 0;
		m_uFormat = 0;
		m_fFps = 0;
		m_bReady = FALSE;

		// We're not running
		m_sigRunning.Reset();

		return oexTRUE;
	}

public:

	//==============================================================
	// IsOpen()
	//==============================================================
	/// Returns non-zero if a capture device is open
	virtual oexBOOL IsOpen()
	{_STT();
		return m_cap.IsConnected();
	}

	//==============================================================
	// Init()
	//==============================================================
	/// Initializes capture system
	/**
	*/
	virtual oexBOOL Init()
	{_STT();
		return m_cap.Init();
	}

	//==============================================================
	// GetSupportedFormats()
	//==============================================================
	virtual CStr GetSupportedFormats()
	{_STT();
//		return oexT( "YUYV,RGB3" );
		return oexT( "RGB3" );
	}

	//==============================================================
	// GetFormatDescription()
	//==============================================================
	virtual CStr GetFormatDescription( oexUINT x_uFormat )
	{_STT();
		switch( x_uFormat )
		{
//			case oexFOURCC( 'Y', 'U', 'Y', 'V' ) :
//				return "YUYV - Y, U (Cb) and V (Cr)";

			case oexFOURCC( 'R', 'G', 'B', '3' ) :
				return "RGB3 - 24-bit / Red(8), Green(8), Blue(8)";

			default:
				break;
		};

		return CStr();
	}

	//==============================================================
	// StartCapture()
	//==============================================================
	virtual oexBOOL StartCapture()
	{_STT();
		return oexTRUE;
//		return S_OK == m_cap.CaptureSequenceNoFile();
	}

	//==============================================================
	// StopCapture()
	//==============================================================
	/// Stops video capture
	virtual oexBOOL StopCapture()
	{_STT();
		// Kill the capture thread
		m_cap.Destroy();

		return oexTRUE;
//		return S_OK == m_cap.CaptureStop();
	}

	//==============================================================
	// WaitForFrame()
	//==============================================================
	/// Waits for a new frame of video
	oexBOOL WaitForFrame( oexUINT x_uTimeout = 0 )
	{_STT();
		CTimeout to; to.SetMs( x_uTimeout );
		while ( !m_bReady && to.IsValid() )
			Sleep( 15 );
		return m_bReady;
	}

	//==============================================================
	// ReleaseFrame()
	//==============================================================
	virtual oexBOOL ReleaseFrame()
	{_STT();
		m_bReady = FALSE;

		return oexTRUE;
	}

	//==============================================================
	// GetBuffer()
	//==============================================================
	/// Returns a pointer to the video buffer
	virtual oexPVOID GetBuffer()
	{_STT();
		if ( !m_pFi )
			return oexNULL;
		return m_pFi->pBuf;
	}

	//==============================================================
	// GetImageSize()
	//==============================================================
	/// +++ Should return the size of the video buffer
	virtual oexINT GetImageSize()
	{_STT();
		return CImage::GetScanWidth( m_nWidth, 24 ) * cmn::Abs( m_nHeight );
	}

	//==============================================================
	// GetBufferSize()
	//==============================================================
	oexINT GetBufferSize()
	{_STT();
		if ( !m_pFi )
			return 0;
		return m_pFi->lImageSize;
	}

	//==============================================================
	// GetImage()
	//==============================================================
	/// Returns an image object
	virtual CImage* GetImage()
	{_STT();
		return oexNULL;
	}

	//==============================================================
	// GetWidth()
	//==============================================================
	/// Returns the image width
	virtual oexINT GetWidth()
	{	return m_nWidth; }

	//==============================================================
	// GetHeight()
	//==============================================================
	/// Returns the image height
	virtual oexINT GetHeight()
	{	return m_nHeight; }

	//==============================================================
	// GetFormat()
	//==============================================================
	/// Returns the image format
	virtual oexUINT GetFormat()
	{	return m_uFormat; }

	//==============================================================
	// GetFps()
	//==============================================================
	/// Returns the frame rate in frames per second
	virtual oexFLOAT GetFps()
	{	return m_fFps; }

	//==============================================================
	// GetFrame()
	//==============================================================
	/// Returns the current frame index
	virtual oexINT64 GetFrame()
	{	return m_llFrame; }

	//==============================================================
	// GetDevices()
	//==============================================================
	/// Returns a list of available input devices
	static oexINT GetDevices( oexUINT x_uType, oex::CPropertyBag *pList )
	{
		if ( !pList )
			return 0;

		return CVfwCap::GetDevices( x_uType, pList );
	}

private:

	/// Name of the device
	CStr							m_sDeviceName;

	/// The current frame number
	oexINT64						m_llFrame;

	/// Image width
	oexINT							m_nWidth;

	/// Image height
	oexINT							m_nHeight;

	/// Image format
	oexUINT							m_uFormat;

	/// Frames per second
	oexFLOAT						m_fFps;

	/// The Video For Windows capture class
	CVfwCap							m_cap;

	/// Holds image frame info during callback
	CCaptureTmpl::SFrameInfo		*m_pFi;

	/// +++ Replace with signal
	volatile BOOL					m_bReady;

};


