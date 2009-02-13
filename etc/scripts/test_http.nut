
function _init()
{
	_self.load_module( "sqmod_http_d.dll", 0 );

	local server = CHttpServer();
	if ( !server.Start( 1234 ) )
		_self.alert( "Unable to start http server" );

}

function _idle()
{
	_self.alert( "Waiting..." );

	return 1;
}

