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

#ifdef OEX_ENABLE_XIMAGE

struct oexSRect
{
	oexLONG		left;
	oexLONG		top;
	oexLONG		right;
	oexLONG		bottom;
};

struct oexSRgbQuad
{
	oexBYTE		rgbBlue;
	oexBYTE		rgbGreen;
	oexBYTE		rgbRed;
	oexBYTE		rgbReserved;
};

typedef oexULONG oexSColorRef;

#define oexRGB( r, g, b ) ( (OEX_NAMESPACE::oexSColorRef)( ( (long)(r) & 0xff << 24 ) \
														 | ( (long)(g) & 0xff << 16 ) \
														 | ( (long)(b) & 0xff ) ) )

//==================================================================
// CImage
//
/// Provides image manipulation, JPEG, PNG, BMP, WMF, EMF, etc...
/**
	Provides image encoding/decoding for JPEG, PNG, BMP, WMF, EMF,
	etc...  This functionality is primarily achieved by encapsulating
	the CxImage implementation.  You will need to download the
	CxImage library from http://www.xdp.it/cximage.htm.  Many thanks
	to Davide Pizzolato for providing this library free of charge.
	If you get the chance, please stop in and give Davide a million
	dollars.
*/
//==================================================================
class CImage
{
public:

	//==============================================================
	// GetUserImage()
	//==============================================================
	/// Opens a user image
	/**
		\return String that can be used in open file dialog boxes

		\see
	*/
	CStr GetUserImageStr();

	//==============================================================
	// GetImageSize()
	//==============================================================
	/// Returns the size, in bytes, of the raw image buffer
	oexINT GetImageSize();

	//==============================================================
	// Encode()
	//==============================================================
	/// Encodes the specified file to a buffer
	/**
		\param [in] x_pBuf		-	Pointer to buffer that receives the
									encoded image
		\param [in] x_nSize		-	Size of the buffer in pBuf
		\param [in]  x_pType	-	File extension specifying type of
									encoding.  "jpg", "png", etc...

		pBuf must be large enough to hold the encoded image.

		\return Number of bytes written to pBuf

		\see
	*/
	oexINT Encode( oexPBYTE x_pBuf, oexINT x_nSize, oexCSTR x_pType );

	//==============================================================
	// Tile()
	//==============================================================
	/// Tiles the image into the specified device context
	/**
		\param [in] x_rImg	-	Handle to device context
		\param [in] x_pRect	-	Portion of hDC to tile

		\return Non-zero if success

		\see
	*/
	oexBOOL Tile( CImage &x_rImg, oexSRect *x_pRect );

	//==============================================================
	// AlphaDelete()
	//==============================================================
	/// Deletes the alpha layer
	oexBOOL AlphaDelete();

	//==============================================================
	// HasAlpha()
	//==============================================================
	/// Returns non-zero if the image has an alpha layer
	oexBOOL HasAlpha();

	//==============================================================
	// ToClipboard()
	//==============================================================
	/// Copies the image to the clipboard
	/**
		\param [in] hWnd	-	Window handle

		\return Non-zero if success

		\see
	*/
//	oexBOOL ToClipboard( HWND hWnd );

	//==============================================================
	// FromClipboard()
	//==============================================================
	/// Grabs an image from the clipboard
	/**
		\param [in] hWnd	-	Non-zero if success

		\return Non-zero if success

		\see
	*/
//	oexBOOL FromClipboard( HWND hWnd );

	//==============================================================
	// DecreaseBpp()
	//==============================================================
	/// Decreases the bits per pixel used to represent the image
	/**
		\param [in] x_nBpp				-	Desired bits per pixel
		\param [in] x_bErrorDiffusion	-	Non-zero to enable error
											diffusion.
		\param [in] x_rgbq				-	Optional palette to use.

		\return	Non-zero if success

		\see
	*/
	oexBOOL DecreaseBpp( oexINT x_nBpp, oexBOOL x_bErrorDiffusion, oexSRgbQuad* x_rgbq = oexNULL );

	//==============================================================
	// IncreaseBpp()
	//==============================================================
	/// Increases the bits per pixel used to represent the image
	/**
		\param [in] x_nBpp		-	Desired bits per pixel

		\return Non-zero if success

		\see
	*/
	oexBOOL IncreaseBpp( oexINT x_nBpp );

	//==============================================================
	// GetNearestIndex()
	//==============================================================
	/// Gets the nearest value from the palette to the specified color
	/**
		\param [in] x_rgb		-	Color value

		\return	Returns the palette index of the nearest color

		\see
	*/
	oexINT GetNearestIndex( oexSRgbQuad x_rgb );

	//==============================================================
	// SetTransIndex()
	//==============================================================
	/// Set the palette index of the transparent color
	/**
		\param [in] x_nIndex	-	Palette index of transparent color

		\return Non-zero if success

		\see
	*/
	oexBOOL SetTransIndex( oexINT x_nIndex );

	//==============================================================
	// GetTransIndex()
	//==============================================================
	/// Returns the palette index of the transparent color
	oexINT GetTransIndex();

	//==============================================================
	// SetPixel()
	//==============================================================
	/// Sets the specified pixel color value
	/**
		\param [in] x_x		-	Horizontal offset of pixel
		\param [in] x_y		-	Vertical offset of pixel
		\param [in] x_rgb	-	New color value for pixel

		\return Non-zero if success

		\see
	*/
	oexBOOL SetPixel( oexINT x_x, oexINT x_y, oexSRgbQuad x_rgb );

	//==============================================================
	// SetPixel()
	//==============================================================
	/// Sets the specified pixel color value
	/**
		\param [in] x_x		-	Horizontal offset of pixel
		\param [in] x_y		-	Vertical offset of pixel
		\param [in] x_rgb	-	New color value for pixel

		\return Non-zero if success

		\see
	*/
	oexBOOL SetPixel( oexINT x_x, oexINT x_y, oexSColorRef x_rgb )
	{	return SetPixel( x_x, x_y, *(oexSRgbQuad*)cmn::RevBytes( (oexPBYTE)&x_rgb, sizeof( x_rgb ) ) ); }

	//==============================================================
	// GetPixel()
	//==============================================================
	/// Returns the color of the specified pixel
	/**
		\param [in] x_x		-	Horizontal offset of pixel
		\param [in] x_y		-	Vertical offset of pixel

		\return Color value of pixel

		\see
	*/
	oexSRgbQuad GetPixel( oexINT x_x, oexINT x_y );

	//==============================================================
	// GetRgbPixel()
	//==============================================================
	/// Returns the color of the specified pixel
	/**
		\param [in] x_x		-	Horizontal offset of pixel
		\param [in] x_y		-	Vertical offset of pixel

		\return Color value of pixel

		\see
	*/
	oexSColorRef GetRgbPixel( oexINT x_x, oexINT x_y )
	{	oexSRgbQuad rgbq = GetPixel( x_x, x_y );
		return *(oexLONG*)cmn::RevBytes( (oexPBYTE)&rgbq, sizeof( rgbq ) );
	}

	//==============================================================
	// GetBpp()
	//==============================================================
	/// Returns the number of bits per pixel used to represent the image
	oexINT GetBpp();

	//==============================================================
	// GetTransColor()
	//==============================================================
	/// Returns the RGB value of the current transparent color
	oexSRgbQuad GetTransColor();

	//==============================================================
	// SetTransColor()
	//==============================================================
	/// Sets the transparent color
	/**
		\param [in] x_rgb	-	Transparent color value

		\return Non-zero if success

		\see
	*/
	oexBOOL SetTransColor( oexSRgbQuad x_rgb );

	//==============================================================
	// IsTransparent()
	//==============================================================
	/// Returns non-zero if transparency is enabled
	oexBOOL IsTransparent();

	//==============================================================
	// Resample()
	//==============================================================
	/// Resamples the image to the new size
	/**
		\param [in] x_pRect	-	New size for image
		\param [in] x_nFast	-	Resampling algorithm to use ( 0, 1 )

		\return Non-zero if success

		\see
	*/
	oexBOOL Resample( oexSRect *x_pRect, oexINT x_nFast = 1 );

	//==============================================================
	// Copy()
	//==============================================================
	/// Copies the specified image
	/**
		\param [in] x_pImg	-	Source image to copy

		\return Non-zero if success

		\see
	*/
	oexBOOL Copy( CImage *x_pImg );

	//==============================================================
	// operator =()
	//==============================================================
	/// Copies the specified image
	/**
		\param [in] x_pImg	-	Source image to copy

		\return Non-zero if success

		\see
	*/
    CImage& operator = ( const CImage &x_pImg )
    {   Copy( (CImage*)&x_pImg ); return *this; }

	//==============================================================
	// Create()
	//==============================================================
	/// Creates an image of the specified size and color depth
	/**
		\param [in] x_w		-	Width of new image
		\param [in] x_h		-	Height of new image
		\param [inx_nBpp	-	Color depth of new image in bits per pixel

		\return Non-zero if success

		\see
	*/
	oexBOOL Create( oexINT x_w, oexINT x_h, oexINT x_nBpp = 24 );

	//==============================================================
	// Create()
	//==============================================================
	/// Creates an image of the specified size
	/**
		\param [in] w	-	Width of new image
		\param [in] h	-	Height of new image

		\return Non-zero if success

		\see
	*/
	oexBOOL Create24bpp( oexINT x_w, oexINT x_h )
    {   return Create( x_w, x_h, 24 ); }

	//==============================================================
	// GetEffWidth()
	//==============================================================
	/// Returns the scan width of a row of image data
	oexINT GetEffWidth();

	//==============================================================
	// GetScanWidth()
	//==============================================================
	/// Returns the scan width for the specified values
	static oexINT GetScanWidth( oexINT lWidth, oexINT lBpp )
	{	return cmn::Abs( lWidth * cmn::FitTo( lBpp, 8 ) ); }


	//==============================================================
	// RGBtoBGR()
	//==============================================================
	/// Swaps the specified buffer values from RGB to BGR or vice versa
	/**
		\param [in] pBuf	-	Input color values
		\param [in] uLen	-	Length of color values

		\return The value in pBuf
	*/
	oexPBYTE SwapRB( oexPBYTE x_pBuf, oexINT x_nLen )
	{	for ( oexINT i = 0; i < x_nLen; i += 3 )
			cmn::Swap( x_pBuf[ i ], x_pBuf[ i + 2 ] );
		return x_pBuf;
	}

	//==============================================================
	// GetBits()
	//==============================================================
	/// Returns a pointer to the raw image buffer
	oexPVOID GetBits();

	//==============================================================
	// Histogram()
	//==============================================================
	/// Generates a historgram for the current image
	/**
		\param [out] x_red			-	Receives the red histogram
		\param [out] x_green		-	Receives the green histogram
		\param [out] x_blue			-	Receives the blue histogram
		\param [out] x_gray			-	Receives the gray scale histogram
		\param [in]  x_colorspace	-	Colorspace

		\return Length of histogram

		\see
	*/
	oexINT Histogram( oexLONG *x_red, oexLONG *x_green = 0, oexLONG *x_blue = 0, oexLONG *x_gray = 0, oexLONG x_colorspace = 0  );

	//==============================================================
	// Dilate()
	//==============================================================
	/// Dialates the image
	/**
		\param [in] x_Ksize	-	Dialation weight

		\return Non-zero if success

		\see
	*/
	oexBOOL Dilate( oexINT x_Ksize = 2 );

	//==============================================================
	// Erode()
	//==============================================================
	/// Erodes the image
	/**
		\param [in] x_Ksize	-	Erosion weight

		\return Non-zero if success

		\see
	*/
	oexBOOL Erode( oexINT x_Ksize = 2 );

	//==============================================================
	// Light()
	//==============================================================
	/// Lightens the image
	/**
		\param [in] x_level		-	Lighten level
		\param [in] x_contrast	-	Contrast

		\return Non-zero if success

		\see
	*/
	oexBOOL Light( oexINT x_level, oexINT x_contrast = 100 );

	//==============================================================
	// Colorize()
	//==============================================================
	/// Colorizes the image
	/**
		\param [in] x_hue		-	Hue
		\param [in] x_sat		-	Saturation

		\return Non-zero if success

		\see
	*/
	oexBOOL Colorize( oexINT x_hue, oexINT x_sat );

	//==============================================================
	// Threshold()
	//==============================================================
	/// Thresholds the image
	/**
		\param [in] x_level	-	Threshold level

		Converts the image to a monochrome image, converting all values
		above the threshold value to white.  All values below the
		threshold are converted to black.

		\return Non-zero if success

		\see
	*/
	oexBOOL Threshold( oexINT x_level );

	//==============================================================
	// ShiftRGB()
	//==============================================================
	/// Changes the RGB component levels
	/**
		\param [in] x_r	-	Red component level
		\param [in] x_g	-	Green component level
		\param [in] x_b	-	Blue component level

		\return Non-zero if success

		\see
	*/
	oexBOOL ShiftRGB( oexINT x_r, oexINT x_g, oexINT x_b );

	//==============================================================
	// Gamma()
	//==============================================================
	/// Gamma correction effect
	/**
		\param [in] x_fGamma	-	Gamma correction value

		\return Non-zero if success

		\see
	*/
	oexBOOL Gamma( oexFLOAT x_fGamma );

	//==============================================================
	// Median()
	//==============================================================
	/// Median image effect
	/**
		\param [in] x_Ksize	-	Median image weight

		\return Non-zero if success

		\see
	*/
	oexBOOL Median( oexINT x_Ksize = 3 );

	//==============================================================
	// Noise()
	//==============================================================
	/// Adds noise to the image
	/**
		\param [in] x_level	-	Noise level

		\return Non-zero if success

		\see
	*/
	oexBOOL Noise( oexINT x_level );

	//==============================================================
	// Crop()
	//==============================================================
	/// Crops an area of the image
	/**
		\param [in] x_left	-	Horizontal offset of upper left corner
		\param [in] x_top	-	Vertical offset of upper left corner
		\param [in] x_right	-	Horizontal offset of lower right corner
		\param [in] x_bottom-	Vertical offset of lower right corner
		\param [in] x_pDst	-	CWinDc object that receives the cropped
								image.  NULL to replace current image.

		\return Non-zero if success

		\see
	*/
	oexBOOL Crop( oexINT x_left, oexINT x_top, oexINT x_right, oexINT x_bottom, CImage *x_pDst = oexNULL );

	//==============================================================
	// Dither()
	//==============================================================
	/// Dithers the image
	/**
		\param [in] x_method	-	Dither method ( 0, 1 )

		\return Non-zero if success

		\see
	*/
	oexBOOL Dither( oexINT x_method = 0 );

	//==============================================================
	// Resample()
	//==============================================================
	/// Resamples the image
	/**
		\param [in] x_newx	-	New horizontal image size
		\param [in] x_newy	-	New vertical image size
		\param [in] x_fast	-	Resampling algorithm to use ( 0, 1 )

		\return Non-zero if success

		\see
	*/
	oexBOOL Resample( oexINT x_newx, oexINT x_newy, oexINT x_fast = 1 );

	//==============================================================
	// Rotate()
	//==============================================================
	/// Rotates the image by the specified ammount
	/**
		\param [in] x_angle	-	Rotation angle in degrees
		\param [in] x_pDst	-	CWinDc object that receives rotated
								image.  NULL to replace current image.

		\return Non-zero if success

		\see
	*/
	oexBOOL Rotate( oexFLOAT x_angle, CImage *x_pDst = oexNULL );

	//==============================================================
	// Negative()
	//==============================================================
	/// Generates negative for image
	/**
		\return Non-zero if success
	*/
	oexBOOL Negative();

	//==============================================================
	// GrayScale()
	//==============================================================
	/// Converts image to gray scale
	/**
		\return Non-zero if success
	*/
	oexBOOL GrayScale();

	//==============================================================
	// Flip()
	//==============================================================
	/// Flips the current image vertically
	/**
		\return Non-zero if success
	*/
	oexBOOL Flip();

	//==============================================================
	// Mirror()
	//==============================================================
	/// Mirrors image horizontally
	/**
		\return Non-zero if success
	*/
	oexBOOL Mirror();

	//==============================================================
	// CreateIcon()
	//==============================================================
	/// Creates an icon from the image
	/**
		\param [in] x_w		-	New icon width
		\param [in] x_h		-	New icon height
		\param [in] x_bck	-	Transparent color used to generate mask.
		\param [in] x_hDC	-	Reference device context, NULL to use
								the desktop.

		\return Handle to new icon or NULL if failure

		\see
	*/
//	HICON CreateIcon( long x_w, long x_h, oexSColorRef x_bck = RGB( 255, 255, 255 ), HDC x_hDC = NULL );

	//==============================================================
	// SetQuality()
	//==============================================================
	/// Sets the quality level for the JPEG encoder
	/**
		\param [in] x_q	-	JPEG quality level

		\return Non-zero if success

		\see
	*/
	oexBOOL SetQuality( oexINT x_q );

	//==============================================================
	// CreateFromHBITMAP()
	//==============================================================
	/// Creates an image from an HBITMAP handle
	/**
		\param [in] hBmp	-	HBITMAP handle

		\return Non-zero if success

		\see
	*/
//	oexBOOL CreateFromHBITMAP( HBITMAP hBmp );

	//==============================================================
	// IsValid()
	//==============================================================
	/// Returns non-zero if there is a valid image
	oexBOOL IsValid();

	//==============================================================
	// GetHeight()
	//==============================================================
	/// Returns the image height
	oexINT GetHeight();

	//==============================================================
	// GetWidth()
	//==============================================================
	/// Returns the image width
	oexINT GetWidth();

	//==============================================================
	// Draw()
	//==============================================================
	/// Draws the image to the specified device context
	/**
		\param [in] x_hDC			-	Handle to device context
		\param [in] x_pRect		-	Destination rectangle for image
		\param [in] x_bStretch	-	Non-zero to stretch to pRect

		\return Non-zero if success

		\see
	*/
//	oexBOOL Draw( HDC hDC, LPRECT pRect, BOOL bStretch = TRUE);

	//==============================================================
	// GetLastError()
	//==============================================================
	/// Returns a string pointer describing the last error
	CStr GetLastError();

	//==============================================================
	// Decode()
	//==============================================================
	/// Decodes an image from a memory buffer
	/**
		\param [in] x_buf	-	Pointer to buffer containing the
								compressed image.
		\param [in] x_size	-	Number of bytes in buf.
		\param [in] x_pType	-	Image type, "jpg", "png", etc...

		\return Non-zero if success

		\see
	*/
	oexBOOL Decode( oexPBYTE x_buf, oexINT x_size, oexCSTR x_pType = oexNULL );

	//==============================================================
	// ReleaseEncodeMemory()
	//==============================================================
	/// Releases temporary memory used by CxImage when encoding an image to memory
	void ReleaseEncodeMemory();

	// Raw JPEG encoder
	//==============================================================
	// InitEncodeJpg()
	//==============================================================
	///
	/**
		\param [in] x_nWidth		-	Image width
		\param [in] x_nHeight		-	Image height
		\param [in] x_nQuality	-	JPEG encoding quality

		This function is called automatically by EncodeJpg(), so
		it is not necessary to call this function directly.

		\return Non-zero if success

		\see EncodeJpg(), ReleaseEncodeJpg()
	*/
	oexBOOL InitEncodeJpg( oexINT x_nWidth, oexINT x_nHeight, oexINT x_nQuality );

	//==============================================================
	// EncodeJpg()
	//==============================================================
	/// Encodes the specified image buffer as a JPEG in memory
	/**
		\param [in] x_pSrc		-	Uncompressed source image
		\param [in] x_nWidth	-	Width of image
		\param [in] x_nHeight	-	Height of image
		\param [in] x_pDst		-	Receives JPEG image
		\param [in] x_dwDst		-	Size of buffer in pDst
		\param [in] x_nQuality	-	JPEG image quality

		I couldn't figure out a way to use CxImage to encode a buffer
		in memory without a memcpy and didn't want to have to depend
		on a modified library.  So this function does not use CxImage
		to encode.

		The image in pSrc must be 24-bit BGR encoded.  Scan lines must
		be DWORD aligned, but that's typical.

		The buffer in pDst must be large enough to hold the image or
		the function fails returning zero.  So it is not possible to
		figure out the required buffer size before calling.  I would
		just make the buffer at least as large as the uncompressed
		image.

		This function also reuses the same JPEG compression structures
		if subsequent calls are made with the same size image.  This
		is to speed things up when compressing several images
		sequentially.

		\return Number of bytes written to pDst, or zero if failure.

		\see InitEncodeJpg(), ReleaseEncodeJpg()
	*/
	oexINT EncodeJpg(oexPBYTE x_pSrc, oexINT x_nWidth, oexINT x_nHeight, oexPBYTE x_pDst, oexINT x_nDst, oexINT x_nQuality );


	//==============================================================
	// ReleaseEncodeJpg()
	//==============================================================
	/// Releases memory allocated by InitEncodeJpg()
	/**
		This function will be called when this CImage object is
		destroyed, so it is not necessary to call this function.

		\see InitEncodeJpg(), EncodeJpg()
	*/
	void ReleaseEncodeJpg();

	// Memory pointer returned by Encode() is
	// good until object is destroyed or this
	// function is called again
	oexINT Encode( oexPBYTE *x_buf, oexINT *x_pnSize, oexCSTR x_pType = oexNULL );

	//==============================================================
	// Save()
	//==============================================================
	/// Saves encoded image to disk
	/**
		\param [in] x_pFile	-	Filename
		\param [in] x_pType	-	Image type "jpg", "png", etc...
								If pType is NULL, the image type is
								determined by examining the file
								extension in pFile.



		\return Non-zero if success.

		\see
	*/
	oexBOOL Save( oexCSTR x_pFile, oexCSTR x_pType = oexNULL );

	//==============================================================
	// GetFileType()
	//==============================================================
	/// Examines the file extension and returns the image type code
	/**
		\param [in] x_pFile	-	Filename to examine

		\return Image type code

		\see
	*/
	static oexINT GetFileType( oexCSTR x_pFile );

	//==============================================================
	// Load()
	//==============================================================
	/// Loads the specified file from disk
	/**
		\param [in] x_pFile	-	Filename
		\param [in] x_pType	-	Image type "jpg", "png", etc...
								If pType is NULL, the image type is
								determined by examining the file
								extension in pFile.

		\return Non-zero if success

		\see
	*/
	oexBOOL Load( oexCSTR x_pFile, oexCSTR x_pType = oexNULL );

	//==============================================================
	// Destroy()
	//==============================================================
	/// Releases the image and all associated resources
	void Destroy();

	/// Default constructor
	CImage();

    /// Copy constructor
    CImage( const CImage &img );

	/// Destructor
	virtual ~CImage();

	//==============================================================
	// GetFileName()
	//==============================================================
	/// Returns the current image filename if available
	CStr GetFileName()
	{	return m_filename; }

	//==============================================================
	// SetTransparency()
	//==============================================================
	/// Enables / disables transparency
	/**
		\param [in] x_b	-	Non-zero to enable transparency.  Zero
							disables transparency.
	*/
	void SetTransparency( oexBOOL x_b ) { m_bTransparent = x_b; }

	//==============================================================
	// GetTransparency()
	//==============================================================
	/// Returns non-zero if transparency is enabled
	oexBOOL GetTransparency() { return m_bTransparent; }

	//==============================================================
	// SetTransparentColor()
	//==============================================================
	/// Sets the transparent color
	/**
		\param [in] x_rgb		-	Transparent color
	*/
	void SetTransparentColor( oexSColorRef x_rgb ) { m_rgbTransparent = x_rgb; }

	//==============================================================
	// GetTransparentColor()
	//==============================================================
	/// Returns the transparent color value
	oexSColorRef GetTransparentColor() { return m_rgbTransparent; }

	//==============================================================
	// LPRECT()
	//==============================================================
	/// Returns the size and position of the image
	operator oexSRect*() { return GetRect(); }

	//==============================================================
	// GetRect()
	//==============================================================
	/// Returns the size and position of the image
	oexSRect* GetRect()
	{	m_rect.top = 0; m_rect.left = 0;
		m_rect.right = GetWidth(); m_rect.bottom = GetHeight();
		return &m_rect;
	}

	//==============================================================
	// GetEncBuffer()
	//==============================================================
	/// Returns a pointer to the internal encoded image buffer
	/**
		This buffer is only valid after a call to Encode()

		\return Pointer to the internal encoded image buffer

		\see
	*/
	oexPBYTE GetEncBuffer() { return m_pMem; }

	//==============================================================
	// GetEncBufferSize()
	//==============================================================
	/// Returns the size of the internal encoded image buffer
	/**
		This buffer is only valid after a call to Encode()

		\return Size of the internal encoded image buffer

		\see
	*/
	oexINT GetEncBufferSize() { return m_nMem; }


	//==============================================================
	// CreateHBITMAP()
	//==============================================================
	/// Creates HBITMAP handle from the current image
	/**
		You must pass the handle to DeleteObject() when it is no
		longer needed.

		\return Handle to bitmap or NULL if error.

		\see
	*/
//	HBITMAP CreateHBITMAP( HDC hDC = NULL );

	//==============================================================
	// operator HBITMAP()
	//==============================================================
	/// Creates HBITMAP handle from the current image
	/**
		You must pass the handle to DeleteObject() when it is no
		longer needed.

		\return Handle to bitmap or NULL if error.

		\see
	*/
//	operator HBITMAP() { return CreateHBITMAP(); }


#ifdef OEX_ENABLE_OPENCV

	//==============================================================
	// InitIplHeader()
	//==============================================================
	/// Initializes the Intel IplImage structure from image
	/**
		\param [in] pImg	-	Pointer to CWinImg object
		\param [out] pIpl	-	Pointer to IplImage structure

		Note, this function does not copy the image!  It only fills
		in pIpl with appropriate values.  When the CWinImg object is
		destroyed, the IplImage structure is no longer valid.

		\return The pointer in pIpl

		\see
	*/
	static IplImage* InitIplHeader( CImage *pImg, IplImage *pIpl );

	//==============================================================
	// InitIplHeader()
	//==============================================================
	/// Initializes the Intel IplImage structure from image
	/**
		\param [out] pIpl	-	Pointer to IplImage structure

		Note, this function does not copy the image!  It only fills
		in pIpl with appropriate values.  When the CWinImg object is
		destroyed, the IplImage structure is no longer valid.

		\return The pointer in pIpl

		\see
	*/
	IplImage* InitIplHeader( IplImage *pIpl )
	{	return InitIplHeader( this, pIpl ); }

#endif

private:

	/// Non-zero if transparency is enabled
	oexBOOL			m_bTransparent;

	/// Transparent color
	oexSColorRef	m_rgbTransparent;

private:

	/// Pointer to CxImage object
	void			*m_pimg;

	/// Pointer to JPEG encoder state structure
	void			*m_pEncoderState;

	/// Pointer to encoder memory created by CxImage
	oexPBYTE		m_pMem;

	/// Size of the buffer in m_pMem
	oexINT			m_nMem;

	/// The filename of the current image if available
	CStr			m_filename;

	/// Size and position of the current image
	oexSRect		m_rect;

};

#endif // OEX_ENABLE_XIMAGE
