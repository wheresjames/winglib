/*------------------------------------------------------------------
// event.cpp
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

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;
							 
oexRESULT CEvent::Create( CResource &x_rEvent, oexCSTR x_pName, oexBOOL x_bManualReset )
{	x_rEvent.SetHandle( CreateEvent( NULL, x_bManualReset, FALSE, x_pName ) );
	return CResource::cInvalid() != x_rEvent.GetHandle();
}

void CEvent::Destroy( CResource &x_rEvent )
{	if ( CResource::cInvalid() != x_rEvent.GetHandle() ) 
		CloseHandle( (HANDLE)x_rEvent.GetHandle() );
	x_rEvent.SetHandle( CResource::cInvalid() );
}

oexBOOL CEvent::SetEvent( CResource &x_rEvent )
{	return ::SetEvent( (HANDLE)x_rEvent.GetHandle() ) ? oexTRUE : oexFALSE; }

oexBOOL CEvent::ResetEvent( CResource &x_rEvent )
{	return ::ResetEvent( (HANDLE)x_rEvent.GetHandle() ) ? oexTRUE : oexFALSE; }

/*
oexINT CEvent::osWaitForSingleObject( t_EVENT x_pEvent, oexUINT x_uTimeout )
{	DWORD dwRet = WaitForSingleObject( (HANDLE)x_pEvent, (DWORD)x_uTimeout );
	if ( WAIT_OBJECT_0 == dwRet ) return waitSuccess;
	else if ( WAIT_TIMEOUT == dwRet ) return waitTimeout;
	return waitFailed;
}

oexINT CEvent::osWaitForMultipleObjects( oexUINT x_uObjects, t_EVENT *x_pEvent, oexBOOL x_bWaitAll, oexUINT x_uTimeout )
{
	DWORD dwRet = WaitForMultipleObjects( (DWORD)x_uObjects, (CONST HANDLE*)x_pEvent, x_bWaitAll, x_uTimeout );

	// WAIT_OBJECT_0 should be zero
	oexSTATIC_ASSERT( WAIT_OBJECT_0 == 0 );

	// Use this line if WAIT_OBJECT_0 is not zero
//	if ( WAIT_OBJECT_0 <= dwRet && ( WAIT_OBJECT_0 + x_uObjects ) > dwRet ) return dwRet - WAIT_OBJECT_0;
	
	if ( ( WAIT_OBJECT_0 + x_uObjects ) > dwRet ) return (etWait)( dwRet - WAIT_OBJECT_0 );

	else if ( WAIT_TIMEOUT == dwRet ) return waitTimeout;

	return waitFailed;
}
*/
