
_self.load_module( "mysql", "" );

function _init()
{
	local server = "localhost";
	local user = "root";
	local pass = "admin";
	local db_name = "mysql";
	local db_table = "user";

	local sq = CSqMysql();

	if ( !sq.Connect( server, 0, user, pass, db_name ) )
	{	_self.echo( sq.getLastError() ); return; }

	if ( !sq.Query( "SELECT * FROM `" + db_table + "`" ) )
	{	_self.echo( sq.getLastError() ); return; }

	local rows = sq.getNumRows();
	local fields = sq.getNumFields();

	_self.echo( "Rows : " + rows );
	_self.echo( "Fields : " + fields );

	_self.echo( "=== FIELDS ===" );

	// Show field info
	for( local f = 0; f < fields; f++ )
	{
		local fi = CSqMap();
		if ( !sq.getFieldInfo( f, fi ) )
			_self.echo( "!!! Error getting info for field : " + f );
		else
			_self.echo( "Field " + f + " = " + _self.replace( fi.print_r( 1 ), "\r\n", ", " ) );

	} // end for

	_self.echo( "=== ROWS ===" );

	local r = 0;
	local row = CSqMap();
	while ( sq.getRow( row ) )
		_self.echo( _self.replace( "Row " + r++ + " = " + row.print_r( 1 ), "\r\n", ", " ) );

	_self.echo( "=== TESTS ===" );

	local test_str = "Hello 'World!'";
	_self.echo( "Escape Test : " + test_str + " = " + sq.Escape( test_str ) );

	local m = CSqMap();
	m[ "1" ] <- "first";
	m[ "2" ] <- "'second'";
	m[ "3" ] <- "`third`";
	m[ "4" ] <- "\"fourth\"";
	_self.echo( "Insert String = " + sq.MakeInsert( m ) );
	_self.echo( "Pairs = " + sq.MakePairs( m ) );

	_self.echo( "=== END ===" );
}


