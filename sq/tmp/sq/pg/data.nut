
_self.include( "config.nut" );

function pg_data( mParams, loggedin )
{
	if ( !loggedin )
		return "#err=Access Denied";

	local data = CSqMulti();

	data[ "item1" ].set( "value1" );
	data[ "item2" ].set( "value2" );
	data[ "item3" ].set( "value3" );
	data[ "item4" ].set( "value4" );
	data[ "item5" ].set( "value5" );

	return data.serialize();
}

