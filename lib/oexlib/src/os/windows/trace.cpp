/*------------------------------------------------------------------
// trace.cpp
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
//   HOWEVER CAUSED AND ON ANY #include "opc/breloc/basic.c"THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

CStr CTrace::GetBacktrace( oexUINT x_uSkip, oexUINT x_uMax )
{
	return CStr();

/*	// Allocate space for pointers
	TMem< void* > memPtrs;
	if ( !memPtrs.OexNew( x_uMax ).Ptr() )
		return CStr();

	// Get backtrace
	oexINT nPtrs = backtrace( memPtrs.Ptr(), x_uMax );
	if ( !nPtrs )
		return CStr();

	// Get function symbols
	char ** sStrings = backtrace_symbols( memPtrs.Ptr(), nPtrs );

	// Build stack
	CStr str;
	for ( oexUINT i = x_uSkip; i < nPtrs; i++ )

		if ( sStrings && sStrings[ i ] && memPtrs.Ptr( i ) )
			str += CStr().Fmt( oexT( "[0x%.8X] %s\r\n" ), (oexUINT)*(oexUINT*)memPtrs.Ptr( i ), sStrings[ i ] );
		else if ( memPtrs.Ptr( i ) )
			str += CStr().Fmt( oexT( "[0x%.8X] ???\r\n" ), (oexUINT)*(oexUINT*)memPtrs.Ptr( i ) );
		else
			str += oexT( "[0x????????] ???\r\n" );

	return str;
*/	
}

static BOOL GetWSAErrorMsg(oexSTR pMsg, oexUINT err)
{
	oexCSTR ptr = NULL;

	switch( err )
	{
		case WSAEACCES:
			ptr =oexT( "Access Denied" );
			break;
		case WSAEADDRINUSE:
			ptr =oexT( "Address already in use" );
			break;
		case WSAEADDRNOTAVAIL:
			ptr =oexT( "Cannot assign requested address" );
			break;
		case WSAEAFNOSUPPORT:
			ptr =oexT( "Address family not supported by protocol family" );
			break;
		case WSAEALREADY:
			ptr =oexT( "Operation already in progress" );
			break;
		case WSAECONNABORTED:
			ptr =oexT( "Software caused connection abort" );
			break;
		case WSAECONNREFUSED:
			ptr =oexT( "Connection refused" );
			break;
		case WSAECONNRESET:
			ptr =oexT( "Connection reset by peer" );
			break;
		case WSAEDESTADDRREQ:
			ptr =oexT( "Destination addres required" );
			break;
		case WSAEFAULT:
			ptr =oexT( "Bad Address" );
			break;
		case WSAEHOSTDOWN:
			ptr =oexT( "Host is down" );
			break;
		case WSAEHOSTUNREACH:
			ptr =oexT( "Host is unreachable" );
			break;
		case WSAEINPROGRESS:
			ptr =oexT( "Operation is now in progress" );
			break;
		case WSAEINTR:
			ptr =oexT( "Interrupted function call" );
			break;
		case WSAEINVAL:
			ptr =oexT( "Invalid argument" );
			break;
		case WSAEISCONN:
			ptr =oexT( "Socket is already connected" );
			break;
		case WSAEMFILE:
			ptr =oexT( "Too many open files" );
			break;
		case WSAEMSGSIZE:
			ptr =oexT( "Message is too long" );
			break;
		case WSAENETDOWN:
			ptr =oexT( "Network is down" );
			break;
		case WSAENETRESET:
			ptr =oexT( "Network dropped connection on reset" );
			break;
		case WSAENETUNREACH:
			ptr =oexT( "Network is unreachable" );
			break;
		case WSAENOBUFS:
			ptr =oexT( "Insufficient buffer space is available" );
			break;
		case WSAENOPROTOOPT:
			ptr =oexT( "Bad protocol option" );
			break;
		case WSAENOTCONN:
			ptr =oexT( "Socket is not connected" );
			break;
		case WSAENOTSOCK:
			ptr =oexT( "Socket operation on non-socket" );
			break;
		case WSAEOPNOTSUPP:
			ptr =oexT( "Operation not supported" );
			break;
		case WSAEPFNOSUPPORT:
			ptr =oexT( "Protocol family not supported" );
			break;
		case WSAEPROCLIM:
			ptr =oexT( "Too many processes" );
			break;
		case WSAEPROTONOSUPPORT:
			ptr =oexT( "Protocol not supported" );
			break;
		case WSAEPROTOTYPE:
			ptr =oexT( "Protocol wrong type for socket" );
			break;
		case WSAESHUTDOWN:
			ptr =oexT( "Cannot send after socket shutdown" );
			break;
		case WSAESOCKTNOSUPPORT:
			ptr =oexT( "Socket type not supported" );
			break;
		case WSAETIMEDOUT:
			ptr =oexT( "Connection timed out" );
			break;
		case WSATYPE_NOT_FOUND:
			ptr =oexT( "Class type not found" );
			break;
		case WSAEWOULDBLOCK:
			ptr =oexT( "Resource temporarily unavailable (Would block)" );
			break;
		case WSAHOST_NOT_FOUND:
			ptr =oexT( "Host not found" );
			break;
//		case WSA_INVALID_HANDLE:
//			ptr =oexT( "Specified event object handle is invalid" );
//			break;
//		case WSA_INVALID_PARAMETER:
//			ptr =oexT( "One or mor parameters are invalid" );
//			break;
//		case WSAINVALIDPROCTABLE;
//			ptr =oexT( "Invalid procedure table from service provider" );
//			break;
//		case WSAINVALIDPROVIDER:
//			ptr =oexT( "Invalid service provider version number" );
//			break;
//		case WSA_IO_INCOMPLETE:
//			ptr =oexT( "Overlapped I/O event object not in signaled state" );
//			break;
//		case WSA_IO_PENDING:
//			ptr =oexT( "Overlapped I/O operations will complete later" );
//			break;
//		case WSA_NOT_ENOUGH_MEMORY:
//			ptr =oexT( "Insufficient memory available" );
//			break;
		case WSANOTINITIALISED: 
			ptr =oexT( "Successful WSAStartup not yet performed" );
			break;
		case WSANO_DATA:
			ptr =oexT( "Valid name, no data record of requested type" );
			break;
		case WSANO_RECOVERY:
			ptr =oexT( "Non-recoverable error has occured" );
			break;
//		case WSAPROVIDERFAILEDINIT:
//			ptr =oexT( "Unable to initialize a service provider" );
//			break;
		case WSASYSCALLFAILURE:
			ptr =oexT( "System call failure" );
			break;
		case WSASYSNOTREADY:
			ptr =oexT( "Network subsystem is unavailable" );
			break;
		case WSATRY_AGAIN:
			ptr =oexT( "Non-authoritative host not found" );
			break;
		case WSAVERNOTSUPPORTED:
			ptr =oexT( "WINSOCK.DLL version not supported" );
			break;
		case WSAEDISCON:
			ptr = oexT( "Graceful shutdown in progress" );
			break;
//		case WSA_OPERATION_ABORTED:
//			ptr =oexT( "Overlapped I/O operation has been aborted" );
//			break;
//		case WSAE:
//			ptr =oexT( "" );
//			break;
		default: 
			zstr::Copy( pMsg, oexSTRSIZE - 1, oexT( "Unspecified error" ) );
			return FALSE;
			break;

	} // end switch

	zstr::Copy( pMsg, oexSTRSIZE - 1, ptr );

	return TRUE;
}

CStr CTrace::GetErrorMsg( oexINT x_nErr )
{
	oexTCHAR msg[ oexSTRSIZE ] = { 0 };

	// What does windows say this error is?
	if ( !FormatMessage(	FORMAT_MESSAGE_FROM_SYSTEM,
							NULL, x_nErr, 0, msg, oexSTRSIZE, NULL ) )

		// Get winsock error
		::GetWSAErrorMsg( msg, x_nErr );

	return msg;
}


