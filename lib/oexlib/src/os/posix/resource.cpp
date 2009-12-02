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

#define OEX_THREAD_TIMEOUTS
#define OEX_THREAD_FORCEFAIRNESS
//#define OEX_THREAD_USE_GETTIMEOFDAY

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

const CResource::t_HANDLE CResource::c_Invalid = (oexPVOID)0;

const oexUINT CResource::c_Infinite = 0xffffffff;

// This is currently the limit in Windows.   Tho there is no real
// limit in Posix, but we want things to be cross platform right? ;)
#define MAXIMUM_WAIT_OBJECTS	64

namespace OEX_NAMESPACE
{
	namespace os
	{
		/// Contains OS specific information about a resource
		struct SResourceInfo
		{
			CResource							cSync;

			// Handle
			union
			{
				pthread_mutex_t					hMutex;
				pthread_cond_t					hCond;
				CBaseFile::t_HFILE				hFile;
				pthread_t						hThread;
			};

			// Owner information
			oexUINT								uOwner;
			oexUINT								uLastOwner;

			// Thread info
			oexPVOID							pData;
			CResource::PFN_ThreadProc			fnCallback;

			// Event status
			oexBOOL								bSignaled;
			oexBOOL								bManualReset;

			// Lock info
			oexINT								nCount;
			oexINT								uWaiting;

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

	// Add a reference count for deletion
	if ( 2 != CAlloc::AddRef( pRi ) )
		return oexNULL;

	pRi->uOwner = 0;
	pRi->uLastOwner = 0;
	pRi->fnCallback = 0;
	pRi->nCount = 0;
	pRi->bSignaled = 0;
	pRi->bManualReset = 0;
	pRi->uWaiting = 0;

	return pRi;
}

static oexINT FreeRi( SResourceInfo* x_pRi, oexINT x_eType, oexUINT x_uTimeout, oexBOOL x_bForce )
{
	// Any resources to release?
	if ( CResource::cInvalid() == x_pRi || CResource::eRtInvalid == x_eType )
		return -1;

	// Get pointer to resource information
	if ( !x_pRi )
		return -1;

	// Check for other references
	oexINT nRef;
	if ( 1 != ( nRef = OexAllocDelete( x_pRi ) ) )
		return nRef;

	// Error code
	oexINT nErr = 0;

	// Execute proper release function if valid handle
	switch( x_eType )
	{
		case CResource::eRtInvalid :
			break;

		case CResource::eRtFile :
			os::CBaseFile::Close( x_pRi->hFile, &nErr );
			break;

		case CResource::eRtSocket :
			break;

		case CResource::eRtThread :
		{
			// Usually the creator will be the destructor
			if ( x_pRi->uOwner != oexGetCurThreadId() )
				oexWARNING( 0, oexT( "Thread not being destroyed by owner!" ) );

			// Wait for thread to exit
			if ( x_pRi->cSync.Wait( x_uTimeout )
				 && pthread_kill( x_pRi->hThread, 0 )
				 && x_pRi->cSync.Wait( x_uTimeout ) )
			{
				// iii  This should not happen, don't ignore the problem,
				//      figure out how to shut this thread down properly!
				oexWARNING( nErr, oexT( "!! Terminating thread !!" ) );

				// Kill the thread
				if ( x_bForce )
					if ( ( nErr = pthread_cancel( x_pRi->hThread ) ) )
						oexERROR( nErr, oexT( "pthread_cancel() failed" ) );

			} // end if

			// Attempt to join the thread
			else if ( ( nErr = pthread_join( x_pRi->hThread, oexNULL ) ) )
				oexERROR( nErr, oexT( "pthread_join() failed" ) );

			// Release Mutex
			x_pRi->cSync.Destroy();

		} break;

		case CResource::eRtLock :
		case CResource::eRtMutex :

			// Initiailze mutex object
			if ( ( nErr = pthread_mutex_destroy( &x_pRi->hMutex ) ) )
				oexERROR( nErr, oexT( "pthread_mutex_destroy() failed : Error releasing mutex object" ) );

			break;

		case CResource::eRtEvent :

			// Release Mutex
			x_pRi->cSync.Destroy();

			// Release condition object
			if ( ( nErr = pthread_cond_destroy( &x_pRi->hCond ) ) )
				oexERROR( nErr, oexT( "pthread_cond_destroy() failed : Error releasing pthread_cond object" ) );

			break;

		default :
			oexERROR( EINVAL, oexT( "Attempt to release unknown resource type" ) );
			break;

	} // end switch

	// Drop the memory for real
	OexAllocDelete( x_pRi );

	return 0;
}

oexINT CResource::AddRef() const
{
	if ( CResource::cInvalid() == m_hHandle || !m_hHandle )
		return -1;
	return CAlloc::AddRef( m_hHandle ) - 1;
}

oexRESULT CResource::Destroy( oexUINT x_uTimeout, oexBOOL x_bForce )
{
	// Ensure reasonable values
	if ( CResource::cInvalid() == m_hHandle || CResource::eRtInvalid == m_eType )
	{	m_hHandle = c_Invalid;
		m_eType = eRtInvalid;
		return -1;
	} // end if

	// Get destroy info
	SResourceInfo* pRi = (SResourceInfo*)m_hHandle;
	E_RESOURCE_TYPE eType = m_eType;

	// Good practice and all...
	m_hHandle = CResource::cInvalid();
	m_eType = CResource::eRtInvalid;

	// Free the resource
	if ( 0 > FreeRi( pRi, eType, x_uTimeout, x_bForce ) )
	{	oexERROR( 0, oexT( "Invalid resource info object pointer" ) );
		return -1;
	} // end if

	return 0;
}


oexUINT CResource::GetOwner()
{
	SResourceInfo *pRi = (SResourceInfo*)m_hHandle;
	if ( !pRi )
		return 0;

	return pRi->uOwner;
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
	if ( !pRi )
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

	// Mutex attributes
	pthread_mutexattr_t attr;
	if ( oexINT nErr = pthread_mutexattr_init( &attr ) )
	{	Destroy(); return oexERROR( nErr, oexT( "pthread_mutexattr_init() failed" ) ); }

	// Set mutex type
	if ( oexINT nErr = pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE ) )
	{	Destroy(); return oexERROR( nErr, oexT( "pthread_mutexattr_settype() failed" ) ); }

	// Initiailze mutex object
	if ( oexINT nErr = pthread_mutex_init( &pRi->hMutex, &attr ) )
	{	Destroy(); return oexERROR( nErr, oexT( "ptherad_mutex_init() failed" ) ); }

	// Does the caller want to own it?
	if ( x_bInitialOwner )
		Wait( 0 );

	return 0;
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

	// Initialize event
	pRi->bSignaled = x_bInitialState;
	pRi->bManualReset = x_bManualReset;

	// Create the mutex object
	if ( oexINT nErr = pRi->cSync.NewMutex() )
	{	Destroy(); return oexERROR( nErr, oexT( "Create mutex failed" ) ); }

	// Create condition object
	if ( oexINT nErr = pthread_cond_init( &pRi->hCond, 0 ) )
	{	Destroy(); return oexERROR( nErr, oexT( "pthread_cond_init: failed" ) ); }

	return 0;
}

oexRESULT CResource::NewLock( oexCSTR x_sName, oexBOOL x_bInitialOwner )
{
	return NewMutex( x_sName, x_bInitialOwner );
}

oexRESULT CResource::Wait( oexUINT x_uTimeout )
{
	// Verify information
	if ( cInvalid() == m_hHandle || eRtInvalid == m_eType )
		return -1;

	// Get pointer to resource information
	SResourceInfo *pRi = (SResourceInfo*)m_hHandle;
	if ( !pRi )
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
			// Check to see if the thread has signaled a normal shutdown
			return pRi->cSync.Wait( x_uTimeout );

		} break;

		case eRtLock :
		case eRtMutex :
		{

#if defined( OEX_THREAD_FORCEFAIRNESS )

			// +++ This is to force fairness among threads
			//     There HAS to be a way to force this natively ????
			if ( pRi->uWaiting )
			{
				// If we don't own the lock,
				// but we were the last to have it,
				// give someone else a chance.
				oexUINT uId = oexGetCurThreadId();
				if ( pRi->uOwner != uId && pRi->uLastOwner == uId )
				{
					// Don't have to wait now
					pRi->uWaiting = 0;

					// iii Zero doesn't work ;)
					oexMicroSleep( oexWAIT_RESOLUTION );

				} // end if

			} // end if

			// Attempt lock
			if ( !pthread_mutex_trylock( &pRi->hMutex ) )
			{	pRi->nCount++;
				pRi->uOwner = oexGetCurThreadId();
				return waitSuccess;
			} // end if

			// Does caller want to wait?
			if ( !x_uTimeout )
				return waitTimeout;

			// Someone is waiting
			pRi->uWaiting++;

#endif

#if defined( OEX_THREAD_TIMEOUTS )

			struct timespec to;

			// Get time

#	if !defined( OEX_THREAD_USE_GETTIMEOFDAY )
			clock_gettime( CLOCK_REALTIME, &to );
#	else
			struct timeval tp;
			gettimeofday( &tp, 0 );
			to.tv_sec = tp.tv_sec; to.tv_nsec = tp.tv_sec
#	endif

			// Add our time
			to.tv_sec += x_uTimeout / 1000;
			to.tv_nsec += ( x_uTimeout % 1000 ) * 1000000;
			if ( to.tv_nsec > 1000000000 )
				to.tv_sec += to.tv_nsec / 1000000000,
				to.tv_nsec %= 1000000000;

			// Wait for the lock
			if ( !pthread_mutex_timedlock( &pRi->hMutex, &to ) )
			{	pRi->nCount++;
				pRi->uOwner = oexGetCurThreadId();
				return waitSuccess;
			} // end if

#else

			// Wait for the lock
			if ( !pthread_mutex_lock( &pRi->hMutex ) )
			{	pRi->nCount++;
				pRi->uOwner = oexGetCurThreadId();
				return waitSuccess;
			} // end if

#endif

			return waitTimeout;

		} break;

		case eRtEvent :
		{
			// Try to get out for free
			if ( pRi->bSignaled )
				return waitSuccess;

			// Does user want to wait?
			if ( !x_uTimeout && pRi->bManualReset )
				return waitTimeout;

			// Lock the mutex
			oexAutoLock al( pRi->cSync, x_uTimeout );
			if ( !al.IsLocked() )
				return waitFailed;

			// Ensure it wasn't signaled while we were waiting
			if ( pRi->bSignaled )
				return waitSuccess;

#if defined( OEX_THREAD_TIMEOUTS )

			struct timespec to;

			// Get time

#	if !defined( OEX_THREAD_USE_GETTIMEOFDAY )
			clock_gettime( CLOCK_REALTIME, &to );
#	else
			struct timeval tp;
			gettimeofday( &tp, 0 );
			to.tv_sec = tp.tv_sec; to.tv_nsec = tp.tv_sec
#	endif

			// Add our time
			to.tv_sec += x_uTimeout / 1000;
			to.tv_nsec += ( x_uTimeout % 1000 ) * 1000000;
			if ( to.tv_nsec > 1000000000 )
				to.tv_sec += to.tv_nsec / 1000000000,
				to.tv_nsec %= 1000000000;

			// Get mutex handle
			SResourceInfo *pRiMutex = (SResourceInfo*)pRi->cSync.m_hHandle;

			// Wait for the lock
			if ( !pthread_cond_timedwait( &pRi->hCond, &pRiMutex->hMutex, &to ) )
				return pRi->bSignaled ? waitSuccess : waitTimeout;

#else

			// Get mutex handle
			SResourceInfo *pRiMutex = (SResourceInfo*)pRi->cSync.m_hHandle;

			// Wait for the lock
			if ( !pthread_cond_wait( &pRi->hCond, &pRiMutex->hMutex ) )
				return pRi->bSignaled ? waitSuccess : waitTimeout;

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
	if ( !pRi )
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

		case eRtLock :
		case eRtMutex :
			return Wait( x_uTimeout );
			break;

		case eRtEvent :
		{
			// Auto reset releases only one thread
			if ( !pRi->bManualReset )
			{	pRi->bSignaled = 0;
				if ( !pthread_cond_signal( &pRi->hCond ) )
					return waitSuccess;
			} // end if

			// Manual reset releases all threads
			else
			{
				// Lock the mutex
				oexAutoLock al( pRi->cSync, x_uTimeout );
				if ( !al.IsLocked() )
					return waitTimeout;

				// Set signal
				pRi->bSignaled = 1;

				// Wake up waiting threads
				if ( !pthread_cond_broadcast( &pRi->hCond ) )
					return waitSuccess;

			} // end else

			return waitFailed;

		} break;

		default :
			oexERROR( -1, oexT( "Attempt to signal unknown resource type" ) );
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
	if ( !pRi )
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

		case eRtLock :
		case eRtMutex :
		{
			// Attempt lock
			if ( pthread_mutex_trylock( &pRi->hMutex ) )
				return waitFailed;

			// Update lock count
			if ( pRi->nCount )
				pRi->nCount--;

			// Cleanup if count is zero
			if ( !pRi->nCount )
			{	pRi->uLastOwner = pRi->uOwner;
				pRi->uOwner = 0;
			} // end if

			// Unlock the mutex twice
			oexINT nErr;
			if ( ( nErr = pthread_mutex_unlock( &pRi->hMutex ) )
			     || ( nErr = pthread_mutex_unlock( &pRi->hMutex ) ) )
			{	oexERROR( nErr, oexT( "pthread_mutex_unlock() failed" ) );
				return waitFailed;
			} // end if

			return waitSuccess;

		} break;

		case eRtEvent :
		{
			pRi->bSignaled = 0;

		} break;

		default :
			oexERROR( -1, oexT( "Attempt to reset unknown resource type" ) );
			break;

	} // end switch

	return nErr;
}

oexINT CResource::WaitMultiple( oexINT x_nCount, CResource **x_pResources, oexUINT x_uTimeout, oexBOOL x_nMin )
{
	oexINT nNumHandles = 0;
	oexINT nNumSignaled = 0;
	oexINT nRealIndex[ MAXIMUM_WAIT_OBJECTS ];
	oexINT nSignaled[ MAXIMUM_WAIT_OBJECTS ];
	oexINT nFailed[ MAXIMUM_WAIT_OBJECTS ];

	// Warn user if we surpass ms windows caps
	if ( x_nCount > MAXIMUM_WAIT_OBJECTS )
		oexWARNING( 0, oexT( "Number of handles specified is beyond MS Windows system capabilities!!!" ) );

	// Organize the wait handles
	for( oexINT i = 0; i < x_nCount && nNumHandles < MAXIMUM_WAIT_OBJECTS; i++ )
	{
		// Valid?
		if ( !oexCHECK_PTR( x_pResources[ i ] ) || !x_pResources[ i ]->IsValid() )
			oexWARNING( 0, oexT( "Invalid handle specified to WaitMultiple()" ) );

		// Add to wait chain
		else
		{
			// Integrity check
			oexRESULT res = x_pResources[ i ]->Wait( 0 );

			// Is it signaled?
			if ( waitSuccess == res )
			{
				// Enough to ditch?
				if ( ++nNumSignaled >= x_nMin )
					return i;

			} // end if

			// We'll have to wait on this one
			else if ( waitTimeout == res )
			{
				// Save info
				nFailed[ nNumHandles ] = 0;
				nSignaled[ nNumHandles ] = 0;
				nRealIndex[ nNumHandles++ ] = i;

			} // end if

			// ??? Must be in an unwaitable state?
			else
				oexWARNING( 0, oexT( "Unwaitable handle specified to WaitMultiple()" ) );

		} // end else

	} // end for

	oexUINT uDelay = 0;

	// Forever
	for( ; ; )
	{
		// See who's signaled
		for ( oexINT i = 0; i < nNumHandles; i++ )
		{
			// Has this one already been signaled?
			if ( !nSignaled[ i ] && !nFailed[ i ] )
			{
				// Wait on this item
				oexRESULT res = x_pResources[ nRealIndex[ i ] ]->Wait( uDelay );

				// Did it succeed?
				if ( waitSuccess == res )
				{
					// Do we have the required number of signals
					if ( ++nNumSignaled >= x_nMin )
						return nRealIndex[ i ];

					// Another one bites the dust
					nSignaled[ i ]++;

				} // end if

				// Failed?
				else if ( waitTimeout != res )
					nFailed[ i ] = 1;

				// Clear delay
				uDelay = 0;

			} // end for

		} // end for

		// ???
		if ( uDelay )
			return waitFailed;

		// Timed out?
		if ( x_uTimeout < eWaitResolution )
			return waitTimeout;

		// How long to wait
		uDelay = eWaitResolution / 1000;

		// Subtract from total
		x_uTimeout -= uDelay;

	} // end forever

	return waitTimeout;
}
