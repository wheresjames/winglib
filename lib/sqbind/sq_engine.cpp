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

int CSqEngineExport::echo( const stdString &sMsg )
{	return oexPrintf( sMsg.c_str() ); }

int CSqEngineExport::import( const stdString &sClass )
{   return OnImport( sClass ); }

int CSqEngineExport::load_module( const stdString &sModule, const stdString &sPath )
{   return OnLoadModule( sModule, sPath ); }

int CSqEngineExport::kill( const stdString &sPath )
{	CSqMsgQueue *q = queue();
	if ( !q ) return -1;
	return q->kill( oexNULL, sPath );
}

void CSqEngineExport::exit( int nExitCode )
{   oex::os::CSys::Quit( nExitCode );
}

stdString CSqEngineExport::path( const stdString &sPath )
{   return OnPath( sPath ); }

stdString CSqEngineExport::root( const stdString &sPath )
{   return oexGetModulePath( sPath.c_str() ).Ptr(); }

int CSqEngineExport::spawn( const stdString &sPath, const stdString &sName, const stdString &sScript, int bFile )
{	CSqMsgQueue *q = queue();
	if ( !q ) return -1;
	return q->spawn( oexNULL, sPath, sName, sScript, bFile );
}

int CSqEngineExport::run( const stdString &sPath, const stdString &sScript )
{	CSqMsgQueue *q = queue();
	if ( !q ) return -1;
	return q->run( oexNULL, sPath, sScript );
}

stdString CSqEngineExport::execute( int nRet, const stdString &sPath, const stdString &sFunction )
{	CSqMsgQueue *q = queue();
	if ( !q )
		return oexT( "" );

	if ( !nRet )
	{	q->execute( oexNULL, sPath, sFunction );
		return oexT( "" );
	} // end if

	stdString sRet;
	q->execute( &sRet, sPath, sFunction );
	return sRet;
}

stdString CSqEngineExport::execute1( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1 )
{	CSqMsgQueue *q = queue();
	if ( !q )
		return oexT( "" );

	if ( !nRet )
	{	q->execute( oexNULL, sPath, sFunction, sP1 );
		return oexT( "" );
	} // end if

	stdString sRet;
	q->execute( &sRet, sPath, sFunction, sP1 );
	return sRet;
}

stdString CSqEngineExport::execute2( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1, const stdString &sP2 )
{	CSqMsgQueue *q = queue();
	if ( !q )
		return oexT( "" );

	if ( !nRet )
	{	q->execute( oexNULL, sPath, sFunction, sP1, sP2 );
		return oexT( "" );
	} // end if

	stdString sRet;
	q->execute( &sRet, sPath, sFunction, sP1, sP2 );
	return sRet;
}

stdString CSqEngineExport::execute3( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3 )
{	CSqMsgQueue *q = queue();
	if ( !q )
		return oexT( "" );

	if ( !nRet )
	{	q->execute( oexNULL, sPath, sFunction, sP1, sP2, sP3 );
		return oexT( "" );
	} // end if

	stdString sRet;
	q->execute( &sRet, sPath, sFunction, sP1, sP2, sP3 );
	return sRet;
}

stdString CSqEngineExport::execute4( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3, const stdString &sP4 )
{	CSqMsgQueue *q = queue();
	if ( !q )
		return oexT( "" );

	if ( !nRet )
	{	q->execute( oexNULL, sPath, sFunction, sP1, sP2, sP3, sP4 );
		return oexT( "" );
	} // end if

	stdString sRet;
	q->execute( &sRet, sPath, sFunction, sP1, sP2, sP3, sP4 );
	return sRet;
}

void CSqEngineExport::sleep( int nMsTime )
{   oex::os::CSys::Sleep( nMsTime ); }

CSqMsgQueue* CSqEngineExport::queue()
{   return OnGetQueue(); }

int CSqEngineExport::OnImport( const stdString &sClass )
{ return 0; }

int CSqEngineExport::OnLoadModule( const stdString &sModule, const stdString &sPath )
{ return 0; }

stdString CSqEngineExport::OnPath( stdString sPath )
{ return oexBuildPath( m_sRoot.c_str(), sPath.c_str() ).Ptr(); }

CSqMsgQueue* CSqEngineExport::OnGetQueue()
{ return oexNULL; }

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
											.func( &CSqEngineExport::echo,				oexT( "echo" ) )
											.func( &CSqEngineExport::import,            oexT( "import" ) )
											.func( &CSqEngineExport::load_module,       oexT( "load_module" ) )
											.func( &CSqEngineExport::sleep,             oexT( "sleep" ) )
											.func( &CSqEngineExport::spawn,             oexT( "spawn" ) )
											.func( &CSqEngineExport::run,               oexT( "run" ) )
											.func( &CSqEngineExport::execute,           oexT( "execute" ) )
											.func( &CSqEngineExport::execute1,          oexT( "execute1" ) )
											.func( &CSqEngineExport::execute2,          oexT( "execute2" ) )
											.func( &CSqEngineExport::execute3,          oexT( "execute3" ) )
											.func( &CSqEngineExport::execute4,          oexT( "execute4" ) )
											.func( &CSqEngineExport::kill,              oexT( "kill" ) )
											.func( &CSqEngineExport::queue,             oexT( "queue" ) )
											.func( &CSqEngineExport::path,              oexT( "path" ) )
											.func( &CSqEngineExport::root,              oexT( "root" ) )
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

oex::oexBOOL CSqEngine::Load( oex::oexCSTR pScript, oex::oexBOOL bFile, oex::oexBOOL bRelative, oex::oexBOOL bStart )
{
	if ( !oexCHECK_PTR( pScript ) || !*pScript )
		return oex::oexFALSE;

	if ( !Init() )
		return oex::oexFALSE;

	_oexTRY
	{
		oex::CStr sFull;

		if ( bFile )
		{
			// +++ Change this to current working director
			//     Add oexWorkingDirectory()

			// Does it point to a valid file?
			if ( oexExists( pScript ) )

				m_sRoot = oex::CStr( pScript ).GetPath().Ptr();

			else
			{
				// Check relative to exe path
				sFull = oexGetModulePath();
				sFull.BuildPath( pScript );
				if ( oexExists( sFull.Ptr() ) )
				{	m_sRoot = sFull.GetPath().Ptr();
					pScript = sFull.Ptr();
				} // end if

				else
				{
					// Check relative to scripts path
					sFull = oexGetModulePath();
					sFull.BuildPath( "scripts" );
					sFull.BuildPath( pScript );
					if ( oexExists( sFull.Ptr() ) )
					{	m_sRoot = sFull.GetPath().Ptr();
						pScript = sFull.Ptr();
					} // end if

					else
					{	oexERROR( 0, oexMks( "Script not found : ", pScript ) );
						return oex::oexFALSE;
					} // end else

				} // end else

			} // end else

		} // end if

		// Save away root path
		else
			m_sRoot = oexGetModulePath().Ptr();

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

oex::oexBOOL CSqEngine::Start()
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
#if defined( OEX_WINDOWS )
	oexRTRACE( oexT( "%s\n" ), m_sErr.c_str() );
#endif
	oexPrintf( oexT( "%s\n" ), m_sErr.c_str() );
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

	oex::CStr sFile = ( oex::CStr() << oexT( "sqmod_" ) <<  sModule.c_str() ).DecorateName( oex::oexTRUE, oex::oexTRUE );

	if ( oex::CFile::Exists( oex::CStr( sFull ).BuildPath( sFile ).Ptr() ) )
		sFull.BuildPath( sFile );

	else if ( oex::CFile::Exists( oex::CStr( sFull ).BuildPath( oexT( "modules" ) ).BuildPath( sFile ).Ptr() ) )
		sFull.BuildPath( oexT( "modules" ) ).BuildPath( sFile );

	else
	{	oexERROR( 0, oexMks( oexT( "Module not found " ), sFile ) );
		return -3;
	} // end if

	// See if the module is already loaded
	oex::oexBOOL bExists = m_pModuleManager->Exists( sFull.Ptr() );

	// Attempt to load the module
	CModuleInstance *pMi = m_pModuleManager->Load( sFull.Ptr() );
	if ( !pMi )
	{	oexERROR( 0, oexMks( oexT( "Failed to load module " ), sFull ) );
		return -4;
	} // end if

	// Export functionality
	if ( !pMi->Export( &m_vm ) )
	{	oexERROR( 0, oexMks( oexT( "Failed to export squirrel symbols from module " ), sFull ) );
		return -5;
	} // end if

	// Log the fact that we loaded said module
	if ( !bExists )
		oexNOTICE( 0, oexMks( oexT( "Module loaded : " ), sFull ) );

	return 0;
}

void CSqEngine::SetMessageQueue( CSqMsgQueue *pMq )
{   m_pMsgQueue = pMq; }

CSqMsgQueue* CSqEngine::OnGetQueue()
{	return m_pMsgQueue; }

/*
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

SquirrelObject CSqEngine::OnExecute4( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3, const stdString &sP4 )
{	CSqMap params;
	params[ oexT( "name" ) ] = sName;
	params[ oexT( "execute3" ) ] = sFunction;
	params[ oexT( "p1" ) ] = sP1;
	params[ oexT( "p2" ) ] = sP2;
	params[ oexT( "p3" ) ] = sP3;
	params[ oexT( "p4" ) ] = sP4;
	return RouteMsg( oexT( "msg" ), params );
}
*/
