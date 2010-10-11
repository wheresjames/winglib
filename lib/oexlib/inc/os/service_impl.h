/*------------------------------------------------------------------
// service_impl.h
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

#pragma once

class CServiceImpl
{
public:

	/// Default constructor
	CServiceImpl();

	/// Destructor
	virtual ~CServiceImpl();

	/// Sets the command line
	void SetCommandLine( int argc, const char **argv );

	/// Returns a reference to the parsed command line parameters
	CPropertyBag& CommandLine()
	{	return m_pbCommandLine; }

	/// Sets the service name, call this from the constructor
	void SetName( CStr sName )
	{	m_sName = sName; }

	/// Returns the name of the service
	CStr& GetName()
	{	return m_sName; }

	/// Set to non-zero to enable auto service restart
	void setAutoRestart( oexBOOL b )
	{	m_bAutoRestart = b; }

	/// Returns the status of the auto restart flag
	oexBOOL getAutoRestart()
	{	return m_bAutoRestart; }

	/// Returns non zero if the service should keep running
	oexBOOL IsRunning()
	{	return m_bRun; }

	/// Call from main() to make this thing go
	static int RunService( int argc, const char** argv, oexCSTR pName, oexCSTR pDesc );

	/// Installs the service
	static int InstallService( oexCSTR pName, oexCSTR pDesc, oexCSTR pExe, oexBOOL bAutoRestart );

	/// Uninstalls the service
	static int RemoveService( oexCSTR pName );

	/// Starts the service
	static int Start( oexCSTR pName );

	/// Stops the service
	static int Stop( oexCSTR pName );

	/// Shutsdown the service
	static int Shutdown( oexCSTR pName );

	/// Restarts the service
	static int Restart( oexCSTR pName );

	/// Registers the specified server
	static oexINT RegisterServer( oexCSTR pID, oexCONST oexGUID &guidCLSID, 
								  oexCSTR pThreadingModel, oexCSTR pPath, 
								  oexBOOL bControl, oexCONST oexGUID *pguidTypeLib );
	static oexINT RegisterServer( oexCSTR pID, oexCSTR pCLSID, 
								  oexCSTR pThreadingModel, oexCSTR pPath, 
								  oexBOOL bControl, oexCSTR pTypeLib );

	/// Unregisters the specified server
	static oexINT UnregisterServer( oexCSTR pID, oexCONST oexGUID &guidCLSID );
	static oexINT UnregisterServer( oexCSTR pID, oexCSTR pCLSID );

	/// Register the specified interface
	static oexINT RegisterInterface( oexCSTR pName, oexCONST oexGUID &guidID, oexLONG nNumMethods );
	static oexINT RegisterInterface( oexCSTR pName, oexCSTR pID, oexLONG nNumMethods );

	/// Unregisters the specified interface
	static oexINT UnregisterInterface( oexCONST oexGUID &guidID );
	static oexINT UnregisterInterface( oexCSTR pID );

public:

	/// Call from main() to make this thing go
	virtual int OnRunService( int argc, const char** argv, oexCSTR pName, oexCSTR pDesc );

	/// Main loop, this is the one you probably want to override
	virtual int OnRun() { return -1; }

	/// Called to initiailize the service, service will exit if return is less than zero
	virtual int OnInitService() { return 0; }

	/// Called on exiting a service
	virtual int OnExitService() { return 0; }

	/// Main service loop
	virtual void OnServiceMain( int argc, const char** argv );

	/// Service control handler
	virtual void OnServiceHandler( unsigned int fdwControl );

public:

	/// Returns the singleton instance
	static CServiceImpl* GetInstance();

private:

	/// Command line argc
	int						m_argc;

	/// Command line arguments
	const char**			m_argv;

	/// Service name, you must set this in the constructor to have any effect
	CStr					m_sName;

	/// The command line params
	CPropertyBag			m_pbCommandLine;

	/// Service instance
	static CServiceImpl		*m_pInstance;

	/// Service handle
	oexPVOID				m_hService;

	/// Non zero if service should be running
	oexBOOL					m_bRun;

	/// Non zero for auto restart
	oexBOOL					m_bAutoRestart;

};
