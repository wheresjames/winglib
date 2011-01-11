
function WaitKey()
{	_self.echo( "\n...press any key...\n" );
	_self.get_key();
}

// Load module
if ( _self.load_module( "mysql", "" ) )
{	_self.echo( "!!! Failed to load MySQL module" );
	WaitKey();
	return 0;
} // end if

function _init()
{
	local server = "localhost";
	local user = "root";
	local pass = "admin";
	local db_name = "mysql";
	local db_table = "user";

	local sq = CSqMysql();

	if ( !sq.Connect( server, 0, user, pass, db_name ) )
	{	_self.echo( sq.getLastError() ); WaitKey(); return; }

	if ( !sq.Query( "SELECT * FROM `" + db_table + "`" ) )
	{	_self.echo( sq.getLastError() ); WaitKey(); return; }

	local rows = sq.getNumRows();
	local fields = sq.getNumFields();

	_self.echo( "Rows : " + rows );
	_self.echo( "Fields : " + fields );

	_self.echo( "\n=== FIELDS ===\n" );

	// Show field info
	for( local f = 0; f < fields; f++ )
	{
		local fi = CSqMulti();
		if ( !sq.getFieldInfo( f, fi ) )
			_self.echo( "!!! Error getting info for field : " + f );
		else
			_self.echo( "Field " + f + " = " + _self.replace( fi.print_r( 1 ), "\r\n", ", " ) );

	} // end for

	_self.echo( "\n=== ROWS ===\n" );

	local r = 0;
	local row = CSqMulti();
	while ( sq.getRow( row ) )
		_self.echo( _self.replace( "Row " + r++ + " = " + row.print_r( 1 ), "\r\n", ", " ) );

	_self.echo( "\n=== STRING TESTS ===\n" );

	local test_str = "Hello 'World!'";
	_self.echo( "Escape Test : " + test_str + " = " + sq.Escape( test_str ) );

	local m = CSqMulti();
	m[ "1" ] <- "first";
	m[ "2" ] <- "'second'";
	m[ "3" ] <- "`third`";
	m[ "4" ] <- "\"fourth\"";
	_self.echo( "Insert String = " + sq.MakeInsert( m ) );
	_self.echo( "Pairs = " + sq.MakePairs( m ) );


	_self.echo( "\n=== CREATE TABLE ===\n" );

	if ( !sq.Connect( server, 0, user, pass, "mytest" ) )
	{	_self.echo( sq.getLastError() ); WaitKey(); return; }

	if ( !sq.Query( "SELECT * FROM `mytable` LIMIT 0" ) )
	{
		local query = "CREATE TABLE `mytable` ("
					  + "`Field1` int(6) NOT NULL auto_increment,"
					  + "`Field2` char(50) NOT NULL,"
					  + "`Field3` tinytext NOT NULL,"
					  + "`Field4` text NOT NULL,"
					  + "PRIMARY KEY (`id`),"
					  + "UNIQUE KEY (`id`)"
					  + ")";

		if ( !sq.Query( query ) )
		{	_self.echo( sq.getLastError() ); WaitKey(); return; }

		_self.echo( "Table created" );

	} // endi f
	else
		_self.echo( "table exists" );

	_self.echo( "\n=== END ===\n" );

	WaitKey();

}


