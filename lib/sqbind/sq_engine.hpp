/*------------------------------------------------------------------
// sq_engine.cpp
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

#include "stdafx.h"
using namespace sqbind;

int CSqEngineExport::alert( const stdString &sMsg )
{	return oex::os::CSys::ShowMessageBox( oexT( "Notice" ), sMsg.c_str() );
}

int CSqEngineExport::import( const stdString &sClass )
{   return OnImport( sClass ); }

int CSqEngineExport::load_module( const stdString &sModule, const stdString &sPath )
{   return OnLoadModule( sModule, sPath ); }

void CSqEngineExport::quit( int nExitCode )
{   oex::os::CSys::Quit( nExitCode );
}

stdString CSqEngineExport::path( const stdString &sPath )
{   return OnPath( sPath ); }

SquirrelObject CSqEngineExport::spawn( const stdString &sName, const stdString &sScript, int bFile )
{   return OnSpawn( sName, sScript, bFile ); }

SquirrelObject CSqEngineExport::execute( const stdString &sName, const stdString &sFunction )
{   return OnExecute( sName, sFunction ); }

SquirrelObject CSqEngineExport::execute1( const stdString &sName, const stdString &sFunction, const stdString &sP1 )
{   return OnExecute1( sName, sFunction, sP1 ); }

SquirrelObject CSqEngineExport::execute2( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2 )
{   return OnExecute2( sName, sFunction, sP1, sP2 ); }

SquirrelObject CSqEngineExport::execute3( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3 )
{   return OnExecute3( sName, sFunction, sP1, sP2, sP3 ); }

void CSqEngineExport::sleep( int nMsTime )
{   oex::os::CSys::Sleep( nMsTime ); }

CSqMsgQueue* CSqEngineExport::queue()
{   return OnGetQueue(); }

int CSqEngineExport::OnImport( const stdString &sClass ) 
{ return 0; }

int CSqEngineExport::OnLoadModule( const stdString &sModule, const stdString &sPath ) 
{ return 0; }

stdString CSqEngineExport::OnPath( stdString sPath ) 
{ return stdString( oexGetModulePath( sPath.c_str() ).Ptr() ); }

SquirrelObject CSqEngineExport::OnSpawn( const stdString &sName, const stdString &sScript, int bFile )
{   return SquirrelObject( NULL ); }

SquirrelObject CSqEngineExport::OnExecute( const stdString &sName, const stdString &sFunction )
{   return SquirrelObject( NULL ); }

SquirrelObject CSqEngineExport::OnExecute1( const stdString &sName, const stdString &sFunction, const stdString &sP1 )
{   return SquirrelObject( NULL ); }

SquirrelObject CSqEngineExport::OnExecute2( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2 )
{   return SquirrelObject( NULL ); }

SquirrelObject CSqEngineExport::OnExecute3( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3 )
{   return SquirrelObject( NULL ); }

CSqMsgQueue* CSqEngineExport::OnGetQueue() 
{ return NULL; }


BOOL CSqEngine::IsScript() 
{ return m_bLoaded; }

stdString& CSqEngine::GetLastError() 
{ return m_sErr; }

stdString& CSqEngine::GetOutput() 
{ return m_sOutput; }

SquirrelVM& CSqEngine::GetVM() 
{ return m_vm; }

CSqEngine::operator SquirrelVM&() 
{ return m_vm; }

SquirrelObject& CSqEngine::GetScriptObj() 
{ return m_script; }

const SquirrelObject& CSqEngine::GetRootTable() 
{ return m_vm.GetRootTable(); }


void CSqEngine::SqPrint( HSQUIRRELVM v, const SQChar* s, ... )
{
	StackHandler sa( v );
	SquirrelObject root( v, sa.GetObjectHandle( 1 ) );

	// Retrieve our class pointer
	SquirrelObject ptr = root.GetValue( SQEXE_SELF );
	CSqEngine *pSqEngine = (CSqEngine*)(CSqEngineExport*)ptr.GetInstanceUP( SqPlus::ClassType< CSqEngineExport >::type() );
	if ( !pSqEngine ) return;

	// Grab string from the command line
	oex::CStr sErr;
	oexVaList ap; oexVaStart( ap, s );
	sErr.vFmt( s, ap );
	oexVaEnd( ap );

	// Save the output string
	pSqEngine->m_sOutput += sErr.Ptr();
}

SQInteger CSqEngine::SqErrorHandler( HSQUIRRELVM v )
{
	if( 0 >= sq_gettop( v ) )
		return 0;

	const SQChar *sErr = 0;
	if( SQ_SUCCEEDED( sq_getstring( v, -1, &sErr ) ) )
		_oexTHROW( SScriptErrorInfo( sErr, oexT( "" ), 0, 0 ) );

	else
		_oexTHROW( SScriptErrorInfo( oexT( "Unknown error" ), oexT( "" ), 0, 0 ) );

	return 0;
}

void CSqEngine::SqCompilerErrorHandler( HSQUIRRELVM v, const SQChar *sErr, const SQChar *sSource, SQInteger line, SQInteger column )
{
	throw SScriptErrorInfo( sErr, sSource, line, column );
}

SQInteger CSqEngine::SqAuxErrorHandler( HSQUIRRELVM v )
{
	if( 0 >= sq_gettop( v ) )
		return 0;

	stdString sErr;
	const SQChar *pErr = 0;
	if( SQ_SUCCEEDED( sq_getstring( v, -1, &pErr ) ) )
		sErr = pErr;
	else
		sErr = oexT( "Unknown Error" );

	// Get stack trace
	SQStackInfos si;
	if ( SQ_SUCCEEDED( sq_stackinfos( v, 1, &si ) ) )
	{   stdString sMsg;
		if ( si.funcname )
		{   sMsg += si.funcname;
			sMsg += oexT( "() : " );
		} // end if
		sMsg += sErr;
		_oexTHROW( SScriptErrorInfo( sMsg.c_str(), si.source ? si.source : oexT( "" ), si.line, 0 ) );
	} // end if

	else
		_oexTHROW( SScriptErrorInfo( sErr.c_str(), oexT( "" ), 0, 0 ) );

	return 0;
}


CSqEngine::CSqEngine() :
	m_vm( SquirrelVM::StdLib_All ),
	m_script( m_vm.GetVMHandle() )
{
	m_bLoaded = oex::oexFALSE;
}

CSqEngine::~CSqEngine() 
{ Destroy(); }

void CSqEngine::Destroy()
{
	Execute( SQBIND_NOREPLY, SQEXE_FN_END );

	m_script.Reset();

	m_vm.Shutdown();

	m_sErr = oexT( "" );
	m_sOutput = oexT( "" );
	m_sRoot = oexT( "" );

	m_bLoaded = oex::oexFALSE;
}

oex::oexBOOL CSqEngine::Init()
{
	Destroy();

	_oexTRY
	{
		// Initialize the virtual machine
		m_vm.Init();

		// Set compiler error handler
		sq_setcompilererrorhandler( m_vm.GetVMHandle(), &CSqEngine::SqCompilerErrorHandler );

		// Set print function
		sq_setprintfunc( m_vm.GetVMHandle(), CSqEngine::SqPrint );

		// Set run time error handler
//        sq_newclosure( m_vm.GetVMHandle(), &CSqEngine::SqErrorHandler, 0 );

		// Set auxiliary error handler
		sq_newclosure( m_vm.GetVMHandle(), &CSqEngine::SqAuxErrorHandler, 0 );

		sq_seterrorhandler( m_vm.GetVMHandle() );

		// Bind Squirrel variables
		sqbind::SqBindAll( m_vm );

		// Squirrel must understand CMsgQueue
		SqPlus::SQClassDef< CSqMsgQueue > ( m_vm, oexT( "CSqMsgQueue" ) )												  ;

		// Define our base class
		SqPlus::SQClassDef< CSqEngineExport > ( m_vm, oexT( "CSqEngineExport" ) )
											.func( &CSqEngineExport::alert,             oexT( "alert" ) )
											.func( &CSqEngineExport::import,            oexT( "import" ) )
											.func( &CSqEngineExport::load_module,       oexT( "load_module" ) )
											.func( &CSqEngineExport::sleep,             oexT( "sleep" ) )
											.func( &CSqEngineExport::spawn,             oexT( "spawn" ) )
											.func( &CSqEngineExport::execute,           oexT( "execute" ) )
											.func( &CSqEngineExport::execute1,          oexT( "execute1" ) )
											.func( &CSqEngineExport::execute2,          oexT( "execute2" ) )
											.func( &CSqEngineExport::execute3,          oexT( "execute3" ) )
											.func( &CSqEngineExport::quit,              oexT( "quit" ) )
											.func( &CSqEngineExport::queue,             oexT( "queue" ) )
											.func( &CSqEngineExport::path,              oexT( "path" ) )
										  ;

		// Set base class pointer
		BindRootVariable( (CSqEngineExport*)this, SQEXE_SELF );

		// Allow derived class to register it's stuff
		OnRegisterVariables();

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	return LogError( oex::oexFALSE, e ); }
	_oexCATCH( SquirrelError &e )
	{	m_sErr = e.desc; return oex::oexFALSE; }

	m_bLoaded = TRUE;

	return oex::oexTRUE;
}

BOOL CSqEngine::Load( oex::oexCSTR pScript, oex::oexBOOL bFile, oex::oexBOOL bRelative, oex::oexBOOL bStart )
{
	if ( !oexCHECK_PTR( pScript ) || !*pScript )
		return oex::oexFALSE;

	if ( !Init() )
		return oex::oexFALSE;

	_oexTRY
	{
		// Save away root path
		m_sRoot = oexGetModulePath().Ptr();

		oex::CStr sFull = oexGetModulePath();
		if ( bFile && bRelative )
		{
			// Append path
			//_tcscat( szFull, pScript );
			sFull.BuildPath( pScript );

			// Point to new path
			pScript = sFull.Ptr();

		} // end if

		// Load the script
		m_script = bFile ? m_vm.CompileScript( pScript )
						 : m_vm.CompileBuffer( pScript );

		if ( bStart )
		{
			// Initialize the script
			m_vm.RunScript( m_script );

			// Execute init function
			Execute( SQBIND_NOREPLY, SQEXE_FN_INIT );

		} // end if

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	return LogError( oex::oexFALSE, e ); }
	_oexCATCH( SquirrelError &e )
	{	m_sErr = e.desc; return oex::oexFALSE; }

	// Save script source information
	m_bFile = bFile;
	m_sScript = pScript;

	return oex::oexTRUE;
}

BOOL CSqEngine::Start()
{
	if ( !IsScript() )
		return oex::oexFALSE;

	_oexTRY
	{
		// Initialize the script
		m_vm.RunScript( m_script );

		// Execute init function
		Execute( SQBIND_NOREPLY, SQEXE_FN_INIT );

	} // end try
	_oexCATCH( SScriptErrorInfo &e )
	{	return LogError( oex::oexFALSE, e ); }
	_oexCATCH( SquirrelError &e )
	{	m_sErr = e.desc; return oex::oexFALSE; }

	return oex::oexTRUE;
}

oex::oexBOOL CSqEngine::Idle()
{
	return Execute( SQBIND_NOREPLY, SQEXE_FN_IDLE );
}


oex::oexBOOL CSqEngine::Run( oex::oexCSTR pScript )
{
	if ( !oexCHECK_PTR( pScript ) || !*pScript )
		return oex::oexFALSE;

	_oexTRY
	{
		SquirrelObject script( m_vm.GetVMHandle() );
		script = m_vm.CompileBuffer( pScript );
		m_vm.RunScript( script );

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	return LogError( oex::oexFALSE, e ); }
	_oexCATCH( SquirrelError &e )
	{	m_sErr = e.desc; return oex::oexFALSE; }

	return oex::oexTRUE;
}

oex::oexINT CSqEngine::LogError( oex::oexINT x_nReturn, SScriptErrorInfo &x_e )
{	oex::CStr sErr = oex::CStr().Fmt( oexT( "%s(%lu)\r\n   %s" ), x_e.sSource.c_str(), x_e.uLine, x_e.sDesc.c_str() );
	oexERROR( 0, sErr );
	m_sErr = sErr.Ptr();
	return x_nReturn;
}


oex::oexBOOL CSqEngine::Execute( stdString *pRet, oex::oexCSTR x_pFunction )
{
	if ( !IsScript() || !x_pFunction  )
		return oex::oexFALSE;

	_oexTRY
	{
		SqPlus::SquirrelFunction< SquirrelObject > f( m_vm, m_vm.GetRootTable(), x_pFunction );

		if ( f.func.IsNull() )
		{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
			return oex::oexFALSE;
		} // end if
		
		// Call the function
		if ( oexCHECK_PTR( pRet ) )
			*pRet = f().ToString();
		else
			f();

	} // end try

	_oexCATCH( SScriptErrorInfo &e )
	{	return LogError( oex::oexFALSE, e ); }
	_oexCATCH( SquirrelError &e )
	{	m_sErr = e.desc; return oex::oexFALSE; }

	return oex::oexTRUE;
}

void CSqEngine::SetModuleManager( CModuleManager *x_pMm )
{   m_pModuleManager = x_pMm; }

int CSqEngine::OnImport( const stdString &sClass )
{
	return -1;
}

int CSqEngine::OnLoadModule( const stdString &sModule, const stdString &sPath )
{
	if ( !sModule.length() )
		return -1;

	if ( !m_pModuleManager )
		return -2;

	oex::CStr sFull;
	if ( sPath.length() )
		sFull = sPath.c_str();
	else
		sFull = oexGetModulePath();

	// Create full path
	sFull.BuildPath( ( oex::CStr() << oexT( "sqmod_" ) <<  sModule.c_str() ).DecorateName( oex::oexTRUE, oex::oexTRUE ) );

	// Attempt to load the module
	CModuleInstance *pMi = m_pModuleManager->Load( sFull.Ptr() );
	if ( !pMi )
		return -3;

	// Export functionality
	if ( !pMi->Export( &m_vm ) )
		return -4;

	return 0;
}

void CSqEngine::SetMessageQueue( CSqMsgQueue *pMq )
{   m_pMsgQueue = pMq; }

CSqMsgQueue* CSqEngine::OnGetQueue()
{	return m_pMsgQueue; }

SquirrelObject CSqEngine::OnSpawn( const stdString &sName, const stdString &sScript, int bFile )
{
	if ( m_pMsgQueue )
	{
		CSqMap params;
		params[ oexT( "name" ) ] = sName;
		params[ oexT( "script" ) ] = sScript;
		params[ oexT( "file" ) ] = bFile ? oexT( "1" ) : oexT( "0" );

		m_pMsgQueue->Msg( oexT( "spawn" ), &params );

	} // end if

	return SquirrelObject( m_vm.GetVMHandle() );
}

SquirrelObject CSqEngine::RouteMsg( const stdString &sMsg, CSqMap &rParams )
{	if ( m_pMsgQueue )
		m_pMsgQueue->Msg( sMsg, &rParams );
	return SquirrelObject( m_vm.GetVMHandle() );
}

SquirrelObject CSqEngine::OnExecute( const stdString &sName, const stdString &sFunction )
{	CSqMap params;
	params[ oexT( "name" ) ] = sName;
	params[ oexT( "execute" ) ] = sFunction;
	return RouteMsg( oexT( "msg" ), params );
}

SquirrelObject CSqEngine::OnExecute1( const stdString &sName, const stdString &sFunction, const stdString &sP1 )
{	CSqMap params;
	params[ oexT( "name" ) ] = sName;
	params[ oexT( "execute1" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	return RouteMsg( oexT( "msg" ), params );
}

SquirrelObject CSqEngine::OnExecute2( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2 )
{	CSqMap params;
	params[ oexT( "name" ) ] = sName;
	params[ oexT( "execute2" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	params[ oexT( "p2" ) ] = sP2;
	return RouteMsg( oexT( "msg" ), params );
}

SquirrelObject CSqEngine::OnExecute3( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3 )
{	CSqMap params;
	params[ oexT( "name" ) ] = sName;
	params[ oexT( "execute3" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	params[ oexT( "p2" ) ] = sP2;
	params[ oexT( "p3" ) ] = sP3;
	return RouteMsg( oexT( "msg" ), params );
}
