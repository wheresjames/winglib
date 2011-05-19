
class CGlobal
{
	cfg = CSqMulti();
};

local _g = CGlobal();

function _init() : ( _g )
{
	InitConsoleLog();
	
	// Read in settings
	_g.cfg.setJSON( CSqFile().get_contents( _self.root( "sntpsync.cfg" ) ) );
	
	// Initialize settings if needed
	if ( !_g.cfg.isset( "client" ) && !_g.cfg.isset( "server" ) )
	{
		_g.cfg[ "client" ][ "enable" ] <- "1";
		_g.cfg[ "client" ][ "server" ] <- "time-a.nist.gov";
		_g.cfg[ "client" ][ "port" ] <- "123";
	
		_g.cfg[ "server" ][ "enable" ] <- "1";
		_g.cfg[ "server" ][ "port" ] <- "123";
		
		CSqFile().put_contents( _self.root( "sntpsync.cfg" ), _g.cfg.getJSON() );
		
	} // end if

	// Start the sntp server
	if ( _g.cfg[ "server" ][ "enable" ].toint() )
	{
		// Start server thread
		_self.spawn( 1, "", "sntp_server", "sntp_server.nut", 1 );
		
		// Start the server
		_self.execute1( 0, "sntp_server", "StartServer", _g.cfg[ "server" ][ "port" ].str() );

	} // end if

	// Start the sntp client
	if ( _g.cfg[ "client" ][ "enable" ].toint() )
	{
		// Start the client thread
		_self.spawn( 1, "", "sntp_client", "sntp_client.nut", 1 );

		SyncTime();

		// Sync time every hour
		_self.set_timer( ".", 60 * 60 * 1000, "SyncTime" );
		
	} // end if
	
	return 0;
}

function _idle()
{
	return 0;
}

function SyncTime() : ( _g )
{
	// Let the client thread do the work
	_self.execute2( 0, "sntp_client", "SyncTime", _g.cfg[ "client" ][ "server" ].str(), _g.cfg[ "client" ][ "port" ].str() );
}

function InitConsoleLog()
{
	// Lose old logs
	CleanupConsoleLogs();
	
	// Cleanup console logs once per hour
	_self.set_timer( ".", 60 * 60 * 1000, "CleanupConsoleLogs" );

	// Enable console capture
//	_self.enable_output_capture( 32, 32 * 128 );

	// Ensure directory
	CSqFile().mkdir( _self.root( "console" ) );

	// Capture console output to file for debugging
	local tm = CSqTime(); tm.GetSystemTime();
	local file = _self.root( "console/" + tm.FormatTime( "%Y%c%d%g%m%s_" ) + CSqFile().get_filename( _self.module_name() ) + ".console.txt" );
	_self.enable_output_file_capture( file, 3 );
}

function CleanupConsoleLogs()
{
	local gmt = _self.gmt_time();
	local clean = "";

	local root = _self.root( "console" );
	if ( !root.len() ) return 0;

	local max = 30;
	local tm = CSqTime();
	if ( CSqFile().exists( root ) )
	{	local min = gmt - ( ( 24 * 60 * 60 ) * max );
		local dir = CSqFile().get_dirlist( root, "*", 1, 0 );
		foreach( k,v in dir )
			if ( tm.ParseTime( "%Y%c%d%g%m%s_", k ) )
				if ( tm.GetUnixTime() < min )
					clean += k + " ",
					CSqFile().unlink( _self.build_path( root, k ) );
	} // end if

	if ( clean.len() )
		_self.echo( "Deleting console logs : " + clean );

	return 0;
}
