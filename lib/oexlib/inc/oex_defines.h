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

#define oexPASSED( r )				( 0 <= r )
#define oexFAILED( r )				( 0 > r )

#define oexCONST					const

#define oexNULL						0

#define oexIGNORE					(void)

#define oexVERSION( h, l )			( ( h << 16 ) | l )
#define oexVERSION_MINOR( v )		( v & 0xffff )
#define oexVERSION_MAJOR( v )		( ( v >> 16 ) & 0xffff )

#if defined( OEX_WIN32 )
#	define oexVaList				oexPVOID
#	define oexVaStart( v, p )		( v = ( ( (oexPVOID*)&p ) + 1 ) )
#	define oexVaEnd( v )
#	define oexVaArg( v, t )			( (t)( v++ ) )
#else
#	define oexVaList				__builtin_va_list
#	define oexVaStart				__builtin_va_start
#	define oexVaEnd					__builtin_va_end
#	define oexVaArg					__builtin_va_arg
#endif

#define oexTT( c, s )				( 1 == sizeof( c ) ? ( ( c* )( s ) ) : ( ( c* )( L##s ) ) )
#define oexTC( c, s )				( 1 == sizeof( c ) ? ( ( c )( s ) ) : ( ( c )( L##s ) ) )
#define oexTTEXT( c, s )			oexTT( c, s )

#if defined( oexUNICODE )

#	define oexT( s )				( L##s )
#	define oexTEXT( s )				oexT( s )

#	define oexStrToMb( s )			OEX_NAMESPACE::CStr8().ToMb( s )
#	define oexStrToMbPtr( s )		OEX_NAMESPACE::CStr8().ToMb( s ).Ptr()
#   define oexStrToStr8( s )        OEX_NAMESPACE::CStr8().Cnv( s )
#   define oexStrToStr8Ptr( s )     OEX_NAMESPACE::CStr8().Cnv( s ).Ptr()
#   define oexStrToStrW( s )        ( s )
#   define oexStrToStrWPtr( s )     ( s )
#   define oexStrToBin( s )         OEX_NAMESPACE::CStr8().Bin( s )
#   define oexStrToBinPtr( s )      OEX_NAMESPACE::CStr8().Bin( s ).Ptr()

#	define oexMbToStr( s )			OEX_NAMESPACE::CStrW().ToWc( s )
#	define oexMbToStrPtr( s )		OEX_NAMESPACE::CStrW().ToWc( s ).Ptr()
#   define oexStr8ToStr( s )        OEX_NAMESPACE::CStr().Cnv( s )
#   define oexStr8ToStrPtr( s )     OEX_NAMESPACE::CStr().Cnv( s ).Ptr()
#   define oexStrWToStr( s )        ( s )
#   define oexStrWToStrPtr( s )     ( s )
#   define oexBinToStr( s )         OEX_NAMESPACE::CStr().Bin( s )
#   define oexBinToStrPtr( s )      OEX_NAMESPACE::CStr().Bin( s ).Ptr()

#else

#	define oexT( s )				s
#	define oexTEXT( s )				s

#	define oexStrToMb( s )			( s )
#	define oexStrToMbPtr( s )		( s )
#   define oexStrToStr8( s )        ( s )
#   define oexStrToStr8Ptr( s )     ( s )
#   define oexStrToStrW( s )        OEX_NAMESPACE::CStrW().ToWc( s )
#   define oexStrToStrWPtr( s )     OEX_NAMESPACE::CStrW().ToWc( s ).Ptr()
#   define oexStrToBin( s )         ( s )
#   define oexStrToBinPtr( s )      ( s )

#	define oexMbToStr( s )			( s )
#	define oexMbToStrPtr( s )		( s )
#   define oexStr8ToStr( s )        ( s )
#   define oexStr8ToStrPtr( s )     ( s )
#   define oexStrWToStr( s )        OEX_NAMESPACE::CStr().ToMb( s )
#   define oexStrWToStrPtr( s )     OEX_NAMESPACE::CStr().ToMb( s ).Ptr()
#   define oexBinToStr( s )         ( s )
#   define oexBinToStrPtr( s )      ( s )

#endif

#define oexSizeofArray( a )         ( sizeof( a ) / sizeof( a[ 0 ] ) )

#define oexZeroMemory( p, sz )		OEX_NAMESPACE::os::CSys::Zero( p, sz );

#define oexEMPTY_STRING             oexT( "" )

#if defined( OEX_WIN32 )
#	define oexNL						oexT( "\r\n" )
#else
#	define oexNL						oexT( "\n" )
#endif

#define oexMemCmp					OEX_NAMESPACE::os::CSys::MemCmp
#define oexMemCpy					OEX_NAMESPACE::os::CSys::MemCpy
#define oexMemSet					OEX_NAMESPACE::os::CSys::MemSet

#define oexINIT()                   OEX_NAMESPACE::COex::Init()
#define oexUNINIT()                 OEX_NAMESPACE::COex::Uninit()
#define oexRUN_TESTS()              OEX_NAMESPACE::CTestManager::Run()
#define oexFREE_TESTS()             OEX_NAMESPACE::CTestManager::FreeTests()

#define oexINIT_SOCKETS()           OEX_NAMESPACE::os::CIpSocket::InitSockets()
#define oexUNINIT_SOCKETS()         OEX_NAMESPACE::os::CIpSocket::InitSockets()

#if OEX_BIGENDIAN
#	define oexLittleEndian( l )		OEX_NAMESPACE::cmn::RevBytes( l )
#	define oexBigEndian( l )		( l )
#else
#	define oexLittleEndian( l )		( l )
#	define oexBigEndian( l )		OEX_NAMESPACE::cmn::RevBytes( l )
#endif

// File functions
#define oexDeletePath				OEX_NAMESPACE::os::CFile::DeletePath
#define oexCreatePath				OEX_NAMESPACE::os::CFile::CreatePath
#define oexDelete					OEX_NAMESPACE::os::CFile::Delete
#define oexExists					OEX_NAMESPACE::os::CFile::Exists
#define oexBuildPath				OEX_NAMESPACE::CStr::BuildPath
#define oexGetModuleFileName		OEX_NAMESPACE::os::CBaseFile::GetModFileName
#define oexGetModulePath			OEX_NAMESPACE::os::CBaseFile::GetModPath

// Time functions
#define oexLocalTime				OEX_NAMESPACE::CSysTime( OEX_NAMESPACE::CSysTime::eLocalTime )
#define oexGmtTime					OEX_NAMESPACE::CSysTime( OEX_NAMESPACE::CSysTime::eGmtTime )
#define oexLocalTimeStr				OEX_NAMESPACE::CSysTime( OEX_NAMESPACE::CSysTime::eLocalTime ).FormatTime
#define oexGmtTimeStr				OEX_NAMESPACE::CSysTime( OEX_NAMESPACE::CSysTime::eGmtTime ).FormatTime

#define oexGuidToString				OEX_NAMESPACE::CStr().GuidToString
#define oexStringToGuid				OEX_NAMESPACE::CStr().GuidToString

/// oexRETRY() - Retry something up to m times
/**
	\param [in] m - maximum number of times to retry
	\param [in] s - statement to retry
	\param [in] w - failure condition i.e. 'while( w )'

	This is mostly for linux where lot's of functions return EINTR and must be retried

	example: to call gethostbyname() properly

    struct hostent *pHe;
	oexRETRY( 100, pHe = gethostbyname( oexStrToStr8Ptr( x_pServer ) ), !pHe && EINTR == h_errno );

	if ( !pHe )
		// handle failure
		;

*/
#define oexRETRY( m, s, w, r )		( { OEX_NAMESPACE::oexUINT um = m, max = (OEX_NAMESPACE::oexUINT)( um ? um : 1 ); do { s; } while ( --max && w ); \
								 	  if ( !max ) { oexERROR( -1, OEX_NAMESPACE::CStr().Fmt( oexT( "oexRETRY( %s ) failed after %d attempts" ), #s, um ) ); } r; } )

/// Same as above, but with a default of 1000 times
#define oexDO( s, w, r )			oexRETRY( 1000, s, w, r )

/// Creates a four character code
#define oexFOURCC( a, b, c, d )  ( (OEX_NAMESPACE::oexULONG)(						\
								  	  ( ( (OEX_NAMESPACE::oexULONG)a ) << 24 ) 	\
									| ( ( (OEX_NAMESPACE::oexULONG)b ) <<  8 ) 	\
									| ( ( (OEX_NAMESPACE::oexULONG)c ) >>  8 ) 	\
									| ( ( (OEX_NAMESPACE::oexULONG)d ) >> 24 ) 	\
								 ) )
								 
#if defined( oexUSE_EXCEPTIONS )
#	define oexTRY				try
#	define oexCATCH( s )		catch( s )
#	define oexCATCH_ALL()		catch( ... )
#	define oexTHROW( s )		throw( s )
#else
#	define oexTRY
#	define oexCATCH( s )		if ( 0 )
#	define oexCATCH_ALL() 		if ( 0 )
#	define oexTHROW( s )
#endif

