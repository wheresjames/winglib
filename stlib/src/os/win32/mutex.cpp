/*------------------------------------------------------------------
// mutex.cpp
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

#include "../../../stlib.h"
#include "stdwin.h"

STLIB_USING_NAMESPACE

// Ensure size
stSTATIC_ASSERT( sizeof( CMutex::t_MUTEX ) == sizeof( HANDLE ) );

const CMutex::t_MUTEX	CMutex::c_Invalid = NULL;

const stUINT	CMutex::c_Infinite = 0xffffffff;
									 
CMutex::t_MUTEX CMutex::osCreateMutex( stCSTR x_pName )
{	return (void*)CreateMutex( NULL, FALSE, x_pName );
}

void CMutex::osDestroyMutex( t_MUTEX x_pMutex )
{	if ( x_pMutex ) CloseHandle( (HANDLE)x_pMutex );
}

void CMutex::osReleaseMutex( t_MUTEX x_pMutex )
{	if ( x_pMutex ) ReleaseMutex( (HANDLE)x_pMutex );
}

stBOOL CMutex::osSetEvent( t_MUTEX x_pMutex )
{	return SetEvent( (HANDLE)x_pMutex ); }

stBOOL CMutex::osResetEvent( t_MUTEX x_pMutex )
{	return ResetEvent( (HANDLE)x_pMutex ); }

stINT CMutex::osWaitForSingleObject( t_MUTEX x_pMutex, stUINT x_uTimeout )
{	DWORD dwRet = WaitForSingleObject( (HANDLE)x_pMutex, (DWORD)x_uTimeout );
	if ( WAIT_OBJECT_0 == dwRet ) return waitSuccess;
	else if ( WAIT_TIMEOUT == dwRet ) return waitTimeout;
	return waitFailed;
}

stINT CMutex::osWaitForMultipleObjects( stUINT x_uObjects, t_MUTEX *x_pMutex, stBOOL x_bWaitAll, stUINT x_uTimeout )
{
	DWORD dwRet = WaitForMultipleObjects( (DWORD)x_uObjects, (CONST HANDLE*)x_pMutex, x_bWaitAll, x_uTimeout );

	// WAIT_OBJECT_0 should be zero
	stSTATIC_ASSERT( WAIT_OBJECT_0 == 0 );

	// Use this line if WAIT_OBJECT_0 is not zero
//	if ( WAIT_OBJECT_0 <= dwRet && ( WAIT_OBJECT_0 + x_uObjects ) > dwRet ) return dwRet - WAIT_OBJECT_0;
	
	if ( ( WAIT_OBJECT_0 + x_uObjects ) > dwRet ) return (etWait)( dwRet - WAIT_OBJECT_0 );

	else if ( WAIT_TIMEOUT == dwRet ) return waitTimeout;

	return waitFailed;
}

stULONG CMutex::osInterlockedIncrement( stLONG *x_puVal )
{	return InterlockedIncrement( x_puVal );
}

stULONG CMutex::osInterlockedDecrement( stLONG *x_puVal )
{	return InterlockedDecrement( x_puVal );
}
