

local cmd = CSqMulti( _self.get( "/", "cmdline" ) );

_self.echo( cmd.print_r( 1 ) );

if ( 3 > cmd.size() )
{
	local params = "";
	for( local i = 0; i < cmd.size(); i++ )
		params += " " + i;

	_self.sqexe( "test_sqexe.nut" + params, "" );

} // end if

_self.echo( "\n...press any key...\n" );
_self.get_key();
