/*------------------------------------------------------------------
// auto_str.h
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

/// Generic string class with cast operators
/**
    
  \code

  \endcode

*/
template < typename T > class TAutoStr : 
    public TStr< T >
{
public:

    /// Constructor
    TAutoStr() : TStr() {}
    TAutoStr( oexCONST TStr &str ) : TStr( (TStr&)str ) {}
    TAutoStr( oexCONST TAutoStr &str ) : TStr( (TStr&)str ) {}
    TAutoStr( oexCONST T *pStr ) : TStr( pStr ) {}
    TAutoStr( oexCONST T *pStr, oexUINT uSize ) : TStr( pStr, uSize ) {}
    TAutoStr( oexCONST T *pStr, oexINT nStart, oexUINT uLen ) : TStr( pStr, nStart, uLen ) {}
    TAutoStr( oexCONST T *pStr, oexUINT uSize, oexINT nStart, oexUINT uLen ) : TStr( pStr, uSize, nStart, uLen ) {}
    
    TAutoStr( oexCONST oexINT nVal ) : TStr( nVal ) {}
    TAutoStr( oexCONST oexUINT uVal ) : TStr( uVal ) {}
    TAutoStr( oexCONST oexFLOAT fStr ) : TStr( (oexDOUBLE)fStr ) {}
    TAutoStr( oexCONST oexDOUBLE dStr ) : TStr( dStr ) {}
    TAutoStr( oexCONST T tVal ) : TStr( tVal ) {}
    
public:

    // Implicit cast
    operator oexCONST oexINT() { return ToInt(); }
    operator oexCONST oexUINT() { return ToUInt(); }
    operator oexCONST oexLONG() { return ToLong(); }
    operator oexCONST oexULONG() { return ToULong(); }
    operator oexCONST oexFLOAT() { return ToFloat(); }
    operator oexCONST oexDOUBLE() { return ToDouble(); }
    operator oexCONST oexCSTR() { return ToString(); }

};

/// Unicode / Multibyte string class
typedef TAutoStr< oexTCHAR >    CAutoStr;

#if ( _MSC_VER < 1300 )

/// Unicode string class
typedef TAutoStr< unsigned short >     CAutoStrW;

#else

/// Unicode string class
typedef TAutoStr< wchar_t >     CAutoStrW;

#endif

/// 8 bit character string
typedef TAutoStr< oexCHAR >     CAutoStr8;

/// 16 bit character string
typedef TAutoStr< oexINT16 >    CAutoStr16;

/// 32 bit character string
typedef TAutoStr< oexINT32 >    CAutoStr32;

/// 64 bit character string
typedef TAutoStr< oexINT64 >    CAutoStr64;
