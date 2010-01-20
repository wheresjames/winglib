
_self.include( "config.nut" );

function _init()
{
	// Create database
	local db = CSqSQLite();
	if ( !db.Open( _cfg( "db_name" ) ) )
		_self.echo( db.GetLastError() );

	// Create tables
	else
		foreach( k,v in _cfg( "db_tables" ) )
			if ( !db.IsTable( k ) )
			{	_self.echo( "Creating table '" + k + "'" );
				if ( !db.Exec( v[ 0 ] ) )
					_self.echo( db.GetLastError() );
			} // end if

	// Start the web server
	_self.spawn( 1, "", "web_server", "web_server.nut", 1 );

//	_self.spawn( 1, "", "txt_server", "H:/app/tmp/sq/txt_server.nut", 1 );
	_self.spawn( 1, "", "txt_server", "/media/OS/linux/code/app/tmp/sq/txt_server.nut", 1 );
}

function _idle()
{
	return 0;
}

