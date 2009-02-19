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
	int alert( const stdString &sMsg );

	int import( const stdString &sClass );

	int load_module( const stdString &sModule, const stdString &sPath );

	void quit( int nExitCode );

	stdString path( const stdString &sPath );

	SquirrelObject spawn( const stdString &sName, const stdString &sScript, int bFile );

	SquirrelObject execute( const stdString &sName, const stdString &sFunction );

	SquirrelObject execute1( const stdString &sName, const stdString &sFunction, const stdString &sP1 );

	SquirrelObject execute2( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2 );

	SquirrelObject execute3( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3 );

	void sleep( int nMsTime );

	// _self.queue()
	CSqMsgQueue* queue();

protected:

	virtual int OnImport( const stdString &sClass );

	virtual int OnLoadModule( const stdString &sModule, const stdString &sPath );

	virtual stdString OnPath( stdString sPath );

	virtual SquirrelObject OnSpawn( const stdString &sName, const stdString &sScript, int bFile );

	virtual SquirrelObject OnExecute( const stdString &sName, const stdString &sFunction );
	virtual SquirrelObject OnExecute1( const stdString &sName, const stdString &sFunction, const stdString &sP1 );
	virtual SquirrelObject OnExecute2( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2 );
	virtual SquirrelObject OnExecute3( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3 );

	virtual CSqMsgQueue* OnGetQueue();

protected:

	/// Non-zero if loaded from file
	oex::oexBOOL        m_bFile;

	/// Either a file name or actual script
	stdString        m_sScript;

};

// Declare class functions
SQBIND_DECLARE_INSTANCE( CSqEngineExport );

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
		SScriptErrorInfo( stdString x_sDesc, stdString x_sSource, oex::oexUINT x_uLine, oex::oexUINT x_uCol )
		{   sDesc = x_sDesc; sSource = x_sSource; uLine = x_uLine; uCol = x_uCol; }

		/// Description
		stdString    sDesc;

		/// Source file
		stdString    sSource;

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
	BOOL IsScript();

	/// Returns the last error
	stdString& GetLastError();

	/// Returns the script output
	stdString& GetOutput();

	/// Returns a reference the to the virtual machine
	SquirrelVM& GetVM();

	/// Returns a reference to the virtual machine
	operator SquirrelVM&();

	/// Returns a referenced to the compiled script object
	SquirrelObject& GetScriptObj();

	/// Returns the scripts root table
	const SquirrelObject& GetRootTable();

public:

	/// Intercepts print statements and puts the data in m_sOutput
	static void SqPrint( HSQUIRRELVM v, const SQChar* s, ... );

	/// Intercepts errors
	static SQInteger SqErrorHandler( HSQUIRRELVM v );

	/// Handles a script compiler error
	static void SqCompilerErrorHandler( HSQUIRRELVM v, const SQChar *sErr, const SQChar *sSource, SQInteger line, SQInteger column );

	/// Auxiliary error handler
	static SQInteger SqAuxErrorHandler( HSQUIRRELVM v );

public:

	/// Default constructor
	CSqEngine();

	/// Destructor
	virtual ~CSqEngine();

	/// Destroys the virtual machine and prepares the class for re-use
	void Destroy();

	/// Initializes the virtual machine
	/**
		\param [in] dwMainThreadId  -   The id of the parent thread

		You don't need to call this directly, it is called by Load().
	*/
	oex::oexBOOL Init();

	/// Loads a script and prepares it for execution
	/**
		\param [in] pScript     -   The script or a file name.
		\param [in] bFile       -   If non-zero, pScript contains a file name.

	*/
	BOOL Load( oex::oexCSTR pScript, oex::oexBOOL bFile, oex::oexBOOL bRelative = FALSE, oex::oexBOOL bStart = TRUE );

	/// Runs the script an executes the initialization function
	/**
		You can just set the bStart flag in Load() to TRUE to avoid
		calling this function if you have no special registrations.
	*/
	BOOL Start();

	/// Destroys the virtual machine and prepares the class for re-use
	oex::oexBOOL Idle();


	/// Executes the specified buffer
	oex::oexBOOL Run( oex::oexCSTR pScript );

	oex::oexINT LogError( oex::oexINT x_nReturn, SScriptErrorInfo &x_e );

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
	oex::oexBOOL Execute( stdString *pRet, oex::oexCSTR x_pFunction );

template< typename T_P1 >
	oex::oexBOOL Execute( stdString *pRet, oex::oexCSTR x_pFunction, T_P1 p1 )
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
			{	SquirrelObject o( m_vm.GetVMHandle() );
				o = f( p1 );
				const char *p = o.ToString();
				if ( oexCHECK_PTR( p ) )
					*pRet = p;
			} // end if
			else
				f( p1 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	m_sErr = e.desc; return oex::oexFALSE; }

		return oex::oexTRUE;
	}


template< typename T_P1, typename T_P2 >
	oex::oexBOOL Execute( stdString *pRet, oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2 )
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
				*pRet = f( p1, p2 ).ToString();
			else
				f( p1, p2 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	m_sErr = e.desc; return oex::oexFALSE; }

		return oex::oexTRUE;
	}

template< typename T_P1, typename T_P2, typename T_P3 >
	oex::oexBOOL Execute( stdString *pRet, oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2, T_P3 p3 )
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
				*pRet = f( p1, p2, p3 ).ToString();
			else
				f( p1, p2, p3 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	m_sErr = e.desc; return oex::oexFALSE; }

		return oex::oexTRUE;
	}

	/// Executes a specific function within the script
	/**
		\param [in] pFunction   -   Name of the function to execute
	*/
template< typename T_RET >
	oex::oexBOOL Execute( T_RET *pRet, oex::oexCSTR x_pFunction )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< T_RET > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = f();
			else
				f();

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	m_sErr = e.desc; return oex::oexFALSE; }

		return oex::oexTRUE;
	}

template< typename T_RET >
	oex::oexBOOL Execute( T_RET &rRet, oex::oexCSTR x_pFunction )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< T_RET& > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( &rRet ) )
				rRet = f();
			else
				f();

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	m_sErr = e.desc; return oex::oexFALSE; }

		return oex::oexTRUE;
	}

template< typename T_RET, typename T_P1 >
	oex::oexBOOL Execute( T_RET *pRet, oex::oexCSTR x_pFunction, T_P1 p1 )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< T_RET > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = f( p1 );
			else
				f( p1 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	m_sErr = e.desc; return oex::oexFALSE; }

		return oex::oexTRUE;
	}

template< typename T_RET, typename T_P1, typename T_P2 >
	oex::oexBOOL Execute( T_RET *pRet, oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2 )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< T_RET > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = f( p1, p2 );
			else
				f( p1, p2 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	m_sErr = e.desc; return oex::oexFALSE; }

		return oex::oexTRUE;
	}

template< typename T_RET, typename T_P1, typename T_P2, typename T_P3 >
	oex::oexBOOL Execute( T_RET *pRet, oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2, T_P3 p3 )
	{
		if ( !IsScript() || !x_pFunction  )
			return oex::oexFALSE;

		_oexTRY
		{
			SqPlus::SquirrelFunction< T_RET > f( m_vm, m_vm.GetRootTable(), x_pFunction );

			if ( f.func.IsNull() )
			{   m_sErr = oexT( "Function not found : " ); m_sErr += x_pFunction;
				return oex::oexFALSE;
			} // end if

			// Call the function
			if ( oexCHECK_PTR( pRet ) )
				*pRet = f( p1, p2, p3 );
			else
				f( p1, p2, p3 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	m_sErr = e.desc; return oex::oexFALSE; }

		return oex::oexTRUE;
	}

public:

	/// Sets pointer to the module manager
	void SetModuleManager( CModuleManager *x_pMm );

	/// Registers library classes
	virtual int OnImport( const stdString &sClass );

	/// Loads the specified module
	virtual int OnLoadModule( const stdString &sModule, const stdString &sPath );

	/// Sets the pointer to the thread message queue
	void SetMessageQueue( CSqMsgQueue *pMq );

	/// Return message queue pointer
	virtual CSqMsgQueue* OnGetQueue();

	/// Spawns a child script process
	virtual SquirrelObject OnSpawn( const stdString &sName, const stdString &sScript, int bFile );

	/// Routes a message to the proper script
	SquirrelObject RouteMsg( const stdString &sMsg, CSqMap &rParams );

	/// Call to send a message to a child script
	virtual SquirrelObject OnExecute( const stdString &sName, const stdString &sFunction );
	virtual SquirrelObject OnExecute1( const stdString &sName, const stdString &sFunction, const stdString &sP1 );
	virtual SquirrelObject OnExecute2( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2 );
	virtual SquirrelObject OnExecute3( const stdString &sName, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3 );


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
	stdString            m_sErr;

	/// Script output
	stdString            m_sOutput;

	/// Root path
	stdString            m_sRoot;

	/// Thread message queue pointer
	CSqMsgQueue             *m_pMsgQueue;
};

