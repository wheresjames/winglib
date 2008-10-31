/*------------------------------------------------------------------
// capture.cpp
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

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;
using namespace OEX_NAMESPACE::vid;

class CV4lCapture
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
		// Capture buffer to use
		eCaptureBuffer = 0
	};

public:

	/// Default contructor
	CV4lCapture()
	{
		m_nFd = -1;
		m_nVersion = 0;
		m_nIoMode = eIoAuto;
		m_llFrame = 0;
		m_nWidth = 0;
		m_nHeight = 0;
		m_nBpp = 24;
		m_fFps = 0;
		m_nBufferSize = 0;
	}

	/// Destructor
	~CV4lCapture()
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
	oexBOOL Open( oexCSTR x_pDevice, oexINT x_nWidth, oexINT x_nHeight, oexINT x_nBpp, oexFLOAT x_fFps )
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
		m_nFd = open( oexStrToMbPtr( x_pDevice ), O_RDWR, 0 ); // | O_NONBLOCK, 0 );
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
	oexBOOL Destroy()
	{
		if ( 0 > m_nFd )
			return oexTRUE;

		// Close the file
		close( m_nFd );

		// Lose the image buffer
		m_image.Destroy();

		m_nFd = -1;
		m_nVersion = 0;
		m_llFrame = 0;
		m_nWidth = 0;
		m_nHeight = 0;
		m_nBpp = 24;
		m_fFps = 0;
		m_nBufferSize = 0;
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
	oexBOOL IsOpen()
	{	return 0 <= m_nFd; }

	/////////////////////////////////////////////////////////////////
	// Init()
	/////////////////////////////////////////////////////////////////
	/// Initializes the device
	/**
	*/
	oexBOOL Init()
	{
		// V2 query device capabilities
#ifndef OEX_VFL1
		if ( -1 == IoCtl( m_nFd, VIDIOC_QUERYCAP, &m_cap2 ) )
#else
		if ( 1 )
#endif
		{
			// Log V2 failure
			oexNOTICE( errno, CStr().Fmt( oexT( "VIDEOC_QUERYCAP : Invalid V4L2 device, %u" ), m_nFd ) );

			// Version 1
			m_nVersion = 1;

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
			m_image.PlainShare( oexTRUE );
			m_image.SetShareHandle( (os::CFMap::t_HFILEMAP)m_nFd );
			if ( !m_image.OexNew( lImageSize ).Ptr() )
			{	oexERROR( errno, CStr().Fmt( oexT( "Failed to allocate shared image buffer size=%d : %d x %d x %d" ), lImageSize, m_nWidth, m_nHeight, m_nBpp ) );
				m_nIoMode = eIoReadWrite;
			} // end if

			else
				m_nIoMode = eIoAsync;

		} // end if

#ifndef OEX_VFL1
		else
		{
			// Version 2
			m_nVersion = 2;

			// Ensure it's actually a capture device
			if ( !( m_cap2.capabilities & V4L2_CAP_VIDEO_CAPTURE ) )
			{	oexERROR( -1, CStr().Fmt( oexT( "V4L2_CAP_VIDEO_CAPTURE : Not a capture device : capabilities = %X" ), m_cap2.capabilities ) );
				return oexFALSE;
			} // end if

			// +++ Detect io mode if eIoAuto / not sure what the prefered order should be.
			//     Should probably do some performance testing...
			if ( eIoAuto == m_nIoMode )
			{
				if ( ( m_cap2.capabilities & V4L2_CAP_READWRITE ) )
					m_nIoMode = eIoReadWrite;

				else if ( ( m_cap2.capabilities & V4L2_CAP_ASYNCIO ) )
					m_nIoMode = eIoAsync;

				else if ( ( m_cap2.capabilities & V4L2_CAP_STREAMING ) )
					m_nIoMode = eIoStreaming;

				else
					oexERROR( -1, CStr().Fmt( oexT( "V4L2_CAP_VIDEO_CAPTURE : No supportd I/O methods. cap=0x%X" ), (int)m_cap2.capabilities ) );

			} // end if

			// Verify image transfer mode is supported
			switch( m_nIoMode )
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

		} // end if
#endif

		return oexTRUE;
	}

	CStr GetSupportedFormats()
	{
		if ( 1 == m_nVersion )
		{
		} // end else if

		else if ( 2 == m_nVersion )
		{
			struct SFormatDesc
			{
				oexINT			nId;
				oexTCHAR		sId[ 256 ];
			};

			#define FORMAT_DESC( s )	{ s, #s }

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
				FORMAT_DESC( V4L2_PIX_FMT_PAL8 ),
				FORMAT_DESC( V4L2_PIX_FMT_YVU410 ),
				FORMAT_DESC( V4L2_PIX_FMT_YVU420 ),
				FORMAT_DESC( V4L2_PIX_FMT_YUYV ),
				FORMAT_DESC( V4L2_PIX_FMT_UYVY ),
				FORMAT_DESC( V4L2_PIX_FMT_YUV422P ),
				FORMAT_DESC( V4L2_PIX_FMT_YUV411P ),
				FORMAT_DESC( V4L2_PIX_FMT_Y41P ),
				FORMAT_DESC( V4L2_PIX_FMT_YUV444 ),
				FORMAT_DESC( V4L2_PIX_FMT_YUV555 ),
				FORMAT_DESC( V4L2_PIX_FMT_YUV565 ),
				FORMAT_DESC( V4L2_PIX_FMT_YUV32 ),
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

		} // end else if

		return CStr();
	}

	oexBOOL StartCapture()
	{
		// v4l version 1
		if ( 1 == m_nVersion )
		{
			if ( eIoAsync == m_nIoMode )
			{
				video_mmap vm;
				vm.frame = eCaptureBuffer;
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

		} // end if

		else if ( 2 == m_nVersion )
		{
	        v4l2_format fmt;
	        oexZeroMemory( &fmt, sizeof( fmt ) );
			fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			fmt.fmt.pix.width = m_nWidth;
			fmt.fmt.pix.height = m_nHeight;
//			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_SBGGR8; //V4L2_PIX_FMT_RGB24; // V4L2_PIX_FMT_YUYV
			fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_GREY;
			fmt.fmt.pix.field = V4L2_FIELD_NONE; // V4L2_FIELD_INTERLACED;

			printf( "1\n" );

			if ( -1 == IoCtl( m_nFd, VIDIOC_S_FMT, &fmt ) )
			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_S_FMT : Failed to set format: %u : w=%d, h=%d" ), m_nFd, m_nWidth, m_nHeight ) );
				return oexFALSE;
			} // end if

			printf( "2\n" );

//			if ( V4L2_PIX_FMT_RGB24 != fmt.fmt.pix.pixelformat )
//			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_S_FMT : Unsupporeted format type: %d" ), (int)fmt.fmt.pix.pixelformat ) );
//				return oexFALSE;
//			} // end if

			printf( "3\n" );

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

			printf( "4\n" );

			v4l2_requestbuffers req;
			oexZeroMemory( &req, sizeof( req ) );
			req.count	= 1;
			req.type	= V4L2_BUF_TYPE_VIDEO_CAPTURE;
			req.memory	= V4L2_MEMORY_MMAP;

			if ( -1 == IoCtl( m_nFd, VIDIOC_REQBUFS, &req ) || 1 > req.count )
			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_REQBUFS : Request for mmap buffers failed.  count=%d" ), (int)req.count ) );
				return oexFALSE;
			} // end if

			printf( "5\n" );

			v4l2_buffer buf;
			oexZeroMemory( &buf, sizeof( buf ) );

			buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory      = V4L2_MEMORY_MMAP;
			buf.index       = 0;

			if ( -1 == IoCtl( m_nFd, VIDIOC_QUERYBUF, &buf ) )
			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOC_QUERYBUF : Request for buffer info failed.  count=%d" ), (int)req.count ) );
				return oexFALSE;
			} // end if

			printf( "6\n" );
/*
			m_nBufferSize = buf.length;
			m_pFrameBuffer = (oexCHAR*) mmap(	oexNULL, m_nBufferSize,
												PROT_READ | PROT_WRITE, MAP_SHARED,
												(int)m_nFd, buf.m.offset );

			if ( MAP_FAILED == m_pFrameBuffer )
			{	oexERROR( errno, CStr().Fmt( oexT( "Failed to allocate shared image buffer size=%d : %d x %d x %d" ), m_nBufferSize, m_nWidth, m_nHeight, m_nBpp ) );
				return oexFALSE;
			} // end if
*/
			// Buffer size
			m_nBufferSize = buf.length;
//			printf( oexT( "offset = %d\n" ), (int)buf.m.offset );

			// Allocate shared memory
			m_image.PlainShare( oexTRUE );
			m_image.SetOffset( buf.m.offset );
			m_image.SetShareHandle( (os::CFMap::t_HFILEMAP)m_nFd );
			if ( !m_image.OexNew( m_nBufferSize ).Ptr() )
			{	oexERROR( errno, CStr().Fmt( oexT( "Failed to allocate shared image buffer size=%d : %d x %d x %d" ), m_nBufferSize, m_nWidth, m_nHeight, m_nBpp ) );
				return oexFALSE;
			} // end if

			printf( "7\n" );

/*
			if ( !m_image.Ptr() || m_image.Size() != m_nBufferSize )
			{
				if ( !m_image.OexNew( m_nBufferSize ).Ptr() )
				{	oexERROR( -1, CStr().Fmt( oexT( "Unable to allocate image buffer : size=%d" ), m_nBufferSize ).Ptr() );
					return oexFALSE;
				} // end if

			} // end if
*/
		} // end else if

		return oexTRUE;
	}

	/// Stops video capture
	oexBOOL StopCapture()
	{
		return oexTRUE;
	}

	/// Waits for a new frame of video
	oexBOOL WaitForFrame( oexUINT x_uTimeout = 0 )
	{
		// v4l version 1
		if ( 1 == m_nVersion )
		{
			oexINT nFrame = eCaptureBuffer;
			if ( -1 == IoCtl( m_nFd, VIDIOCSYNC, &nFrame ) )
			{	oexERROR( errno, CStr().Fmt( oexT( "VIDIOCSYNC : Failed to wait for frame sync, %u" ), m_nFd ) );
				return oexFALSE;
			} // end if

			// Count a frame
			m_llFrame++;

		} // end if

		else if ( 2 == m_nVersion )
		{
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
		} // end else

		return oexTRUE;
	}

	/// Returns a pointer to the video buffer
	oexPVOID GetBuffer()
	{	return m_image.Ptr(); } //return m_pFrameBuffer; } // return m_image.Ptr(); }

	/// +++ Should return the size of the video buffer
	oexINT GetImageSize()
	{	return CImage::GetScanWidth( m_nWidth, m_nBpp ) * cmn::Abs( m_nHeight ); }

	oexINT GetBufferSize()
	{	return m_nBufferSize; }

	/// Returns an image object
	CImage* GetImage()
	{	return oexNULL; }

	/// Returns the image width
	oexINT GetWidth()
	{	return m_nWidth; }

	/// Returns the image height
	oexINT GetHeight()
	{	return m_nHeight; }

	/// Returns the bits-per-pixel of the current image format
	oexINT GetBpp()
	{	return m_nBpp; }

	/// Returns the frame rate in frames per second
	oexFLOAT GetFps()
	{	return m_fFps; }

	/// Returns the current frame index
	oexINT64 GetFrame()
	{	return m_llFrame; }

private:

	/// Name of the device
	CStr				m_sDeviceName;

	/// Device file descriptor
	oexINT				m_nFd;

	/// API version
	oexINT				m_nVersion;

	/// IO method
	oexINT				m_nIoMode;

	/// The current frame number
	oexINT64			m_llFrame;

	/// Device capabilities
	video_capability	m_cap1;

#ifndef OEX_VFL1
	v4l2_capability		m_cap2;
#endif

	/// Video buffer 1 format
	video_mbuf			m_buf1;

	/// Image memory
	TMem< oexCHAR >		m_image;

	oexINT				m_nWidth;

	oexINT				m_nHeight;

	oexINT				m_nBpp;

	oexFLOAT			m_fFps;

	oexINT				m_nBufferSize;

	/// Image buffer
//	CImage				m_img;

	/// Frame buffer
	oexCHAR				*m_pFrameBuffer;

	/// Cropping capabilities
//	v4l2_cropcap		m_cropcap;

	/// Cropping info
//	v4l2_crop			m_crop;

	/// Video format information
//	v4l2_format			m_fmt;

};

CCapture::CCapture()
{
	m_pDevice = oexNULL;
}

/// Destructor
CCapture::~CCapture()
{
	Destroy();
}

void CCapture::Destroy()
{
	/// Lose device if any
	if ( oexCHECK_PTR( m_pDevice ) )
		OexAllocDelete< CV4lCapture >( (CV4lCapture*)m_pDevice );
	m_pDevice = oexNULL;
}

//	CV4lCapture				m_v4lCap;

oexBOOL CCapture::Open( oexCSTR x_sDevice, oexINT x_nWidth, oexINT x_nHeight, oexINT x_nBpp, oexFLOAT x_fFps )
{
	// Lose previous device
	Destroy();

	// Allocate a new capture device
	m_pDevice = OexAllocConstruct< CV4lCapture >();
	if ( !m_pDevice )
		return oexFALSE;

	// Attempt to open the capture device
	if ( !( (CV4lCapture*)m_pDevice )->Open( x_sDevice, x_nWidth, x_nHeight, x_nBpp, x_fFps ) )
	{	Destroy();
		return oexFALSE;
	} // end if

	return oexTRUE;
}

oexBOOL CCapture::WaitForFrame( oexUINT x_uTimeout )
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return ( (CV4lCapture*)m_pDevice )->WaitForFrame( x_uTimeout );
}

oexBOOL CCapture::StartCapture()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return ( (CV4lCapture*)m_pDevice )->StartCapture();
}

oexBOOL CCapture::StopCapture()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return ( (CV4lCapture*)m_pDevice )->StopCapture();
}

oexPVOID CCapture::GetBuffer()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return ( (CV4lCapture*)m_pDevice )->GetBuffer();
}

oexINT CCapture::GetBufferSize()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return ( (CV4lCapture*)m_pDevice )->GetBufferSize();
}

oexINT CCapture::GetImageSize()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return ( (CV4lCapture*)m_pDevice )->GetImageSize();
}

CImage* CCapture::GetImage()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return ( (CV4lCapture*)m_pDevice )->GetImage();
}

oexINT CCapture::GetWidth()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return ( (CV4lCapture*)m_pDevice )->GetWidth();
}

oexINT CCapture::GetHeight()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return ( (CV4lCapture*)m_pDevice )->GetHeight();
}

oexINT CCapture::GetBpp()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return ( (CV4lCapture*)m_pDevice )->GetBpp();
}

oexFLOAT CCapture::GetFps()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return ( (CV4lCapture*)m_pDevice )->GetFps();
}

oexINT64 CCapture::GetFrame()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return oexFALSE;
	} // end if

	return ( (CV4lCapture*)m_pDevice )->GetFrame();
}

CStr CCapture::GetSupportedFormats()
{
	if ( !oexCHECK_PTR( m_pDevice ) )
	{	oexERROR( -1, CStr().Fmt( oexT( "Invalid device pointer : %d" ), m_pDevice ) );
		return CStr();
	} // end if

	return ( (CV4lCapture*)m_pDevice )->GetSupportedFormats();
}

