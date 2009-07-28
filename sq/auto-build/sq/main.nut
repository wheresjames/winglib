
_self.include( "config.nut" );
local _cfg = CConfig();

function _init()
{
	// Build thread collector
	_self.spawn( 1, "", "build_mgr", "build_mgr.nut", 1 );

	// Start the web server
	_self.spawn( 1, "", "web_server", "web_server.nut", 1 );
}

function _idle()
{
	return 0;
}

