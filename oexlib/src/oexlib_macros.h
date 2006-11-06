/*------------------------------------------------------------------
// oexlib_macros.h
// Copyright (c) 2006 
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

// Unicode
#define oexT

// Pointers
#define oexTYPEOF_PTR				unsigned int
#define oexSIZEOF_PTR				( sizeof( oexTYPEOF_PTR ) )

#define oexMIN_VAL( a, b )			( a < b ? a : b )
#define oexMAX_VAL( a, b )			( a > b ? a : b )
#define oexMIN_VAL_3D( a, b, c )	( a < b ? ( a < c ? a : c ) : ( b < c ? b : c ) )
#define oexMAX_VAL_3D( a, b, c )	( a > b ? ( a > c ? a : c ) : ( b > c ? b : c ) )
#define oexDIF_VAL( a, b )			( a > b ? a - b : b - a )

// Macro strings
#define OEXLIB_STR( x )				#x
#define OEXLIB_XSTR( x )			OEXLIB_STR( x )

// True / False
#define oexTRUE						( 1	)
#define oexFALSE					( 0 )

// Return codes
#define oexRES						int
#define oexOK						( 0 )
#define oexERR						( -1 )

// Return code checks
#define oexSUCCEEDED( s )			( ( s >= oexOK ) ? oexTRUE : oexFALSE )
#define oexFAILED( s )				( ( s < oexOK ) ? oexTRUE : oexFALSE )

// Bool conversions
#define oexBOOL						oexRES
#define oexTO_BOOL( s )				( ( s ) ? oexTRUE : oexFALSE )

// CRLF
#define oexCRLF						"\r\n"

// Rectangle macros
#define oexRW( rect )				( rect.right - rect.left )
#define oexRH( rect )				( rect.bottom - rect.top )
#define oexPRW( rect )				( rect->right - rect->left )
#define oexPRH( rect )				( rect->bottom - rect->top )

#define oexGETBIT32( buf, bit )		\
	( ( buf[ ( bit & 0xfffffff8 ) >> 3 ] & ( 0x01 << ( bit & 0x07 ) ) ) ? 1 : 0 )
#define oexSETBIT32( buf, bit, set )\
	( ( set ) ? buf[ ( bit & 0xfffffff8 ) >> 3 ] |= ( 0x01 << ( bit & 0x07 ) ) \
	: buf[ ( bit & 0xfffffff8 ) >> 3 ] &= ~( 0x01 << ( bit & 0x07 ) ) )

