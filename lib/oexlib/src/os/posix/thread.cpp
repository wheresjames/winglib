/*------------------------------------------------------------------
// thread.cpp
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
						   ,-.        _.---._
						  |  `\.__.-''       `.
						   \  _        _  ,.   \
	 ,+++=._________________)_||______|_|_||    |
	(_.ooo.===================||======|=|=||    |
	   ~~'                 |  ~'      `~' o o  /
							\   /~`\     o o  /
							 `~'    `-.____.-'
*/

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

CThread::CThread()
{
    m_pData = 0;
    m_uSleep = 0;
}

CThread::~CThread()
{	Stop();
}

oexPVOID CThread::ThreadProc( oexPVOID x_pData )
{
    // Get pointer
    CThread *pThread = (CThread*)x_pData;
    if ( !oexCHECK_PTR( x_pData ) )
        return (oexPVOID)-1111;

	// Bad init value
    oexINT nRet = -2222;

    // Count one thread
    CThread::IncThreadCount();
    CThread::IncRunningThreadCount();

    // Get user params
    oexPVOID pData = pThread->GetUserData();
    oexUINT uSleep = pThread->GetSleepTime();

	// Initialize thread
	oexBOOL bInit = pThread->InitThread( pData );

	// Signal that we're initialized
	pThread->m_evInit.Signal();

	// Was initialization a success?
	if ( bInit )
	{
		// Loop while we're not supposed to stop
		if ( bInit && pThread->m_evStop.Wait( 0 ) )
			while ( pThread->DoThread( pData ) &&
					pThread->m_evStop.Wait( uSleep ) );

		// Kill the thread
		nRet = pThread->EndThread( pData );

	} // end if

    /// Decrement the running thread count
    CThread::DecRunningThreadCount();

    return (oexPVOID)nRet;

}

oexRESULT CThread::Start( oexPVOID x_pData, oexUINT x_uSleep )
{
    // Are we already running?
    if ( IsRunning() )
        return 0;

    // Save users data
    m_pData = x_pData;
    m_uSleep = x_uSleep;

    // Give the thread a fighting chance
    m_evStop.CreateEvent();
    m_evInit.CreateEvent();

	// Attempt to create the thread
	if ( CResource::CreateThread( CThread::ThreadProc, this ) )
		return -1;

    return 0;
}

oexRESULT CThread::Stop( oexUINT x_uWait, oexBOOL x_bKill )
{
	// Ensure thread resource is valid
	if ( !CResource::IsValid() )
		return 0;

	// Signal that the thread should exit
	m_evStop.Signal();

	// Wait for thread to stop
	if ( !WaitThreadExit( x_uWait ) )
		oexERROR( 0, "Thread failed to shutdown gracefully" );

	// Kill the thread
	oexINT nErr = CResource::Destroy( x_uWait, x_bKill );

	// Lose the events
    m_evStop.Destroy();
    m_evInit.Destroy();

    // Clear thread data
    m_pData = 0;
    m_uSleep = 0;

    return nErr;
}

oexBOOL CThread::IsRunning()
{	return !WaitThreadExit( 0 );
}

oexBOOL CThread::WaitThreadExit( oexUINT x_uTimeout )
{
	// Ensure valid thread handle
	if ( !CResource::IsValid() )
		return oexTRUE;

	// See if the thread is still alive
	if ( CResource::waitSuccess != CResource::Wait( x_uTimeout ) )
		return oexFALSE;

	return oexTRUE;
}


// The number of threads running
oexLONG CThread::m_lThreadCount = 0;
oexLONG CThread::m_lRunningThreadCount = 0;

oexUINT CThread::GetThreadCount()
{   return m_lThreadCount; }

oexUINT CThread::GetRunningThreadCount()
{   return m_lRunningThreadCount; }

void CThread::IncThreadCount()
{   CSys::InterlockedIncrement( &m_lThreadCount ); }

void CThread::IncRunningThreadCount()
{   CSys::InterlockedIncrement( &m_lRunningThreadCount ); }

void CThread::DecRunningThreadCount()
{   if ( oexVERIFY( m_lRunningThreadCount ) )
        CSys::InterlockedDecrement( &m_lRunningThreadCount );
}


