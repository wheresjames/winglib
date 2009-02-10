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

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

const CResource::t_HANDLE CResource::c_Invalid = (oexPVOID)0xffffffff;

const oexUINT CResource::c_Infinite = 0xffffffff;

struct SEventInfo
{
	CResource						cMutex;
	pthread_cond_t					cond;
	oexBOOL							bManualReset;
	oexBOOL							bSignaled;
};

struct SThreadInfo
{
	pthread_t						hThread;
	oexPVOID						pData;
	CResource::PFN_ThreadProc		fnCallback;
};
/*
#define oexWAIT_UNTIL( to, st )									\
( { oexBOOL bRet = oexFALSE;										\
	if ( to )														\
	{	oexUINT uTimeout = (oexUINT)to;								\
		struct timeval tmStart;										\
		struct timeval tmNow;										\
		gettimeofday( &tmStart, 0 );								\
		while ( uTimeout && !( bRet = ( st ) ) )					\
		{	oexUINT uWait = uTimeout * 1000;						\
			if ( uWait > eWaitResolution )							\
				uWait = eWaitResolution;							\
			usleep( uWait );										\
			gettimeofday( &tmNow, 0 );								\
			oexINT dif = 0;											\
			if ( tmNow.tv_sec > tmStart.tv_sec )					\
				dif += ( tmNow.tv_sec - tmStart.tv_sec ) * 1000;	\
			if ( tmNow.tv_usec > tmStart.tv_usec )					\
				dif += ( tmNow.tv_usec - tmStart.tv_usec ) / 1000;	\
			if ( uTimeout > dif )									\
				uTimeout -= dif;									\
			else													\
				uTimeout = 0;										\
		}															\
	} else bRet = ( st );											\
	bRet;															\
} )
*/
#define oexWAIT_UNTIL( to, st )									\
( { oexBOOL bRet = oexFALSE;										\
	if ( to )														\
	{	oexUINT uTimeout = (oexUINT)to * 1000;						\
		while ( uTimeout && !( bRet = ( st ) ) )					\
		{	oexUINT uWait = uTimeout;								\
			if ( uWait > eWaitResolution )							\
				uWait = eWaitResolution;							\
			uTimeout -= uWait;										\
			usleep( uWait );										\
		}															\
	} else bRet = ( st );											\
	bRet;															\
} )

oexRESULT CResource::Destroy( oexUINT x_uTimeout, oexBOOL x_bForce )
{
	// Error code
	oexINT nErr = 0;

	// Execute proper release function if valid handle
	if ( cInvalid() != m_hHandle && eRtInvalid != m_eType )
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
			SThreadInfo *pTi = (SThreadInfo*)m_hHandle;
			if ( !oexCHECK_PTR( pTi ) )
				oexERROR( 0, oexT( "Invalid thread object" ) );
			else
			{
				// Wait for thread to exit
				if ( waitSuccess != Wait( x_uTimeout ) )
				{
					// iii  This should not happen, don't ignore the problem,
					//      figure out how to shut this thread down properly!
					oexWARNING( nErr, oexT( "!! Terminating thread !!" ) );

					// Kill the thread
					if ( nErr = pthread_cancel( pTi->hThread ) )
						oexERROR( nErr, oexT( "pthread_cancel() failed" ) );

				} // end if

				// Attempt to join the thread
				else if ( oexINT nErr = pthread_join( pTi->hThread, oexNULL ) )
					oexERROR( nErr, oexT( "pthread_join() failed" ) );

				// Drop the memory
				OexAllocDelete( pTi );

			} // end else

		} break;

		case eRtMutex :

			if ( !oexCHECK_PTR( m_hHandle ) )
				oexERROR( 0, oexT( "Invalid mutex object" ) );
			else
			{
				// Initiailze mutex object
				if ( nErr = pthread_mutex_destroy( (pthread_mutex_t*)m_hHandle ) )
					return oexERROR( nErr, oexT( "pthread_mutex_destroy() failed : Error releasing mutex object" ) );

				// Drop the memory
				OexAllocDelete( (pthread_mutex_t*)m_hHandle );

			} // end else

			break;

		case eRtEvent :
		{
			SEventInfo *pEi = (SEventInfo*)m_hHandle;
			if ( !oexCHECK_PTR( pEi ) )
				oexERROR( 0, oexT( "Invalid event object" ) );
			else
			{
				// Release mutex object
				pEi->cMutex.Destroy();

				// Release mutex object
				if ( nErr = pthread_cond_destroy( &pEi->cond ) )
					oexERROR( nErr, oexT( "pthread_cond_destroy() failed : Error releasing pthread_cond object" ) );

				// Drop the memory
				OexAllocDelete( pEi );

			} // end else

		} break;

		default :
			oexERROR( EINVAL, oexT( "Attempt to release unknown resource type" ) );
			break;

	} // end switch

	// Clear handle info
	m_hHandle = c_Invalid;
	m_eType = eRtInvalid;

	return 0;
}

oexRESULT CResource::CreateEvent( oexCSTR x_sName, oexBOOL x_bManualReset, oexBOOL x_bInitialState )
{
	SEventInfo *pEi = OexAllocNew< SEventInfo >( 1 );
	if ( !pEi )
		return oexERROR( ENOMEM, oexT( "Out of memory" ) );
	oexZeroMemory( pEi, sizeof( SEventInfo ) );

	// Initialize member variables
	m_eType = eRtEvent;
	m_hHandle = (CResource::t_HANDLE)pEi;

	// Initialize structure
	pEi->bManualReset = x_bManualReset;
	pEi->bSignaled = x_bInitialState;

	// Create the mutex object
	if ( oexINT nErr = pEi->cMutex.CreateMutex() )
	{	Destroy(); return oexERROR( nErr, oexT( "Create mutex failed" ) ); }

	// Create condition object
	if ( oexINT nErr = pthread_cond_init( &pEi->cond, 0 ) )
	{	Destroy(); return oexERROR( nErr, oexT( "pthread_cond_init: failed" ) ); }

	return 0;
}

oexRESULT CResource::CreateMutex( oexCSTR x_sName, oexBOOL x_bInitialOwner )
{
	// Out with the old
	Destroy();

	// Allocate memory for mutex object
	pthread_mutex_t *pTm = OexAllocNew< pthread_mutex_t >( 1 );
	if ( !pTm )
		return oexERROR( ENOMEM, oexT( "Out of memory" ) );
	oexZeroMemory( pTm, sizeof( pthread_mutex_t ) );

	// Save information
	m_eType = eRtMutex;
	m_hHandle = (CResource::t_HANDLE)pTm;

	// Initiailze mutex object
	if ( oexINT nErr = pthread_mutex_init( pTm, 0 ) )
	{	Destroy(); return oexERROR( nErr, oexT( "ptherad_mutex_init() failed" ) ); }

	if ( x_bInitialOwner )
		Wait( 0 );

	return 0;
}

oexRESULT CResource::CreateThread( PFN_ThreadProc x_fnCallback, oexPVOID x_pData )
{
	// Out with the old
	Destroy();

	SThreadInfo *pTi = OexAllocNew< SThreadInfo >( 1 );
	if ( !pTi )
		return oexERROR( ENOMEM, oexT( "Out of memory" ) );
	oexZeroMemory( pTi, sizeof( SThreadInfo ) );

	m_eType = eRtThread;
	m_hHandle = (CResource::t_HANDLE)pTi;

	// Save user data
	pTi->pData = x_pData;
	pTi->fnCallback = x_fnCallback;

	// Create the thread
	oexINT nRet = pthread_create( &pTi->hThread, oexNULL,
							      CResource::ThreadProc, pTi );
	if ( nRet )
	{	oexERROR( nRet, "Error creating thread" );
		Destroy();
		return nRet;
	} // end if

    return 0;
}

oexPVOID CResource::ThreadProc( oexPVOID x_pData )
{
	SThreadInfo *pTi = (SThreadInfo*)x_pData;
	if ( !pTi )
		return (oexPVOID)-1;

	// Call user thread
	return pTi->fnCallback( pTi->pData );
}

oexRESULT CResource::Wait( oexUINT x_uTimeout )
{
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
		{
			SThreadInfo *pTi = (SThreadInfo*)m_hHandle;
			if ( !oexCHECK_PTR( pTi ) )
				return waitFailed;

			oexINT nErr = 0;
			if ( oexWAIT_UNTIL( x_uTimeout, ( nErr = pthread_kill( pTi->hThread, 0 ) ) ) )
				return waitSuccess;

			return waitTimeout;

		} break;

		case eRtMutex :
		{
			pthread_mutex_t* pTm = (pthread_mutex_t*)m_hHandle;
			if ( !oexCHECK_PTR( pTm ) )
				return waitFailed;
/*
			oexINT nErr = 0;
			oexUINT to = x_uTimeout;
			{
				if ( to )
				{
					oexUINT uTimeout = (oexUINT)to;
					struct timeval tmStart;
					struct timeval tmNow;
					gettimeofday( &tmStart, 0 );

					while ( uTimeout && ( nErr = pthread_mutex_trylock( pTm ) ) )
					{
						oexM();

						oexUINT uWait = uTimeout;
						if ( uWait > eWaitResolution )
							uWait = eWaitResolution;

						usleep( uWait );
						gettimeofday( &tmNow, 0 );
						oexINT dif = 0;

						if ( tmNow.tv_sec > tmStart.tv_sec )
							dif += ( tmNow.tv_sec - tmStart.tv_sec ) * 1000;
						if ( tmNow.tv_usec > tmStart.tv_usec )
							dif += ( tmNow.tv_usec - tmStart.tv_usec ) / 1000;
						if ( uTimeout > dif )
							uTimeout -= dif;
						else
							uTimeout = 0;
					}
				}
				else
					nErr = pthread_mutex_trylock( pTm );

				oexSHOW( nErr );
			}

			if ( !nErr )
				return waitSuccess;

*/
			oexINT nErr;
			if ( oexWAIT_UNTIL( x_uTimeout, !( nErr = pthread_mutex_trylock( pTm ) ) ) )
				return waitSuccess;


//			oexINT nErr = 0;
//			if ( oexWAIT_WHILE( x_uTimeout, ( nErr = pthread_mutex_trylock( pTm ) ) ) )
//				return waitSuccess;
//				if ( !nErr || EDEADLK == nErr )
	//				return waitSuccess;

			return waitTimeout;

		} break;

		case eRtEvent :
		{
			SEventInfo *pEi = (SEventInfo*)m_hHandle;
			if ( !oexCHECK_PTR( pEi ) )
				return oexERROR( -1, oexT( "Invalid event object" ) );

			if ( oexWAIT_UNTIL( x_uTimeout, pEi->bSignaled ) )
				return waitSuccess;

			return waitTimeout;

		} break;

	} // end switch

	// Unwaitable, ha ha
	oexERROR( waitFailed, oexT( "Attempt to wait on unwaitable resource type" ) );

	return waitFailed;
}

oexRESULT CResource::Signal( oexUINT x_uTimeout )
{
	// Error code
	oexINT nErr = -1;

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
			return waitSuccess == Wait( x_uTimeout );
			break;

		case eRtEvent :
		{
			SEventInfo *pEi = (SEventInfo*)m_hHandle;
			if ( !oexCHECK_PTR( pEi ) )
				return oexERROR( -1, oexT( "Invalid event object" ) );

			if ( pEi->cMutex.Wait( x_uTimeout ) )
				return oexERROR( nErr, oexT( "Unable to acquire lock to signal event object" ) );

			// Set state to signaled
			pEi->bSignaled = oexTRUE;

			// Unblock everyone if manual reset
			if ( pEi->bManualReset )
			{	if ( nErr = pthread_cond_broadcast( &pEi->cond ) )
					return oexERROR( nErr, oexT( "pthread_cond_signal failed" ) );
			} // end if

			// Unblock single waiting thread if auto
			else if ( nErr = pthread_cond_signal( &pEi->cond ) )
				return oexERROR( nErr, oexT( "pthread_cond_signal failed" ) );

			// Clear lock
			if ( nErr = pEi->cMutex.Reset() )
				return oexERROR( nErr, oexT( "Failed to release lock on event object" ) );

		} break;

		default :
			oexERROR( -1, oexT( "Attempt to wait on unknown resource type" ) );
			break;

	} // end switch

	return nErr;
}

oexRESULT CResource::Reset( oexUINT x_uTimeout )
{
	// Error code
	oexINT nErr = -1;

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
		{
			pthread_mutex_t* pTm = (pthread_mutex_t*)m_hHandle;
			if ( !oexCHECK_PTR( pTm ) )
				return -1;

			nErr = pthread_mutex_unlock( pTm );

		} break;

		case eRtEvent :
		{
			SEventInfo *pEi = (SEventInfo*)m_hHandle;
			if ( !oexCHECK_PTR( pEi ) )
				return oexERROR( -1, oexT( "Invalid event object" ) );

			if ( nErr = pEi->cMutex.Wait( x_uTimeout ) )
				return oexERROR( nErr, oexT( "Unable to acquire lock to signal event object" ) );

			// Set state to signaled
			pEi->bSignaled = oexFALSE;

			// Clear lock
			if ( nErr = pEi->cMutex.Reset() )
				return oexERROR( nErr, oexT( "Failed to release lock on event object" ) );

		} break;

		default :
			oexERROR( -1, oexT( "Attempt to wait on unknown resource type" ) );
			break;

	} // end switch

	return nErr;
}
