/*------------------------------------------------------------------
// capture.h
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

#pragma once

// !!! Add new ones freely, but never *change* the oexFOURCC types below

#define oexVIDSUB_AUTO		0
#define oexVIDSUB_AUTO_STR		oexT( "Auto Select" )
#define oexVIDSUB_AUTO_TAG		oexT( "AUTO" )

// Linux
#define oexVIDSUB_VFL1			oexFOURCC( '1', 'V', 'F', 'L' )
#define oexVIDSUB_VFL1_STR		oexT( "Video For Linux 1" )
#define oexVIDSUB_VFL1_TAG		oexT( "VFL1" )

#define oexVIDSUB_VFL2			oexFOURCC( '2', 'V', 'F', 'L' )
#define oexVIDSUB_VFL2_STR		oexT( "Video For Linux 2" )
#define oexVIDSUB_VFL2_TAG		oexT( "VFL2" )

#define oexVIDSUB_DAVINCI		oexFOURCC( '1', 'D', 'A', 'V' )
#define oexVIDSUB_DAVINCI_STR	oexT( "TI DaVinci video capture" )
#define oexVIDSUB_DAVINCI_TAG	oexT( "DAVINCI" )

// Windows
#define oexVIDSUB_VFW			oexFOURCC( '1', 'V', 'F', 'W' )
#define oexVIDSUB_VFW_STR		oexT( "Video For Windows" )
#define oexVIDSUB_VFW_TAG		oexT( "VFW" )

#define oexVIDSUB_DSHOW			oexFOURCC( '1', 'D', 'S', 'H' )
#define oexVIDSUB_DSHOW_STR		oexT( "Direct Show" )
#define oexVIDSUB_DSHOW_TAG		oexT( "DSHOW" )

struct SVideoSystemInfo
{
	/// Unique index for this system
	// Note that this number may change from build to build.
	oexUINT32		nIndex;

	/// Type of video system
	// This will never change
	oexUINT32		uType;

	/// Short tag name ( compatible with a file name )
	oexCSTR			pTag;

	/// Human readable name
	oexCSTR			pName;

	/// If the interface is supported in the current build
	oexBOOL			bSupported;
};

static oexCONST SVideoSystemInfo g_VideoSystemInfo[] =
{
	{ 0, oexVIDSUB_AUTO, oexVIDSUB_AUTO_TAG, oexVIDSUB_AUTO_STR, 1 },

	{ 1, oexVIDSUB_DAVINCI, oexVIDSUB_DAVINCI_TAG, oexVIDSUB_DAVINCI_STR,
#if defined( OEX_LINUX ) && defined( OEX_CPU_ARM )
	1,
#else
	0,
#endif
	},

	{ 2, oexVIDSUB_VFL2, oexVIDSUB_VFL2_TAG, oexVIDSUB_VFL2_STR,
#if defined( OEX_LINUX )
	1,
#else
	0,
#endif
	},

	{ 3, oexVIDSUB_VFL1, oexVIDSUB_VFL1_TAG, oexVIDSUB_VFL1_STR,
#if defined( OEX_LINUX )
	1,
#else
	0,
#endif
	},

	{ 4, oexVIDSUB_DSHOW, oexVIDSUB_DSHOW_TAG, oexVIDSUB_DSHOW_STR,
#if defined( OEX_WINDOWS )
	1,
#else
	0,
#endif
	},

	{ 5, oexVIDSUB_VFW, oexVIDSUB_VFW_TAG, oexVIDSUB_VFW_STR,
#if defined( OEX_WINDOWS )
	1,
#else
	0,
#endif
	}

};

//==================================================================
// CCaptureTmpl
//
/// Defines the capture class interface
/**


*/
//==================================================================
class CCaptureTmpl
{
public:

	/// Structure describes the frame information
	struct SFrameInfo
	{
		/// Size of this structure
		oexINT32			lSize;

		/// Pointer to the buffer containing the image data
		oexPVOID			pBuf;

		/// Size of the data in pBuf
		oexINT32			lImageSize;

		/// The frame index
		oexINT64			llFrame;

		/// Image width
		oexINT32			lWidth;

		/// Image height
		oexINT32			lHeight;

		/// Width of the data in one horizontal row of the image
		oexINT32			lScanWidth;
	};

	// Frame callback function type
	typedef oexRESULT (*cbf_OnFrame)( SFrameInfo *pFi, oexPVOID pUser );

public:
	virtual oexBOOL Destroy() = 0;
	virtual oexBOOL Open( oexUINT x_uType, oexCSTR x_pFile, oexINT x_nWidth, oexINT x_nHeight, oexUINT x_uFormat, oexFLOAT x_fFps, oexBOOL x_bInit ) = 0;
	virtual oexBOOL Open( oexUINT x_uType, oexUINT x_uDevice, oexUINT x_uSource, oexINT x_nWidth, oexINT x_nHeight, oexUINT x_uFormat, oexFLOAT x_fFps, oexBOOL x_bInit ) = 0;
	virtual oexBOOL Init() = 0;
	virtual oexBOOL IsOpen() = 0;
	virtual oexBOOL StartCapture() = 0;
	virtual oexBOOL StopCapture() = 0;
	virtual oexBOOL WaitForFrame( oexUINT x_uTimeout ) = 0;
	virtual oexBOOL ReleaseFrame() = 0;
	virtual oexPVOID GetBuffer() = 0;
	virtual oexINT GetBufferSize() = 0;
	virtual oexINT GetImageSize() = 0;
#if defined ( OEX_ENABLE_XIMAGE )
	virtual CImage* GetImage() = 0;
#endif
	virtual oexINT GetWidth() = 0;
	virtual oexINT GetHeight() = 0;
	virtual oexUINT GetFormat() = 0;
	virtual oexFLOAT GetFps() = 0;
	virtual oexINT64 GetFrame() = 0;
	virtual CStr GetSupportedFormats() = 0;
	virtual CStr GetFormatDescription( oexUINT x_uFormat ) = 0;

	/// Returns non-zero once the capture device is up and running
	virtual oexBOOL IsRunning( oexUINT x_uTimeout = 0 )
	{	return os::CResource::waitSuccess == m_sigRunning.Wait( x_uTimeout );
	}

	/// Returns the number of available sub systems
	static oexINT GetAvailableSystems()
	{	return oexSizeOfArray( g_VideoSystemInfo ); }

	/// Returns the number of supported systems
	static oexINT GetSupportedSystems()
	{	oexINT nSupported = 0;
		for( oexINT i = 0; i < GetAvailableSystems(); i++ )
			if ( g_VideoSystemInfo[ i ].bSupported )
				nSupported++;
		return nSupported;
	}

	static oexCONST SVideoSystemInfo* GetSystemInfo( oexINT x_idx )
	{
		if ( x_idx >= GetAvailableSystems() )
			return oexNULL;

		return &g_VideoSystemInfo[ x_idx ];
	}

	static oexCONST SVideoSystemInfo* GetNextAvailableSystem( oexINT x_idx )
	{
		if ( 0 > x_idx )
			x_idx = 0;

		else if ( ++x_idx >= GetAvailableSystems() )
			return oexNULL;

		return &g_VideoSystemInfo[ x_idx ];
	}

	static oexCONST SVideoSystemInfo* GetNextSupportedSystem( oexINT x_idx )
	{
		oexCONST SVideoSystemInfo *pVsi = GetNextAvailableSystem( x_idx );
		while ( oexCHECK_PTR( pVsi ) )
			if ( pVsi->bSupported )
				return pVsi;
			else
				pVsi = GetNextAvailableSystem( pVsi->nIndex );

		return oexNULL;
	}

protected:

	/// Set once the graph is running
	oexSignal			m_sigRunning;

};

//==================================================================
// CCapture
//
/// Generic video capture
/**


*/
//==================================================================
class CCapture : public CCaptureTmpl
{
public:

	static oexCONST oexUINT32		c_Fmt_AUTO	 	= 0;
	static oexCONST oexUINT32		c_Fmt_RGB24 	= oexFOURCC( 'R', 'G', 'B', '3' );
	static oexCONST oexUINT32		c_Fmt_BGR24 	= oexFOURCC( 'B', 'G', 'R', '3' );
	static oexCONST oexUINT32		c_Fmt_YUYV		= oexFOURCC( 'Y', 'U', 'Y', 'V' );

	enum
	{
		// QVGA
		eQVGAWidth = 320,
		eQVGAHeight = 240,

		// VGA
		eVGAWidth = 640,
		eVGAHeight = 480,

		// Standard NTSC video size
		eNTSCWidth = 720,
		eNTSCHeight = 576,

		// Standard PAL video size
		ePALWidth = 768,
		ePALHeight = 576,

		// CIF video size
		eCIFWidth = 352,
		eCIFHeight = 240,

		// 4CIF video size
		e4CIFWidth = 704,
		e4CIFHeight = 576,

		// 9CIF video size
		e9CIFWidth = 1056,
		e9CIFHeight = 864,

		// 16CIF video size
		e16CIFWidth = 1408,
		e16CIFHeight = 1152,

		// 1080i/p video size
		e1080Width = 1920,
		e1080Height = 1080
	};

public:

	/// Contains information about available capture devices
	struct SDeviceInfo
	{
		/// Capture device name
		CStr			sName;

		/// Device id
		oexINT			nId;

		/// Capture device type
		oexINT			nType;

		/// Number of channels
		oexINT			nChannels;
	};

public:

	/// Default constructor
	CCapture();

	/// Destructor
	virtual ~CCapture();

	/// Closes the capture device and releases resources
	virtual oexBOOL Destroy();

	/// Opens the specified capture device
	/**
		\param [in] x_uType		- Capture API to use
		\param [in] x_uDevice	- Index of the capture device to open
		\param [in] x_uSource	- Index of video stream to open
		\param [in] x_nWidth	- Desired video width
		\param [in] x_nHeight	- Desired video height
		\param [in] x_uFormat	- Desired image format
		\param [in] x_fFps		- Desired capture frame rate
		\param [in] x_bInit		- Non-zero to initialize the device for capture

		\return Non-zero if success.
	*/
	virtual oexBOOL Open( oexUINT x_uType, oexUINT x_uDevice, oexUINT x_uSource, oexINT x_nWidth, oexINT x_nHeight, oexUINT x_uFormat, oexFLOAT x_fFps, oexBOOL x_bInit );

	/// Opens the specified file
	/**
		\param [in] x_uType		- Capture API to use
		\param [in] x_uDevice	- Index of the capture device to open
		\param [in] x_uSource	- Index of video stream to open
		\param [in] x_nWidth	- Desired video width
		\param [in] x_nHeight	- Desired video height
		\param [in] x_uFormat	- Desired image format
		\param [in] x_fFps		- Desired capture frame rate
		\param [in] x_bInit		- Non-zero to initialize the device for capture

		\return Non-zero if success.
	*/
	virtual oexBOOL Open( oexUINT x_uType, oexCSTR x_pFile, oexINT x_nWidth, oexINT x_nHeight, oexUINT x_uFormat, oexFLOAT x_fFps, oexBOOL x_bInit );

	/// Initializes the device for capture
	virtual oexBOOL Init()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->Init();
	}

	/// Returns non-zero if a capture device is currently open
	virtual oexBOOL IsOpen()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->IsOpen();
	}

	/// Returns non-zero once the capture device is up and running
	virtual oexBOOL IsRunning( oexUINT x_uTimeout = 0 )
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->IsRunning( x_uTimeout );
	}

	/// Starts video capture
	virtual oexBOOL StartCapture()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->StartCapture();
	}

	/// Stops video capture
	virtual oexBOOL StopCapture()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->StopCapture();
	}

	/// Waits for a new video frame to become available
	virtual oexBOOL WaitForFrame( oexUINT x_uTimeout = 0 )
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->WaitForFrame( x_uTimeout );
	}

	/// Releases current capture frame
	/**
		You must call this after calling WaitForFrame()
	 	when you are done with the video frame.
	*/
	virtual oexBOOL ReleaseFrame()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->ReleaseFrame();
	}

	/// Returns a pointer to the video buffer
	virtual oexPVOID GetBuffer()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->GetBuffer();
	}

	/// Returns the size of the valid image data
	virtual oexINT GetImageSize()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->GetImageSize();
	}

	/// Returns the size of the video buffer
	virtual oexINT GetBufferSize()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->GetBufferSize();
	}

#if defined ( OEX_ENABLE_XIMAGE )

	/// Returns an image object containing the video frame
	virtual CImage* GetImage()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->GetImage();
	}

#endif

	/// Returns the image width
	virtual oexINT GetWidth()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->GetWidth();
	}

	/// Returns the image height
	virtual oexINT GetHeight()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->GetHeight();
	}

	/// Returns the bits-per-pixel of the current image format
	virtual oexUINT GetFormat()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->GetFormat();
	}

	/// Returns the frame rate in frames per second
	virtual oexFLOAT GetFps()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->GetFps();
	}

	/// Returns the current frame index
	virtual oexINT64 GetFrame()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->GetFrame();
	}

	/// Returns a string describing the supported formats
	virtual CStr GetSupportedFormats()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return CStr();

		return m_pDevice->GetSupportedFormats();
	}

	virtual CStr GetFormatDescription( oexUINT x_uFormat )
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return CStr();

		return m_pDevice->GetFormatDescription( x_uFormat );
	}

	/// Returns a list of devices for the specified driver   type
	static oexINT GetDevices( oexUINT x_uType, oex::CPropertyBag *pList );

private:

	/// System specific capture device class
	CCaptureTmpl		*m_pDevice;

	/// The type of capture system being used
	oexUINT				m_uType;

};

