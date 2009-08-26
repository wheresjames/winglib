/*------------------------------------------------------------------
// resource.cpp
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

// +++ Need to get timedlocks working one day
//     and do lots of clean up on the locks / mutex
//#if defined(_POSIX_TIMEOUTS) && (_POSIX_TIMEOUTS - 200112L) >= 0L
//#	define OEX_USE_TIMEDLOCK 1
//#else
#	undef OEX_USE_TIMEDLOCK
//#endif

// +++ Not sure what to do here, it would seem that
//     pthread_cond_timedwait() is subjet to malfunctioning if the
//     system clock changes.  I may just rely on polling for now. :(
#undef OEX_COND_EVENTS
//#define OEX_COND_EVENTS


OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

const CResource::t_HANDLE CResource::c_Invalid = (oexPVOID)0;

const oexUINT CResource::c_Infinite = 0xffffffff;

// This is currently the limit in Windows.   Tho there is no real
// limit in Posix, We want things to be cross platform right? ;)
#define MAXIMUM_WAIT_OBJECTS	64

namespace OEX_NAMESPACE
{
	namespace os
	{
		/// Contains OS specific information about a resource
		struct SResourceInfo
		{
			CResource							cSync;
			oexUINT								uOwner;
			oexUINT								uLastOwner;

			pthread_mutex_t						hMutex;
			pthread_cond_t						hCond;

			// Event info
			oexBOOL								bManualReset;
			oexBOOL								bSignaled;

			// Thread info
			pthread_t							hThread;
			oexPVOID							pData;
			CResource::PFN_ThreadProc			fnCallback;

			// Lock info
			oexINT								nCount;

			// Threads waiting for lock
			oexINT								nWaiting;
		};
	};
};

static SResourceInfo* CreateRi()
{
	SResourceInfo *pRi = OexAllocConstruct< SResourceInfo >();
	if ( !pRi )
	{	oexERROR( ENOMEM, oexT( "Out of memory" ) );
		return oexNULL;
	} // end if

	pRi->uOwner = 0;
	pRi->uLastOwner = 0;
	pRi->pData = 0;
	pRi->bManualReset = 1;
	pRi->bSignaled = 0;
	pRi->pData = 0;
	pRi->fnCallback = 0;
	pRi->nCount = 0;
	pRi->nWaiting = 0;

	return pRi;
}

#define oexWAIT_UNTIL( to, st )									\
( { oexBOOL bRet = oexFALSE;										\
	if ( to )														\
	{	oexUINT uTimeout = (oexUINT)to * 1000;						\
		while ( uTimeout && !( bRet = ( st ) ) )					\
		{	oexUINT uWait = uTimeout;								\
			if ( uWait > eWaitResolution )							\
				uWait = eWaitResolution;							\
			uTimeout -= uWait;										\
			oexMicroSleep( uWait );									\
		}															\
	} else bRet = ( st );											\
	bRet;															\
} )

oexRESULT CResource::Destroy( oexUINT x_uTimeout, oexBOOL x_bForce )
{
	// Any resources to release?
	if ( cInvalid() == m_hHandle || eRtInvalid == m_eType )
		return 0;

	// Get pointer to resource information
	SResourceInfo *pRi = (SResourceInfo*)m_hHandle;
	if ( !oexCHECK_PTR( pRi ) )
	{	m_hHandle = c_Invalid;
		m_eType = eRtInvalid;
		oexERROR( 0, oexT( "Invalid resource info object pointer" ) );
		return -1;
	} // end if

	// Error code
	oexINT nErr = 0;

	// Execute proper release function if valid handle
	switch( m_eType )
	{
		case eRtInvalid :
			break;

		case eRtFile :
			os::CBaseFile::Close( m_hHandle, &nErr );
			break;

		case eRtSocket :
			break;

		case eRtThread :
		{
			// Usually the creator will be the destructor
			if ( pRi->uOwner != oexGetCurThreadId() )
				oexWARNING( 0, oexT( "Thread not being destroyed by owner!" ) );

			// Wait for thread to exit
			if ( waitSuccess != Wait( x_uTimeout ) )
			{
				// iii  This should not happen, don't ignore the problem,
				//      figure out how to shut this thread down properly!
				oexWARNING( nErr, oexT( "!! Terminating thread !!" ) );

				// Kill the thread
				if ( x_bForce )
					if ( ( nErr = pthread_cancel( pRi->hThread ) ) )
						oexERROR( nErr, oexT( "pthread_cancel() failed" ) );

			} // end if

			// Attempt to join the thread
			else if ( ( nErr = pthread_join( pRi->hThread, oexNULL ) ) )
				oexERROR( nErr, oexT( "pthread_join() failed" ) );

		} break;

		case eRtMutex :

			// Initiailze mutex object
			if ( ( nErr = pthread_mutex_destroy( &pRi->hMutex ) ) )
				oexERROR( nErr, oexT( "pthread_mutex_destroy() failed : Error releasing mutex object" ) );

			break;

		case eRtEvent :
		{
			// Release Lock
			pRi->cSync.Destroy();

			// Release condition object
			if ( ( nErr = pthread_cond_destroy( &pRi->hCond ) ) )
				oexERROR( nErr, oexT( "pthread_cond_destroy() failed : Error releasing pthread_cond object" ) );

		} break;

		case eRtLock :
		{
			// Release mutex object
			pRi->cSync.Destroy();

		} break;

		default :
			oexERROR( EINVAL, oexT( "Attempt to release unknown resource type" ) );
			break;

	} // end switch

	// Clear handle info
	m_hHandle = c_Invalid;
	m_eType = eRtInvalid;

	// Drop the memory
	OexAllocDelete( pRi );

	return nErr;
}

oexUINT CResource::GetOwner()
{
	SResourceInfo *pRi = (SResourceInfo*)m_hHandle;
	if ( !oexCHECK_PTR( pRi ) )
		return 0;

	return pRi->uOwner;
}

oexRESULT CResource::NewEvent( oexCSTR x_sName, oexBOOL x_bManualReset, oexBOOL x_bInitialState )
{
	// Out with the old
	Destroy();

	SResourceInfo *pRi = CreateRi();
	if ( !pRi )
		return oexERROR( ENOMEM, oexT( "Out of memory" ) );

	// Initialize member variables
	m_eType = eRtEvent;
	m_hHandle = (CResource::t_HANDLE)pRi;
	EnableAutoRelease();

	// Initialize structure
	pRi->bManualReset = x_bManualReset;
	pRi->bSignaled = x_bInitialState;

#ifdef OEX_COND_EVENTS

	// Create the mutex object
	if ( oexINT nErr = pRi->cSync.NewMutex() )
	{	Destroy(); return oexERROR( nErr, oexT( "Create mutex failed" ) ); }

#else

	// Create the mutex object
	if ( oexINT nErr = pRi->cSync.NewLock() )
	{	Destroy(); return oexERROR( nErr, oexT( "Create mutex failed" ) ); }

#endif

	// Create condition object
	if ( oexINT nErr = pthread_cond_init( &pRi->hCond, 0 ) )
	{	Destroy(); return oexERROR( nErr, oexT( "pthread_cond_init: failed" ) ); }

	return 0;
}

oexRESULT CResource::NewMutex( oexCSTR x_sName, oexBOOL x_bInitialOwner )
{
	// Out with the old
	Destroy();

	SResourceInfo *pRi = CreateRi();
	if ( !pRi )
		return oexERROR( ENOMEM, oexT( "Out of memory" ) );

	// Save information
	m_eType = eRtMutex;
	m_hHandle = (CResource::t_HANDLE)pRi;
	EnableAutoRelease();

	// Initiailze mutex object
	if ( oexINT nErr = pthread_mutex_init( &pRi->hMutex, 0 ) )
	{	Destroy(); return oexERROR( nErr, oexT( "ptherad_mutex_init() failed" ) ); }

	if ( x_bInitialOwner )
		Wait( 0 );

	return 0;
}

oexRESULT CResource::NewThread( PFN_ThreadProc x_fnCallback, oexPVOID x_pData )
{
	// Out with the old
	Destroy();

	SResourceInfo *pRi = CreateRi();
	if ( !pRi )
		return oexERROR( ENOMEM, oexT( "Out of memory" ) );

	m_eType = eRtThread;
	m_hHandle = (CResource::t_HANDLE)pRi;
	EnableAutoRelease();

	// Save user data
	pRi->pData = x_pData;
	pRi->fnCallback = x_fnCallback;
	pRi->uOwner = oexGetCurThreadId();

	// Create event object to indicate when thread has shut down properly
	if ( oexINT nErr = pRi->cSync.NewEvent() )
	{	Destroy(); return oexERROR( nErr, oexT( "Create event failed" ) ); }

	// Make thread joinable
	pthread_attr_t attr;
	pthread_attr_init( &attr );
	pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );

	// Create the thread
	oexINT nRet = pthread_create( &pRi->hThread, &attr,
							      CResource::ThreadProc, pRi );

	// Lose attributes structure
	pthread_attr_destroy( &attr );

	if ( nRet )
	{	oexERROR( nRet, oexT( "Error creating thread" ) );
		Destroy();
		return nRet;
	} // end if

    return 0;
}

oexPVOID CResource::ThreadProc( oexPVOID x_pData )
{
	// Get pointer to resource information
	SResourceInfo *pRi = (SResourceInfo*)x_pData;
	if ( !oexCHECK_PTR( pRi ) )
	{	oexERROR( 0, oexT( "Invalid resource info object pointer" ) );
		return (oexPVOID)-1;
	} // end if

	// Call user thread
	oexPVOID pRet = pRi->fnCallback( pRi->pData );

	// Signal that we're done
	pRi->cSync.Signal();

	// Quit thread
	pthread_exit( pRet );

	// ???
	oexERROR( 0, oexT( "pthread_exit() returned!" ) );

	return pRet;
}

oexRESULT CResource::NewLock( oexCSTR x_sName, oexBOOL x_bInitialOwner )
{
	// Out with the old
	Destroy();

	SResourceInfo *pRi = CreateRi();
	if ( !pRi )
		return oexERROR( ENOMEM, oexT( "Out of memory" ) );

	// Initialize member variables
	m_eType = eRtLock;
	m_hHandle = (CResource::t_HANDLE)pRi;
	EnableAutoRelease();

	// Initialize structure
	pRi->nCount = 0;
	pRi->uOwner = 0;

	// Create the mutex object
	if ( oexINT nErr = pRi->cSync.NewMutex() )
	{	Destroy(); return oexERROR( nErr, oexT( "Create mutex failed" ) ); }

	return 0;
}

oexRESULT CResource::Wait( oexUINT x_uTimeout )
{
	// Verify information
	if ( cInvalid() == m_hHandle || eRtInvalid == m_eType )
		return -1;

	// Get pointer to resource information
	SResourceInfo *pRi = (SResourceInfo*)m_hHandle;
	if ( !oexCHECK_PTR( pRi ) )
	{	oexERROR( 0, oexT( "Invalid resource info object pointer" ) );
		return -1;
	} // end if

	// Execute proper release function if valid handle
	switch( m_eType )
	{
		case eRtInvalid :
			break;

		case eRtFile :
			break;

		case eRtSocket :
			break;

		case eRtThread :
		{
			// Check to see if the thread has signaled a safe shutdown
			if ( 0 == pRi->cSync.Wait( 0 ) )
				return waitSuccess;

			// Wait on the thread handle
			if ( oexWAIT_UNTIL( x_uTimeout, pthread_kill( pRi->hThread, 0 ) ) )
				return waitSuccess;
			return waitTimeout;

		} break;

		case eRtMutex :
		{
			// Have to give other threads polling time to grab the mutex
			// Otherwise this one could just hog it all the time
			oexUINT uId = oexGetCurThreadId();
//			if ( pRi->uLastOwner == uId )
//				oexMicroSleep( eWaitResolution );

#if defined( OEX_USE_TIMEDLOCK )

			// Do we already own the mutex?
			if ( pRi->uOwner == oexGetCurThreadId() )
			{	pRi->nCount++;
				return waitSuccess;
			} // end if

			if ( pRi->nWaiting && pRi->uLastOwner == uId )
				oexMicroSleep( eWaitResolution );

			struct timespec to;
			clock_gettime( CLOCK_REALTIME, &to );
			to.tv_sec += x_uTimeout / 1000;
			to.tv_nsec += ( x_uTimeout % 1000 ) * 1000000;

			// Attempt to acquire lock
			if ( !pthread_mutex_trylock( &pRi->hMutex ) )
			{	pRi->nCount = 1;
				pRi->uOwner = uId;
				pRi->nWaiting = 0;
				return waitSuccess;
			} // end if

			else
			{
				pRi->nWaiting++;

				if ( !pthread_mutex_timedlock( &pRi->hMutex, &to ) )
				{	pRi->nCount = 1;
					pRi->uOwner = uId;
					pRi->nWaiting = 0;
					return waitSuccess;
				} // end if

			} // end else

#else
			oexINT nErr = 0;
			if ( oexWAIT_UNTIL( x_uTimeout, !( nErr = pthread_mutex_trylock( &pRi->hMutex ) ) || EDEADLK == nErr ) )
			{	pRi->uOwner = uId;
				return waitSuccess;
			} // end if

#endif

			return waitTimeout;

		} break;

		case eRtEvent :
		{
			// Verify mutex data pointer
			if ( !oexCHECK_PTR( pRi->cSync.GetRi() ) )
			{	oexERROR( 0, oexT( "Invalid mutex data pointer" ) );
				return waitFailed;
			} // end if

#ifndef OEX_COND_EVENTS

#	if defined( OEX_USE_TIMEDLOCK )

			oexAutoLock al( pRi->cSync, x_uTimeout );
			if ( !al.IsLocked() )
				return waitTimeout;
			return waitSuccess;

#	else

			// Just poll the signal
			if ( oexWAIT_UNTIL( x_uTimeout, pRi->bSignaled ) )
				return waitSuccess;
			return waitTimeout;

#	endif

#else
			oexAutoLock al( pRi->cSync, x_uTimeout );
			if ( !al.IsLocked() )
				return waitTimeout;

			// Is the thing signaled?
			if ( pRi->bSignaled )
				return waitSuccess;

			// We have to give pthread_cond_timedwait() time to react
			if ( 0 == x_uTimeout )
				x_uTimeout = eWaitResolution / 1000;

			// Calculate a timeout
			struct timespec to;
			if ( oexINT nErr = clock_gettime( CLOCK_REALTIME, &to ) )
				return oexERROR( nErr, oexT( "clock_gettime() failed" ) );
			to.tv_sec += x_uTimeout / 1000;
			to.tv_nsec += ( x_uTimeout - ( x_uTimeout / 1000 * 1000 ) ) * 1000000;

			// Wait for the signal
			if ( !pthread_cond_timedwait( &pRi->hCond, &pRi->cSync.GetRi()->hMutex, &to ) )
			{
				oexBOOL bSignaled = pRi->bSignaled;

				// Auto reset?
				if ( pRi->bSignaled && !pRi->bManualReset )
					pRi->bSignaled = oexFALSE;

				// Return the signaled state
				return bSignaled ? waitSuccess : waitFailed;

			} // end if

			return waitTimeout;
#endif

		} break;

		case eRtLock :
		{
			// Verify mutex data pointer
			if ( !oexCHECK_PTR( pRi->cSync.GetRi() ) )
			{	oexERROR( 0, oexT( "Invalid mutex data pointer" ) );
				return waitFailed;
			} // end if

			oexUINT uId = oexGetCurThreadId();

#if defined( OEX_USE_TIMEDLOCK )

			if ( waitSuccess == pRi->cSync.Wait( x_uTimeout ) )
			{
				// Is it owned?
				if ( !pRi->uOwner || pRi->uOwner == uId )
				{	pRi->nCount++;
					pRi->uOwner = uId;
					return waitSuccess;
				} // end if
				else
					oexEcho( "Thread locks are trashed!!!" );

			} // end if

#else

			// Have to give other threads polling time to grab the mutex
			// Otherwise this one could just hog it all the time
			if ( pRi->nWaiting && pRi->uLastOwner == uId )
				oexMicroSleep( eWaitResolution );

			while ( x_uTimeout > eWaitResolution )
			{
				{ // Lock scope

					// Lock the mutex
					oexAutoLock al( pRi->cSync, eWaitResolution );
					if ( al.IsLocked() )
					{
						// Is it owned?
						if ( !pRi->uOwner || pRi->uOwner == uId )
						{	pRi->nCount++;
							pRi->uOwner = uId;
							pRi->nWaiting = 0;
							return waitSuccess;
						} // end if

						// Someone is waiting
						else
							pRi->nWaiting++;

					} // end if

				} // end lock scope

				// Wait a bit
				oexUINT uDelay = x_uTimeout * 1000;
				if ( uDelay > eWaitResolution )
					uDelay = eWaitResolution;
				x_uTimeout -= uDelay / 1000;
				oexMicroSleep( uDelay );

			} // end if

#endif

			return waitTimeout;

		} break;

	} // end switch

	// Unwaitable, ha ha
	oexERROR( waitFailed, oexT( "Attempt to wait on unwaitable resource type" ) );

	return waitFailed;
}

oexRESULT CResource::Signal( oexUINT x_uTimeout )
{
	// Verify information
	if ( cInvalid() == m_hHandle || eRtInvalid == m_eType )
		return -1;

	// Get pointer to resource information
	SResourceInfo *pRi = (SResourceInfo*)m_hHandle;
	if ( !oexCHECK_PTR( pRi ) )
	{	oexERROR( 0, oexT( "Invalid resource info object pointer" ) );
		return -1;
	} // end if

	// Error code
	oexINT nErr = 0;

	// Execute proper release function if valid handle
	if ( cInvalid() != m_hHandle )
		switch( m_eType )
	{
		case eRtInvalid :
			break;

		case eRtFile :
			break;

		case eRtSocket :
			break;

		case eRtThread :
			break;

		case eRtMutex :
			return Wait( x_uTimeout );
			break;

		case eRtLock :
			return Wait( x_uTimeout );
			break;

		case eRtEvent :
		{
#if defined( OEX_USE_TIMEDLOCK )

			// Signal
			if ( waitSuccess != pRi->cSync.Wait( 0 ) )
				return waitFailed;

			pRi->bSignaled = oexTRUE;
			pRi->uOwner = oexGetCurThreadId();

			return waitSuccess;

#else
			oexAutoLock al( pRi->cSync, x_uTimeout );
			if ( !al.IsLocked() )
				return oexERROR( nErr, oexT( "Unable to acquire lock to signal event object" ) );

			// Save thread id on first signal
			if ( !pRi->bSignaled )
				pRi->uOwner = oexGetCurThreadId();

			// Set state to signaled
			pRi->bSignaled = oexTRUE;

#	ifndef OEX_COND_EVENTS

			// Unblock everyone if manual reset
			if ( pRi->bManualReset )
			{	if ( ( nErr = pthread_cond_broadcast( &pRi->hCond ) ) )
					return oexERROR( nErr, oexT( "pthread_cond_broadcast() failed" ) );
			} // end if

			// Unblock single waiting thread if auto
			else if ( ( nErr = pthread_cond_signal( &pRi->hCond ) ) )
				return oexERROR( nErr, oexT( "pthread_cond_signal() failed" ) );
#	endif

#endif

		} break;

		default :
			oexERROR( -1, oexT( "Attempt to wait on unknown resource type" ) );
			break;

	} // end switch

	return nErr;
}

oexRESULT CResource::Reset( oexUINT x_uTimeout )
{
	// Verify information
	if ( cInvalid() == m_hHandle || eRtInvalid == m_eType )
		return -1;

	// Get pointer to resource information
	SResourceInfo *pRi = (SResourceInfo*)m_hHandle;
	if ( !oexCHECK_PTR( pRi ) )
	{	oexERROR( 0, oexT( "Invalid resource info object pointer" ) );
		return -1;
	} // end if

	// Error code
	oexINT nErr = 0;

	// Execute proper release function if valid handle
	if ( cInvalid() != m_hHandle )
		switch( m_eType )
	{
		case eRtInvalid :
			break;

		case eRtFile :
			break;

		case eRtSocket :
			break;

		case eRtThread :
			break;

		case eRtMutex :

#if defined( OEX_USE_TIMEDLOCK )

			if ( !pRi->uOwner )
				return waitSuccess;

			if ( pRi->uOwner != oexGetCurThreadId() )
				return waitFailed;

			if ( 0 >= pRi->nCount )
				return waitFailed;

			if ( --pRi->nCount )
				return waitFailed;

			if ( !( nErr = pthread_mutex_unlock( &pRi->hMutex ) ) )
			{	pRi->uLastOwner = pRi->uOwner;
				pRi->uOwner = 0;
			} // end if
#else
			if ( !( nErr = pthread_mutex_unlock( &pRi->hMutex ) ) )
			{	pRi->uLastOwner - pRi->uOwner;
				pRi->uOwner = 0;
			} // end if
#endif
			break;

		case eRtEvent :
		{

#if defined( OEX_USE_TIMEDLOCK )

			// Signal
			if ( waitSuccess != pRi->cSync.Reset( x_uTimeout ) )
				return waitFailed;

			pRi->bSignaled = oexFALSE;
			pRi->uOwner = 0;

			return waitSuccess;

#else

			// Verify mutex data pointer
			if ( !oexCHECK_PTR( pRi->cSync.GetRi() ) )
			{	oexERROR( 0, oexT( "Invalid mutex data pointer" ) );
				return waitFailed;
			} // end if

			oexAutoLock al( pRi->cSync, x_uTimeout );
			if ( !al.IsLocked() )
				return oexERROR( nErr, oexT( "Unable to acquire lock to signal event object" ) );

			// Set state to signaled
			pRi->bSignaled = oexFALSE;
			pRi->uOwner = 0;

#endif
		} break;

		case eRtLock :
		{
			// Verify mutex data pointer
			if ( !oexCHECK_PTR( pRi->cSync.GetRi() ) )
			{	oexERROR( 0, oexT( "Invalid mutex data pointer" ) );
				return waitFailed;
			} // end if

#if defined( OEX_USE_TIMEDLOCK )

			if ( !pRi->uOwner || pRi->uOwner != oexGetCurThreadId() )
				return waitFailed;

			if ( 0 >= pRi->nCount )
				return waitFailed;

			if ( 0 == --pRi->nCount )
			{	pRi->uLastOwner = pRi->uOwner;
				pRi->uOwner = 0;
				pRi->cSync.Reset( x_uTimeout );
			} // end if

#else
			// Lock the mutex
			oexAutoLock al( pRi->cSync, x_uTimeout );
			if ( !al.IsLocked() )
				return waitTimeout;

			if ( pRi->uOwner && pRi->uOwner != oexGetCurThreadId() )
				return waitFailed;

			if ( 0 >= pRi->nCount )
				return waitFailed;

			if ( 0 == --pRi->nCount )
			{	pRi->uLastOwner = pRi->uOwner;
				pRi->uOwner = 0;
			} // end if
#endif
			return waitSuccess;

		} break;

		default :
			oexERROR( -1, oexT( "Attempt to wait on unknown resource type" ) );
			break;

	} // end switch

	return nErr;
}

oexINT CResource::WaitMultiple( oexINT x_nCount, CResource **x_pResources, oexUINT x_uTimeout, oexBOOL x_nMin )
{
	oexINT nNumHandles = 0;
	CResource *pRes[ MAXIMUM_WAIT_OBJECTS ];
	oexINT nRealIndex[ MAXIMUM_WAIT_OBJECTS ];
	oexINT nSignaled[ MAXIMUM_WAIT_OBJECTS ];
	oexZeroMemory( nSignaled, sizeof( nSignaled ) );

	if ( x_nCount > MAXIMUM_WAIT_OBJECTS )
		oexWARNING( 0, oexT( "Number of handles specified is beyond MS Windows system capabilities!!!" ) );

	// Loop through the handles
	for( oexINT i = 0; i < x_nCount && nNumHandles < MAXIMUM_WAIT_OBJECTS; i++ )
	{
		if ( !x_pResources[ i ]->IsValid() )
			oexWARNING( 0, oexT( "Invalid handle specified to WaitMultiple()" ) );
		else
		{
			SResourceInfo *pRi = (SResourceInfo*)x_pResources[ i ]->GetHandle();
			if ( !oexCHECK_PTR( pRi ) )
				oexERROR( 0, oexT( "Invalid data pointer" ) );

			else
				pRes[ nNumHandles ] = x_pResources[ i ], nRealIndex[ nNumHandles++ ] = i;

		} // end else

	} // end for

	oexINT nNumSignaled = 0;
	while ( x_uTimeout > eWaitResolution )
	{
		// See who's signaled
		for ( oexINT i = 0; i < nNumHandles; i++ )
			if ( !nSignaled[ i ] )
				if ( waitSuccess == pRes[ i ]->Wait( 0 ) )
				{	nSignaled[ i ]++;
					if ( ++nNumSignaled >= x_nMin )
						return nRealIndex[ i ];
				} // end if

		// Wait a bit
		oexUINT uDelay = x_uTimeout * 1000;
		if ( uDelay > eWaitResolution )
			uDelay = eWaitResolution;
		x_uTimeout -= uDelay / 1000;
		oexMicroSleep( uDelay );

	} // end while

	return waitTimeout;
}
