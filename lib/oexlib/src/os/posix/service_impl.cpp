/*------------------------------------------------------------------
// service_impl.cpp
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

// Singleton instance
CServiceImpl *CServiceImpl::m_pInstance = oexNULL;

CServiceImpl::CServiceImpl()
{
	// Save instance pointer
	m_pInstance = this;

	// No command line yet
	m_argc = oexNULL;
	m_argv = oexNULL;

	// Set service handle to null
	m_hService = oexNULL;

	// I assume we want to start off running
	// ( this is needed to ensure things work when running 'as exe')
	m_bRun = oexTRUE;

	// Auto restart service by default
	m_bAutoRestart = oexTRUE;
}

CServiceImpl::~CServiceImpl()
{
	m_pInstance = oexNULL;
	m_argc = oexNULL;
	m_argv = oexNULL;
	m_hService = oexNULL;
	m_bRun = oexFALSE;
}

CServiceImpl* CServiceImpl::GetInstance()
{
	return m_pInstance;
}

int CServiceImpl::RunService( int argc, const char** argv, oexCSTR pName, oexCSTR pDesc  )
{
	// Get singleton instance
	OEX_NAMESPACE::os::CServiceImpl *pInstance = OEX_NAMESPACE::os::CServiceImpl::GetInstance();
	if ( !pInstance )
	{	oexERROR( 0, oexT( "Service not implemented" ) );
		return -1;
	} // end if
	
	// Scope in
	oexINT nRet = pInstance->OnRunService( argc, argv, pName, pDesc );
	
	return nRet;
}

int CServiceImpl::OnRunService( int argc, const char** argv, oexCSTR pName, oexCSTR pDesc )
{
	// Set the command line
	SetCommandLine( argc, argv );

	// Run as plain old executable?
	if ( CommandLine().IsKey( oexT( "run" ) ) )
		return OnRun();

	if ( CommandLine().IsKey( oexT( "install" ) ) )
	{
		int nErr = InstallService( pName, pDesc, getAutoRestart() );
		if ( 0 > nErr )
		{	oexEcho( oexT( "Error installing service" ) );
			return nErr;
		} // end if

		oexEcho( oexT( "Installed service" ) );

		return 0;

	} // end if

	if ( CommandLine().IsKey( oexT( "remove" ) ) 
		 || CommandLine().IsKey( oexT( "uninstall" ) ) )
	{		
		int nErr = RemoveService( pName );
		if ( 0 > nErr )
		{	oexEcho( oexT( "Error removing service" ) );
			return nErr;
		} // end if

		oexEcho( oexT( "Removed service" ) );

		return 0;

	} // end if

	// Fork and return from parent
	if ( oexINT nRet = oexFork() )
	{
		oexPrintf( "Return value : %d\n", nRet );
		
//		oexEcho( "deamon started" );
	
		return nRet;
		
	} // end if
		
	// *** Child fork now *** //

	// Run as deamon
	return OnRun();
}

void CServiceImpl::SetCommandLine( int argc, const char **argv )
{
	// Save command line pointers
	m_argc = argc;
	m_argv = argv;

	if ( argc && argv )
		// Get command line params
		m_pbCommandLine = oex::CParser::ParseCommandLine( argc, argv );
	else
		m_pbCommandLine.Destroy();
}

void CServiceImpl::OnServiceMain( int argc, const char** argv )
{
	return;
}

void CServiceImpl::OnServiceHandler( unsigned int fdwControl )
{
	return; 
}

/// +++ Figure out Linux deamon install / uninstall 

int CServiceImpl::InstallService( oexCSTR pName, oexCSTR pDesc, oexBOOL bAutoRestart )
{
	if ( !pName || !*pName )
	{	oexERROR( 0, oexT( "Invalid Parameter" ) );
		return -1;
	} // end if

	// Use name as description if not specified
	if ( !pDesc || !*pDesc )
		pDesc = pName;

	oexEcho( "Service Install/Uninstall Not Implemented" );

	return -1;
}

int CServiceImpl::RemoveService( oexCSTR pName )
{
	if ( !pName )
	{	oexERROR( 0, oexT( "Invalid Parameter" ) );
		return -1;
	} // end if

	oexEcho( "Service Install/Uninstall Not Implemented" );

	return -1;
}

