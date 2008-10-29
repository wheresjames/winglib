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

//==================================================================
// CCapture
//
/// Generic video capture
/**


*/
//==================================================================
class CCapture
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
	void Destroy();

	/// Opens the specified capture device
	/**
		\param [in] x_sDevice	- Name of the capture device to open

		\return Non-zero if success.
	*/
	oexBOOL Open( oexCSTR x_sDevice, oexINT x_nWidth, oexINT x_nHeight, oexINT x_nBpp, oexFLOAT x_fFps );

	/// Returns non-zero if a capture device is currently open
	oexBOOL IsOpen()
	{	return ( oexNULL != m_pDevice ); }

	/// Starts video capture
	oexBOOL StartCapture();

	/// Stops video capture
	oexBOOL StopCapture();

	/// Waits for a new video frame to become available
	oexBOOL WaitForFrame( oexUINT x_uTimeout = 0 );

	/// Returns a pointer to the video buffer
	oexPVOID GetBuffer();

	/// Returns the size of the video buffer
	oexINT GetImageSize();

	/// Returns an image object containing the video frame
	CImage* GetImage();

	/// Returns the image width
	oexINT GetWidth();

	/// Returns the image height
	oexINT GetHeight();

	/// Returns the bits-per-pixel of the current image format
	oexINT GetBpp();

	/// Returns the frame rate in frames per second
	oexFLOAT GetFps();

	/// Returns the current frame index
	oexINT64 GetFrame();

public:


private:

	/// System specific capture device class
	oexPVOID			m_pDevice;
};

