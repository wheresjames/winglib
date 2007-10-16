/*------------------------------------------------------------------
// oex_defines.h
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

#define oexSUCCEEDED( r )			( 0 <= r )
#define oexFAILED( r )				( 0 > r )

#define oexCONST					const

#define oexNULL						0

#define oexTT( c, s )				( 1 == sizeof( c ) ? ( ( c* )( s ) ) : ( ( c* )( L##s ) ) )
#define oexTC( c, s )				( 1 == sizeof( c ) ? ( ( c )( s ) ) : ( ( c )( L##s ) ) )
#define oexTTEXT( c, s )			oexTT( c, s )

#if defined( oexUNICODE )

#	define oexT( s )				( L##s )
#	define oexTEXT( s )				oexT( s )

#   define oexStrToStr8( s )        OEX_NAMESPACE::CStr8().Cnv( s )
#   define oexStrToStr8Ptr( s )     OEX_NAMESPACE::CStr8().Cnv( s ).Ptr()
#   define oexStrToStrW( s )        ( s )
#   define oexStrToStrWPtr( s )     ( s )
#   define oexStrToBin( s )         OEX_NAMESPACE::CStr8().Bin( s )
#   define oexStrToBinPtr( s )      OEX_NAMESPACE::CStr8().Bin( s ).Ptr()

#   define oexStr8ToStr( s )        OEX_NAMESPACE::CStr().Cnv( s )
#   define oexStr8ToStrPtr( s )     OEX_NAMESPACE::CStr().Cnv( s ).Ptr()
#   define oexStrWToStr( s )        ( s )
#   define oexStrWToStrPtr( s )     ( s )
#   define oexBinToStr( s )         OEX_NAMESPACE::CStr().Bin( s )
#   define oexBinToStrPtr( s )      OEX_NAMESPACE::CStr().Bin( s ).Ptr()

#else

#	define oexT( s )				s
#	define oexTEXT( s )				s

#   define oexStrToStr8( s )        ( s )
#   define oexStrToStr8Ptr( s )     ( s )
#   define oexStrToStrW( s )        OEX_NAMESPACE::CStrW().Set( s )
#   define oexStrToStrWPtr( s )     OEX_NAMESPACE::CStrW().Set( s ).Ptr()
#   define oexStrToBin( s )         ( s )
#   define oexStrToBinPtr( s )      ( s )

#   define oexStr8ToStr( s )        ( s )
#   define oexStr8ToStrPtr( s )     ( s )
#   define oexStrWToStr( s )        OEX_NAMESPACE::CStr().Set( s )
#   define oexStrWToStrPtr( s )     OEX_NAMESPACE::CStr().Set( s ).Ptr()
#   define oexBinToStr( s )         ( s )
#   define oexBinToStrPtr( s )      ( s )

#endif

#define oexSizeofArray( a )         ( sizeof( a ) / sizeof( a[ 0 ] ) )

#define oexEMPTY_STRING             oexT( "" )

#define oexNL						oexT( "\r\n" )

#define oexINIT()                   OEX_NAMESPACE::COex::Init()
#define oexUNINIT()                 OEX_NAMESPACE::COex::Uninit()
#define oexRUN_TESTS()              OEX_NAMESPACE::CTestManager::Run()
#define oexFREE_TESTS()             OEX_NAMESPACE::CTestManager::FreeTests()

#define oexINIT_SOCKETS()           OEX_NAMESPACE::os::CIpSocket::InitSockets()
#define oexUNINIT_SOCKETS()         OEX_NAMESPACE::os::CIpSocket::InitSockets()


