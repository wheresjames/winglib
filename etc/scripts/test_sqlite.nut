
function _init()
{
	local sq = CSqSQLite();

	_self.echo( "========== Create database and table ==========" );

	if ( !sq.Open( ":memory:" ) )
		_self.echo( sq.GetLastError() );

	else if ( !sq.Exec( "CREATE TABLE test ( id INTEGER NOT NULL, name CHAR(64), title CHAR(64), PRIMARY KEY(id) )" ) )
		_self.echo( sq.GetLastError() );

	else
		sq.QueryColumnInfo();
		_self.echo( sq.GetLastError() );
		_self.echo( sq.Result().print_r( 1 ) );


	_self.echo( "========== Insert a couple people into the database ==========" );

	if ( !sq.Exec( "INSERT INTO test VALUES(0,'Tim','King')" ) )
		_self.echo( sq.GetLastError() );

	else if ( !sq.Exec( "INSERT INTO test (name,title) VALUES('Amy','Queen')" ) )
		_self.echo( sq.GetLastError() );

	else if ( !sq.Exec( "SELECT * FROM test" ) )
		_self.echo( sq.GetLastError() );

	else
		_self.echo( sq.GetLastQuery() ),
		_self.echo( "Rows : " + sq.NumRows() ),
		_self.echo( sq.Result().print_r( 1 ) );


	_self.echo( "========== Change Amy to a Dutchess ==========" );

	local pb = CSqMulti();
	pb[ "title" ] <- "Dutchess";

	if ( !sq.Update( "test", "`name`='Amy'", pb ) )
		_self.echo( sq.GetLastError() );

	else if ( !sq.Exec( "SELECT * FROM test WHERE `name`='Amy'" ) )
		_self.echo( sq.GetLastError() );

	else
		_self.echo( sq.GetLastQuery() ),
		_self.echo( "Rows : " + sq.NumRows() ),
		_self.echo( sq.Result().print_r( 1 ) );

}


