/*------------------------------------------------------------------
// vfl2.hpp
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


class CV4l2 : public CCaptureTmpl
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
	CV4l2()
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
	~CV4l2()
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
		{	oexERROR( errno, CStr().Fmt( oexT( "Unable to open device : %s" ), oexStrToMbPtr( x_pDevice ) ) );
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
		// V2 query device capabilities
		if ( -1 == IoCtl( m_nFd, VIDIOC_QUERYCAP, &m_cap2 ) )
		{	oexNOTICE( errno, CStr().Fmt( oexT( "VIDEOC_QUERYCAP : Invalid V4L2 device, %u" ), m_nFd ) );
			return oexFALSE;
		} // end if

		// Ensure it's actually a capture device
		if ( !( m_cap2.capabilities & V4L2_CAP_VIDEO_CAPTURE ) )
		{	oexERROR( -1, CStr().Fmt( oexT( "V4L2_CAP_VIDEO_CAPTURE : Not a capture device : capabilities = %X" ), m_cap2.capabilities ) );
			return oexFALSE;
		} // end if

		// +++ Detect io mode if eIoAuto / not sure what the prefered order should be.
		//     Should probably do some performance testing...
		if ( eIoAuto == m_nIoMode )
		{
			if ( ( m_cap2.capabilities & V4L2_CAP_STREAMING ) )
				m_nIoMode = eIoStreaming;

			else if ( ( m_cap2.capabilities & V4L2_CAP_READWRITE ) )
				m_nIoMode = eIoReadWrite;

			else if ( ( m_cap2.capabilities & V4L2_CAP_ASYNCIO ) )
				m_nIoMode = eIoAsync;

			else
				oexERROR( -1, CStr().Fmt( oexT( "V4L2_CAP_VIDEO_CAPTURE : No supportd I/O methods. cap=0x%X" ), (int)m_cap2.capabilities ) );

		} // end if

		// Verify image transfer mode is supported
		else switch( m_nIoMode )
		{
			case eIoReadWrite :
				if ( !( m_cap2.capabilities & V4L2_CAP_READWRITE ) )
				{	oexERROR( -1, CStr().Fmt( oexT( "V4L2_CAP_READWRITE : Read/Write IO not supported" ) ) );
					return oexFALSE;
				} // end if

				break;

			case eIoAsync :
				if ( !( m_cap2.capabilities & V4L2_CAP_ASYNCIO ) )
				{	oexERROR( -1, CStr().Fmt( oexT( "V4L2_CAP_ASYNCIO : Async IO not supported" ) ) );
					return oexFALSE;
				} // end if

				oexERROR( -1, CStr().Fmt( oexT( "V4L2_CAP_ASYNCIO : +++ Not Implemented" ) ) );
				return oexFALSE;

				break;

			case eIoStreaming :
				if ( !( m_cap2.capabilities & V4L2_CAP_STREAMING ) )
				{	oexERROR( -1, CStr().Fmt( oexT( "V4L2_CAP_STREAMING : Stream based IO not supported" ) ) );
					return oexFALSE;
				} // end if

				break;

			default :
				oexERROR( -1, CStr().Fmt( oexT( "Invalid mode" ) ) );
				return oexFALSE;
				break;

		} // end switch

		// Set the input channel (often fails as not all devices support this)
		int input = 0;
		if ( -1 == IoCtl ( m_nFd, VIDIOC_S_INPUT, &input ) )
			oexNOTICE( errno, CStr().Fmt( oexT( "VIDIOC_S_INPUT : Failed : m_nFd = %u" ), m_nFd ) );

		// Set auto video stream

		// VPFE_CMD_RESIZE
		#define VPFE_STD_AUTO ((v4l2_std_id)(0x1000000000000000ULL))
//		#define VPFE_STD_AUTO_SQP ((v4l2_std_id)(0x2000000000000000ULL))
		v4l2_std_id std = VPFE_STD_AUTO;
		if ( -1 == IoCtl( m_nFd, VIDIOC_S_STD, &std ) )
		{	oexNOTICE( errno, CStr().Fmt( oexT( "VIDIOC_S_STD : Failed : m_nFd = %u" ), m_nFd ) );
//			return oexFALSE;
		} // end if

		v4l2_format fmt;
		oexZeroMemory( &fmt, sizeof( fmt ) );
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width = m_nWidth;
		fmt.fmt.pix.height = m_nHeight;
//		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR8; //V4L2_PIX_FMT_RGB24; // V4L2_PIX_FMT_YUYV
		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
		fmt.fmt.pix.field = V4L2_FIELD_NONE;
//		fmt.fmt.pix.field = V4L2_FIELD_ANY;
//		fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

		if ( -1 == IoCtl( m_nFd, VIDIOC_S_FMT, &fmt ) )
		{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_S_FMT : Failed to set format: %u : w=%d, h=%d" ), m_nFd, m_nWidth, m_nHeight ) );
			return oexFALSE;
		} // end if

//			if ( V4L2_PIX_FMT_RGB24 != fmt.fmt.pix.pixelformat )
//			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_S_FMT : Unsupporeted format type: %d" ), (int)fmt.fmt.pix.pixelformat ) );
//				return oexFALSE;
//			} // end if

		// Did we get a new width / height
		if ( m_nWidth != fmt.fmt.pix.width || m_nHeight != fmt.fmt.pix.height )
		{
			oexWARNING( 0, CStr().Fmt( oexT( "VIDIOC_S_FMT : Changed video size: %u : %d -> %d x %d -> %d" ),
										   m_nFd, m_nWidth, fmt.fmt.pix.width, m_nHeight, fmt.fmt.pix.height ) );

			// Save new dimensions
			m_nWidth = fmt.fmt.pix.width;
			m_nHeight = fmt.fmt.pix.height;

		} // end if

		m_nBufferSize = fmt.fmt.pix.sizeimage;
		if ( 0 >= m_nBufferSize )
		{	oexERROR( -1, CStr().Fmt( oexT( "Invalid image size %d" ), m_nBufferSize ).Ptr() );
			return oexFALSE;
		} // end if

		v4l2_cropcap cropcap;
		cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if ( 0 == IoCtl( m_nFd, VIDIOC_CROPCAP, &cropcap ) )
		{
			oexSHOWVAL( cropcap.defrect.left, "%d - " );
			oexSHOWVAL( cropcap.defrect.top, "%d - " );
			oexSHOWVAL( cropcap.defrect.width, "%d - " );
			oexSHOWVAL( cropcap.defrect.height, "%d\n" );

			oexSHOWVAL( cropcap.bounds.left, "%d - " );
			oexSHOWVAL( cropcap.bounds.top, "%d - " );
			oexSHOWVAL( cropcap.bounds.width, "%d - " );
			oexSHOWVAL( cropcap.bounds.height, "%d\n" );

			v4l2_crop crop;
			crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			crop.c = cropcap.defrect;
//				crop.c = cropcap.bounds;
//				crop.c.left = 0; crop.c.top = 0;
//				crop.c.width = cropcap.bounds.width; crop.c.height = cropcap.bounds.height;
			if ( -1 == IoCtl( m_nFd, VIDIOC_S_CROP, &crop ) )
			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_S_CROP failed" ) ) );
				return oexFALSE;
			} // end if

		} // end if

		else
			oexNOTICE( errno, oexT( "VIDIOC_CROPCAP is not supported" ) );

		v4l2_requestbuffers req;
		oexZeroMemory( &req, sizeof( req ) );
		req.count	= eMaxBuffers;
		req.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory	= V4L2_MEMORY_MMAP;

		if ( -1 == IoCtl( m_nFd, VIDIOC_REQBUFS, &req ) || eMaxBuffers > req.count )
		{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_REQBUFS : Request for mmap buffers failed.  count=%d" ), (int)req.count ) );
			return oexFALSE;
		} // end if

		for( oexINT i = 0; i < eMaxBuffers; i++ )
		{
			v4l2_buffer buf;
			oexZeroMemory( &buf, sizeof( buf ) );

			buf.type 		= V4L2_BUF_TYPE_VIDEO_CAPTURE;
//			buf.type        = req.type;
			buf.memory      = V4L2_MEMORY_MMAP;
			buf.index       = i;

			if ( -1 == IoCtl( m_nFd, VIDIOC_QUERYBUF, &buf ) )
			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_QUERYBUF : Request for buffer info failed.  count=%d" ), (int)req.count ) );
				return oexFALSE;
			} // end if

			// Buffer size
//			if ( !i )
//				m_nBufferSize = buf.length;

			// Allocate shared memory
			m_image[ i ].PlainShare( oexTRUE );
			m_image[ i ].SetOffset( buf.m.offset );
			m_image[ i ].SetShareHandle( (os::CFMap::t_HFILEMAP)m_nFd );
			if ( !m_image[ i ].OexNew( buf.length ).Ptr() )
			{	oexERROR( errno, CStr().Fmt( oexT( "Failed to allocate shared image buffer buf.index = %d, size=%d : %d x %d x %d" ), i, m_nBufferSize, m_nWidth, m_nHeight, m_nBpp ) );
				return oexFALSE;
			} // end if

			// Queue the buffer into the driver
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;
			if ( -1 == IoCtl( m_nFd, VIDIOC_QBUF, &buf ) )
			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_QBUF : Failed : m_nFd = %u" ), m_nFd ) );
				return oexFALSE;
			} // end if

		} // end for

		return oexTRUE;
	}

	virtual CStr GetSupportedFormats()
	{
		struct SFormatDesc
		{
			oexINT			nId;
			oexTCHAR		sId[ 256 ];
		};

		#define FORMAT_DESC( s )	{ s, oexT( #s ) }

		static SFormatDesc l_formats[] =
		{
			FORMAT_DESC( V4L2_PIX_FMT_RGB332 ),
			FORMAT_DESC( V4L2_PIX_FMT_RGB444 ),
			FORMAT_DESC( V4L2_PIX_FMT_RGB555 ),
			FORMAT_DESC( V4L2_PIX_FMT_RGB565 ),
			FORMAT_DESC( V4L2_PIX_FMT_RGB555X ),
			FORMAT_DESC( V4L2_PIX_FMT_RGB565X ),
			FORMAT_DESC( V4L2_PIX_FMT_BGR24 ),
			FORMAT_DESC( V4L2_PIX_FMT_RGB24 ),
			FORMAT_DESC( V4L2_PIX_FMT_BGR32 ),
			FORMAT_DESC( V4L2_PIX_FMT_RGB32 ),
			FORMAT_DESC( V4L2_PIX_FMT_GREY ),
//				FORMAT_DESC( V4L2_PIX_FMT_PAL8 ),
			FORMAT_DESC( V4L2_PIX_FMT_YVU410 ),
			FORMAT_DESC( V4L2_PIX_FMT_YVU420 ),
			FORMAT_DESC( V4L2_PIX_FMT_YUYV ),
			FORMAT_DESC( V4L2_PIX_FMT_UYVY ),
			FORMAT_DESC( V4L2_PIX_FMT_YUV422P ),
			FORMAT_DESC( V4L2_PIX_FMT_YUV411P ),
			FORMAT_DESC( V4L2_PIX_FMT_Y41P ),
//				FORMAT_DESC( V4L2_PIX_FMT_YUV444 ),
//				FORMAT_DESC( V4L2_PIX_FMT_YUV555 ),
//				FORMAT_DESC( V4L2_PIX_FMT_YUV565 ),
//				FORMAT_DESC( V4L2_PIX_FMT_YUV32 ),
			FORMAT_DESC( V4L2_PIX_FMT_NV12 ),
			FORMAT_DESC( V4L2_PIX_FMT_NV21 ),
			FORMAT_DESC( V4L2_PIX_FMT_YUV410 ),
			FORMAT_DESC( V4L2_PIX_FMT_YUV420 ),
			FORMAT_DESC( V4L2_PIX_FMT_YYUV ),
			FORMAT_DESC( V4L2_PIX_FMT_HI240 ),
			FORMAT_DESC( V4L2_PIX_FMT_HM12 ),
			FORMAT_DESC( V4L2_PIX_FMT_SBGGR8 ),
			FORMAT_DESC( V4L2_PIX_FMT_MJPEG ),
			FORMAT_DESC( V4L2_PIX_FMT_JPEG ),
			FORMAT_DESC( V4L2_PIX_FMT_DV ),
			FORMAT_DESC( V4L2_PIX_FMT_MPEG ),
			FORMAT_DESC( V4L2_PIX_FMT_WNVA ),
			FORMAT_DESC( V4L2_PIX_FMT_SN9C10X ),
			FORMAT_DESC( V4L2_PIX_FMT_PWC1 ),
			FORMAT_DESC( V4L2_PIX_FMT_PWC2 ),
			FORMAT_DESC( V4L2_PIX_FMT_ET61X251 )
		};

		v4l2_format fmt;
		CStr sStr;
		for ( oexINT i = 0; i < oexSizeofArray( l_formats ); i++ )
		{
			oexZeroMemory( &fmt, sizeof( fmt ) );
			fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			fmt.fmt.pix.width = m_nWidth;
			fmt.fmt.pix.height = m_nHeight;
			fmt.fmt.pix.pixelformat = l_formats[ i ].nId;
			fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

			try
			{	if ( !IoCtl( m_nFd, VIDIOC_S_FMT, &fmt ) )
					sStr += CStr().Fmt( oexT( "%s : (0x%X)\n" ), oexStrToMbPtr( l_formats[ i ].sId ), l_formats[ i ].nId );
			} // end try
			catch( ... )
			{
			} // end catch

		} // end for

		return sStr;
	}

	virtual oexBOOL StartCapture()
	{
/*		for( oexINT i = 0; i < eMaxBuffers; i++ )
		{
			v4l2_buffer buf;
			oexZeroMemory( &buf, sizeof( buf ) );
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;
			if ( -1 == IoCtl( m_nFd, VIDIOC_QBUF, &buf ) )
			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_QBUF : Failed : m_nFd = %u" ), m_nFd ) );
				return oexFALSE;
			} // end if

		} // end for
*/
		int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if ( -1 == IoCtl ( m_nFd, VIDIOC_STREAMON, &type ) )
		{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_STREAMON : Failed : m_nFd = %u" ), m_nFd ) );
			return oexFALSE;
		} // end if

		return oexTRUE;
	}

	/// Stops video capture
	virtual oexBOOL StopCapture()
	{
		int type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if ( -1 == IoCtl ( m_nFd, VIDIOC_STREAMOFF, &type ) )
		{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_STREAMOFF : Failed : m_nFd = %u" ), m_nFd ) );
			return oexFALSE;
		} // end if

		return oexTRUE;
	}

	/// Waits for a new frame of video
	oexBOOL WaitForFrame( oexUINT x_uTimeout = 0 )
	{
//		os::CSys::printf( "calling VIDIOC_DQBUF...\n" );

		v4l2_buffer buf;
		oexZeroMemory( &buf, sizeof( buf ) );
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;

		if ( -1 == IoCtl( m_nFd, VIDIOC_DQBUF, &buf ) || eMaxBuffers >= buf.index )
		{
			// +++ This failure seems to be unreliable
			oexNOTICE( errno, CStr().Fmt( oexT( "VIDIOC_DQBUF : Failed : m_nFd = %d, buf.index = %d, buf.flags = %x" ), m_nFd, buf.index, buf.flags ) );
//			return oexFALSE;
		} // end if

		else
			os::CSys::printf( "VIDIOC_DQBUF actually succeeded - errno=%d\n" );

//		os::CSys::Sleep( 3000 );

//		os::CSys::printf( "Active Buffer is %d\n", buf.index );

		m_nActiveBuf = buf.index;

/*			if ( eIoReadWrite == m_nIoMode )
		{
			oexINT nImageSize = GetBufferSize();
			if ( 0 >= nImageSize )
			{	oexERROR( -1, CStr().Fmt( oexT( "Invalid image size %d" ), nImageSize ).Ptr() );
				return oexFALSE;
			} // end if

			if ( !m_image.Ptr() || m_image.Size() != GetImageSize() )
			{
				if ( !m_image.OexNew( GetImageSize() ).Ptr() )
				{	oexERROR( -1, CStr().Fmt( oexT( "Unable to allocate image buffer : size=%d" ), nImageSize ).Ptr() );
					return oexFALSE;
				} // end if

			} // end if

			// Attempt to read image data
			if ( -1 == read( m_nFd, m_image.Ptr(), nImageSize )
				 && EAGAIN != errno && EIO != errno )
			{	oexERROR( errno, CStr().Fmt( oexT( "read() failed! : fp=%d, buffer=0x%X, size=%d" ), (int)m_nFd, (int)m_image.Ptr(), (int)nImageSize ).Ptr() );
				return oexFALSE;
			} // end if

		} // end if

		// Memory mapped
		else if ( eIoAsync == m_nIoMode )
		{

		} // end else if
*/
		return oexTRUE;
	}

	virtual oexBOOL ReleaseFrame()
	{
		v4l2_buffer buf;
		oexZeroMemory( &buf, sizeof( buf ) );
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = m_nActiveBuf;
		if ( -1 == IoCtl( m_nFd, VIDIOC_QBUF, &buf ) )
		{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_QBUF : Failed : m_nFd = %u" ), m_nFd ) );
			return oexFALSE;
		} // end if

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

	oexINT GetBufferSize()
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
	virtual oexINT GetHeight()
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
	v4l2_capability		m_cap2;

	/// Video buffer 1 format
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


