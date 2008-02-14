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

oexINT COex::Init()
{
    // Initialize system
    oexVERIFY( os::CSys::Init() );

#ifdef OEX_ENABLE_TESTS

	// Register standard library tests
//	OEX_REGISTER_TEST( oex_standard_tests );

#endif

	m_nStartupCode = 1;

	return m_nStartupCode;
}


oexINT COex::Uninit()
{
	m_nShutdownCode = 1;

    // We have to allow for the thread pool if it's still running
//    oexUINT uAllowedThreads = 0;
//    if ( COexThreadMgr::IsRunning() )
//        uAllowedThreads++;

    // You should have shutdown all threads by now!
//    if ( !oexVERIFY( CThread::GetRunningThreadCount() == 0 ) )
//        m_nShutdownCode |= -1;

    // Release thread pool memory
//    if ( !oexVERIFY( COexThreadMgr::Stop() ) )
//        m_nShutdownCode |= -2;

//    if ( !oexVERIFY( COexThreadPool::Stop() ) )
//        m_nShutdownCode |= -4;

//    if ( !oexVERIFY( msgOrb.Destroy() ) )
//        m_nShutdownCode |= -1;

//    if ( !oexVERIFY( oexNet.Destroy() ) )
//        m_nShutdownCode |= -1;

    // Uninit the system stuff
    if ( !oexVERIFY( os::CSys::Uninit() ) )
        m_nShutdownCode |= -8;

    // Free all the test classes
//    oexFREE_TESTS();
    
#if defined( OEX_CRT_LEAK_DETECTION ) && defined( _DEBUG )

    oexTRACE( oexT( "\n-------------------------------- oexlib is dumping memory leaks ---\n" ) );
    
    os::CMem::DumpLeaks();

      oexTRACE( oexT( "-------------------------------- Memory Report --------------------\n" ) );

    os::CMem::MemReport();

      oexTRACE( oexT( "-------------------------------- End oexlib memory report ---------\n\n" ) );
#endif
    
	return m_nShutdownCode;
}

