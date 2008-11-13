/*------------------------------------------------------------------
// vfl1.hpp
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

class CV4l1 : public CCaptureTmpl
{
public:

	enum
	{
		// Automatic IO type selection
		eIoAuto,

		// use read/write system calls
		eIoReadWrite,

		// Async IO
		eIoAsync,

		// streaming IO ioctls
		eIoStreaming
	};

	enum
	{
		eMaxBuffers = 2
	};

public:

	/// Default contructor
	CV4l1()
	{
		m_nFd = -1;
		m_nIoMode = eIoAuto;
		m_llFrame = 0;
		m_nWidth = 0;
		m_nHeight = 0;
		m_nBpp = 24;
		m_fFps = 0;
		m_nBufferSize = 0;
		m_pFrameBuffer = 0;
		m_nActiveBuf = 0;
	}

	/// Destructor
	~CV4l1()
	{
		Destroy();
	}

	/////////////////////////////////////////////////////////////////
	// Open()
	/////////////////////////////////////////////////////////////////
	/**
		\param [in] x_pDevice	-	Video device name
										- Example: /dev/video0
	*/
	virtual oexBOOL Open( oexUINT x_uType, oexCSTR x_pDevice, oexINT x_nWidth, oexINT x_nHeight, oexINT x_nBpp, oexFLOAT x_fFps )
	{
		// Sanity checks
		if ( !oexCHECK_PTR( x_pDevice ) || !*x_pDevice )
		{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device name" ) ) );
			return oexFALSE;
		} // end if

		struct stat st;
		if ( 0 > stat( oexStrToMbPtr( x_pDevice ), &st ) )
		{	oexERROR( errno, CStr().Fmt( oexT( "Device name is invalid : %s" ), oexStrToMbPtr( x_pDevice ) ) );
			return oexFALSE;
		} // end if

		if ( !S_ISCHR( st.st_mode ) )
		{	oexERROR( -1, CStr().Fmt( oexT( "Device name is invalid : %s" ), oexStrToMbPtr( x_pDevice ) ) );
			return oexFALSE;
		} // end if

		// Attempt to open the device
		m_nFd = open( oexStrToMbPtr( x_pDevice ), O_RDWR | O_NONBLOCK, 0 ); // | O_NONBLOCK, 0 );
		if ( 0 > m_nFd )
		{	oexERROR( errno, CStr().Fmt( oexT( "Unable to open file : %s" ), oexStrToMbPtr( x_pDevice ) ) );
			return oexFALSE;
		} // end if

		// Save device name
		m_sDeviceName = x_pDevice;
		m_nWidth = x_nWidth;
		m_nHeight = x_nHeight;
		m_nBpp = x_nBpp;
		m_fFps = x_fFps;

		// Attempt to initialize the device
		if ( !Init() )
		{	Destroy();
			return oexFALSE;
		} // end if

		return oexTRUE;
	}

	/////////////////////////////////////////////////////////////////
	// Destroy()
	/////////////////////////////////////////////////////////////////
	/// Closes any open capture device
	virtual oexBOOL Destroy()
	{
		if ( 0 > m_nFd )
			return oexTRUE;

		// Close the file
		close( m_nFd );

		// Lose the image buffer(s)
		for( oexINT i = 0; i < eMaxBuffers; i++ )
			m_image[ i ].Destroy();

		m_nFd = -1;
		m_llFrame = 0;
		m_nWidth = 0;
		m_nHeight = 0;
		m_nBpp = 24;
		m_fFps = 0;
		m_nBufferSize = 0;
		m_pFrameBuffer = 0;
		m_nActiveBuf = 0;
	}

public:

	/// Proper ioctl call
	static int IoCtl( int fd, int request, void * arg )
	{	int nErr; return oexDO( nErr = ioctl( fd, request, arg ), EINTR == nErr, nErr ); }

public:

	/////////////////////////////////////////////////////////////////
	// IsOpen()
	/////////////////////////////////////////////////////////////////
	/// Returns non-zero if a capture device is open
	virtual oexBOOL IsOpen()
	{	return 0 <= m_nFd; }

	/////////////////////////////////////////////////////////////////
	// Init()
	/////////////////////////////////////////////////////////////////
	/// Initializes the device
	/**
	*/
	virtual oexBOOL Init()
	{
		// Initialize the structure
		oexZeroMemory( &m_cap1, sizeof( m_cap1 ) );

		// V1 Device?
		if ( -1 == IoCtl( m_nFd, VIDIOCGCAP, &m_cap1 ) )
			oexWARNING( errno, CStr().Fmt( oexT( "VIDIOCGCAP : Failed! fd=%u, assuming V4L1 device and continuing." ), m_nFd ) );

		else
		{
			// Ensure it's a capture device
			if ( !( m_cap1.type & VID_TYPE_CAPTURE ) )
			{	oexERROR( -1, CStr().Fmt( oexT( "VID_TYPE_CAPTURE : Not a capture device : type = %X" ), m_cap1.type ) );
				return oexFALSE;
			} // end if

		} // end else

		m_buf1.size = 1 * GetImageSize();
		m_buf1.frames = 1;
		m_buf1.offsets[ 0 ] = 0;

		// Set video buffer
		if ( -1 == IoCtl( m_nFd, VIDIOCGMBUF, &m_buf1 ) )
			oexWARNING( errno, CStr().Fmt( oexT( "VIDIOCGMBUF : Failed! Unable to set video buffer, fd=%u, ignoring error" ), m_nFd ) );

		oexINT lImageSize = GetImageSize();
		if ( 0 >= lImageSize )
		{	oexERROR( errno, CStr().Fmt( oexT( "Invalid Image size %d : %d x %d x %d" ), lImageSize, m_nWidth, m_nHeight, m_nBpp ) );
			return oexFALSE;
		} // end if

		// Allocate shared memory
		m_image[ 0 ].PlainShare( oexTRUE );
		m_image[ 0 ].SetShareHandle( (os::CFMap::t_HFILEMAP)m_nFd );
		if ( !oexCHECK_PTR( m_image[ 0 ].OexNew( lImageSize ).Ptr() ) )
			oexERROR( errno, CStr().Fmt( oexT( "Failed to allocate shared image buffer size=%d : %d x %d x %d" ), lImageSize, m_nWidth, m_nHeight, m_nBpp ) );

		return oexTRUE;
	}

	virtual CStr GetSupportedFormats()
	{
		return CStr();
	}

	virtual oexBOOL StartCapture()
	{
		if ( eIoAsync == m_nIoMode )
		{
			video_mmap vm;
			vm.frame = 0;
			vm.width = m_nWidth;
			vm.height = m_nHeight;
			vm.format = VIDEO_PALETTE_RGB24;

			try
			{
				if ( -1 == IoCtl( m_nFd, VIDIOCMCAPTURE, &vm ) )
				{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOCMCAPTURE : Capture frame failed : %u" ), m_nFd ) );
					return oexFALSE;
				} // end if
			} // end try
			catch( ... )
			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOCMCAPTURE : Exception occured!  Capture frame failed : %u" ), m_nFd ) );
				return oexFALSE;
			} // end catch

		} // end if

		return oexTRUE;
	}

	/// Stops video capture
	virtual oexBOOL StopCapture()
	{
		return oexTRUE;
	}

	/// Waits for a new frame of video
	virtual oexBOOL WaitForFrame( oexUINT x_uTimeout = 0 )
	{
		oexINT nFrame = 0;
		if ( -1 == IoCtl( m_nFd, VIDIOCSYNC, &nFrame ) )
		{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOCSYNC : Failed to wait for frame sync, %u" ), m_nFd ) );
			return oexFALSE;
		} // end if

		// Count a frame
		m_llFrame++;

		return oexTRUE;
	}

	virtual oexBOOL ReleaseFrame()
	{
		return oexTRUE;
	}

	/// Returns a pointer to the video buffer
	virtual oexPVOID GetBuffer()
	{
		if ( m_pFrameBuffer )
			return m_pFrameBuffer;

		return m_image[ m_nActiveBuf ].Ptr();
	}

	/// +++ Should return the size of the video buffer
	virtual oexINT GetImageSize()
	{	return CImage::GetScanWidth( m_nWidth, m_nBpp ) * cmn::Abs( m_nHeight ); }

	virtual oexINT GetBufferSize()
	{
		if ( m_pFrameBuffer )
			return m_nBufferSize;
		return GetImageSize();
	}

	/// Returns an image object
	virtual CImage* GetImage()
	{	return oexNULL; }

	/// Returns the image width
	virtual oexINT GetWidth()
	{	return m_nWidth; }

	/// Returns the image height
	oexINT GetHeight()
	{	return m_nHeight; }

	/// Returns the bits-per-pixel of the current image format
	virtual oexINT GetBpp()
	{	return m_nBpp; }

	/// Returns the frame rate in frames per second
	virtual oexFLOAT GetFps()
	{	return m_fFps; }

	/// Returns the current frame index
	virtual oexINT64 GetFrame()
	{	return m_llFrame; }

private:

	/// Name of the device
	CStr				m_sDeviceName;

	/// Device file descriptor
	oexINT				m_nFd;

	/// IO method
	oexINT				m_nIoMode;

	/// The current frame number
	oexINT64			m_llFrame;

	/// Device capabilities
	video_capability	m_cap1;

	/// Video buffer format
	video_mbuf			m_buf1;

	/// Image memory
	TMem< oexCHAR >		m_image[ eMaxBuffers ];

	/// Active buffer
	oexINT				m_nActiveBuf;

	oexINT				m_nWidth;

	oexINT				m_nHeight;

	oexINT				m_nBpp;

	oexFLOAT			m_fFps;

	oexINT				m_nBufferSize;

	/// Frame buffer
	oexCHAR				*m_pFrameBuffer;
};


