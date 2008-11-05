/*------------------------------------------------------------------
// image.h
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
// CImage
//
/// Generic image format
/**

*/
//==================================================================
class CImage
{
public:

#pragma pack( push, 1 )

	/// Header for a standard dib file (.bmp)
	struct SDIBFileHeader
	{
		// The magic number ASCII 'BM'
		enum { eMagicNumber = 0x4d42 };

		/// Magic number, must be 0x42 0xFD (BM)
		oexUSHORT 		uMagicNumber;

		/// Size of the file in bytes
		oexULONG		uSize;

		/// Reserved
		oexUSHORT		uReserved1;

		/// Reserved
		oexUSHORT		uReserved2;

		/// Offset to start of image data
		oexULONG		uOffset;

	};

	/// Standard bitmap structure
	struct SBitmapInfoHeader
	{
		/// Size of this structure
		oexULONG				biSize;

		/// Image width
		oexLONG					biWidth;

		/// Image height
		oexLONG					biHeight;

		/// Number of bit planes in the image
		oexUSHORT				biPlanes;

		/// Bits per pixel / color depth
		oexUSHORT				biBitCount;

		/// Default bitmap encoding types
		enum
		{
			eBiRgb 			= 0,
			eBiRle8 		= 1,
			eBiRle4			= 2,
			eBiBitfields	= 3,
			eBiJpeg			= 4,
			eBiPng			= 5
		};

		/// Type of compression
		/**
			This value can be a fourcc code or one of the above
			enumerations. eBiRgb, etc...
		*/
		oexULONG				biCompression;

		/// The total size of the image data,
		/// can be zero for eBiRgb encoded images.
		oexULONG				biSizeImage;

		/// Horizontal resolution in pixels per meter
		oexLONG					biXPelsPerMeter;

		/// Vertical resolution in pixels per meter
		oexLONG					biYPelsPerMeter;

		/// Total number of colors actually used in the image,
		/// zero for all colors used.
		oexULONG				biClrUsed;

		/// Number of colors required for displaying the image,
		/// zero for all colors required.
		oexULONG				biClrImportant;
	};

	/// This structure holds bitmap data
	struct SBitmapInfo
	{
		/// Bitmap information
		SBitmapInfoHeader		bmiHeader;

		/// Color palette
		oexULONG				bmiColors[ 1 ];
	};

	// This structure contains the memory image
	struct SImageData
	{
		/// Windows compatible image information
		SBitmapInfoHeader		bih;

		/// Image data
		oexCHAR					aImage[1];
	};

#pragma pack( pop )

public:

    /// Default constructor
    CImage()
    {
    }

    /// Destructor
    ~CImage()
    {
    	Destroy();
    }

    /// Releases image resources
    oexBOOL Destroy();

    /// Returns the image size
    oexINT64 Size()
    {	return 0; } // +++ Image size goes here }

	//==============================================================
	// Create()
	//==============================================================
	/// Creates an empty image
	/**
		\param [in]  x_pShared	- Shared memory name
		\param [in]  x_hShared	- Shared memory file handle
		\param [in]  x_lWidth	- Image width
		\param [in]  x_lHeight	- Image height
		\param [in]  x_lBpp		- Image bits per pixel
		\param [in]  x_bInit	- Non-zero if the image header should
								  be initialized

		\return Non-zero if success

		\see
	*/
    oexBOOL Create( oexCSTR x_pShared, os::CFMap::t_HFILEMAP x_hShared, oexINT x_lWidth, oexINT x_lHeight, oexINT x_lBpp, oexBOOL m_bInit );

	/// Returns the scan width for the specified values
	static oexINT GetScanWidth( oexINT lWidth, oexINT lBpp )
	{	return cmn::Abs( lWidth * cmn::FitTo( lBpp, 8 ) ); }

	oexBOOL IsValid()
	{
		return m_image.IsValid();
	}

	/// Returns a pointer to the image header
	SImageData* Image()
	{	if ( !m_image.IsValid() )
			return oexNULL;
		return m_image.Ptr();
	}

	/// Returns a pointer to the image header
	SImageData* GetImageHeader()
	{	if ( !m_image.IsValid() )
			return oexNULL;
		return m_image.Ptr();
	}

	/// Returns the size of the image header
	oexINT GetImageHeaderSize()
	{	return sizeof( SImageData ); }


	/// Returns the image width
	oexINT GetWidth()
	{	if ( !m_image.IsValid() )
			return 0;
		return Image()->bih.biWidth;
	}

	/// Returns the image height
	oexINT GetHeight()
	{	if ( !m_image.IsValid() )
			return 0;
		return Image()->bih.biHeight;
	}

	/// Returns the image buffer
	oexPVOID GetBuffer()
	{
		if ( !m_image.IsValid() )
			return oexNULL;

		return ( (oexCHAR*)m_image.Ptr() ) + Image()->bih.biSize;
	}

	/// Returns the size of the image buffer
	oexINT GetBufferSize()
	{
		if ( !m_image.IsValid() )
			return oexNULL;

		/// Calculate image buffer size
		return GetScanWidth( Image()->bih.biWidth, Image()->bih.biBitCount )
		       * cmn::Abs( Image()->bih.biHeight );
	}

	/// Writes a DIB file to disk
	static oexBOOL SaveDibFile( oexCSTR x_pFile, SImageData *x_pId, oexCPVOID x_pData, oexINT x_nData );

	/// Convert image
	oexBOOL CopySBGGR8( oexPVOID x_pData );

	/// Convert from grey scale image
	oexBOOL CopyGrey( oexPVOID x_pData );

private:

	/// Image memory
	TMem< oexCHAR, SImageData > m_image;
};

