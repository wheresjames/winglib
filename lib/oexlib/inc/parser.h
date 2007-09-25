/*------------------------------------------------------------------
// parser.h
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

// Special List types
typedef TList< CStr > CStrList;
typedef TList< CStrW > CStrWList;
typedef TList< CStr8 > CStr8List;
typedef TList< CStr16 > CStr16List;
typedef TList< CStr32 > CStr32List;

typedef TAssoList< CStr, CStr > CStrAssoList;
typedef TAssoList< CStrW, CStrW > CStrWAssoList;
typedef TAssoList< CStr8, CStr8 > CStr8AssoList;
typedef TAssoList< CStr16, CStr16 > CStr16AssoList;
typedef TAssoList< CStr32, CStr32 > CStr32AssoList;

class CParser
{
private:

	CParser() {}

	virtual ~CParser() {}

public:

	/// Parsers a list by breaking it at separator strings
	static CStrList Explode( oexCSTR pStr, oexUINT uSize, oexCSTR pSep, oexUINT uSep );
	static CStrList Explode( oexCSTR pStr, oexCSTR pSep )
	{	return Explode( pStr, zstr::Length( pStr ), pSep, zstr::Length( pSep ) ); }
	static CStrList Explode( CStr sStr, oexCSTR pSep )
	{	return Explode( sStr.Ptr(), sStr.Length(), pSep, zstr::Length( pSep ) ); }
	static CStrList Explode( CStr sStr, CStr sSep )
	{	return Explode( sStr.Ptr(), sStr.Length(), sSep.Ptr(), sSep.Length() ); }

	template < class T_LIST >
		static CStr Implode( T_LIST lst, oexCSTR pSep )
	{	CStr str;
		for ( T_LIST::iterator it; lst.Next( it ); )
		{	if ( str.Length() ) str << pSep;
			str << it->ToString();
		} // end for
		return str;
	}

	/// Returns a list of tokens from a string
	static CStrList GetTokens( oexCSTR pStr, oexCSTR pValid );

	/// Splits a string into an array by cutting on any character in pSep
	static CStrList CParser::Split( oexCSTR pStr, oexUINT uSize, oexCSTR pSep );
	static CStrList Split( oexCSTR pStr, oexCSTR pSep )
	{	return Split( pStr, zstr::Length( pStr ), pSep ); }
	static CStrList Split( CStr str, oexCSTR pSep )
	{	return Split( str.Ptr(), str.Length(), pSep ); }

	/// Parses an array template
//	static CPropertyBag CompileTemplate( CStr str );

	/// Compile the statement
//	static oexBOOL CompileStatement( CStr &str, CPropertyBag &pb );

	/// Encodes a string from a format definition
//	static CStr Encode( CPropertyBag &enc, CPropertyBag &fmt, CStr root = "*" );
//	static CStr Encode( CPropertyBag &enc, CStr str )
//	{	return Encode( enc, CompileTemplate( str ) ); }

	/// Encodes a single parameter
//	static oexBOOL EncodeParam( CStr& str, CStr &key, CStr &val, CPropertyBag fmt, oexBOOL bMore );
//	static oexBOOL EncodeParam( CStr& str, CStr &key, CPropertyBag &val, CPropertyBag &fmt, oexBOOL bFirst, oexBOOL bLast );

	/// Decodes url type params such as "a=b&c=d"
	static CPropertyBag DecodeUrlParams( CStr str );

	/// Encodes url type params such as "a=b&c=d"
	static CStr EncodeUrlParams( CPropertyBag pb );

	/// Returns non-zero if the character is a valid url character
	static oexBOOL CParser::IsUrlChar( oexTCHAR ch );

	/// Encoded a url string "Hello World" -> "Hello%20World"
	static CStr CParser::UrlEncode( CStr str );

	/// Decodes a url string "Hello%20World" -> "Hello World"
	static CStr CParser::UrlDecode( CStr str );

    /// Generic property bag deserializing
    static CPropertyBag Deserialize( oexCSTR x_pStr )
    {   CPropertyBag pb; Deserialize( CStr( x_pStr ), pb ); return pb; }

    /// Generic property bag deserializing
    static CPropertyBag Deserialize( CStr &x_sStr )
    {   CPropertyBag pb; Deserialize( x_sStr, pb ); return pb; }

    /// Generic property bag deserializing
    static oexLONG Deserialize( CStr &x_sStr, CPropertyBag &x_pb, oexBOOL x_bMerge = oexFALSE, oexLONG *x_pLast = oexNULL );

    /// Generic property bag serializing
    static CStr Serialize( CPropertyBag &x_pb );

    /// Returns the next token in x_sStr if it is in lst
    static CStr GetToken( CStr &x_sStr, CStrList x_lst, oexBOOL x_bCaseSensitive );

    /// Returns the next token in x_sStr if it is in lst
    static CStr ParseToken( CStr &x_sStr, CStrList x_lst, oexBOOL x_bCaseSensitive );

    /// Decode MIME
    static CPropertyBag DecodeMIME( CStr &x_sStr );

    /// Encode MIME
    static CStr EncodeMime( CPropertyBag &x_pb );

protected:



};


