
dofile( _self.path( "config.nut" ) );
local _cfg = CConfig();

function _init()
{
	// Build thread collector
	_self.spawn( 1, "", "build_mgr", _self.path( "build_mgr.nut" ), 1 );

	// Start the web server
	_self.spawn( 1, "", "web_server", _self.path( "web_server.nut" ), 1 );
}

function _idle()
{
	return 0;
}

