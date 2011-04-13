
_self.include( "config.nut" );

function _init()
{
	// Enable console capture for web viewing
	_self.enable_output_capture( 32, 32 * 128 );

	// Echo the version / build to make life easy
	_self.echo( "WetCoin.com Integration Service : " + _self.get_version() + " / " + _self.get_build() );

	// Set unique id for process spawn
	_self.set( "/", "spawnid", _self.unique() );

	// Start the web server
	_self.spawn( 1, "", "web_server", "web_server.nut", 1 );
}

function _idle()
{
	return 0;
}

