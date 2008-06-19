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

public:

	/// Default contructor
	CV4lCapture()
	{
		m_nFd = -1;
		m_nVersion = 0;
		m_nIoMode = eIoAuto;
		m_pFrameBuffer = oexNULL;
		m_llFrame = 0;
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
	oexBOOL Open( oexCSTR x_pDevice )
	{
		// Sanity checks
		if ( !x_pDevice || !*x_pDevice )
		{	oexLOG( -1, CStr().Fmt( oexT( "Invalid device name" ) ) );
			return oexFALSE;
		} // end if

		struct stat st;
		if ( 0 > stat( oexStrToMbPtr( x_pDevice ), &st ) )
		{	oexLOG( errno, CStr().Fmt( oexT( "Device name is invalid : %s" ), x_pDevice ) );
			return oexFALSE;
		} // end if

		if ( !S_ISCHR( st.st_mode ) )
		{	oexLOG( -1, CStr().Fmt( oexT( "Device name is invalid : %s" ), x_pDevice ) );
			return oexFALSE;
		} // end if

		// Attempt to open the device
		m_nFd = open( oexStrToMbPtr( x_pDevice ), O_RDWR, 0 ); // | O_NONBLOCK, 0 );
		if ( 0 > m_nFd )
		{	oexLOG( errno, CStr().Fmt( oexT( "Unable to open file : %s" ), x_pDevice ) );
			return oexFALSE;
		} // end if

		// Save device name
		m_sDeviceName = x_pDevice;

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

		m_nFd = -1;
		m_nVersion = 0;
		m_pFrameBuffer = oexNULL;
		m_llFrame = 0;

	}

public:

	/// Proper ioctl call
	static int IoCtl( int fd, int request, void * arg )
	{	return TEMP_FAILURE_RETRY( ioctl( fd, request, arg ) ); }

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
		if ( -1 == IoCtl( m_nFd, VIDIOC_QUERYCAP, &m_cap2 ) )
		{
			// Log V2 failure
			oexLOG( errno, CStr().Fmt( oexT( "VIDEOC_QUERYCAP : Invalid V4L2 device, %u" ), m_nFd ) );

			// V1 Device?
			if ( -1 == IoCtl( m_nFd, VIDIOCGCAP, &m_cap1 ) )
			{	oexLOG( errno, CStr().Fmt( oexT( "VIDEOCGCAP : Invalid V4L1 device, %u" ), m_nFd ) );
				return oexFALSE;
			} // end if

			// Version 1
			m_nVersion = 1;

			// Ensure it's a capture device
			if ( !( m_cap1.type & VID_TYPE_CAPTURE ) )
			{	oexLOG( -1, CStr().Fmt( oexT( "VID_TYPE_CAPTURE : Not a capture device : type = %X" ), m_cap1.type ) );
				return oexFALSE;
			} // end if

			// Set video buffer
			if ( -1 == IoCtl( m_nFd, VIDIOCGMBUF, &m_buf1 ) )
			{	oexLOG( errno, CStr().Fmt( oexT( "VIDIOCGMBUF : Unable to set video buffer, %u" ), m_nFd ) );
				return oexFALSE;
			} // end if

			// Allocate a frame buffer
			m_pFrameBuffer = (oexCHAR*)mmap( 0, m_buf1.size,
											 PROT_READ | PROT_WRITE,
											 MAP_SHARED,
											 m_nFd, 0 );

			// Did we get the frame buffer
			if ( -1 == (oexINT)m_pFrameBuffer )
			{	oexLOG( errno, oexT( "mmap failed : cannot use shared memory" ) );
				m_nIoMode = eIoReadWrite;
			} // end if

			else
				m_nIoMode = eIoAsync;

		} // end if

		else
		{
			// Version 2
			m_nVersion = 2;

			// Ensure it's actually a capture device
			if ( !( m_cap2.capabilities & V4L2_CAP_VIDEO_CAPTURE ) )
			{	oexLOG( -1, CStr().Fmt( oexT( "V4L2_CAP_VIDEO_CAPTURE : Not a capture device : capabilities = %X" ), m_cap2.capabilities ) );
				return oexFALSE;
			} // end if

			// +++ Detect io mode if eIoAuto
			if ( eIoAuto == m_nIoMode )
				m_nIoMode = eIoReadWrite;

			// Verify image transfer mode is supported
			switch( m_nIoMode )
			{
				case eIoReadWrite :
					if ( !( m_cap2.capabilities & V4L2_CAP_READWRITE ) )
					{	oexLOG( -1, CStr().Fmt( oexT( "V4L2_CAP_READWRITE : Read/Write IO not supported" ) ) );
						return oexFALSE;
					} // end if

					break;

				case eIoAsync :

					if ( !( m_cap2.capabilities & V4L2_CAP_ASYNCIO ) )
					{	oexLOG( -1, CStr().Fmt( oexT( "V4L2_CAP_ASYNCIO : Async IO not supported" ) ) );
						return oexFALSE;
					} // end if

					oexLOG( -1, CStr().Fmt( oexT( "V4L2_CAP_ASYNCIO : +++ Not Implemented" ) ) );
					return oexFALSE;

					break;

				case eIoStreaming :

					if ( !( m_cap2.capabilities & V4L2_CAP_STREAMING ) )
					{	oexLOG( -1, CStr().Fmt( oexT( "V4L2_CAP_STREAMING : Stream based IO not supported" ) ) );
						return oexFALSE;
					} // end if

					break;

				default :
					oexLOG( -1, CStr().Fmt( oexT( "Invalid mode" ) ) );
					return oexFALSE;
					break;

			} // end switch

		} // end if

		return oexTRUE;
	}

	oexBOOL StartCapture()
	{
		// v4l version 1
		if ( 1 == m_nVersion )
		{
			if ( eIoAsync == m_nIoMode )
			{
				video_mmap vm;
				vm.frame = ++m_llFrame;
				vm.width = 320;
				vm.height = 240;
				vm.format = VIDEO_PALETTE_RGB24;

				if ( -1 == IoCtl( m_nFd, VIDIOCMCAPTURE, &vm ) )
				{	oexLOG( errno, CStr().Fmt( oexT( "VIDIOCMCAPTURE : Capture frame failed : %u" ), m_nFd ) );
					return oexFALSE;
				} // end if

			} // end if

		} // end if

	}

	/// Stops video capture
	oexBOOL StopCapture()
	{
		return oexTRUE;
	}

	/// Waits for a new frame of video
	oexBOOL WaitForFrame( oexUINT x_uTimeout = 0 )
	{
		oexINT nFrame = (oexINT)m_llFrame;
		if ( -1 == IoCtl( m_nFd, VIDIOCSYNC, &nFrame ) )
		{	oexLOG( errno, CStr().Fmt( oexT( "VIDIOCSYNC : Failed to wait for frame sync, %u" ), m_nFd ) );
			return oexFALSE;
		} // end if

		return oexTRUE;
	}

	/// Returns a pointer to the video buffer
	oexCHAR* GetBuffer()
	{	return m_pFrameBuffer; }

	/// +++ Should return the size of the video buffer
	oexUINT GetBufferSize()
	{	return 320 * 240 * 3; }

	/// Returns an image object
	CImage* GetImage()
	{	return oexNULL; }

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
	v4l2_capability		m_cap2;

	/// Video buffer 1 format
	video_mbuf			m_buf1;

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
	if ( m_pDevice )
	{	OexAllocDelete( (CV4lCapture*)m_pDevice );
		m_pDevice = oexNULL;
	} // end if
}

//	CV4lCapture				m_v4lCap;

oexBOOL CCapture::Open( oexCSTR x_sDevice )
{
	// Lose previous device
	Destroy();

	// Allocate a new capture device
	m_pDevice = OexAllocNew< CV4lCapture >( 1 );
	if ( !m_pDevice )
		return oexFALSE;

	// Attempt to open the capture device
	if ( !( (CV4lCapture*)m_pDevice )->Open( x_sDevice ) )
	{	Destroy();
		return oexFALSE;
	} // end if

	return oexTRUE;
}

oexBOOL CCapture::WaitForFrame( oexUINT x_uTimeout )
{
	if ( !m_pDevice )
		return oexFALSE;

	return ( (CV4lCapture*)m_pDevice )->WaitForFrame( x_uTimeout );
}

oexBOOL CCapture::StartCapture()
{
	if ( !m_pDevice )
		return oexFALSE;

	return ( (CV4lCapture*)m_pDevice )->StartCapture();
}

oexBOOL CCapture::StopCapture()
{
	if ( !m_pDevice )
		return oexFALSE;

	return ( (CV4lCapture*)m_pDevice )->StopCapture();
}

oexCHAR* CCapture::GetBuffer()
{
	if ( !m_pDevice )
		return oexFALSE;

	return ( (CV4lCapture*)m_pDevice )->GetBuffer();
}

oexUINT CCapture::GetBufferSize()
{
	if ( !m_pDevice )
		return oexFALSE;

	return ( (CV4lCapture*)m_pDevice )->GetBufferSize();
}

CImage* CCapture::GetImage()
{
	if ( !m_pDevice )
		return oexFALSE;

	return ( (CV4lCapture*)m_pDevice )->GetImage();
}

