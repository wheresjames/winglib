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

struct SFormatDesc
{
	oexUINT			uId;
	oexTCHAR		sId[ 256 ];
};

#define FORMAT_DESC( s )	{ s, oexT( #s ) }

static SFormatDesc _g_oexvid_formats[] =
{
	FORMAT_DESC( V4L2_PIX_FMT_RGB332 ),
//	FORMAT_DESC( V4L2_PIX_FMT_RGB444 ),
	FORMAT_DESC( V4L2_PIX_FMT_RGB555 ),
	FORMAT_DESC( V4L2_PIX_FMT_RGB565 ),
	FORMAT_DESC( V4L2_PIX_FMT_RGB555X ),
	FORMAT_DESC( V4L2_PIX_FMT_RGB565X ),
	FORMAT_DESC( V4L2_PIX_FMT_BGR24 ),
	FORMAT_DESC( V4L2_PIX_FMT_RGB24 ),
	FORMAT_DESC( V4L2_PIX_FMT_BGR32 ),
	FORMAT_DESC( V4L2_PIX_FMT_RGB32 ),
	FORMAT_DESC( V4L2_PIX_FMT_GREY ),
//	FORMAT_DESC( V4L2_PIX_FMT_PAL8 ),
	FORMAT_DESC( V4L2_PIX_FMT_YVU410 ),
	FORMAT_DESC( V4L2_PIX_FMT_YVU420 ),
	FORMAT_DESC( V4L2_PIX_FMT_YUYV ),
	FORMAT_DESC( V4L2_PIX_FMT_UYVY ),
	FORMAT_DESC( V4L2_PIX_FMT_YUV422P ),
	FORMAT_DESC( V4L2_PIX_FMT_YUV411P ),
	FORMAT_DESC( V4L2_PIX_FMT_Y41P ),
//	FORMAT_DESC( V4L2_PIX_FMT_YUV444 ),
//	FORMAT_DESC( V4L2_PIX_FMT_YUV555 ),
//	FORMAT_DESC( V4L2_PIX_FMT_YUV565 ),
//	FORMAT_DESC( V4L2_PIX_FMT_YUV32 ),
	FORMAT_DESC( V4L2_PIX_FMT_NV12 ),
	FORMAT_DESC( V4L2_PIX_FMT_NV21 ),
	FORMAT_DESC( V4L2_PIX_FMT_YUV410 ),
	FORMAT_DESC( V4L2_PIX_FMT_YUV420 ),
	FORMAT_DESC( V4L2_PIX_FMT_YYUV ),
	FORMAT_DESC( V4L2_PIX_FMT_HI240 ),
//	FORMAT_DESC( V4L2_PIX_FMT_HM12 ),
	FORMAT_DESC( V4L2_PIX_FMT_SBGGR8 ),
	FORMAT_DESC( V4L2_PIX_FMT_MJPEG ),
	FORMAT_DESC( V4L2_PIX_FMT_JPEG ),
	FORMAT_DESC( V4L2_PIX_FMT_DV ),
	FORMAT_DESC( V4L2_PIX_FMT_MPEG ),
	FORMAT_DESC( V4L2_PIX_FMT_WNVA ),
	FORMAT_DESC( V4L2_PIX_FMT_SN9C10X ),
//	FORMAT_DESC( V4L2_PIX_FMT_PWC1 ),
//	FORMAT_DESC( V4L2_PIX_FMT_PWC2 ),
//	FORMAT_DESC( V4L2_PIX_FMT_ET61X251 )
};

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
#ifdef OEX_ARM
		eMaxBuffers = 1
#else
		eMaxBuffers = 8
#endif
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
		m_uFormat = 0;
		m_fFps = 0;
		m_nBufferSize = 0;
		m_pFrameBuffer = 0;
		m_nActiveBuf = 0;
		m_uFormat = 0;
	}

	/// Destructor
	~CV4l2()
	{	Destroy();
	}

	/////////////////////////////////////////////////////////////////
	// Open()
	/////////////////////////////////////////////////////////////////
	/**
		\param [in] x_pDevice	-	Video device name
										- Example: /dev/video0
	*/
	virtual oexBOOL Open( oexUINT x_uType, oexUINT x_uDevice, oexUINT x_uSource, oexINT x_nWidth, oexINT x_nHeight, oexUINT x_uFormat, oexFLOAT x_fFps, oexBOOL x_bInit )
	{
		CStr sDevice = CStr().Fmt( oexT( "/dev/video%d" ), x_uDevice );

		struct stat st;
		if ( 0 > stat( oexStrToMbPtr( sDevice.Ptr() ), &st ) )
		{	oexERROR( errno, CStr().Fmt( oexT( "Device name is invalid : %s" ), oexStrToMbPtr( sDevice.Ptr() ) ) );
			return oexFALSE;
		} // end if

		if ( !S_ISCHR( st.st_mode ) )
		{	oexERROR( -1, CStr().Fmt( oexT( "Device name is invalid : %s" ), oexStrToMbPtr( sDevice.Ptr() ) ) );
			return oexFALSE;
		} // end if

		// Attempt to open the device
		m_nFd = open( oexStrToMbPtr( sDevice.Ptr() ), O_RDWR, 0 );
		if ( 0 > m_nFd )
		{	oexERROR( errno, CStr().Fmt( oexT( "Unable to open device : %s" ), oexStrToMbPtr( sDevice.Ptr() ) ) );
			return oexFALSE;
		} // end if

		// V2 query device capabilities
		if ( -1 == IoCtl( m_nFd, VIDIOC_QUERYCAP, &m_cap2 ) )
		{	oexNOTICE( errno, CStr().Fmt( oexT( "VIDEOC_QUERYCAP : Invalid V4L2 device, %u" ), m_nFd ) );
			return oexFALSE;
		} // end if

		// Ensure it's actually a capture device
		if ( !( m_cap2.capabilities & V4L2_CAP_VIDEO_CAPTURE ) )
		{	oexERROR( -1, CStr().Fmt( oexT( "V4L2_CAP_VIDEO_CAPTURE : Not a capture device : capabilities = %X" ), m_cap2.capabilities ) );
			Destroy();
			return oexFALSE;
		} // end if

		// Save device name
		m_sDeviceName = sDevice;
		m_nWidth = x_nWidth;
		m_nHeight = x_nHeight;
		m_uFormat = x_uFormat;
		m_fFps = x_fFps;

		// Attempt to initialize the device
		if ( x_bInit )
			if ( !Init() )
			{	Destroy();
				return oexFALSE;
			} // end if

		return oexTRUE;
	}

	/// Open file
	oexBOOL Open( oexUINT x_uType, oexCSTR x_pFile, oexINT x_nWidth, oexINT x_nHeight, oexUINT x_uFormat, oexFLOAT x_fFps, oexBOOL x_bInit )
	{
		return oexFALSE;
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
		m_uFormat = 0;
		m_fFps = 0;
		m_nBufferSize = 0;
		m_pFrameBuffer = 0;
		m_nActiveBuf = 0;
		m_uFormat = 0;

		return oexTRUE;
	}

public:

	/// Proper ioctl call
	static int IoCtl( int fd, int request, void * arg )
//	{	int nErr; while ( -1 == ( nErr = ioctl( fd, request, arg ) ) && EINTR == errno ); return nErr; }
	{	int nErr; return oexDO( nErr = ioctl( fd, request, arg ), EINTR == nErr, nErr ); return nErr; }

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

		if ( !m_uFormat )
			m_uFormat = AutoFormat();

		if ( !m_uFormat )
		{	oexERROR( 0, CStr().Fmt( oexT( "Could not find suitable video format : m_nFd = %u" ), m_nFd ) );
			Destroy();
			return oexFALSE;
		} // end if

		v4l2_format fmt;
		oexZeroMemory( &fmt, sizeof( fmt ) );
		fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		fmt.fmt.pix.width = m_nWidth;
		fmt.fmt.pix.height = m_nHeight;
		fmt.fmt.pix.pixelformat = m_uFormat;
//		fmt.fmt.pix.bytesperline = 0;
//		fmt.fmt.pix.sizeimage = 0;
//		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_UYVY;
//		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV;
//		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
//		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR8;
//		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
//		fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
		fmt.fmt.pix.field = V4L2_FIELD_NONE;
//		fmt.fmt.pix.field = V4L2_FIELD_ANY;
//		fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;
//		fmt.fmt.pix.colorspace = V4L2_COLORSPACE_SRGB;

		if ( -1 == IoCtl( m_nFd, VIDIOC_S_FMT, &fmt ) )
		{
			oexINT nErr = errno;

			oexSHOW( fmt.fmt.pix.width );
			oexSHOW( fmt.fmt.pix.height );
			oexPrintf( "%x\n", fmt.fmt.pix.pixelformat );
			oexSHOW( (int)fmt.fmt.pix.field );
			oexSHOW( fmt.fmt.pix.bytesperline );
			oexSHOW( fmt.fmt.pix.sizeimage );
			oexSHOW( fmt.fmt.pix.colorspace );
			oexSHOW( fmt.fmt.pix.priv );

			// Log failure
			oexERROR( nErr, CStr().Fmt( oexT( "VIDIOC_S_FMT : Failed to set format: %u : f=%x w=%d, h=%d" ),
											 m_nFd, (unsigned int)m_uFormat, m_nWidth, m_nHeight ) );

			if ( m_uFormat != fmt.fmt.pix.pixelformat )
				oexERROR( nErr, CStr().Fmt( oexT( "VIDIOC_S_FMT : Unsupported format: %x - suggested type: %x" ),
											 (unsigned int)m_uFormat,
											 (unsigned int)fmt.fmt.pix.pixelformat ) );

			if ( m_nWidth != (oexINT)fmt.fmt.pix.width || m_nHeight != (oexINT)fmt.fmt.pix.height )
				oexERROR( nErr, CStr().Fmt( oexT( "VIDIOC_S_FMT : Unsupported size: %d x %d - suggested size: %d x %d" ),
											 (int)m_nWidth,
											 (int)m_nHeight,
											 (int)fmt.fmt.pix.width,
											 (int)fmt.fmt.pix.height ) );

			Destroy();
			return oexFALSE;
		} // end if

		if ( m_uFormat != fmt.fmt.pix.pixelformat )
		{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_S_FMT : Unsupported format: %x - suggested type: %x" ),
											(unsigned int)m_uFormat,
											(unsigned int)fmt.fmt.pix.pixelformat ) );
			return oexFALSE;
		} // end if

		// Did we get a new width / height
		if ( m_nWidth != (oexINT)fmt.fmt.pix.width || m_nHeight != (oexINT)fmt.fmt.pix.height )
		{
			oexWARNING( 0, CStr().Fmt( oexT( "VIDIOC_S_FMT : Changed video size: %u : %d -> %d x %d -> %d" ),
										   m_nFd, m_nWidth, fmt.fmt.pix.width, m_nHeight, fmt.fmt.pix.height ) );

			// Save new dimensions
			m_nWidth = fmt.fmt.pix.width;
			m_nHeight = fmt.fmt.pix.height;

		} // end if

		m_nBufferSize = fmt.fmt.pix.sizeimage;
		if ( 0 >= m_nBufferSize )
		{	oexERROR( -1, CStr().Fmt( oexT( "Invalid image size %d - %d x %d" ),
						              m_nBufferSize, m_nWidth, m_nHeight ).Ptr() );
			Destroy();
			return oexFALSE;
		} // end if
/*
		v4l2_cropcap cropcap;
		oexZeroMemory( &cropcap, sizeof( cropcap ) );
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
			oexZeroMemory( &crop, sizeof( crop ) );
			crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			crop.c = cropcap.defrect;
//				crop.c = cropcap.bounds;
//				crop.c.left = 0; crop.c.top = 0;
//				crop.c.width = cropcap.bounds.width; crop.c.height = cropcap.bounds.height;
			if ( -1 == IoCtl( m_nFd, VIDIOC_S_CROP, &crop ) )
			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_S_CROP failed" ) ) );
//				return oexFALSE;
			} // end if

		} // end if

//		else
//			oexNOTICE( errno, oexT( "VIDIOC_CROPCAP is not supported" ) );
*/
		v4l2_requestbuffers req;
		oexZeroMemory( &req, sizeof( req ) );
		req.count	= eMaxBuffers;
		req.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory	= V4L2_MEMORY_MMAP;

		if ( -1 == IoCtl( m_nFd, VIDIOC_REQBUFS, &req ) || eMaxBuffers > req.count )
		{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_REQBUFS : Request for mmap buffers failed.  count=%d" ), (int)req.count ) );
			Destroy();
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

//oexSHOW( buf.length );

			// Allocate shared memory
			m_image[ i ].PlainShare( oexTRUE );
			m_image[ i ].SetOffset( buf.m.offset );
			m_image[ i ].SetShareHandle( (os::CFMap::t_HFILEMAP)m_nFd );
			if ( !m_image[ i ].OexNew( buf.length ).Ptr() )
			{	oexERROR( errno, CStr().Fmt( oexT( "Failed to allocate shared image buffer buf.index = %d, size=%d : %d x %d x %d" ), i, m_nBufferSize, m_nWidth, m_nHeight, m_uFormat ) );
				Destroy();
				return oexFALSE;
			} // end if

			// Queue the buffer into the driver
/*			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;
			if ( -1 == IoCtl( m_nFd, VIDIOC_QBUF, &buf ) )
			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_QBUF : Failed : m_nFd = %u" ), m_nFd ) );
				return oexFALSE;
			} // end if
*/
		} // end for

		return oexTRUE;
	}

	virtual oexUINT AutoFormat()
	{
		v4l2_format fmt;
		CStr sStr;
		for ( oexINT i = 0; i < (oexINT)oexSizeOfArray( _g_oexvid_formats ); i++ )
		{
			oexZeroMemory( &fmt, sizeof( fmt ) );
			fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			fmt.fmt.pix.width = 320;
			fmt.fmt.pix.height = 240;
			fmt.fmt.pix.pixelformat = _g_oexvid_formats[ i ].uId;
//			fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

			oexTRY
			{	if ( -1 != IoCtl( m_nFd, VIDIOC_TRY_FMT, &fmt ) )
					if ( _g_oexvid_formats[ i ].uId == fmt.fmt.pix.pixelformat )
						return fmt.fmt.pix.pixelformat;
			} // end try
			oexCATCH_ALL()
			{
			} // end catch

		} // end for

		return 0;
	}

	virtual CStr GetSupportedFormats()
	{
		v4l2_format fmt;
		CStr sStr;
		for ( oexINT i = 0; i < (oexINT)oexSizeOfArray( _g_oexvid_formats ); i++ )
		{
			oexZeroMemory( &fmt, sizeof( fmt ) );
			fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			fmt.fmt.pix.width = 320;
			fmt.fmt.pix.height = 240;
			fmt.fmt.pix.pixelformat = _g_oexvid_formats[ i ].uId;
//			fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

			oexTRY
			{	if ( -1 != IoCtl( m_nFd, VIDIOC_TRY_FMT, &fmt ) )
					if ( _g_oexvid_formats[ i ].uId == fmt.fmt.pix.pixelformat )
						sStr << oexFOURCC_STR( _g_oexvid_formats[ i ].uId ) << oexT( "," );
			} // end try
			oexCATCH_ALL()
			{
			} // end catch

		} // end for

		return sStr;
	}

	virtual CStr GetFormatDescription( oexUINT x_uFormat )
	{
		for ( oexINT i = 0; i < (oexINT)oexSizeOfArray( _g_oexvid_formats ); i++ )
				if ( _g_oexvid_formats[ i ].uId == x_uFormat )
						return CStr().Fmt( "0x%x - ", x_uFormat ) << _g_oexvid_formats[ i ].sId;

		return oexT( "" );
	}

	virtual oexBOOL StartCapture()
	{
		if ( 0 > m_nFd )
			return oexFALSE;

		for( oexINT i = 0; i < eMaxBuffers; i++ )
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
		if ( 0 > m_nFd )
			return oexFALSE;

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
		if ( 0 > m_nFd )
			return oexFALSE;

//		oexEcho( "calling VIDIOC_DQBUF..." );

		v4l2_buffer buf;
		oexZeroMemory( &buf, sizeof( buf ) );
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = m_nActiveBuf;

		errno = 0;
		if ( -1 == IoCtl( m_nFd, VIDIOC_DQBUF, &buf ) || 0 > buf.index || eMaxBuffers <= buf.index )
		{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_DQBUF : Failed : m_nFd = %d, buf.index = %d, buf.flags = %x" ), m_nFd, buf.index, buf.flags ) );
			return oexFALSE;
		} // end if

//		os::CSys::Sleep( 3000 );

//		oexPrintf( "Active Buffer is %d\n", buf.index );

		m_nActiveBuf = buf.index;
		m_nBufferSize = buf.bytesused;

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

		if ( eMaxBuffers <= ++m_nActiveBuf )
			m_nActiveBuf = 0;

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
	{	return CImage::GetScanWidth( m_nWidth, 24 ) * cmn::Abs( m_nHeight ); }

	oexINT GetBufferSize()
	{
		if ( m_nBufferSize )
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
	virtual oexUINT GetFormat()
	{	return m_uFormat; }

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

	oexUINT				m_uFormat;

	oexFLOAT			m_fFps;

	oexINT				m_nBufferSize;

	/// Frame buffer
	oexCHAR				*m_pFrameBuffer;
};


