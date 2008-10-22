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
	CResource			cMutex;
	pthread_cond_t		cond;
	oexBOOL				bManualReset;
	oexBOOL				bSignaled;
};

oexRESULT CResource::Create( E_RESOURCE_TYPE x_eType, oexCSTR x_sName )
{
	// Lose any existing resource
	Destroy();

	oexINT nErr = -1;

	// Execute proper Init function if valid handle
	if ( cInvalid() != m_hHandle )
		switch( x_eType )
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

			// Allocate memory for mutex object
			m_hHandle = (CResource::t_HANDLE)OexAllocNew< pthread_mutex_t >( 1 );

			// Initiailze mutex object
			nErr = pthread_mutex_init( (pthread_mutex_t*)m_hHandle, oexNULL );

			break;

		case eRtEvent :
		{
			SEventInfo *pEi = OexAllocNew< SEventInfo >( 1 );
			if ( !pEi )
				return oexERROR( -1, oexT( "Out of memory" ) );

			// Allocate memory for mutex object
			m_hHandle = (CResource::t_HANDLE)pEi;

			// Create the mutex object
			if ( nErr = pEi->cMutex.Create( eRtMutex ) )
				return oexERROR( nErr, oexT( "Create mutex failed" ) );

			// Create condition object
			if ( nErr = pthread_cond_init( &pEi->cond, 0 ) )
				return oexERROR( nErr, oexT( "pthread_cond_init: failed" ) );

			// Init state
			pEi->bManualReset = oexTRUE;
			pEi->bSignaled = oexFALSE;

		} break;

		default :
			oexERROR( -1, oexT( "Attempt to release unknown resource type" ) );
			break;

	} // end switch

	m_eType = x_eType;

	return nErr;
}

oexRESULT CResource::Destroy()
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
			os::CBaseFile::Close( m_hHandle, &nErr );
			break;

		case eRtSocket :
			break;

		case eRtThread :
			break;

		case eRtMutex :

			// Initiailze mutex object
			if ( nErr = pthread_mutex_destroy( (pthread_mutex_t*)m_hHandle ) )
				return oexERROR( nErr, oexT( "Error releasing mutex object" ) );

			// Drop the memory
			OexAllocDelete( (pthread_mutex_t*)m_hHandle );

			m_hHandle = cInvalid();

			break;

		case eRtEvent :
		{
			SEventInfo *pEi = (SEventInfo*)m_hHandle;
			if ( !oexCHECK_PTR( pEi ) )
				return oexERROR( -1, oexT( "Invalid event object" ) );

			// Release mutex object
			if ( nErr = pEi->cMutex.Destroy() )
				return oexERROR( nErr, oexT( "Error releasing mutex object" ) );

			// Release mutex object
			if ( nErr = pthread_cond_destroy( &pEi->cond ) )
				return oexERROR( nErr, oexT( "Error releasing pthread_cond object" ) );

			// Drop the memory
			OexAllocDelete( pEi );

			m_hHandle = cInvalid();

		} break;

		default :
			oexERROR( -1, oexT( "Attempt to release unknown resource type" ) );
			break;

	} // end switch

	// Clear handle info
	m_hHandle = c_Invalid;
	m_eType = eRtInvalid;

	return nErr;
}

oexRESULT CResource::Wait( oexUINT x_uTimeout )
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
			struct timeval tmStart;
			struct timeval tmNow;

			if ( x_uTimeout )
				gettimeofday( &tmStart, 0 );

			// +++ This sux, is there a better way in linux?
			while ( nErr = pthread_mutex_trylock( (pthread_mutex_t*)m_hHandle ) )
			{
				if ( !x_uTimeout )
					return waitTimeout;

				// Wait
				usleep( eWaitResolution );

				// Get the current time and update the timeout
				gettimeofday( &tmNow, 0 );

				oexINT dif = 0;
				if ( tmNow.tv_sec > tmStart.tv_sec )
					dif += ( tmNow.tv_sec - tmStart.tv_sec ) * 1000;

				if ( tmNow.tv_usec > tmStart.tv_usec )
					dif += ( tmNow.tv_usec - tmStart.tv_usec ) / 1000;

				if ( x_uTimeout > dif )
					x_uTimeout -= dif;
				else
					x_uTimeout = 0;

			} // end while

			return waitSuccess;

		} break;

		case eRtEvent :
		{
			SEventInfo *pEi = (SEventInfo*)m_hHandle;
			if ( !oexCHECK_PTR( pEi ) )
				return oexERROR( -1, oexT( "Invalid event object" ) );

			struct timeval tmStart;
			struct timeval tmNow;

			if ( x_uTimeout )
				gettimeofday( &tmStart, 0 );

			while ( !pEi->bSignaled )
			{

			} // end while

			return waitSuccess;

		} break;

		default :
			oexERROR( -1, oexT( "Attempt to wait on unknown resource type" ) );
			break;

	} // end switch

	return nErr;
}

oexRESULT CResource::Signal()
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

			nErr = pthread_mutex_lock( (pthread_mutex_t*)m_hHandle );

			break;

		case eRtEvent :
		{
			SEventInfo *pEi = (SEventInfo*)m_hHandle;
			if ( !oexCHECK_PTR( pEi ) )
				return oexERROR( -1, oexT( "Invalid event object" ) );

			if ( nErr = pEi->cMutex.Wait() )
				return oexERROR( nErr, oexT( "Enable to acquire lock to signal event object" ) );

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

oexRESULT CResource::Reset()
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

			nErr = pthread_mutex_unlock( (pthread_mutex_t*)m_hHandle );

			break;

		case eRtEvent :
		{
			SEventInfo *pEi = (SEventInfo*)m_hHandle;
			if ( !oexCHECK_PTR( pEi ) )
				return oexERROR( -1, oexT( "Invalid event object" ) );

			if ( nErr = pEi->cMutex.Wait() )
				return oexERROR( nErr, oexT( "Enable to acquire lock to signal event object" ) );

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
