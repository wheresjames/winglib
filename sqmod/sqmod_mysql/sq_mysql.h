// sq_mysql.h

class CSqMysql
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CSqMysql )
	_SQBIND_CLASS_CTOR_END( CSqMysql )

	/// Default constructor
	CSqMysql();

	/// Destructor
	~CSqMysql();

	/** \addtogroup CSqMysql
		@{
	*/

	/// Disconnects from the database
	void Destroy();

	/// Frees memory for last query
	void FreeResult();

	/// Connects to specified database
	int Connect( const sqbind::stdString &sServer,
				 int nPort,
				 const sqbind::stdString &sUser,
				 const sqbind::stdString &sPass,
				 const sqbind::stdString &sDatabase );

	/// Performs database query
	int Query( const sqbind::stdString &sQuery );

	/// Escapes a string so it is safe to put into the database
	sqbind::stdString Escape( const sqbind::stdString &s );

	/// Escapes a list so it is safe to put into the database
	sqbind::stdString MakeInsert( sqbind::CSqMulti *mInfo );

	/// Escapes a list so it is safe to put into the database
	sqbind::stdString MakePairs( sqbind::CSqMulti *mInfo );

	/// Get number of rows available from last query
	int getNumRows();

	/// Get the number of fields
	int getNumFields();

	/// Returns information about the specified field
	int getFieldInfo( int i, sqbind::CSqMulti *mInfo );

	/// Records the last error from mysql
	int LogError( oex::oexCSTR pPrefix );

	/// Returns a string describing the last error
	sqbind::stdString getLastError()
	{	return m_sLastError; }

	/// Returns the last error code
	int getLastErrorCode()
	{	return m_nLastError; }

	/// Returns non-zero if the database is open
	int isOpen()
	{	return m_bOpen; }

	/// Returns a row of data
	int getRow( sqbind::CSqMulti *mRow );

	/** @} */

private:

	/// SQL database server
	sqbind::stdString		m_sServer;

	/// TCP port to use
	int						m_nPort;

	/// SQL database
	sqbind::stdString		m_sDatabase;

	/// User name for SQL database
	sqbind::stdString		m_sUser;

	/// Password for SQL database
	sqbind::stdString		m_sPass;

	/// Flag indicating open database
	int						m_bOpen;

	/// Database pointer
	MYSQL					m_db;

	/// Last result
	MYSQL_RES				*m_res;

	/// String describing last error
	sqbind::stdString		m_sLastError;

	/// Last error code
	int						m_nLastError;

};
