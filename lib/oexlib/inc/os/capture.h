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

#define oexVIDSUB_AUTO		0

// Linux
#define oexVIDSUB_VFL1		oexFOURCC( '1', 'V', 'F', 'L' )
#define oexVIDSUB_VFL2		oexFOURCC( '2', 'V', 'F', 'L' )
#define oexVIDSUB_DAVINCI	oexFOURCC( '1', 'D', 'A', 'V' )

// Windows
#define oexVIDSUB_VFW		oexFOURCC( '1', 'V', 'F', 'W' )
#define oexVIDSUB_DSHOW		oexFOURCC( '1', 'D', 'S', 'H' )

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
	virtual oexBOOL Open( oexUINT x_uType, oexCSTR x_pFile, oexINT x_nWidth, oexINT x_nHeight, oexINT x_nBpp, oexFLOAT x_fFps ) = 0;
	virtual oexBOOL Open( oexUINT x_uType, oexUINT x_uDevice, oexUINT x_uSource, oexINT x_nWidth, oexINT x_nHeight, oexINT x_nBpp, oexFLOAT x_fFps ) = 0;
	virtual oexBOOL IsOpen() = 0;
	virtual oexBOOL StartCapture() = 0;
	virtual oexBOOL StopCapture() = 0;
	virtual oexBOOL WaitForFrame( oexUINT x_uTimeout ) = 0;
	virtual oexBOOL ReleaseFrame() = 0;
	virtual oexPVOID GetBuffer() = 0;
	virtual oexINT GetImageSize() = 0;
	virtual oexINT GetBufferSize() = 0;
	virtual CImage* GetImage() = 0;
	virtual oexINT GetWidth() = 0;
	virtual oexINT GetHeight() = 0;
	virtual oexINT GetBpp() = 0;
	virtual oexFLOAT GetFps() = 0;
	virtual oexINT64 GetFrame() = 0;
	virtual CStr GetSupportedFormats() = 0;
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
		\param [in] x_nBpp		- Desired bits per pixel
		\param [in] x_fFps		- Desired capture frame rate

		\return Non-zero if success.
	*/
	virtual oexBOOL Open( oexUINT x_uType, oexUINT x_uDevice, oexUINT x_uSource, oexINT x_nWidth, oexINT x_nHeight, oexINT x_nBpp, oexFLOAT x_fFps );

	/// Opens the specified file
	/**
		\param [in] x_uType		- Capture API to use
		\param [in] x_uDevice	- Index of the capture device to open
		\param [in] x_uSource	- Index of video stream to open
		\param [in] x_nWidth	- Desired video width
		\param [in] x_nHeight	- Desired video height
		\param [in] x_nBpp		- Desired bits per pixel
		\param [in] x_fFps		- Desired capture frame rate

		\return Non-zero if success.
	*/
	virtual oexBOOL Open( oexUINT x_uType, oexCSTR x_pFile, oexINT x_nWidth, oexINT x_nHeight, oexINT x_nBpp, oexFLOAT x_fFps );

	/// Returns non-zero if a capture device is currently open
	virtual oexBOOL IsOpen()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->IsOpen();
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

	/// Returns an image object containing the video frame
	virtual CImage* GetImage()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->GetImage();
	}

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
	virtual oexINT GetBpp()
	{
		if ( !oexCHECK_PTR( m_pDevice ) )
			return oexFALSE;

		return m_pDevice->GetBpp();
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
			return oexFALSE;

		return m_pDevice->GetSupportedFormats();
	}

private:

	/// System specific capture device class
	CCaptureTmpl		*m_pDevice;

	/// The type of capture system being used
	oexUINT				m_uType;
};

