
_self.include( "config.nut" );

function pg_data( mParams, loggedin )
{
	if ( !loggedin )
		return "#err=Access Denied";

/*
	local data = CSqMulti();

	data[ "item1" ].set( "value1" );
	data[ "item2" ].set( "value2" );
	data[ "item3" ].set( "value3" );
	data[ "item4" ].set( "value4" );
	data[ "item5" ].set( "value5" );
*/

	local data = CSqMulti();
	
	local dir = _self.root( "box/in" );
	
	local list = CSqFile().get_dirlist( dir, "*", 1, 0 );
	foreach( k,v in list )
	{
		local full = _self.build_path( dir, k );
		local cc = CSqMulti();
		cc.deserialize( CSqFile().get_contents( full ) );
		
		data[ k ].set( cc[ "from" ].str() + " - "
		               + cc[ "subject" ].str() + " - "
		               + cc[ "body" ].str() );
	
	} // end foreach	

	return data.serialize();
}

