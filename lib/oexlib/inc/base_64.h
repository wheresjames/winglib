/*------------------------------------------------------------------
// base_64.h
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
// CBase64
//
/// For standard base-64 encoding and decoding
/**
	All functions of this class are static.  It is therefore, not
	necessary to create an instance of this class.
*/
//==================================================================
class CBase64
{

	enum
	{
		/// Maximum line length of the output
		eMaxLineLength = 76
	};

public:

	/// Default Constructor
	CBase64();

	/// Destructor
	virtual ~CBase64();


	//==============================================================
	// Encode()
	//==============================================================
	/// Encode data as base-64 string with CRLF
	/**
		\param [in] dst		- Receives encoded string
		\param [in] dstsize	- Size of dst
		\param [in] src		- Data to be encoded
		\param [in] srcsize	- Size of src

		\return Returns number of bytes encoded

		\warning For multiple calls, size must be divisible by four
	*/
	static oexINT Encode( oexSTR dst, oexINT dstsize, oexCONST oexBYTE *src, oexINT srcsize );
	static CStr8 Encode( oexCPVOID src, oexINT srcsize );
	static CStr8 Encode( CStr8 src ) { return Encode( (oexCONST oexBYTE*)src.Ptr(), src.Length() ); }

	//==============================================================
	// Decode()
	//==============================================================
	/// Decode base-64 string
	/**
		\param [in] src				- Data to be decoded
		\param [in] srcsize			- Size of src
		\param [in] dst				- Receives decoded data
		\param [in] dstsize			- Size of dst
		\param [out] done			- Set to non-zero when decoding is done

		\return Returns number of bytes decoded
	*/
	static oexINT Decode( oexCSTR src, oexINT *srcsize, oexBYTE *dst, oexINT *dstsize, oexINT *done );
	static CStr8 Decode( oexCSTR src, oexINT srcsize );
	static CStr8 Decode( CStr8 src ) { return Decode( src.Ptr(), src.Length() ); }


	//==============================================================
	// GetEncodedSize()
	//==============================================================
	/// Calculates size of encoded string
	/**
		\param [in] size	- Size of unencoded data

		\return Returns calculated size of encoded string

		\warning For multiple calls, must be divisible by four
	*/
	static oexINT GetEncodedSize( oexINT size )
	{	oexINT total = ( size * 8 ) / 6;
		switch ( size % 3 )
		{	case 1 : total++; case 2 : total++; }
		total += ( total / eMaxLineLength ) * 2;
		return total;
	}

	//==============================================================
	// GetDecodedSize()
	//==============================================================
	/// Calculates size of decoded string
	/**
		\param [in] size	- Size of encoded data

		\return Returns calculated size of unencoded string

		\warning For multiple calls, must be divisible by four
	*/
	static oexINT GetDecodedSize( oexINT size )
	{	if ( ( size & 0x03 ) != 0 ) return ( ( size * 6 ) / 8 ) + 1;
		return  ( size * 6 ) / 8; }

	/// Base-64 encoding lookup table
	static oexBYTE m_b64encode[];

	/// Base-64 decode lookup table
	static oexBYTE m_b64decode[];

};
