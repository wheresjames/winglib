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

class CSqEngine;

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

	SQBIND_CLASS_CTOR_BEGIN( CSqEngineExport )
	SQBIND_CLASS_CTOR_END( CSqEngineExport )

	/// Returns the buffer for a binary share
	CSqBinary get_binshare( const stdString &sName );

	/// Returns the buffer for a binary share
	int set_binshare( const stdString &sName, CSqBinary *pBin );

	/// Returns the buffer for a binary share
	int is_binshare( const stdString &sName );

	/// Runs garbage collection on binary shares
	int cleanup_binshare();

	/// Sets the specified property
	void set( const stdString &sPath, const stdString &sKey, const stdString &sVal );

	/// Gets the specified property
	stdString get( const stdString &sPath, const stdString &sKey );

	/// Returns non-zero if the specified value exists
	int isset( const stdString &sPath, const stdString &sKey );

	/// Returns a copy of the entire property bag
	CSqMulti pb( const stdString &sPath );

	/// Requests the appliation exit
	void exit( int nExitCode );

	/// Immediatly stops the application
	void terminate( int nExitCode );

	/// Displays a message box containing string
	int alert( const stdString &sMsg );

	/// Sends data to STDOUT
	int print( const stdString &sMsg );

	/// Sends data to STDOUT
	//  Same as print, but appends new line
	int echo( const stdString &sMsg );

	/// Flushes STDOUT
	int flush();

	/// Imports the specified class
	int import( const stdString &sClass );

	/// Includes the specified script
	int include( const stdString &sScript );

	/// Loads the specified module
	int load_module( const stdString &sModule, const stdString &sPath );

	/// Sends the kills message to the specified thread
	int kill( const stdString &sPath );

	/// Sends the kills message to the specified thread and doesn't return until it exits
	int kill_wait( const stdString &sPath );

	/// Returns boot time in seconds
	float clock();

	/// Returns boot count
	unsigned int ticks();

	/// Returns the script name
	stdString get_name();

	/// Get child scripts
	stdString get_children( const stdString &sPath );

	/// Decorates the file with appropriate compile time options
	stdString decorate( const stdString &sPath, int bExe, int bLib );

	/// Returns the file path to the current script
	stdString path( const stdString &sPath );

	/// Returns the file path to the current executable
	stdString root( const stdString &sPath );

	/// Returns the file path of the current executable
	stdString module_path();

	/// Returns the file name of the current executable
	stdString module_name();

	/// Returns the md5 for the specified string
	stdString md5( const stdString &sStr );

	/// Base64 encodes a string
	stdString base64_encode( const stdString &sStr );

	/// Base64 decodes a string
	stdString base64_decode( const stdString &sStr );

	/// Base16 encodes a string
	stdString base16_encode( const stdString &sStr );

	/// Base16 decodes a string
	stdString base16_decode( const stdString &sStr );

	/// Returns the file path to the current script
	stdString build_path( const stdString &sS1,  const stdString &sS2 );

	/// Logs an error
	void error( int e, const stdString &sStr );

	/// Logs a warning
	void warning( int e, const stdString &sStr );

	/// Logs a message
	void notice( int e, const stdString &sStr );

	/// Trims the specified characters from the beginning of the string
	stdString ltrim( const stdString &sS, const stdString &sChs );

	/// Trims the specified characters from the end of the string
	stdString rtrim( const stdString &sS, const stdString &sChs );

	/// Trims the specified characters from the beginning and end of the string
	stdString trim( const stdString &sS, const stdString &sChs );

	/// Trims the white space from the beginning and end of the string
	stdString trimws( const stdString &sS );

	/// Finds sub string sSub in sS, returns offset or less than zero if not found
	int find( const stdString &sS, const stdString &sSub );

	/// Replaces the occurences of the specified string
	stdString replace( const stdString &sS, const stdString &sFind, const stdString &sReplace );

	/// Drop specified characters from a string or all but
	stdString drop( const stdString &sS, const stdString &sDrop, int bInclusive );

	/// Drop the specified range of characters from a string or all but
	stdString drop_range( const stdString &sS, const stdString &sBegin, const stdString &sEnd, int bInclusive );

	/// URL encodes the string
	stdString urlencode( const stdString &sS );

	/// URL decodes the string
	stdString urldecode( const stdString &sS );

	/// HTML encodes the string
	stdString htmlencode( const stdString &sS );

	/// HTML decodes the string
	stdString htmldecode( const stdString &sS );

	/// Compresses string using zlib/deflate
	stdString compress( const stdString &sS );

	/// Uncompresses string using zlib/inflate
	stdString uncompress( const stdString &sS );

	/// Returns a unique md5 hash
	stdString unique();

	/// Returns local time stamp
	int local_time();

	/// Returns gmt time stamp
	int gmt_time();

	/// Returns a formated time string
	stdString local_timestr( const stdString &fmt );

	/// Returns a formated time string
	stdString gmt_timestr( const stdString &fmt );

	/// Creates a thread
	int spawn( int nRet, const stdString &sPath, const stdString &sName, const stdString &sScript, int bFile );

	/// Executes the specified script
	int run( const stdString &sPath, const stdString &sScript );

	/// Checks for path/threads existence
	int is_path( const stdString &sPath );

	/// Executes shell command
	int shell( const stdString &sFile, const stdString &sParams, const stdString &sDirectory );

	/// Returns the current cpu load
	double get_cpu_load();

	/// Returns a linked in resource
	stdString get_resource( const stdString &sRes, int bFileOverrideOk );

	/// Execute functions
	stdString execute( int nRet, const stdString &sPath, const stdString &sFunction );
	stdString execute1( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1 );
	stdString execute2( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1, const stdString &sP2 );
	stdString execute3( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3 );
	stdString execute4( int nRet, const stdString &sPath, const stdString &sFunction, const stdString &sP1, const stdString &sP2, const stdString &sP3, const stdString &sP4 );

	void sleep( int nMsTime );

	// _self.queue()
	CSqMsgQueue* queue();

	virtual SquirrelVM* GetVmPtr();

	virtual CSqEngine* GetEnginePtr();

	int tolong( const stdString &s )
	{	return oexStrToLong( s.c_str() ); }

	int toulong( const stdString &s )
	{	return oexStrToULong( s.c_str() ); }

	float tofloat( const stdString &s )
	{	return oexStrToFloat( s.c_str() ); }

	double todouble( const stdString &s )
	{	return oexStrToDouble( s.c_str() ); }

	stdString& GetRoot() { return m_sRoot; }

protected:

	virtual int OnImport( const stdString &sClass );

	virtual int OnInclude( const stdString &sScript );

	virtual int OnLoadModule( const stdString &sModule, const stdString &sPath );

	virtual stdString OnPath( stdString sPath );

	virtual CSqMsgQueue* OnGetQueue();

protected:

	/// Non-zero if loaded from file
	oex::oexBOOL        m_bFile;

	/// Either a file name or actual script
	stdString        	m_sScript;

	/// Root path
	stdString           m_sRoot;

	/// Name of the script
	stdString					m_sScriptName;

};

// Declare class functions
}; SQBIND_DECLARE_INSTANCE( sqbind::CSqEngineExport, CSqEngineExport ); namespace sqbind {

#define LogErrorM( r, e )	LogError( r, e, oexTEXT( __FILE__ ), __LINE__ )

/// Squirrel engine
class CSqEngine : public CSqEngineExport
{
public:

	#define     SQEXE_FN_INIT       oexT( "_init" )
	#define     SQEXE_FN_IDLE       oexT( "_idle" )
	#define     SQEXE_FN_EXIT       oexT( "_exit" )
	#define     SQEXE_SELF          oexT( "_self" )

public:

	/// Contains information about a script error
	struct SScriptErrorInfo
	{
		/// Constructor
		SScriptErrorInfo( stdString x_sDesc, stdString x_sSource, oex::oexUINT x_uLine, oex::oexUINT x_uCol )
		{   sDesc = x_sDesc; sSource = x_sSource; uLine = x_uLine; uCol = x_uCol; }

		/// Description
		stdString    	sDesc;

		/// Source file
		stdString    	sSource;

		/// Line number
		oex::oexUINT    uLine;

		/// Column number
		oex::oexUINT    uCol;
	};

	// Callback function for custom script source
	typedef int (*t_IncludeScript)( const stdString &sScript, stdString &sData, stdString &sName );

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

	/// Calls script exit function if it hasn't already been called
	/// This only works once
	void Exit();

	/// Initializes the virtual machine
	/**
		\param [in] dwMainThreadId  -   The id of the parent thread

		You don't need to call this directly, it is called by Load().
	*/
	oex::oexBOOL Init();

	/// Sets the name of the script used for debugging
	/**
		\param [in] pScript     -   The script or a file name.

	*/
	void SetScriptName( oex::oexCSTR pScriptName );

	/// Loads a script and prepares it for execution
	/**
		\param [in] pScript     -   The script or a file name.
		\param [in] bFile       -   If non-zero, pScript contains a file name.

	*/
	oex::oexBOOL Load( oex::oexCSTR pScript, oex::oexBOOL bFile, oex::oexBOOL bRelative = FALSE, oex::oexBOOL bStart = TRUE );

	/// Runs the script an executes the initialization function
	/**
		You can just set the bStart flag in Load() to TRUE to avoid
		calling this function if you have no special registrations.
	*/
	oex::oexBOOL Start();

	/// Destroys the virtual machine and prepares the class for re-use
	oex::oexBOOL Idle();


	/// Executes the specified buffer
	oex::oexBOOL Run( oex::oexCSTR pScript );

	oex::oexINT LogError( oex::oexINT x_nReturn, SScriptErrorInfo &x_e );

	oex::oexINT LogError( oex::oexINT x_nReturn, oex::oexCSTR x_pErr, oex::oexCSTR x_pFile, oex::oexUINT x_uLine );

public:

	/// Binds variables to the root table
	template< typename T >
		void BindRootVariable( T *pVar, oex::oexCSTR pName, SqPlus::VarAccessType access = SqPlus::VAR_ACCESS_READ_WRITE )
		{   SquirrelObject root = m_vm.GetRootTable();
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
	oex::oexBOOL Execute( stdString *pRet, oex::oexCSTR x_pFunction )
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
			{
				SquirrelObject o( m_vm.GetVMHandle() );
				o = f();

				oex::oexLONG l = o.Len();
				oex::oexCSTR p = o.ToString();
				if ( oexCHECK_PTR( p ) && l )
					pRet->assign( p, l );

			} // end if
			else
				f();

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, e.desc ); }

		return oex::oexTRUE;
	}

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
			{
				SquirrelObject o( m_vm.GetVMHandle() );
				o = f( p1 );

				oex::oexLONG l = o.Len();
				oex::oexCSTR p = o.ToString();
				if ( oexCHECK_PTR( p ) && l )
					pRet->assign( p, l );

			} // end if
			else
				f( p1 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, e.desc ); }

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
			{
				SquirrelObject o( m_vm.GetVMHandle() );
				o = f( p1, p2 );

				oex::oexLONG l = o.Len();
				oex::oexCSTR p = o.ToString();
				if ( oexCHECK_PTR( p ) && l )
					pRet->assign( p, l );

			} // end if
			else
				f( p1, p2 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, oexMks( x_pFunction, oexT( " : " ), e.desc ).Ptr() ); }

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
			{
				SquirrelObject o( m_vm.GetVMHandle() );
				o = f( p1, p2, p3 );

				oex::oexLONG l = o.Len();
				oex::oexCSTR p = o.ToString();
				if ( oexCHECK_PTR( p ) && l )
					pRet->assign( p, l );

			} // end if
			else
				f( p1, p2, p3 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, e.desc ); }

		return oex::oexTRUE;
	}

template< typename T_P1, typename T_P2, typename T_P3, typename T_P4 >
	oex::oexBOOL Execute( stdString *pRet, oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4 )
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
			{
				SquirrelObject o( m_vm.GetVMHandle() );
				o = f( p1, p2, p3, p4 );

				oex::oexLONG l = o.Len();
				oex::oexCSTR p = o.ToString();
				if ( oexCHECK_PTR( p ) && l )
					pRet->assign( p, l );

			} // end if
			else
				f( p1, p2, p3, p4 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, e.desc ); }

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
		{
			// +++ Startup code needs to check for function existance first
			/*return LogErrorM( oex::oexFALSE, e.desc );*/

			return oex::oexFALSE;
		}

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
		{	return LogErrorM( oex::oexFALSE, e.desc ); }

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
		{	return LogErrorM( oex::oexFALSE, e.desc ); }

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
		{	return LogErrorM( oex::oexFALSE, e.desc ); }

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
		{	return LogErrorM( oex::oexFALSE, e.desc ); }

		return oex::oexTRUE;
	}

template< typename T_RET, typename T_P1, typename T_P2, typename T_P3, typename T_P4 >
	oex::oexBOOL Execute( T_RET *pRet, oex::oexCSTR x_pFunction, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4 )
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
				*pRet = f( p1, p2, p3, p4 );
			else
				f( p1, p2, p3, p4 );

		} // end try

		_oexCATCH( SScriptErrorInfo &e )
		{	return LogError( oex::oexFALSE, e ); }
		_oexCATCH( SquirrelError &e )
		{	return LogErrorM( oex::oexFALSE, e.desc ); }

		return oex::oexTRUE;
	}

    template< typename T_P1 >
        oex::oexBOOL Execute( SquirrelObject &soFunction, const T_P1 &p1 )
        {
            if ( !IsScript() || oexNULL == &soFunction )
                return oex::oexFALSE;

            try
            {
                SqPlus::SquirrelFunction< void > f( m_vm, m_vm.GetRootTable(), soFunction );

                if ( f.func.IsNull() )
                {   m_sErr = "Invalid function object";
                    return oex::oexFALSE;
                } // end if

                f( p1 );

            } // end try

			_oexCATCH( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			_oexCATCH( SquirrelError &e )
			{	return LogErrorM( oex::oexFALSE, e.desc ); }
			return oex::oexTRUE;
		}


    template< typename T_P1, typename T_P2 >
        oex::oexBOOL Execute( SquirrelObject &soFunction, const T_P1 &p1, const T_P2 &p2 )
        {
            if ( !IsScript() || oexNULL == &soFunction )
                return oex::oexFALSE;

            try
            {
                SqPlus::SquirrelFunction< void > f( m_vm, m_vm.GetRootTable(), soFunction );

                if ( f.func.IsNull() )
                {   m_sErr = "Invalid function object";
                    return oex::oexFALSE;
                } // end if

                f( p1, p2 );

            } // end try

			_oexCATCH( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			_oexCATCH( SquirrelError &e )
			{	return LogErrorM( oex::oexFALSE, e.desc ); }
			return oex::oexTRUE;
		}

    template< typename T_P1, typename T_P2, typename T_P3 >
        oex::oexBOOL Execute( SquirrelObject &soFunction, const T_P1 &p1, const T_P2 &p2, const T_P3 &p3 )
        {
            if ( !IsScript() || oexNULL == &soFunction )
                return oex::oexFALSE;

            try
            {
                SqPlus::SquirrelFunction< void > f( m_vm, m_vm.GetRootTable(), soFunction );

                if ( f.func.IsNull() )
                {   m_sErr = oexT( "Invalid function object" );
                    return oex::oexFALSE;
                } // end if

                f( p1, p2, p3 );

            } // end try

			_oexCATCH( SScriptErrorInfo &e )
			{	return LogError( oex::oexFALSE, e ); }
			_oexCATCH( SquirrelError &e )
			{	return LogErrorM( oex::oexFALSE, e.desc ); }
			return oex::oexTRUE;
		}

public:

	/// Sets pointer to the module manager
	void SetModuleManager( CModuleManager *x_pMm );

	/// Registers library classes
	virtual int OnImport( const stdString &sClass );

	/// Includes the specified script
	virtual int OnInclude( const stdString &sScript );

	/// Loads the specified module
	virtual int OnLoadModule( const stdString &sModule, const stdString &sPath );

	/// Sets the pointer to the thread message queue
	void SetMessageQueue( CSqMsgQueue *pMq );

	/// Return message queue pointer
	virtual CSqMsgQueue* OnGetQueue();

	/// Return vm pointer
	virtual SquirrelVM* GetVmPtr();

	/// Returns the engine pointer
	virtual CSqEngine* GetEnginePtr() { return this; }

	/// Call to set a function that is called when thread starts
	/// to export custom symbols
	void SetExportFunction( PFN_SQBIND_Export_Symbols fn, sqbind::SSqAllocator *pa );

	/// Returns pointer to symbol export function
	PFN_SQBIND_Export_Symbols GetExportFunction() { return m_fExportSymbols; }

	/// Returns pointer to memory allocator
	sqbind::SSqAllocator* GetAllocator() { return m_pSqAllocator; }

	/// Custom include function
	void SetIncludeScriptFunction( t_IncludeScript f )
	{	m_fIncludeScript = f; }

	/// Returns custom include function
	t_IncludeScript GetIncludeScriptFunction()
	{	return m_fIncludeScript; }

private:

	/// Non-zero if a script is loaded
	oex::oexBOOL				m_bLoaded;

	/// Squirrel virtual machine
	SquirrelVM              	m_vm;

	/// Pointer to module manager object
	CModuleManager          	*m_pModuleManager;

	/// Squirrel script
	SquirrelObject          	m_script;

	/// Last squirrel error
	stdString            		m_sErr;

	/// Script output
	stdString            		m_sOutput;

	/// Thread message queue pointer
	CSqMsgQueue             	*m_pMsgQueue;

	/// Function that is called by thread to export symbols
	PFN_SQBIND_Export_Symbols	m_fExportSymbols;

	/// Custom memory allocator
	sqbind::SSqAllocator		*m_pSqAllocator;

	/// Custom script include function
	t_IncludeScript				m_fIncludeScript;

	/// Non-zero if exit function needs calling
	BOOL						m_bCallExit;

};



