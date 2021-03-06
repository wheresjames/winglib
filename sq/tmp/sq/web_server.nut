
_self.include( "config.nut" );

class CGlobal
{
	server = CSqHttpServer();

	tick = 0;
	day = 0;
};

local _g = CGlobal();

function OnAuthenticate( type, data, params )
{
	local mParams = CSqMulti();
	mParams.deserialize( params );
//	_self.echo( mParams.print_r( 1 ) );
	_self.echo( type + ":" + data + " - " + mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + " - " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	if ( !type )
		return 1;

	if ( mParams[ "SESSION" ].isset( "user" ) )
		return 1;

	_self.echo( "denied" );

	return -1;
}

function _init() : ( _g )
{
	// Create logs folder
	CSqFile().mkdir( _self.root( "weblogs" ) );

	SetWebLog();

	// Multi-threaded
//	_g.server.SetSessionCallbackScript( _self.queue(), "session.nut", 1, "OnProcessRequest" );

	// Single-threaded
	_self.include( "web_session.nut" );
	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );
	_g.server.SetAuthenticationCallback( _self.queue(), "OnAuthenticate" );

	_g.server.MapFolder( "js", _self.path( "js" ) );

	_g.server.EnableMultiThreading( 0 );
	if ( !_g.server.Start( _cfg( "tcp_port" ) ) )
		_self.alert( "Unable to start http server" );

	_self.echo( "web server is running..." );
}

function SetWebLog() : ( _g )
{
	// Get current time
	local t = CSqTime();
	t.GetLocalTime();

	// Has the day changed?
	if ( t.GetDay() == _g.day ) return;
	_g.day = t.GetDay();

	// Set new log file name
	_g.server.SetLogFile( _self.root( "weblogs/" + t.FormatTime( "weblog_%Y%c%d.txt" ) ) );
}

function _idle() : ( _g )
{
	if ( 1000 < _g.tick++ )
	{	_g.tick = 0;
		SetWebLog();
	} // end if

	return 0;
}

