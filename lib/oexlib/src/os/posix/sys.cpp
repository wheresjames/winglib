/*------------------------------------------------------------------
// sys.cpp
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

/*
   oOoOo
,==|||||
|| |||||
|| |||||
'==#####

*/

#include "oexlib.h"


#include "std_os.h"
#include <termios.h>
#include <sys/reboot.h>

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

//oexSTATIC_ASSERT( oexSTRSIZE >= MAX_PATH );

//oexSTATIC_ASSERT( sizeof( uuid_t ) == sizeof( oexGUID ) );

//oexSTATIC_ASSERT( sizeof( CSys::t_WAITABLE ) == sizeof( HANDLE ) );

const oexUINT		CSys::c_StrErr_OK = 0;
const oexUINT		CSys::c_StrErr_INVALID_PARAMETER = (oexUINT)-1;
const oexUINT		CSys::c_StrErr_INSUFFICIENT_BUFFER = (oexUINT)-2;

//const oexUINT	    CSys::c_Infinite = INFINITE;
//const oexUINT		CSys::c_uMaximumWaitObjects = MAXIMUM_WAIT_OBJECTS;

//oexSTATIC_ASSERT( CSys::eMaximumWaitObjects == MAXIMUM_WAIT_OBJECTS );

#if !defined( OEX_CPU_64 )

void* CSys::CThunk::Stdcall2This( const void *pThis, void *pFun  )
{
	return 0;
/*
	p = (SThunkDataStdcall*)malloc( 250 );
	if ( !p )
		return 0;

	// Enable execute permissions
	long pgsz = sysconf( _SC_PAGE_SIZE );
	if ( 0 < pgsz && !( pgsz | ( pgsz - 1 ) ) )
	{	long sz = pgsz, root = oexPtrToULong( p ) & ~pgsz;
		while ( sz && ( oexPtrToULong( p ) - root ) + sz < sizeof( SThunkDataStdcall ) )
			sz += pgsz;
		if ( 0 < sz )
			mprotect( (void*)root, sz + pgsz, PROT_READ | PROT_WRITE | PROT_EXEC )
	} // end if

	cacheflush( p, sizeof( SThunkDataStdcall ), ICACHE );

	return p;
*/
}

void* CSys::CThunk::CDecl2This( const void *pThis, void *pFun, long lCleanup )
{
	return 0;
}

void CSys::CThunk::Release()
{
	if ( m_p )
		free( m_p ), m_p = 0;
}

#endif

SResInfo* SResInfo::Create( oexCSTR x_pName )
{
	SResInfo *pRi = new SResInfo;
	if ( !oexCHECK( pRi ) )
		return oexNULL;

	// Initialize event structure
	oexZeroMemory( pRi, sizeof( SResInfo ) );

	if ( x_pName && *x_pName )
	{
		// Save name
		pRi->pName = new CStr( x_pName );
		if ( !oexCHECK( pRi->pName ) )
		{	SResInfo::Release( pRi );
			return oexNULL;
		} // end if

	} // end if

	return pRi;
}

void SResInfo::Release( SResInfo *x_pRi )
{
	if ( !x_pRi )
		return;

	oexVERIFY_PTR( x_pRi );

	// Lose any name object
	if ( x_pRi->pName )
	{	oexVERIFY_PTR( x_pRi->pName );
		delete (CStr*)x_pRi->pName;
	} // end if

	delete x_pRi;
}

oexUINT CSys::Rand( oexPVOID p, oexUINT sz )
{
	if ( !p )
		return 0;

	// Randomize the buffer
	unsigned char *buf = (unsigned char*)p;
	while ( sz )
		*(buf++) = (unsigned char)rand(), sz--;

	return sz;
}

oexUINT CSys::Rand( oexUINT uMax )
{
	oexUINT u;
	Rand( &u, sizeof( oexUINT ) );
	return uMax ? ( u % uMax ) : u;
}

void CSys::RandSeed( oexUINT uSeed )
{
	srand( uSeed );
}

oexINT CSys::ShowMessageBox( oexCSTR x_pTitle, oexCSTR x_pStr )
{
#if !defined( OEX_NOMSGBOX )

	if ( !oexCHECK_PTR( x_pTitle ) )
		x_pTitle = oexT( "" );
 	if ( !oexCHECK_PTR( x_pStr ) )
		x_pStr = oexT( "" );

//	printf( oexT( "%s : %s" ), x_pTitle, x_pStr );

	oexINT nRet = system( oexStrToMbPtr( oexMks( oexT( "xmessage -center \"" ), x_pTitle, oexT( " : " ), x_pStr, oexT( "\"" ) ).Ptr() ) );
	if ( -1 == nRet )
	{	oexERROR( errno, oexT( "Failed to dislay dialog box" ) );
		oexNOTICE( 0, oexMks( x_pTitle, oexT( " : " ), x_pStr ) );
		return -1;
	} // end if

	// +++ how's this work???
	if ( WIFSIGNALED( nRet ) && ( WTERMSIG( nRet ) == SIGINT || WTERMSIG( nRet ) == SIGQUIT ) )
		return 0;

	return -2;
#else
	return 0;
#endif
}

oexCPVOID CSys::GetInstanceHandle()
{
	return (oexCPVOID)(oexLONG)getpid();
}

oexBOOL CSys::SetThreadPriority( oexINT x_nPriority )
{
	return 0;
//	return nice( x_nPriority );
}

oexUINT CSys::SetThreadAffinity( oexUINT x_nAffinity )
{
	return 0;
}

oexINT CSys::Quit( oexINT x_nReturnCode )
{	return -1;
}

oexINT CSys::Flush_stdout()
{	return fflush( stdout );
}

void CSys::Exit( oexINT x_nRet )
{
	exit( x_nRet );
}

int _is_key( int to )
{	struct timeval tv;
	fd_set read_fd;
	tv.tv_sec = 0;
	tv.tv_usec = to * 1000;
	FD_ZERO( &read_fd );
	FD_SET( 0, &read_fd );
	if ( -1 != select( 1, &read_fd, NULL, NULL, &tv )
	     && FD_ISSET( 0, &read_fd ) )
		return 1;
	return 0;
}

int CSys::IsKey()
{//_STT();
	return _is_key( 0 );
}

int CSys::GetKey()
{//_STT();

	char ch;
	struct termios otios, tios;
	ioctl( 0, TCGETS, &tios );
	otios.c_lflag = tios.c_lflag;
	tios.c_lflag &= ~( ECHO | ICANON );
	ioctl( 0, TCSETS, &tios );
	if ( 0 > read( 0, &ch, 1 ) )
		ch = 0;
	tios.c_lflag = otios.c_lflag;
	ioctl( 0, TCSETS, &tios );
	return ch;
}

oexINT CSys::SetRoot()
{
	return setuid( 0 );
}

oexINT CSys::IsRoot()
{
	return getuid() ? 0 : 1;
}

//#if defined( OEX_ANDROID )
#	define REBOOT( n )	reboot( n )
//#else
//#	define REBOOT( n )	__reboot( 0xfee1dead, 369367448, n, pMsg )
//#endif

oexINT CSys::CtrlComputer( int nCmd, int nForce, oexCSTR pMsg )
{
	if ( !pMsg || !*pMsg )
		pMsg = 0;

	sync();
	switch( nCmd )
	{	case eReboot : return REBOOT( 0x1234567 ); break;
		case eRestart : return REBOOT( 0xa1b2c3d4 ); break;
		case ePowerOff : return REBOOT( 0x4321fedc ); break;
		case eLogOff : break;
		case eShutdown : return REBOOT( 0xcdef0123 ); break;
	} // end switch

	return 0;
}

// **** Multi-byte

oexCSTR8 CSys::StrFmt( oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	oexCSTR8 pStr = vStrFmt( x_pDst, x_uMax, x_pFmt, ap );
	oexVaEnd( ap );
	return pStr;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTR8 CSys::vStrFmt( oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, oexVaList x_pArgs )
{
	return vStrFmt( oexNULL, x_pDst, x_uMax, x_pFmt, x_pArgs );
}

oexCSTR8 CSys::StrFmt( oexRESULT *x_pRes, oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	oexCSTR8 pStr = vStrFmt( x_pRes, x_pDst, x_uMax, x_pFmt, ap );
	oexVaEnd( ap );
	return pStr;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTR8 CSys::vStrFmt( oexRESULT *x_pRes, oexSTR8 x_pDst, oexUINT x_uMax, oexCSTR8 x_pFmt, oexVaList x_pArgs )
{
	if ( x_pRes )
		*x_pRes = 0;

	// Verify data pointers
	if ( !oexVERIFY_PTR( x_pDst ) || !oexVERIFY_PTR( x_pFmt ) || !x_uMax )
	{
		if ( x_pRes )
			*x_pRes = c_StrErr_INVALID_PARAMETER;

		return x_pDst;
	} // end if

	oexRESULT res = 0;

	// Create format string
	if ( 0 > vsnprintf( x_pDst, x_uMax, x_pFmt, x_pArgs ) )
	{
		// Null terminate buffer
		x_pDst[ x_uMax - 1 ] = 0;

		// Let the user know what went wrong
		res = c_StrErr_INSUFFICIENT_BUFFER;

	} // end if

	// What to do with the result
	if ( x_pRes )
		*x_pRes = res;

	else
		oexASSERT( 0 == res );

	return x_pDst;
}

oexINT64 CSys::StrToInt64( oexCSTR8 x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
    return strtoll( x_pStr, NULL, x_uRadix );
}

oexUINT64 CSys::StrToUInt64( oexCSTR8 x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
    return strtoll( x_pStr, NULL, x_uRadix );
}

oexLONG CSys::StrToLong( oexCSTR8 x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return strtol( x_pStr, NULL, x_uRadix );
}

oexULONG CSys::StrToULong( oexCSTR8 x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return strtoul( x_pStr, NULL, x_uRadix );
}

oexFLOAT CSys::StrToFloat( oexCSTR8 x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return strtof( x_pStr, NULL );
}

oexDOUBLE CSys::StrToDouble( oexCSTR8 x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return strtod( x_pStr, NULL );
}

// **** Unicode
#if !defined( OEX_NOWCHAR )

oexCSTRW CSys::StrFmt( oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	oexCSTRW pStr = vStrFmt( x_pDst, x_uMax, x_pFmt, ap );
	oexVaEnd( ap );
	return pStr;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTRW CSys::vStrFmt( oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, oexVaList x_pArgs )
{
	return vStrFmt( oexNULL, x_pDst, x_uMax, x_pFmt, x_pArgs );
}

oexCSTRW CSys::StrFmt( oexRESULT *x_pRes, oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	oexCSTRW pStr = vStrFmt( x_pRes, x_pDst, x_uMax, x_pFmt, ap );
	oexVaEnd( ap );
	return pStr;
}

//	wvsprintf( pDst, pFmt, (va_list)pArgs );
oexCSTRW CSys::vStrFmt( oexRESULT *x_pRes, oexSTRW x_pDst, oexUINT x_uMax, oexCSTRW x_pFmt, oexVaList x_pArgs )
{
	if ( x_pRes )
		*x_pRes = 0;

	// Verify data pointers
	if ( !oexVERIFY_PTR( x_pDst ) || !oexVERIFY_PTR( x_pFmt ) || !x_uMax )
	{
		if ( x_pRes )
			*x_pRes = c_StrErr_INVALID_PARAMETER;

		return x_pDst;
	} // end if

	oexRESULT res = 0;

	// Create format string
	if ( 0 > vswprintf( x_pDst, x_uMax, x_pFmt, x_pArgs ) )
	{
		// Null terminate buffer
		x_pDst[ x_uMax - 1 ] = 0;

		// Let the user know what went wrong
		res = c_StrErr_INSUFFICIENT_BUFFER;

	} // end if

	// What to do with the result
	if ( x_pRes )
		*x_pRes = res;

	else
		oexASSERT( 0 == res );

	return x_pDst;
}

oexINT64 CSys::StrToInt64( oexCSTRW x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
#if !defined( OEX_NOWCSTO )
	return wcstoll( x_pStr, NULL, x_uRadix );
#else
	return 0;
#endif
}

oexUINT64 CSys::StrToUInt64( oexCSTRW x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
#if !defined( OEX_NOWCSTO )
	return wcstoll( x_pStr, NULL, x_uRadix );
#else
	return 0;
#endif
}

oexLONG CSys::StrToLong( oexCSTRW x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstol( x_pStr, NULL, x_uRadix );
}

oexULONG CSys::StrToULong( oexCSTRW x_pStr, oexUINT x_uRadix )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstoul( x_pStr, NULL, x_uRadix );
}

oexFLOAT CSys::StrToFloat( oexCSTRW x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
#if !defined( OEX_NOWCSTO )
	return wcstof( x_pStr, NULL );
#else
	return 0;
#endif
}

oexDOUBLE CSys::StrToDouble( oexCSTRW x_pStr )
{	if ( !oexVERIFY_PTR( x_pStr ) )
		return 0;
	return wcstod( x_pStr, oexNULL );
}

#if defined( OEX_NOWCSTO )
size_t oex_wcstombs( char* mbstr, const wchar_t* wcstr, size_t max )
{
	return wcsrtombs( mbstr, (const wchar_t**)&wcstr, max, oexNULL );
}
#endif

oexUINT CSys::WcsToMbs( oexSTR8 pDst, oexUINT uMax, oexCSTRW pSrc, oexUINT uLen )
{
	if ( !pSrc || !uLen )
		return 0;

	// +++ Ensure source is NULL terminated
//	pSrc[ uLen - 1 ] = 0;

#if defined( OEX_NOWCSTO )
	return oex_wcstombs( pDst, pSrc, uMax );
#else
	return wcstombs( pDst, pSrc, uMax );
#endif
}

#if defined( OEX_NOWCSTO )
size_t oex_mbstowcs(wchar_t* wcstr, const char* mbstr, size_t max)
{
	return mbsrtowcs(wcstr, (const char**)&mbstr, max, oexNULL );
}
#endif

oexUINT CSys::MbsToWcs( oexSTRW pDst, oexUINT uMax, oexCSTR8 pSrc, oexUINT uLen )
{
	if ( !pSrc || !uLen )
		return 0;

	// +++ Ensure source is NULL terminated
//	pSrc[ uLen - 1 ] = 0;

#if defined( OEX_NOWCSTO )
	return oex_mbstowcs( pDst, pSrc, uMax );
#else
	return mbstowcs( pDst, pSrc, uMax );
#endif
}

/// vprintf
int CSys::vPrintf( oexCSTRW x_pFmt, oexVaList pArgs )
{	return ::vwprintf( x_pFmt, pArgs );
}

/// printf function
int CSys::Printf( oexCSTRW x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	int ret = vwprintf( x_pFmt, ap );
	oexVaEnd( ap );
	return ret;
}

int CSys::vPrint( oexCSTRW x_pFmt, oexVaList pArgs )
{
	CStrW s;
	s.vFmt( x_pFmt, pArgs );
	Echo( s.Ptr() );
	return s.Length();
}

int CSys::Print( oexCSTRW x_pFmt, ... )
{
	CStrW s;
	oexVaList ap; oexVaStart( ap, x_pFmt );
	s.vFmt( x_pFmt, ap );
	oexVaEnd( ap );
	Echo( s.Ptr() );
	return s.Length();
}


int CSys::Echo( oexCSTRW x_pFmt )
{	if ( !x_pFmt )
		return 0;
	CStr8 s = oexStrWToMb( x_pFmt );
#if defined( oexUNICODE )
	CUtil::AddOutput( x_pFmt, s.Length(), oexTRUE );
#else
	CUtil::AddOutput( s.Ptr(), s.Length(), oexTRUE );
#endif
	return ::puts( s.Ptr() );
}

int CSys::Echo( oexCSTRW x_pFmt, oexLONG x_lLen )
{
	if ( !x_pFmt || 0 >= x_lLen )
		return 0;
#if defined( oexUNICODE )
	CUtil::AddOutput( x_pFmt, x_lLen, oexTRUE );
#else
	CStr8 s = oexStrWToStr( CStrW( x_pFmt, x_lLen ) );
	CUtil::AddOutput( s.Ptr(), s.Length(), oexTRUE );
#endif
	return ::fwrite( x_pFmt, 1, x_lLen, stdout );
}


#endif // OEX_NOWCHAR

oexPVOID CSys::MemCpy( oexPVOID x_pDst, oexCPVOID x_pSrc, oexUINT x_uSize )
{
	return ::memcpy( x_pDst, x_pSrc, x_uSize );
}

oexINT CSys::MemCmp( oexCPVOID x_pDst, oexCPVOID x_pSrc, oexUINT x_uSize )
{
	return ::memcmp( x_pDst, x_pSrc, x_uSize );
}

oexPVOID CSys::MemSet( oexPVOID x_pDst, oexINT x_nCh, oexUINT x_uSize )
{
	return ::memset( x_pDst, x_nCh, x_uSize );
}

static oexINT64 g_int = 0;
static oexGUID g_random = IID_ZEROS;
struct SRandomSeeds
{	oexPVOID		pFunction;
	oexPVOID		pHeap;
	oexINT64		nCount;
	CSysTime		st;
	oexGUID			guid;
};

// +++ Probably be nice to have a bit stronger randomizer here.
oexGUID * CSys::CreateGuid( oexGUID *pGuid )
{
	if ( !oexCHECK_PTR( pGuid ) )
		return oexNULL;

	oexGUID guid = IID_ZEROS;

	// Use other random sources
	SRandomSeeds rs;
	rs.pFunction = (oexPVOID)&CreateGuid;
//	rs.pHeap = CMem::GetRawAllocator().fMalloc( 16 );
	rs.nCount = g_int++;
	rs.st.GetSystemTime();
	oexMemCpy( &rs.guid, &g_random, sizeof( rs.guid ) );

	oss::CMd5::Transform( &guid, &rs, sizeof( rs ) );
//	CMem::GetRawAllocator().fFree( rs.pHeap );

	// Add existing random data
	guid::CopyGuid( pGuid, &guid );
	guid::XorGuid( pGuid, &g_random );
	guid::XorGuid( &g_random, &guid );
	
	// Indicate random GUID
	pGuid->ucBuf[ 7 ] = 0x40 | ( pGuid->ucBuf[ 7 ] & 0x0f );
	
    return pGuid;
}

void CSys::Sleep( oexUINT uMilliseconds, oexUINT uSeconds )
{
/*
	if ( uMilliseconds > 1000 )
	{	uSeconds += uMilliseconds / 1000;
		uMilliseconds %= 1000;
	} // end if

	if ( uSeconds )
		::sleep( uSeconds );

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = uMilliseconds * 1000000;
	nanosleep( &ts, 0 );
*/

	if ( uSeconds )
		::sleep( uSeconds );

	if ( !uSeconds || uMilliseconds )
		::usleep( uMilliseconds * 1000 );

}

oexBOOL CSys::MicroSleep( oexUINT uMicroseconds, oexUINT uSeconds )
{
/*
	if ( uMicroseconds > 1000000 )
	{	uSeconds += uMicroseconds / 1000000;
		uMicroseconds %= 1000000;
	} // end if

	if ( uSeconds )
		::sleep( uSeconds );

	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = uMicroseconds * 1000;
	return nanosleep( &ts, 0 );
*/

	if ( uSeconds )
		::sleep( uSeconds );

	if ( !uSeconds || uMicroseconds )
		::usleep( uMicroseconds );

	return oexTRUE;

}

void CSys::CloseHandle( t_WAITABLE x_pHandle )
{
//	if ( x_pHandle )
//		CloseHandle( (HANDLE)x_pHandle );
}

CSys::t_WAITABLE CSys::DuplicateHandle( t_WAITABLE x_pHandle )
{
/*    HANDLE hDup = oexNULL;
    if ( !::DuplicateHandle( GetCurrentProcess(), (HANDLE)x_pHandle,
                             GetCurrentProcess(), &hDup,
                             0, oexFALSE, DUPLICATE_SAME_ACCESS ) )
        return oexNULL;
    return (t_WAITABLE)hDup;
*/

	return 0;
}

oexINT CSys::WaitForSingleObject( CSys::t_WAITABLE x_pHandle, oexUINT x_uTimeout )
{
/*
	DWORD dwRet = ::WaitForSingleObject( (HANDLE)x_pHandle, (DWORD)x_uTimeout );
	if ( WAIT_OBJECT_0 == dwRet )
        return waitSuccess;
	else if ( WAIT_TIMEOUT == dwRet )
        return waitTimeout;
*/
	return waitFailed;
}

oexINT CSys::WaitForMultipleObjects( oexUINT x_uObjects, CSys::t_WAITABLE *x_pHandle, oexBOOL x_bWaitAll, oexUINT x_uTimeout )
{
/*    // Currently 64, don't blame me, this is a Windows limitation...
    oexASSERT( MAXIMUM_WAIT_OBJECTS >= x_uObjects );

    // Do the wait
    DWORD dwRet = ::WaitForMultipleObjects( (DWORD)x_uObjects, (HANDLE*)x_pHandle, x_bWaitAll, x_uTimeout );

	// WAIT_OBJECT_0 should be zero
	oexSTATIC_ASSERT( WAIT_OBJECT_0 == 0 );

	// Use this line if WAIT_OBJECT_0 is not zero
//	if ( WAIT_OBJECT_0 <= dwRet && ( WAIT_OBJECT_0 + x_uObjects ) > dwRet )
//        return dwRet - WAIT_OBJECT_0;

	if ( ( WAIT_OBJECT_0 + x_uObjects ) > dwRet )
        return (etWait)dwRet;

	else if ( WAIT_TIMEOUT == dwRet )
        return waitTimeout;
*/
	return waitFailed;
}

oexLONG CSys::increment( oexLONG *x_puVal )
{
	return __sync_add_and_fetch( x_puVal, 1 );
}

oexLONG CSys::decrement( oexLONG *x_puVal )
{
	return __sync_sub_and_fetch( x_puVal, 1 );
}

void CSys_SystemTimeToSTime( struct tm* tinfo, CSys::STime &t )
{
    t.uYear = 1900 + tinfo->tm_year;
    t.uMonth = tinfo->tm_mon + 1;
    t.uDayOfWeek = tinfo->tm_wday;
    t.uDay = tinfo->tm_mday;
    t.uHour = tinfo->tm_hour;
    t.uMinute = tinfo->tm_min;
    t.uSecond = tinfo->tm_sec;
    t.nTzBias = 0;
    t.uMillisecond = 0;
    t.uMicrosecond = 0;
    t.uNanosecond = 0;
}

void CSys_STimeToSystemTime( CSys::STime &t, struct tm* tinfo )
{
    tinfo->tm_year = t.uYear - 1900;
    tinfo->tm_mon = t.uMonth ? t.uMonth - 1 : 0;
    tinfo->tm_wday = t.uDayOfWeek;
    tinfo->tm_mday = t.uDay;
    tinfo->tm_hour = t.uHour;
    tinfo->tm_min = t.uMinute;
    tinfo->tm_sec = t.uSecond;
//   = t.uMillisecond;
//   = t.uMicrosecond;
//   = t.uNanosecond;
}

oexBOOL CSys::GetLocalTime( STime &t )
{
	oexZeroMemory( &t, sizeof( t ) );

	time_t current_time;
	time( &current_time );
	struct tm tinfo, *ptinfo = &tinfo;

	_oexTRY
	{
#if !defined( OEX_NOLOCALTIME_R )
		localtime_r( &current_time, &tinfo );
#else
		ptinfo = localtime( &current_time );
		if ( !ptinfo )
			return oexFALSE;
#endif
	} // end try

	_oexCATCH_ALL()
	{
		return oexFALSE;
	}

	CSys_SystemTimeToSTime( ptinfo, t );

#ifdef OEX_NANOSECONDS
	struct timespec	ts;
	clock_gettime( CLOCK_REALTIME, &ts );
	t.uMillisecond = ts.tv_nsec / 1000 / 1000;
	t.uMicrosecond = ts.tv_nsec / 1000 % 1000;
	t.uNanosecond = ts.tv_nsec % 1000;
#else
	struct timeval tp;
	gettimeofday( &tp, oexNULL );
	t.uMillisecond = tp.tv_usec / 1000;
	t.uMicrosecond = tp.tv_usec % 1000;
#endif

	// +++ Add time zone crap
//	t.nTzBias = tz.Bias;

    return oexTRUE;
}

oexBOOL CSys::SetLocalTime( STime &t )
{
#if defined( OEX_NOSETTIME )
	return oexFALSE;
#else
	struct tm tinfo;
	CSys_STimeToSystemTime( t, &tinfo );

	time_t new_time = timelocal( &tinfo );
	return 0 == stime( &new_time );

    return oexTRUE;
#endif
}

oexINT CSys::GetLocalTzBias()
{
/*    TIME_ZONE_INFORMATION tz;
    CSys::Zero( &tz, sizeof( tz ) );

    ::GetTimeZoneInformation( &tz );
    return tz.Bias;
*/

	return 0;
}

/*
struct tm
  {
     int   tm_sec;
     int   tm_min;
     int   tm_hour;
     int   tm_mday;
     int   tm_mon;
     int   tm_year;
     int   tm_wday;
     int   tm_yday;
     int   tm_isdst;
};
*/

oexBOOL CSys::GetSystemTime( STime &t )
{
	oexZero( t );

	time_t current_time;
	time( &current_time );
	struct tm tinfo, *ptinfo = &tinfo;

	_oexTRY
	{
#if !defined( OEX_NOGMTTIME_R )
		gmtime_r( &current_time, &tinfo );
#else
		ptinfo = gmttime( &current_time );
		if ( !ptinfo )
			return oexFALSE;
#endif
	} // end try

	_oexCATCH_ALL()
	{
		return oexFALSE;
	}

	CSys_SystemTimeToSTime( ptinfo, t );

#ifdef OEX_NANOSECONDS
	struct timespec	ts;
	clock_gettime( CLOCK_REALTIME, &ts );
	t.uMillisecond = ts.tv_nsec / 1000 / 1000;
	t.uMicrosecond = ts.tv_nsec / 1000 % 1000;
	t.uNanosecond = ts.tv_nsec % 1000;
#else
	struct timeval tp;
	gettimeofday( &tp, oexNULL );
	t.uMillisecond = tp.tv_usec / 1000;
	t.uMicrosecond = tp.tv_usec % 1000;
#endif

    return oexTRUE;
}

oexBOOL CSys::SetSystemTime( STime &t )
{
#if defined( OEX_NOSETTIME )
	return oexFALSE;
#else
	struct tm tinfo;
	CSys_STimeToSystemTime( t, &tinfo );

	time_t new_time = timegm( &tinfo );
	return 0 == stime( &new_time );

    return oexTRUE;
#endif
}


// Some helpful delta's
// 1970 = 1900 + 2208988800
// 1980 = 1970 + 315536400
// 1980 = 1900 + 2524525200
// 1900 = 1601 + 9435484800
// 1970 = 1601 + 11644473600
// 1980 = 1601 + 11960010000

#define FTOFF_1900		9435484800LL
#define FTOFF_1970		11644473600LL
#define FTOFF_1980		11960010000LL

#if defined( OEX_NOTIMEGM )
time_t timegm (struct tm *tm) {
    time_t ret;
    char *tz = getenv( "TZ" );
    setenv( "TZ", "", 1 );
    tzset();
    ret = mktime( tm );
    if ( tz )
        setenv( "TZ", tz, 1 );
    else
        unsetenv( "TZ" );
    tzset();
    return ret;
}
#endif

/// Converts an STime structure to file time
oexINT64 CSys::SystemTimeToFileTime( STime &x_st )
{
	struct tm tinfo;
	CSys_STimeToSystemTime( x_st, &tinfo );
//	return ( (oexINT64)mktime( &tinfo ) + FTOFF_1970 ) * 10000000LL
	return ( (oexINT64)timegm( &tinfo ) + FTOFF_1970 ) * 10000000LL
	       + ( 10000LL * (oexINT64)x_st.uMillisecond )
	       + ( 10LL * (oexINT64)x_st.uMicrosecond )
	       + ( (oexINT64)x_st.uNanosecond / 100LL )
	       ;
}

/// Converts a file time to an STime structure
void CSys::FileTimeToSystemTime( STime &x_st, oexINT64 x_ft )
{
	x_st.uNanosecond = ( x_ft * 100LL ) % 1000LL;
	x_st.uMicrosecond = ( x_ft / 10000LL ) % 1000LL;
	x_st.uMillisecond = ( x_ft / 10000000LL ) % 1000LL;

	time_t tTime = ( x_ft / 10000000LL ) - FTOFF_1970;
	struct tm tinfo, *ptinfo = &tinfo;

	_oexTRY
	{
#if !defined( OEX_NOGMTTIME_R )
		gmtime_r( &tTime, &tinfo );
#else
		ptinfo = gmttime( &tTime );
		if ( !ptinfo )
			return;
#endif
	} // end try

	_oexCATCH_ALL()
	{
		return;
	}

	CSys_SystemTimeToSTime( ptinfo, x_st );
}


oexCSTR CSys::SetLocale( oexINT nCategory, oexCSTR pLocal )
{
//  +++ Resolve temporary string problem
//	return setlocale( nCategory, oexStrToMbPtr( pLocal ) );
	return oexT( "" );
}

oexUINT CSys::GetUnixTime()
{
	return time( NULL );
}

/// printf function
int CSys::Printf( oexCSTR8 x_pFmt, ... )
{
	oexVaList ap; oexVaStart( ap, x_pFmt );
	int ret = ::vprintf( x_pFmt, ap );
	oexVaEnd( ap );
	return ret;
}

/// vprintf
int CSys::vPrintf( oexCSTR8 x_pFmt, oexVaList pArgs )
{	return ::vprintf( x_pFmt, pArgs );
}

int CSys::vPrint( oexCSTR8 x_pFmt, oexVaList pArgs )
{
	CStr8 s;
	s.vFmt( x_pFmt, pArgs );
	Echo( s.Ptr() );
	return s.Length();
}

int CSys::Print( oexCSTR8 x_pFmt, ... )
{
	CStr8 s;
	oexVaList ap; oexVaStart( ap, x_pFmt );
	s.vFmt( x_pFmt, ap );
	oexVaEnd( ap );
	Echo( s.Ptr() );
	return s.Length();
}

int CSys::Echo( oexCSTR8 x_pFmt )
{
	if ( !oexCHECK_PTR( x_pFmt ) )
		return 0;

	if ( CUtil::isOutputBuffer() )
	{
#if defined( oexUNICODE )
		CStr s = oexMbToStr( x_pFmt );
		CUtil::AddOutput( s.Ptr(), s.Length(), oexTRUE );
#else
		CUtil::AddOutput( x_pFmt, 0, oexTRUE );
#endif
	} // end if

	return ::puts( x_pFmt );
}

int CSys::Echo( oexCSTR8 x_pFmt, oexLONG x_lLen )
{//_STT();

	if ( !x_pFmt || 0 >= x_lLen )
		return 0;

	if ( CUtil::isOutputBuffer() )
	{
#if !defined( oexUNICODE )
		CUtil::AddOutput( x_pFmt, x_lLen, oexTRUE );
#else
		CStr s = oexStrToStrW( CStr8( x_pFmt, x_lLen ) );
		CUtil::AddOutput( s.Ptr(), s.Length(), oexTRUE );
#endif
	} // end if

	return ::fwrite( x_pFmt, 1, x_lLen, stdout );
}

oexLONG CSys::Read_stdin( oexSTR8 x_pBuf, oexLONG x_lMax )
{//_STT();

	if ( !x_pBuf )
		return 0;

	fcntl( 0, F_SETFL, fcntl( 0, F_GETFL, 0 ) | O_NONBLOCK );
	return fread( x_pBuf, 1, x_lMax, stdin );
}

oexUINT CSys::GetCurThreadId()
{	return (oexUINT)::pthread_self();
}

oexBOOL CSys::PumpThreadMessages()
{
	return oex::oexFALSE;
}


oexINT CSys::Fork( oexCSTR x_pWorkingDirectory, oexCSTR x_pLogFile )
{
	pid_t pid, sid;

	// Fork from the parent
	pid = fork();
	if ( 0 > pid )
	{	oexPrintf( "fork() failed : %d\n", errno );
		oexERROR( errno, oexT( "fork() failed" ) );
		return pid;
	} // end if

	// Exit parent process
	if ( 0 < pid )
	{	//CLog::GlobalLog().OpenLogFile( oexNULL, oexNULL, oexNULL );
		//oexPrintf( "Exit parent : %d\n", (int)pid );
		oexNOTICE( 0, CStr().Fmt( oexT( "fork() = %d (0x%x)" ), (int)pid, (int)pid ) );
		return pid;
	} // end if

	// Change file mode mask
	umask( 0 );

	// Child process needs a SID
	sid = setsid();
	if ( 0 > sid )
	{	oexPrintf( "setsid() failed : %d\n", (int)sid );
		oexERROR( errno, oexT( "setsid() failed" ) );
		return -1;
	} // end if

	// Switch to custom log file
	if ( oexCHECK_PTR( x_pLogFile ) && *x_pLogFile )
		CLog::GlobalLog().OpenLogFile( oexNULL, x_pLogFile, oexT( ".fork.debug.log" ) );
	else
		CLog::GlobalLog().OpenLogFile( oexNULL, oexNULL, oexT( ".fork.debug.log" ) );

	// Log child sid
	oexNOTICE( 0, CStr().Fmt( oexT( "Child fork() : setsid() = %d" ), (int)sid ) );

	// Set the current working directory
	CStr sWorkingDirectory;
	if ( !oexCHECK_PTR( x_pWorkingDirectory ) || !*x_pWorkingDirectory )
		x_pWorkingDirectory = ( sWorkingDirectory = oexGetModulePath() ).Ptr();

	if ( oexCHECK_PTR( x_pWorkingDirectory ) && *x_pWorkingDirectory )
		if ( 0 > chdir( oexStrToMbPtr( x_pWorkingDirectory ) ) )
		{	oexPrintf( "Failed to set working directory : %d : %s\n", errno, x_pWorkingDirectory );
			oexERROR( errno, oexMks( oexT( "chdir( '" ), x_pWorkingDirectory, oexT( "' ) failed" ) ) );
			return -1;
		} // end if

	// No more terminals
	if ( 0 > close( STDIN_FILENO ) )
		oexWARNING( errno, oexT( "Unable to close STDIN_FILENO" ) );
	if ( 0 > close( STDOUT_FILENO ) )
		oexWARNING( errno, oexT( "Unable to close STDIN_FILENO" ) );
	if ( 0 > close( STDERR_FILENO ) )
		oexWARNING( errno, oexT( "Unable to close STDIN_FILENO" ) );

	// Return from child process
	return 0;
}

oexBOOL CSys::Shell( oexCSTR x_pFile, oexCSTR x_pParams, oexCSTR x_pDirectory )
{
	if ( !oexCHECK_PTR( x_pFile ) )
		return oexFALSE;

	CStr sCmd;

	// Changing direcories?
	if ( oexCHECK_PTR( x_pDirectory ) && *x_pDirectory )
		sCmd = oexMks( oexT( "cd \"" ), x_pDirectory, oexT( "\"; " ) );

	// File name
	sCmd += x_pFile;

	// Params?
	if ( oexCHECK_PTR( x_pParams ) && *x_pParams )
		sCmd += oexMks( oexT( " " ), x_pParams );

//	oexEcho( oexMks( oexT( "system() : " ), sCmd ).Ptr() );

	return system( oexStrToMb( sCmd ).Ptr() ) ? oexTRUE : oexFALSE;
}

oexUINT CSys::GetCurrentProcessId()
{
	return 0;
}

oexUINT CSys::GetProcessVersion( oexUINT uPid )
{
	return 0; 
}

oexINT CSys::KillProcess( oexINT nPid, oexUINT uTimeout, oexUINT uExit )
{
	return 0;
}

oexUINT CSys::StartProcess( oexCSTR x_pFile, oexCSTR x_pParams, oexCSTR x_pDirectory )
{
	if ( !oexCHECK_PTR( x_pFile ) )
		return oexFALSE;

	// Fork the process
	int nRet = CSys::Fork( x_pDirectory );
	if ( 0 > nRet )
		return oexFALSE;
	else if ( 0 < nRet )
		return (oexUINT)nRet;

	// Switch to user process
	execv( oexStrToMbPtr( x_pFile ), (char*const*)oexStrToMbPtr( x_pParams ) );

	// Error / quit child process
	exit( EXIT_SUCCESS );
	return oexFALSE;
}


struct SProcStatInfo
{
	// Proc stat file
	CFile 			fProcStat;

	// Values
	CPropertyBag	pb;
};

/*
static oexBOOL UpdateProcStat( SProcStatInfo *pPsi )
{
	// Verify processor pointer
	if ( !oexCHECK_PTR( pPsi ) )
		return oexFALSE;

	// Lose old values
	pPsi->pb.Destroy();

	// Open processor status file if needed
	if ( !pPsi->fProcStat.IsOpen() )
	{	if ( !pPsi->fProcStat.OpenExisting( oexT( "/proc/stat" ), os::CBaseFile::eAccessRead ).IsOpen() )
			return oexFALSE;
	} // end if

	// Start at the start
	pPsi->fProcStat.SetPtrPosBegin( 0 );

	// Read in info
	CStr sInfo = oexMbToStr( pPsi->fProcStat.Read( 64 * 1024 ) );
	if ( !sInfo.Length() )
		return oexFALSE;

	// Parse the data
	CStrList lst = CParser::Split( sInfo, oexT( "\r\n" ) );
	for ( CStrList::iterator it; lst.Next( it ); )
	{
		CStrList lstParams = CParser::Split( it.Obj(), oexT( " \t" ) );

		// Iterate through the values
		CStrList::iterator itK = lstParams.First();
		CStrList::iterator itV = itK;

		// Read in values
		oexLONG i = 0;
		if ( itK.Obj() != oexT( "intr" ) )
		{	CPropertyBag &r = pPsi->pb[ itK.Obj() ];
			while ( itV.Next() )
				r[ i++ ] = itV.Obj();
		} // end if
	}

	return oexTRUE;
}
*/

static oexBOOL UpdateProcStatItem( SProcStatInfo *pPsi, oexCSTR pItem )
{
	// Verify processor pointer
	if ( !oexCHECK_PTR( pPsi ) || !oexCHECK_PTR( pItem ) )
		return oexFALSE;

	// Lose old values
	pPsi->pb.Destroy();

	// Open processor status file if needed
	if ( !pPsi->fProcStat.IsOpen() )
	{	if ( !pPsi->fProcStat.OpenExisting( oexT( "/proc/stat" ), os::CBaseFile::eAccessRead ).IsOpen() )
			return oexFALSE;
	} // end if

	// Start at the start
	pPsi->fProcStat.SetPtrPosBegin( 0 );

	// Read in info
	CStr sInfo = oexMbToStr( pPsi->fProcStat.Read( 64 * 1024 ) );
	if ( !sInfo.Length() )
		return oexFALSE;

	oexINT nItem = zstr::Length( pItem );
	while ( sInfo.Length() )
	{
		// Is this the one we're looking for?
		if ( sInfo.CmpLen( pItem, nItem ) )
		{
			// Skip tag name
			sInfo.LTrim( nItem );
			sInfo.Skip( oexT( "\t " ) );

			// Create a key for data
			CPropertyBag &r = pPsi->pb[ pItem ];

			// Read in values
			oexINT nSep, i = 0, nDone = 0;
			while ( !nDone && 0 < ( nSep = sInfo.FindChars( oexT( "\r\n\t " ) ) ) )
			{
				// Add item
				r[ i++ ] = CStr( sInfo.Ptr(), nSep );

				// Are we done?
				if ( oexT( ' ' ) != sInfo[ nSep ] && oexT( '\t' ) != sInfo[ nSep ] )
					nDone = 1;

				// Skip
				sInfo.LTrim( nSep );
				sInfo.Skip( oexT( "\t " ) );

			} // end while

			return oexTRUE;

		} // end if

		else
			sInfo.SkipPast( oexT( "\r\n" ) );

	} // end while

	return oexFALSE;
}

static oexBOOL ReleaseProcStat( SProcStatInfo *pPsi )
{
	// Verify processor pointer
	if ( !oexCHECK_PTR( pPsi ) )
		return oexFALSE;

	// Close the file
	if ( pPsi->fProcStat.IsOpen() )
		pPsi->fProcStat.Close();

	// Release propertys
	pPsi->pb.Destroy();

	return oexTRUE;
}

struct SCpuLoadInfo
{
	// Non zero if cpu values are valid
	oexBOOL			bValid;

	// Last cpu values
	oexINT64		cpu[ 16 ];

};

static SProcStatInfo 	g_psi;
static SCpuLoadInfo		g_cli = { oexFALSE };
static oexLock			g_cpu_lock;
oexDOUBLE CSys::GetCpuLoad()
{
	oexAutoLock ll( g_cpu_lock );
	if ( !ll.IsLocked() )
		return 0;

	// Get proc status
	if ( !UpdateProcStatItem( &g_psi, oexT( "cpu" ) ) )
		return 0;

	// Ensure cpu key
	if ( !g_psi.pb.IsKey( oexT( "cpu" ) ) )
		return 0;

	oexDOUBLE dCpu = 0;
	CPropertyBag &cpu = g_psi.pb[ oexT( "cpu" ) ];

	// Must have values
	if ( !cpu.Size() )
		return 0;

	if ( !g_cli.bValid )
	{
		g_cli.bValid = oexTRUE;
		for( oexUINT i = 0; i < oexSizeOfArray( g_cli.cpu ); i++ )
			g_cli.cpu[ i ] = cpu[ i ].ToInt64();

	} // end if

	else
	{
		oexINT64 total = 0;
		oexINT64 vals[ oexSizeOfArray( g_cli.cpu ) ];
		for( oexUINT i = 0; i < oexSizeOfArray( g_cli.cpu ) && i < cpu.Size(); i++ )
		{
			oexINT64 v = cpu[ i ].ToInt64();
			vals[ i ] = v - g_cli.cpu[ i ];
			g_cli.cpu[ i ] = v;

			total += vals[ i ];

		} // end for

		if ( total )
			dCpu = (oexDOUBLE)( total - vals[ 3 ] ) * 100. / (oexDOUBLE)total;

	} // end else

	return dCpu;
}


oexDOUBLE CSys::GetCpuLoad( oexCSTR x_pProcessName )
{
	// +++ Get process cpu load from linux?
	return 0;
}

oexBOOL CSys::Init()
{
    return oexTRUE;
}

oexBOOL CSys::Uninit()
{
	oexAutoLock ll( g_cpu_lock );
	if ( !ll.IsLocked() )
		return 0;

	// Release proc stat info
	ReleaseProcStat( &g_psi );

	return oexTRUE;
}

volatile oexINT CSys::s_last_error = 0;

oexNORETURN void CSys::ThrowException()
{
	// Throw exception
	_oexTHROW( oexEXCEPTION( s_last_error ) );

	// Just in case
	Exit( s_last_error );

	// Forever
	for(;;) Sleep( 1000 );
}

void CSys::InitException()
{
	volatile int i = 0;
	if ( i )
		_oexTHROW( oexEXCEPTION() );
}

oexUINT CSys::InjectException( oexPVOID hThread, oexINT nError )
{
	s_last_error = nError;

	return -1;
}

oexLONG CSys::IsProcessAdmin()
{
	return -1;
}

oexLONG CSys::IsUserAdmin()
{
	return -1;
}



oexLONG CSys::SwitchToAdmin()
{
	return -1;
}

