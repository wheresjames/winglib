/*------------------------------------------------------------------
// oex.cpp
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

#include "../oexlib.h"

OEX_USING_NAMESPACE

#ifdef OEX_ENABLE_TESTS
//#	include "../inc/test_functions.h"
#endif

oexINT					COex::m_nStartupCode = 0;
oexINT					COex::m_nShutdownCode = 0;
COex::CVerifyStartup	COex::m_cVerifyStartup;

#if defined( oexDEBUG )
CMemLeak				COex::m_cMemLeak;
CMemLeak& COex::GetMemLeak() { return m_cMemLeak; }
#endif

CBinShare& COex::GetBinShare() 
{	
	CBinShare *p = (CBinShare*)CMem::GetRawAllocator().pBinShare;
	if ( !oexVERIFY_PTR( p ) )
		return CBinShare();

	return *p;
}


oexINT COex::Init()
{
#if defined( oexDEBUG )

	// Create memory leak detector
	m_cMemLeak.Create();

#endif

	// Initialize file system
	os::CBaseFile::InitFileSystem();

    // Initialize system
    oexVERIFY( os::CSys::Init() );

#ifdef OEX_ENABLE_TESTS

	// Register standard library tests
//	OEX_REGISTER_TEST( oex_standard_tests );

#endif

	m_nStartupCode = 1;

	return m_nStartupCode;
}

#if defined( OEX_GCC )

	extern TAssoList< oexINT, CStr8 > g_lstFileMappingInfo;

#endif

oexINT COex::Uninit()
{
	m_nShutdownCode = 1;

    // We have to allow for the thread pool if it's still running
//    oexUINT uAllowedThreads = 0;
//    if ( COexThreadMgr::IsRunning() )
//        uAllowedThreads++;

    // You should have shutdown all threads by now!
    if ( !oexVERIFY( CThread::GetRunningThreadCount() == 0 ) )
	{	oexERROR( 0, oexT( "Shutdown attempted while threads are still running" ) );

		// Give threads a moment to shutdown
		oexUINT uTimeout = oexDEFAULT_WAIT_TIMEOUT * 1000;
		while ( uTimeout && CThread::GetRunningThreadCount() )
		{	oexUINT uDelay = uTimeout;
			if ( uDelay > os::CResource::eWaitResolution )
				uDelay = os::CResource::eWaitResolution;
			uTimeout -= uDelay;
			oexMicroSleep( uDelay );
		} // end while

		// Log again to let them know it's going to blow up
		if ( CThread::GetRunningThreadCount() )
			oexERROR( 0, oexT( "Threads failed to exit before timeout" ) );

        m_nShutdownCode |= -1;

	} // end if

    // Release thread pool memory
//    if ( !oexVERIFY( COexThreadMgr::Stop() ) )
//        m_nShutdownCode |= -2;

//    if ( !oexVERIFY( COexThreadPool::Stop() ) )
//        m_nShutdownCode |= -4;

//    if ( !oexVERIFY( msgOrb.Destroy() ) )
//        m_nShutdownCode |= -1;

//    if ( !oexVERIFY( oexNet.Destroy() ) )
//       m_nShutdownCode |= -1;

	// Verify sockets were released correctly
	oexVERIFY( !os::CIpSocket::GetInitCount() );

	// Close the log file
	CLog::GlobalLog().Destroy();

    // Uninit the system stuff
    if ( !oexVERIFY( os::CSys::Uninit() ) )
        m_nShutdownCode |= -8;

	// Free file system
	os::CBaseFile::FreeFileSystem();

	// Free any binary shares
	GetBinShare().Destroy();

    // Free all the test classes
//	oexFREE_TESTS();

#if defined( OEX_GCC )

	// Lose file mappings
	g_lstFileMappingInfo.Destroy();

#endif

#if defined( oexDEBUG )

	// Freeze memory statistics
	m_cMemLeak.Freeze();

	CStr sReport = m_cMemLeak.Report();

//	oexEcho( sReport.Ptr() );

	oexTRACE( sReport.Ptr() );

	// Create memory leak detector
	m_cMemLeak.Destroy();

#endif


/*
#if !defined( OEX_NOCRTDEBUG ) && defined( OEX_CRT_LEAK_DETECTION ) && defined( oexDEBUG ) && defined( OEX_WIN32 )

    oexTRACE( oexT( "\n-------------------------------- oexlib is dumping memory leaks ---\n" ) );

    os::CMem::DumpLeaks();

      oexTRACE( oexT( "-------------------------------- Memory Report --------------------\n" ) );

    os::CMem::MemReport();

      oexTRACE( oexT( "-------------------------------- End oexlib memory report ---------\n\n" ) );
#endif
*/

	return m_nShutdownCode;
}

