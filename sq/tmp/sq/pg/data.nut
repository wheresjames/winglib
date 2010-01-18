
_self.include( "config.nut" );

function pg_data( mParams, loggedin )
{
	if ( !loggedin )
		return "#err=Access Denied";

	local data = CSqMulti();

	data[ "item1" ].set( "value1" );
	data[ "item2" ].set( "value2" );

	return data.serialize();
}

