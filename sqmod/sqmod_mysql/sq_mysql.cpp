// sq_mysql.cpp

#include "stdafx.h"

CSqMysql::CSqMysql()
{_STT();
	m_bOpen = 0;
	m_nPort = 3306;
	oexZero( m_db );
	m_res = 0;
	m_nLastError = 0;
}

CSqMysql::~CSqMysql()
{_STT();
	Destroy();
}

void CSqMysql::Destroy()
{_STT();
	// Free any left over results
	FreeResult();

	try
	{
		// Close database if it's open
		if ( m_bOpen )
			mysql_close( &m_db );

	} // end try
	catch( ... ) { oexERROR( 0, oexT( "Exception in mysql_close()" ) ); }

	oexZero( m_db );
	m_bOpen = 0;
}

void CSqMysql::FreeResult()
{_STT();
	try
	{
		// Free any result
		if ( m_res )
			mysql_free_result( m_res );

	} // end try
	catch( ... ) { oexERROR( 0, oexT( "Exception in mysql_free_result()" ) ); }

	m_res = 0;
}

int CSqMysql::LogError( oex::oexCSTR pPrefix )
{_STT();
	// Grab error info
	const char *pErr = mysql_error( &m_db );
	m_nLastError = mysql_errno( &m_db );

	// Build string
	if ( pErr && *pErr ) m_sLastError = oexMbToStr( pErr );
	else m_sLastError = oexFmt( oexT( "%d (%x)" ), m_nLastError, m_nLastError ).Ptr();

	// Log it
	oexERROR( m_nLastError, oexMks( pPrefix, m_sLastError.c_str() ) );

	return m_nLastError;
}

int CSqMysql::Connect(	const sqbind::stdString &sServer,
					    int nPort,
						const sqbind::stdString &sUser,
						const sqbind::stdString &sPass,
						const sqbind::stdString &sDatabase )
{_STT();
	// Lose old connection
	Destroy();
	
	try
	{
		// Initialize new db connection
		if ( !mysql_init( &m_db ) )
		{	LogError( oexT( "mysql_init() : " ) );
			return 0;
		} // end if

		// Guess it's open
		m_bOpen = 1;

	} // end try
	catch( ... ) { oexERROR( 0, oexT( "Exception in mysql_init()" ) ); }
	
	// Use default port?
	if ( !nPort )
		nPort = m_nPort;

	try
	{
		// Attempt to connect the database
		if ( !mysql_real_connect( &m_db, sServer.c_str(), 
								  sUser.c_str(), sPass.c_str(), 
								  sDatabase.c_str(), nPort, NULL, 0 ) )
		{	LogError( oexMks( oexT( "mysql_real_connect( " ),
							  sServer.c_str(), oexT( ", " ),
							  sDatabase.c_str(), oexT( ", " ),
							  sUser.c_str(), oexT( ", **** ) : " ) ).Ptr() );
			Destroy();
			return 0;
		} // end if

	} // end try
	catch( ... ) 
	{
		Destroy(); 
		oexERROR( 0, oexMks( oexT( "Exception in mysql_real_connect( " ),
							  sServer.c_str(), oexT( ", " ),
							  sDatabase.c_str(), oexT( ", " ),
							  sUser.c_str(), oexT( ", **** ) : " ) ) ); 
	} // end catch

	// Save connection information
	m_sServer = sServer;
	m_nPort = nPort;
	m_sDatabase = sDatabase;
	m_sUser = sUser;
	m_sPass = sPass;
	
	return 1;
}

sqbind::stdString CSqMysql::Escape( const sqbind::stdString &s )
{_STT();
	if ( !m_bOpen )
		return oexT( "" );

	oex::CStr8 in = oexStrToMb( oex::CStr( s.c_str(), s.length() ) ), out;

	oex::oexSTR8 pOut = out.OexAllocate( in.Length() * 2 + 1 );
	if ( !pOut )
		return oexT( "" );

	try
	{
		// Attempt to escape the string
		unsigned long l = mysql_real_escape_string( &m_db, pOut, in.Ptr(), in.Length() );
		if ( 0 > l )
			return oexT( "" );

		// Set new length
		out.SetLength( l );

	} // end try
	catch( ... ) { oexERROR( 0, oexMks( oexT( "Exception in mysql_real_escape_string()" ) ) ); }
	
	// Return string
	oex::CStr sRet( oexMbToStr( out ) );
	return sqbind::stdString( sRet.Ptr(), sRet.Length() );
}

sqbind::stdString CSqMysql::MakePairs( sqbind::CSqMap *mInfo )
{_STT();
	if ( !mInfo )
		return oexT( "" );

	sqbind::stdString s;
	for ( sqbind::CSqMap::t_List::iterator it = mInfo->list().begin(); it != mInfo->list().end(); it++ )
	{
		if ( s.length() )
			s += oexT( "," );

		s += "`";
		s += Escape( it->first );
		s += "`='";
		s += Escape( it->second );
		s += "'"; 

	} // end for

	return s;
}	

sqbind::stdString CSqMysql::MakeInsert( sqbind::CSqMap *mInfo )
{_STT();
	if ( !mInfo )
		return oexT( "" );

	int n = 0;
	sqbind::stdString s = oexT( "(" );
	for ( sqbind::CSqMap::t_List::iterator it = mInfo->list().begin(); it != mInfo->list().end(); it++ )
	{
		if ( n++ )
			s += oexT( "," );

		s += "`";
		s += Escape( it->first );
		s += "`";

	} // end for

	s += oexT( ") VALUES (" );

	n = 0;
	for ( sqbind::CSqMap::t_List::iterator it = mInfo->list().begin(); it != mInfo->list().end(); it++ )
	{
		if ( n++ )
			s += oexT( "," );

		s += "'";
		s += Escape( it->second );
		s += "'"; 

	} // end for

	s += ")"; 

	return s;
}	

int CSqMysql::Query( const sqbind::stdString &sQuery )
{_STT();
	// Ensure open database
	if ( !m_bOpen )
		return 0;

	// Free last result
	FreeResult();

	try
	{
		// Attempt query
		if ( mysql_query( &m_db, oexStrToMbPtr( sQuery.c_str() ) ) )
		{	LogError( oexMks( oexT( "mysql_query( " ), sQuery.c_str(), oexT( " ) : " ) ).Ptr() );
			return 0;
		} // end if

	} // end try
	catch( ... ) { oexERROR( 0, oexMks( oexT( "Exception in mysql_query() : " ), sQuery.c_str() ) ); }

	try
	{
		// Grab result
		m_res = mysql_store_result( &m_db );

	} // end try
	catch( ... ) { oexERROR( 0, oexMks( oexT( "Exception in mysql_store_result() : " ), sQuery.c_str() ) ); }

	return 1;
}

int CSqMysql::getNumRows()
{_STT();
	if ( !m_res ) 
		return 0;

	try	
	{	
		return mysql_num_rows( m_res );

	} // end try
	catch( ... ) { oexERROR( 0, oexT( "Exception in mysql_num_rows()" ) ); }

	return 0;
}

int CSqMysql::getNumFields()
{_STT();
	if ( !m_res ) 
		return 0;

	try	
	{	
		return mysql_num_fields( m_res );

	} // end try
	catch( ... ) { oexERROR( 0, oexT( "Exception in mysql_num_rows()" ) ); }

	return 0;
}

int CSqMysql::getFieldInfo( int i, sqbind::CSqMap *mInfo )
{_STT();
	if ( !m_res || !mInfo ) 
		return 0;

	try	
	{	
		// Get field information
		MYSQL_FIELD *field = mysql_fetch_field_direct( m_res, i );
		if ( !field )
			return 0;

		// Grab field information
		if ( field->name )
			mInfo->set( oexT( "name" ), oexMbToStrPtr( field->name ) );
		if ( field->def )
			mInfo->set( oexT( "def" ), oexMbToStrPtr( field->def ) );
		mInfo->set( oexT( "type" ), oexMks( (int)field->type ).Ptr() );
		mInfo->set( oexT( "max_length" ), oexMks( (int)field->max_length ).Ptr() );
		
	} // end try
	catch( ... ) { oexERROR( 0, oexT( "Exception in mysql_num_rows()" ) ); return 0; }

	return 1;
}


int CSqMysql::getRow( sqbind::CSqMap *mRow )
{_STT();
	// How many rows are in the result?
	int nRows = getNumRows();
	if ( !nRows )
		return 0;

	int nFields = getNumFields();
	if ( !nFields )
		return 0;

	MYSQL_ROW row = 0;

	try
	{
		// Get row
		row = mysql_fetch_row( m_res );
		if ( !row )
			return 0;

	} // end try
	catch( ... ) { oexERROR( 0, oexT( "Exception in mysql_fetch_row()" ) ); return 0; }

	try
	{
		// Compile row data
		sqbind::CSqMap mFi;
		for ( int i = 0; i < nFields; i++ )
			if ( getFieldInfo( i, &mFi ) )
				mRow->set( mFi[ "name" ], row[ i ] ? oexMbToStrPtr( (const char*)row[ i ] ) : oexT( "" ) );

	} // end try
	catch( ... ) { oexERROR( 0, oexT( "Exception in reading row data from mysql_fetch_row()" ) ); return 0; }

	return nFields;
}
