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
#if defined( OEX_NOEXECINFO )
	return CStr();
#else

	// Allocate space for pointers
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
	for ( oexUINT i = x_uSkip; (oexINT)i < nPtrs; i++ )
	{
		if ( sStrings && sStrings[ i ] && oexCHECK_PTR( memPtrs.Ptr( i ) ) )
			str += CStr().Fmt( oexT( "[0x%.8X] %s" oexNL8 ), (oexUINT)*(oexUINT*)memPtrs.Ptr( i ), sStrings[ i ] );
		else if ( oexCHECK_PTR( memPtrs.Ptr( i ) ) )
			str += CStr().Fmt( oexT( "[0x%.8X] ???" oexNL8 ), (oexUINT)*(oexUINT*)memPtrs.Ptr( i ) );
		else
			str += oexT( "[0x????????] ???" oexNL8 );

	} // end if

	memPtrs.Zero();

	// Release the memory
	if ( sStrings )
		free( sStrings );

	return str;
#endif
}


CStr CTrace::GetErrorMsg( oexINT x_nErr )
{
	oexSTR8 pErr = strerror( x_nErr );
	if ( !oexCHECK_PTR( pErr ) )
		return CStr( oexT( "Undefined error" ) );

	return CStr( oexMbToStr( pErr ) );
}

oexINT CTrace::GetLastError()
{
	return errno;
}



