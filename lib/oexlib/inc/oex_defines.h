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

#define oexPASSED( r )				        ( 0 <= r )
#define oexFAILED( r )				        ( 0 > r )

#define oexCONST					        const

#define oexNULL						        0

#define oexIGNORE					        (void)

#define oexVERSION( h, l )			        ( ( h << 16 ) | l )
#define oexVERSION_MINOR( v )		        ( v & 0xffff )
#define oexVERSION_MAJOR( v )		        ( ( v >> 16 ) & 0xffff )

#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

// For tracking allocations
#   define OexAllocNew             OEX_NAMESPACE::CAlloc( __LINE__, oexTEXT( __FILE__ ) ).New
#   define OexAllocDelete          OEX_NAMESPACE::CAlloc( __LINE__, oexTEXT( __FILE__ ) ).Delete
#   define OexAllocResize          OEX_NAMESPACE::CAlloc( __LINE__, oexTEXT( __FILE__ ) ).Resize

#   define OexAllocConstruct       OEX_NAMESPACE::CAlloc( __LINE__, oexTEXT( __FILE__ ) ).Construct
#   define OexAllocConstructArray  OEX_NAMESPACE::CAlloc( __LINE__, oexTEXT( __FILE__ ) ).ConstructArray
#   define OexAllocDestruct        OEX_NAMESPACE::CAlloc( __LINE__, oexTEXT( __FILE__ ) ).Destruct


// Object function macros
#   define OexNew                  _Log( __LINE__, oexTEXT( __FILE__ ) ).New
#   define OexDelete               _Log( __LINE__, oexTEXT( __FILE__ ) ).Delete
#   define OexResize               _Log( __LINE__, oexTEXT( __FILE__ ) ).Resize

#   define OexConstruct            _Log( __LINE__, oexTEXT( __FILE__ ) ).Construct
#   define OexConstructArray       _Log( __LINE__, oexTEXT( __FILE__ ) ).ConstructArray
#   define OexDestruct             _Log( __LINE__, oexTEXT( __FILE__ ) ).Destruct

#   define OexAllocate             _Log( __LINE__, oexTEXT( __FILE__ ) ).Allocate

#else

// For tracking allocations
#   define OexAllocNew             OEX_NAMESPACE::CAlloc().New
#   define OexAllocDelete          OEX_NAMESPACE::CAlloc().Delete
#   define OexAllocResize          OEX_NAMESPACE::CAlloc().Resize

#   define OexAllocConstruct       OEX_NAMESPACE::CAlloc().Construct
#   define OexAllocConstructArray  OEX_NAMESPACE::CAlloc().ConstructArray
#   define OexAllocDestruct        OEX_NAMESPACE::CAlloc().Destruct


// Object function macros
#   define OexNew                  New
#   define OexDelete               Delete
#   define OexResize               Resize

#   define OexConstruct            Construct
#   define OexConstructArray       ConstructArray
#   define OexDestruct             Destruct

#   define OexAllocate             Allocate

#endif

#if defined( OEX_GCC )
#	define oexVaList				        __builtin_va_list
#	define oexVaStart				        __builtin_va_start
#	define oexVaEnd					        __builtin_va_end
#	define oexVaArg					        __builtin_va_arg
#elif defined( OEX_WIN32 )
#	define oexVaList				        OEX_NAMESPACE::oexPVOID
#	define oexVaStart( v, p )		        ( v = ( ( (OEX_NAMESPACE::oexPVOID*)&p ) + 1 ) )
#	define oexVaEnd( v )
#	define oexVaArg( v, t )			        ( (t)( v++ ) )
#else
// +++ Please fix this, don't want windows.h in the name space
#if defined( OEX_WINCE )
#	define WIN32_WINNT 0x0400
#	include <windows.h>
#endif
#	include <stdarg.h>
#	define oexVaList				        va_list
#	define oexVaStart		    		    va_start
#	define oexVaEnd						    va_end
#	define oexVaArg					        va_arg
#endif

#define oexTT( c, s )				        ( 1 == sizeof( c ) ? ( ( c* )( s ) ) : ( ( c* )( L##s ) ) )
#define oexTC( c, s )				        ( 1 == sizeof( c ) ? ( ( c )( s ) ) : ( ( c )( L##s ) ) )
#define oexTTEXT( c, s )			        oexTT( c, s )

#if defined( OEX_WINDOWS )
#	define oexPathSep			'\\'
#	define oexTCPathSep( c )	oexTC( c, '\\' )
#else
#	define oexPathSep			'/'
#	define oexTCPathSep( c )	oexTC( c, '/' )
#endif

// Pointer conversion
#define oexPtrToInt( p )	OEX_NAMESPACE::__oexCPtrCnv( p ).nInt
#define oexPtrToUInt( p ) 	OEX_NAMESPACE::__oexCPtrCnv( p ).uInt
#define oexPtrToLong( p ) 	OEX_NAMESPACE::__oexCPtrCnv( p ).lInt
#define oexPtrToULong( p ) 	OEX_NAMESPACE::__oexCPtrCnv( p ).ulInt
#define oexPtrToInt64( p ) 	OEX_NAMESPACE::__oexCPtrCnv( p ).llInt
#define oexPtrToUInt64( p ) OEX_NAMESPACE::__oexCPtrCnv( p ).ullInt
#define oexPtrToPtr( p ) OEX_NAMESPACE::__oexCPtrCnv( p ).ptr

#define oexStrWToMb( s )			        OEX_NAMESPACE::CStr8().ToMb( s )
#define oexStrWToMbPtr( s )		        	OEX_NAMESPACE::CStr8().ToMb( s ).Ptr()
#define oexMbToStrW( s )			        OEX_NAMESPACE::CStrW().ToWc( s )
#define oexMbToStrWPtr( s )		  		    OEX_NAMESPACE::CStrW().ToWc( s ).Ptr()

#if defined( oexUNICODE )

#	define oexT( s )				        ( L##s )
#	define oexTEXT( s )				        oexT( s )

#	define oexPT( s )				        ( L##s )
#	define oexPTEXT( s )			        oexPT( s )

#	define oexStrToMb( s )			        OEX_NAMESPACE::CStr8().ToMb( s )
#	define oexStrToMbPtr( s )		        OEX_NAMESPACE::CStr8().ToMb( s ).Ptr()
#   define oexStrToStr8( s )                OEX_NAMESPACE::CStr8().Cnv( s )
#   define oexStrToStr8Ptr( s )             OEX_NAMESPACE::CStr8().Cnv( s ).Ptr()
#   define oexStrToStrW( s )                ( s )
#   define oexStrToStrWPtr( s )             ( s )
#   define oexStrToBin( s )                 OEX_NAMESPACE::CStr8().Bin( s )
#   define oexStrToBinPtr( s )              OEX_NAMESPACE::CStr8().Bin( s ).Ptr()

#	define oexMbToStr( s )			        OEX_NAMESPACE::CStrW().ToWc( s )
#	define oexMbToStrPtr( s )		        OEX_NAMESPACE::CStrW().ToWc( s ).Ptr()
#   define oexStr8ToStr( s )                OEX_NAMESPACE::CStr().Cnv( s )
#   define oexStr8ToStrPtr( s )             OEX_NAMESPACE::CStr().Cnv( s ).Ptr()
#   define oexStrWToStr( s )                ( s )
#   define oexStrWToStrPtr( s )             ( s )
#   define oexBinToStr( s )                 OEX_NAMESPACE::CStr().Bin( s )
#   define oexBinToStrPtr( s )              OEX_NAMESPACE::CStr().Bin( s ).Ptr()

#else

#	define oexT( s )				        s
#	define oexTEXT( s )				        s

#if defined( OEX_WINCE )
#	define oexPT( s )				        ( L##s )
#	define oexPTEXT( s )			        oexPT( s )
#else
#	define oexPT( s )				        s
#	define oexPTEXT( s )			        s
#endif

#	define oexStrToMb( s )			        ( s )
#	define oexStrToMbPtr( s )		        ( s )
#   define oexStrToStr8( s )                ( s )
#   define oexStrToStr8Ptr( s )             ( s )
#   define oexStrToStrW( s )                OEX_NAMESPACE::CStrW().ToWc( s )
#   define oexStrToStrWPtr( s )             OEX_NAMESPACE::CStrW().ToWc( s ).Ptr()
#   define oexStrToBin( s )                 ( s )
#   define oexStrToBinPtr( s )              ( s )

#	define oexMbToStr( s )			        ( s )
#	define oexMbToStrPtr( s )		        ( s )
#   define oexStr8ToStr( s )                ( s )
#   define oexStr8ToStrPtr( s )             ( s )
#   define oexStrWToStr( s )                OEX_NAMESPACE::CStr().ToMb( s )
#   define oexStrWToStrPtr( s )             OEX_NAMESPACE::CStr().ToMb( s ).Ptr()
#   define oexBinToStr( s )                 ( s )
#   define oexBinToStrPtr( s )              ( s )

#endif

#define oexSizeOfArray( a )         		( sizeof( a ) / sizeof( a[ 0 ] ) )

#define oexZeroMemory( p, sz )				OEX_NAMESPACE::os::CSys::Zero( p, sz );

#define oexEMPTY_STRING             		oexT( "" )

#if defined( OEX_WIN32 )
#	define oexNL							oexT( "\r\n" )
#	define oexNL8							"\r\n"
#else
#	define oexNL							oexT( "\n" )
#	define oexNL8							"\n"
#endif

#define oexMemCmp							OEX_NAMESPACE::os::CSys::MemCmp
#define oexMemCpy							OEX_NAMESPACE::os::CSys::MemCpy
#define oexMemSet							OEX_NAMESPACE::os::CSys::MemSet

#define oexINIT()                   		OEX_NAMESPACE::COex::Init()
#define oexUNINIT()                 		OEX_NAMESPACE::COex::Uninit()
#define oexRUN_TESTS()              		OEX_NAMESPACE::CTestManager::Run()
#define oexFREE_TESTS()             		OEX_NAMESPACE::CTestManager::FreeTests()

#define oexINIT_SOCKETS()         			OEX_NAMESPACE::os::CIpSocket::InitSockets()
#define oexUNINIT_SOCKETS()         		OEX_NAMESPACE::os::CIpSocket::InitSockets()

#if OEX_BIGENDIAN
#	define oexLittleEndian( l )				OEX_NAMESPACE::cmn::RevBytes( l )
#	define oexBigEndian( l )				( l )
#else
#	define oexLittleEndian( l )				( l )
#	define oexBigEndian( l )				OEX_NAMESPACE::cmn::RevBytes( l )
#endif

// String functions
#define oexMks								OEX_NAMESPACE::CStr().Mks
#define oexMks8								OEX_NAMESPACE::CStr8().Mks
#define oexMksW								OEX_NAMESPACE::CStrW().Mks

// File functions
#define oexDeletePath						OEX_NAMESPACE::CFile::DeletePath
#define oexCreatePath						OEX_NAMESPACE::CFile::CreatePath
#define oexDelete							OEX_NAMESPACE::CFile::Delete
#define oexExists							OEX_NAMESPACE::CFile::Exists
#define oexBuildPath						OEX_NAMESPACE::CStr::BuildPath
#define oexGetPath( f )						OEX_NAMESPACE::CStr( f ).GetPath()
#define oexGetFileName( f )					OEX_NAMESPACE::CStr( f ).GetFileName()
#define oexGetFileExtension( f )			OEX_NAMESPACE::CStr( f ).GetFileExtension()
#define oexGetModuleFileName				OEX_NAMESPACE::os::CBaseFile::GetModFileName
#define oexGetModulePath					OEX_NAMESPACE::os::CBaseFile::GetModPath
#define oexFileGetContents( f )				OEX_NAMESPACE::CFile().OpenExisting( f ).Read()
#define oexFilePutContents( f, c )			OEX_NAMESPACE::CFile().CreateAlways( f ).Write( c )
#define oexFilePutContentsLen( f, c, l )	OEX_NAMESPACE::CFile().CreateAlways( f ).Write( c, l )

// Time functions
#define oexLocalTime						OEX_NAMESPACE::CSysTime( OEX_NAMESPACE::CSysTime::eLocalTime )
#define oexGmtTime							OEX_NAMESPACE::CSysTime( OEX_NAMESPACE::CSysTime::eGmtTime )
#define oexLocalTimeStr						OEX_NAMESPACE::CSysTime( OEX_NAMESPACE::CSysTime::eLocalTime ).FormatTime
#define oexGmtTimeStr						OEX_NAMESPACE::CSysTime( OEX_NAMESPACE::CSysTime::eGmtTime ).FormatTime
#define oexGetUnixTime						OEX_NAMESPACE::os::CSys::GetUnixTime

#define oexSleep							OEX_NAMESPACE::os::CSys::Sleep
#define oexGetBootCount						OEX_NAMESPACE::os::CHqTimer::GetBootCount
#define oexGetBootSeconds					OEX_NAMESPACE::os::CHqTimer::GetTimerSeconds
#define oexMicroSleep						OEX_NAMESPACE::os::CSys::MicroSleep
#define oexPrintf							OEX_NAMESPACE::os::CSys::Printf
#define oexEcho								OEX_NAMESPACE::os::CSys::Echo
#define oexFmt								OEX_NAMESPACE::CStr().Fmt
#define oexAlert							OEX_NAMESPACE::os::CSys::ShowMessageBox
#define oexFork								OEX_NAMESPACE::os::CSys::Fork

#define oexGuidToString						OEX_NAMESPACE::CStr().GuidToString
#define oexUniqueGuid( g )					OEX_NAMESPACE::guid::StringToGuid( g, (OEX_NAMESPACE::oexCSTR)NULL, 0 )

// Threads
#define oexGetCurThreadId					OEX_NAMESPACE::os::CSys::GetCurThreadId
#define oexLock								OEX_NAMESPACE::CLock
#define oexAutoLock							OEX_NAMESPACE::CScopeLock
#define oexEvent							OEX_NAMESPACE::CEvent
#define oexSignal							OEX_NAMESPACE::CSignal
#define oexWaitMultiple						OEX_NAMESPACE::CResource::WaitMultiple
#define oexInterlockedIncrement				OEX_NAMESPACE::os::CSys::InterlockedIncrement
#define oexInterlockedDecrement				OEX_NAMESPACE::os::CSys::InterlockedDecrement
#define oexCloseLog							OEX_NAMESPACE::CLog::GlobalLog().Destroy

// std library
#define oexStdTString( t )	                std::basic_string< t, std::char_traits< t >, OEX_NAMESPACE::COexStdAllocator< t > >
#define oexStdString		                oexStdTString( OEX_NAMESPACE::oexTCHAR )
#define oexStdMap( k, v )	                std::map< k, v, std::less< k >, OEX_NAMESPACE::COexStdAllocator< k > >
#define oexStdList( t )		                std::list< t, OEX_NAMESPACE::COexStdAllocator< t > >
#define oexStdVector( t )	                std::vector< t, OEX_NAMESPACE::COexStdAllocator< t > >

// Number conversion
#define oexStrToInt64		                OEX_NAMESPACE::os::CSys::StrToInt64
#define oexStrToUInt64		                OEX_NAMESPACE::os::CSys::StrToUInt64
#define oexStrToLong		                OEX_NAMESPACE::os::CSys::StrToLong
#define oexStrToULong		                OEX_NAMESPACE::os::CSys::StrToULong
#define oexStrToFloat		                OEX_NAMESPACE::os::CSys::StrToFloat
#define oexStrToDouble		                OEX_NAMESPACE::os::CSys::StrToDouble

#define oexHtmlToRgb						OEX_NAMESPACE::CParser::HtmlToRgb
#define oexRgbToHtml						OEX_NAMESPACE::CParser::oexRgbToHtml


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

#if defined( oexUSE_EXCEPTIONS ) && !defined( OEX_NOEXCEPTIONS )
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

// This is for things that *must* be caught, like 3rd party libraries,
// of course, sometimes, exceptions just aren't available, ;)
#if !defined( OEX_NOEXCEPTIONS )
#	define _oexTRY				try
#	define _oexCATCH( s )		catch( s )
#	define _oexCATCH_ALL()		catch( ... )
#	define _oexTHROW( s )		throw( s )
#else
#	define _oexTRY
#	define _oexCATCH( s )		if ( 0 )
#	define _oexCATCH_ALL() 		if ( 0 )
#	define _oexTHROW( s )
#endif

