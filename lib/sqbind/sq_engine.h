/*------------------------------------------------------------------
// sq_engine.h
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

//==================================================================
// CSqEngineExport
//
/// This base class is imported in to the squirrel environment
//==================================================================
class CSqEngineExport
{
public:

	enum
	{
		/// Size of a standard string
		eStrSize = 1024
	};

public:

	// Simple alert function
	int alert( const std::tstring &sMsg )
	{	return oex::os::CSys::ShowMessageBox( oexT( "Notice" ), sMsg.c_str() );
	}

	int import( const std::tstring &sClass )
	{   return OnImport( sClass ); }

	int load_module( const std::tstring &sModule, const std::tstring &sPath )
	{   return OnLoadModule( sModule, sPath ); }

	void quit( int nExitCode )
	{   oex::os::CSys::Quit( nExitCode );
	}

	std::tstring path( const std::tstring &sPath )
	{   return OnPath( sPath ); }

	SquirrelObject spawn( const std::tstring &sName, const std::tstring &sScript, int bFile )
	{   return OnSpawn( sName, sScript, bFile ); }

	SquirrelObject execute( const std::tstring &sName, const std::tstring &sFunction )
	{   return OnExecute( sName, sFunction ); }

	SquirrelObject execute1( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1 )
	{   return OnExecute1( sName, sFunction, sP1 ); }

	SquirrelObject execute2( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1, const std::tstring &sP2 )
	{   return OnExecute2( sName, sFunction, sP1, sP2 ); }

	SquirrelObject execute3( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1, const std::tstring &sP2, const std::tstring &sP3 )
	{   return OnExecute3( sName, sFunction, sP1, sP2, sP3 ); }

	void sleep( int nMsTime )
	{   oex::os::CSys::Sleep( nMsTime ); }

	// _self.queue()
	CSqMsgQueue* queue()
	{   return OnGetQueue(); }

protected:

	virtual int OnImport( const std::tstring &sClass ) { return 0; }

	virtual int OnLoadModule( const std::tstring &sModule, const std::tstring &sPath ) { return 0; }

	virtual std::tstring OnPath( std::tstring sPath ) { return std::tstring( oexGetModulePath( sPath.c_str() ).Ptr() ); }

	virtual SquirrelObject OnSpawn( const std::tstring &sName, const std::tstring &sScript, int bFile )
	{   return SquirrelObject( NULL ); }

	virtual SquirrelObject OnExecute( const std::tstring &sName, const std::tstring &sFunction )
	{   return SquirrelObject( NULL ); }
	virtual SquirrelObject OnExecute1( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1 )
	{   return SquirrelObject( NULL ); }
	virtual SquirrelObject OnExecute2( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1, const std::tstring &sP2 )
	{   return SquirrelObject( NULL ); }
	virtual SquirrelObject OnExecute3( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1, const std::tstring &sP2, const std::tstring &sP3 )
	{   return SquirrelObject( NULL ); }

	virtual CSqMsgQueue* OnGetQueue() { return NULL; }

protected:

	/// Non-zero if loaded from file
	oex::oexBOOL        m_bFile;

	/// Either a file name or actual script
	std::tstring        m_sScript;

};

// Declare class functions
DECLARE_INSTANCE_TYPE( CSqEngineExport );

namespace sqbind
{
	/// Squirrel engine
    class CSqEngine : public CSqEngineExport
    {
	public:

		#define     SQEXE_FN_INIT       oexT( "_init" )
		#define     SQEXE_FN_IDLE       oexT( "_idle" )
		#define     SQEXE_FN_END        oexT( "_end" )
		#define     SQEXE_SELF          oexT( "_self" )

	public:

		/// Contains information about a script error
		struct SScriptErrorInfo
		{
			/// Constructor
			SScriptErrorInfo( std::tstring x_sDesc, std::tstring x_sSource, oex::oexUINT x_uLine, oex::oexUINT x_uCol )
			{   sDesc = x_sDesc; sSource = x_sSource; uLine = x_uLine; uCol = x_uCol; }

			/// Description
			std::tstring    sDesc;

			/// Source file
			std::tstring    sSource;

			/// Line number
			oex::oexUINT    uLine;

			/// Column number
			oex::oexUINT    uCol;
		};

	public:

		/// Over-ride to perform special registration
		virtual void OnRegisterVariables() {}

	public:

		/// Returns non-zero if squirrel has been initialized
		BOOL IsScript() { return m_bLoaded; }

		/// Returns the last error
		std::tstring& GetLastError() { return m_sErr; }

		/// Returns the script output
		std::tstring& GetOutput() { return m_sOutput; }

		/// Returns a reference the to the virtual machine
		SquirrelVM& GetVM() { return m_vm; }

		/// Returns a reference to the virtual machine
		operator SquirrelVM&() { return m_vm; }

		/// Returns a referenced to the compiled script object
		SquirrelObject& GetScriptObj() { return m_script; }

		/// Returns the scripts root table
		const SquirrelObject& GetRootTable() { return m_vm.GetRootTable(); }


	public:

		/// Intercepts print statements and puts the data in m_sOutput
		static void SqPrint( HSQUIRRELVM v, const SQChar* s, ... )
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

		/// Intercepts errors
		static SQInteger SqErrorHandler( HSQUIRRELVM v )
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

		/// Handles a script compiler error
		static void SqCompilerErrorHandler( HSQUIRRELVM v, const SQChar *sErr, const SQChar *sSource, SQInteger line, SQInteger column )
		{
			throw SScriptErrorInfo( sErr, sSource, line, column );
		}

		/// Auxiliary error handler
		static SQInteger SqAuxErrorHandler( HSQUIRRELVM v )
		{
			if( 0 >= sq_gettop( v ) )
				return 0;

			std::tstring sErr;
			const SQChar *pErr = 0;
			if( SQ_SUCCEEDED( sq_getstring( v, -1, &pErr ) ) )
				sErr = pErr;
			else
				sErr = oexT( "Unknown Error" );

			// Get stack trace
			SQStackInfos si;
			if ( SQ_SUCCEEDED( sq_stackinfos( v, 1, &si ) ) )
			{   std::tstring sMsg;
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

	public:

		/// Default constructor
		CSqEngine() :
		    m_vm( SquirrelVM::StdLib_All ),
			m_script( m_vm.GetVMHandle() )
		{
			m_bLoaded = oex::oexFALSE;
		}

		/// Destructor
		virtual ~CSqEngine() { Destroy(); }

		/// Destroys the virtual machine and prepares the class for re-use
		void Destroy()
		{
			Execute( SQEXE_FN_END );

			m_script.Reset();

			m_vm.Shutdown();

			m_sErr = oexT( "" );
			m_sOutput = oexT( "" );
			m_sRoot = oexT( "" );

			m_bLoaded = oex::oexFALSE;
		}

		/// Initializes the virtual machine
		/**
			\param [in] dwMainThreadId  -   The id of the parent thread

			You don't need to call this directly, it is called by Load().
		*/
		oex::oexBOOL Init()
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

		/// Loads a script and prepares it for execution
		/**
			\param [in] pScript     -   The script or a file name.
			\param [in] bFile       -   If non-zero, pScript contains a file name.

		*/
		BOOL Load( oex::oexCSTR pScript, oex::oexBOOL bFile, oex::oexBOOL bRelative = FALSE, oex::oexBOOL bStart = TRUE )
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
					Execute( SQEXE_FN_INIT );

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

		/// Runs the script an executes the initialization function
		/**
			You can just set the bStart flag in Load() to TRUE to avoid
			calling this function if you have no special registrations.
		*/
		BOOL Start()
		{
			if ( !IsScript() )
				return oex::oexFALSE;

			_oexTRY
			{
				// Initialize the script
				m_vm.RunScript( m_script );

				// Execute init function
				Execute( SQEXE_FN_INIT );

			} // end try
			_oexCATCH( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			_oexCATCH( SquirrelError &e )
			{	m_sErr = e.desc; return oex::oexFALSE; }

			return oex::oexTRUE;
		}

		/// Destroys the virtual machine and prepares the class for re-use
		oex::oexBOOL Idle()
		{
			return Execute( SQEXE_FN_IDLE );
		}


	    /// Executes the specified buffer
		oex::oexBOOL Run( oex::oexCSTR pScript )
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

		oex::oexINT LogError( oex::oexINT x_nReturn, SScriptErrorInfo &x_e )
		{	oex::CStr sErr = oex::CStr().Fmt( oexT( "%s(%lu)\r\n   %s" ), x_e.sSource.c_str(), x_e.uLine, x_e.sDesc.c_str() );
			oexERROR( 0, sErr );
			m_sErr = sErr.Ptr();
			return x_nReturn;
		}

	public:

		/// Binds variables to the root table
		template< typename T >
			void BindRootVariable( T *pVar, oex::oexCSTR pName, oex::oexCSTR pImport = NULL, SqPlus::VarAccessType access = SqPlus::VAR_ACCESS_READ_WRITE )
			{   if ( pImport && *pImport ) import( pImport );
				SquirrelObject root = m_vm.GetRootTable();
				SqPlus::BindVariable( m_vm, root, pVar, pName, access );
			}

		/// Binds functions to the root table
		template< typename T >
			void BindRootFunction( T pFunction, oex::oexCSTR pName )
			{   SqPlus::RegisterGlobal( m_vm, pFunction, pName );
			}

		/// Binds member functions to the root table
		template< typename T_CLASS, typename T_FUNCTION >
			void BindRootMemberFunction( T_CLASS pClass, T_FUNCTION pFunction, oex::oexCSTR pName )
			{   SqPlus::RegisterGlobal( m_vm, pClass, pFunction, pName );
			}

	public:

		/// Executes a specific function within the script
		/**
			\param [in] pFunction   -   Name of the function to execute
		*/
		oex::oexBOOL Execute( oex::oexCSTR x_pFunction )
		{
			if ( !IsScript() || !x_pFunction  )
				return oex::oexFALSE;

			_oexTRY
			{
				SqPlus::SquirrelFunction< void > f( m_vm, m_vm.GetRootTable(), x_pFunction );

				if ( f.func.IsNull() )
				{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
					return oex::oexFALSE;
				} // end if

				// Call the function
				f();

			} // end try

			_oexCATCH( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			_oexCATCH( SquirrelError &e )
			{	m_sErr = e.desc; return oex::oexFALSE; }

			return oex::oexTRUE;
		}

	template< typename T_P1 >
		oex::oexBOOL Execute( oex::oexCSTR x_pFunction, T_P1 p1 )
		{
			if ( !IsScript() || !x_pFunction  )
				return oex::oexFALSE;

			_oexTRY
			{
				SqPlus::SquirrelFunction< void > f( m_vm, m_vm.GetRootTable(), x_pFunction );

				if ( f.func.IsNull() )
				{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
					return oex::oexFALSE;
				} // end if

				// Call the function
				f( p1 );

			} // end try

			_oexCATCH( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			_oexCATCH( SquirrelError &e )
			{	m_sErr = e.desc; return oex::oexFALSE; }

			return oex::oexTRUE;
		}

	template< typename T_P1, typename T_P2 >
		oex::oexBOOL Execute( oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2 )
		{
			if ( !IsScript() || !x_pFunction  )
				return oex::oexFALSE;

			_oexTRY
			{
				SqPlus::SquirrelFunction< void > f( m_vm, m_vm.GetRootTable(), x_pFunction );

				if ( f.func.IsNull() )
				{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
					return oex::oexFALSE;
				} // end if

				// Call the function
				f( p1, p2 );

			} // end try

			_oexCATCH( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			_oexCATCH( SquirrelError &e )
			{	m_sErr = e.desc; return oex::oexFALSE; }

			return oex::oexTRUE;
		}

	template< typename T_P1, typename T_P2, typename T_P3 >
		oex::oexBOOL Execute( oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2, T_P3 p3 )
		{
			if ( !IsScript() || !x_pFunction  )
				return oex::oexFALSE;

			_oexTRY
			{
				SqPlus::SquirrelFunction< void > f( m_vm, m_vm.GetRootTable(), x_pFunction );

				if ( f.func.IsNull() )
				{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
					return oex::oexFALSE;
				} // end if

				// Call the function
				f( p1, p2, p3 );

			} // end try

			_oexCATCH( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			_oexCATCH( SquirrelError &e )
			{	m_sErr = e.desc; return oex::oexFALSE; }

			return oex::oexTRUE;
		}

	template< typename T_RET >
		oex::oexBOOL ExecuteRet( oex::oexCSTR x_pFunction, T_RET *x_pRet )
        {
            if ( !IsScript() )
                return FALSE;

            if ( !x_pFunction )
                return FALSE;;

            _oexTRY
            {
                SqPlus::SquirrelFunction< T_RET > f( m_vm, m_vm.GetRootTable(), x_pFunction );

                if ( f.func.IsNull() )
                {   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
					return oex::oexFALSE;
                } // end if

                if ( x_pRet )
                    *x_pRet = f();
                else
                    f();

            } // end try

			_oexCATCH( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			_oexCATCH( SquirrelError &e )
			{	m_sErr = e.desc; return oex::oexFALSE; }

			return oex::oexTRUE;
        }

	public:

		/// Sets pointer to the module manager
		void SetModuleManager( CModuleManager *x_pMm )
		{   m_pModuleManager = x_pMm; }

		/// Registers library classes
		virtual int OnImport( const std::tstring &sClass )
		{
			return -1;
		}

		/// Loads the specified module
		virtual int OnLoadModule( const std::tstring &sModule, const std::tstring &sPath )
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

		/// Sets the pointer to the thread message queue
		void SetMessageQueue( CSqMsgQueue *pMq )
		{   m_pMsgQueue = pMq; }

		/// Return message queue pointer
		virtual CSqMsgQueue* OnGetQueue()
		{	return m_pMsgQueue; }

		/// Spawns a child script process
		virtual SquirrelObject OnSpawn( const std::tstring &sName, const std::tstring &sScript, int bFile )
		{
			if ( m_pMsgQueue )
			{
				CSqMsgQueue::t_Params params;
				params[ oexT( "name" ) ] = sName;
				params[ oexT( "script" ) ] = sScript;
				params[ oexT( "file" ) ] = bFile ? oexT( "1" ) : oexT( "0" );

				m_pMsgQueue->Msg( oexT( "spawn" ), &params );

			} // end if

			return SquirrelObject( m_vm.GetVMHandle() );
		}

		/// Routes a message to the proper script
		SquirrelObject RouteMsg( const std::tstring &sMsg, CSqMsgQueue::t_Params &rParams )
		{	if ( m_pMsgQueue )
				m_pMsgQueue->Msg( oexT( "msg" ), &rParams );
			return SquirrelObject( m_vm.GetVMHandle() );
		}

		/// Call to send a message to a child script
		virtual SquirrelObject OnExecute( const std::tstring &sName, const std::tstring &sFunction )
		{	CSqMsgQueue::t_Params params;
			params[ oexT( "name" ) ] = sName;
			params[ oexT( "execute" ) ] = sFunction;
			return RouteMsg( oexT( "msg" ), params );
		}

		virtual SquirrelObject OnExecute1( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1 )
		{	CSqMsgQueue::t_Params params;
			params[ oexT( "name" ) ] = sName;
			params[ oexT( "execute1" ) ] = sFunction;
			params[ oexT( "p1" ) ] = sP1;
			return RouteMsg( oexT( "msg" ), params );
		}

		virtual SquirrelObject OnExecute2( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1, const std::tstring &sP2 )
		{	CSqMsgQueue::t_Params params;
			params[ oexT( "name" ) ] = sName;
			params[ oexT( "execute2" ) ] = sFunction;
			params[ oexT( "p1" ) ] = sP1;
			params[ oexT( "p2" ) ] = sP2;
			return RouteMsg( oexT( "msg" ), params );
		}

		virtual SquirrelObject OnExecute3( const std::tstring &sName, const std::tstring &sFunction, const std::tstring &sP1, const std::tstring &sP2, const std::tstring &sP3 )
		{	CSqMsgQueue::t_Params params;
			params[ oexT( "name" ) ] = sName;
			params[ oexT( "execute3" ) ] = sFunction;
			params[ oexT( "p1" ) ] = sP1;
			params[ oexT( "p2" ) ] = sP2;
			params[ oexT( "p3" ) ] = sP3;
			return RouteMsg( oexT( "msg" ), params );
		}


	private:

		/// Non-zero if a script is loaded
		oex::oexBOOL			m_bLoaded;

		/// Squirrel virtual machine
		SquirrelVM              m_vm;

	    /// Pointer to module manager object
		CModuleManager          *m_pModuleManager;

		/// Squirrel script
		SquirrelObject          m_script;

		/// Last squirrel error
		std::tstring            m_sErr;

		/// Script output
		std::tstring            m_sOutput;

		/// Root path
		std::tstring            m_sRoot;

		/// Thread message queue pointer
		CSqMsgQueue             *m_pMsgQueue;
	};

}; // end namespace
