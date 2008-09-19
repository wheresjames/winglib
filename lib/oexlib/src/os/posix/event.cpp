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

/*
             __
        _   / /|
       |\\  \/_/
       \_\| / __
          \/_/__\           .-=='/~\
   ____,__/__,_____,______)/   /{~}}}
   -,------,----,-----,---,\'-' {{~}}
                            '-==.\}/
*/

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

// Ensure size
oexSTATIC_ASSERT( sizeof( CResource::t_HANDLE ) == sizeof( void* ) );

CEvent::t_EVENT CEvent::osCreateEvent( oexCSTR x_pName, oexBOOL x_bManualReset )
{
	SResInfo *pRi = SResInfo::Create( x_pName );
	if ( !oexCHECK( pRi ) )
		return oexNULL;

	if ( x_pName && *x_pName )
	{
		key_t key = ftok();


/*
		shm_id = shmget(key(), volume, 0600|IPC_CREAT|IPC_EXCL);
		if(-1 == shm_id)  Throw(ERROR__IPC_GET);
		address = shmat(shm_id, NULL, 0);
		if((int)address == -1)  Throw(ERROR__IPC_ATTACH);
*/


//		sem_t semOne;

		// Create named pipe
//		if ( mknod( oexStrToMbPtr( x_pName ), S_IFIFO | 0777, 0 ) )
/*		if ( mkfifo( oexStrToMbPtr( x_pName ), 0777 ) )
		{	oexERROR( -1, CStr().Fmt( oexT( "mknod failed : %d" ), (int)errno ) );
			return oexNULL;
		} // end if

		// Open pipe
		oexINT nFile = open( oexStrToMbPtr( x_pName ), O_RDWR, 0777 ); // O_WRONLY );
		if ( -1 == nFile )
		{	SResInfo::Release( pRi );
			return oexNULL;
		} // end if
*/
		// Save resource type
		pRi->uType = SResInfo::eNamedEvent;

	} // end if


/*
	// +++ Must figure out a way to get named events
	pthread_cond_t *hSig = new pthread_cond_t;
	if ( !hSig )
		return CEvent::c_Invalid;
	pthread_cond_init( hSig, NULL );
	return (CEvent::t_EVENT)hSig;

/*
	pthread_mutex_t *hEv = new pthread_mutex_t;
	if ( !hEv )
		return CEvent::c_Invalid;
	pthread_mutex_init( hEv, NULL );
	return (CEvent::t_EVENT)hEv;
*/
}

void CEvent::osDestroyEvent( t_EVENT x_pEvent )
{

/*	if ( !oexCHECK_PTR( x_pEvent ) )
		return;

	pthread_cond_t *hSig = (pthread_cond_t*)x_pEvent;
	pthread_cond_destroy( hSig );
	delete hSig;

/*	if ( !oexCHECK_PTR( x_pEvent ) )
		return;

	pthread_mutex_t *hEv = (pthread_mutex_t*)x_pEvent;
	pthread_mutex_destroy( hEv );		hSig
	delete hEv;
*/
}

oexBOOL CEvent::osSetEvent( t_EVENT x_pEvent )
{
	return oexFALSE;

/*	if ( !oexCHECK_PTR( x_pEvent ) )
		return FALSE;

	pthread_cond_signal( (pthread_cond_t*)x_pEvent )

	return SetEvent( (HANDLE)x_pEvent ) ? oexTRUE : oexFALSE;
*/
}

oexBOOL CEvent::osResetEvent( t_EVENT x_pEvent )
{
	return oexFALSE;

//	return ResetEvent( (HANDLE)x_pEvent ) ? oexTRUE : oexFALSE;
}

oexINT CEvent::osWaitForSingleObject( t_EVENT x_pEvent, oexUINT x_uTimeout )
{
	return oexFALSE;

/*	DWORD dwRet = WaitForSingleObject( (HANDLE)x_pEvent, (DWORD)x_uTimeout );
	if ( WAIT_OBJECT_0 == dwRet ) return waitSuccess;
	else if ( WAIT_TIMEOUT == dwRet ) return waitTimeout;
	return waitFailed;
*/
}

oexINT CEvent::osWaitForMultipleObjects( oexUINT x_uObjects, t_EVENT *x_pEvent, oexBOOL x_bWaitAll, oexUINT x_uTimeout )
{
	return oexFALSE;
/*
	DWORD dwRet = WaitForMultipleObjects( (DWORD)x_uObjects, (CONST HANDLE*)x_pEvent, x_bWaitAll, x_uTimeout );

	// WAIT_OBJECT_0 should be zero
	oexSTATIC_ASSERT( WAIT_OBJECT_0 == 0 );

	// Use this line if WAIT_OBJECT_0 is not zero
//	if ( WAIT_OBJECT_0 <= dwRet && ( WAIT_OBJECT_0 + x_uObjects ) > dwRet ) return dwRet - WAIT_OBJECT_0;

	if ( ( WAIT_OBJECT_0 + x_uObjects ) > dwRet ) return (etWait)( dwRet - WAIT_OBJECT_0 );

	else if ( WAIT_TIMEOUT == dwRet ) return waitTimeout;

	return waitFailed;
*/
}

