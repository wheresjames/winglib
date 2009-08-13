
_self.include( "config.nut" );

function _init()
{
	// Start the web server
	_self.spawn( 1, "", "web_server", "web_server.nut", 1 );
}

function _idle()
{
	return 0;
}

