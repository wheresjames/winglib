
class CGlobal
{
	quit = 0;
	session = 0;
	fps = 0;
	rtmp = 0;
};

local _g = CGlobal();

function _idle() : ( _g )
{
	return _g.quit;
}

function _init() : ( _g )
{
	// Start the update timer
	if ( 0 >= _g.fps ) _g.fps = 30;
	_self.set_timer( ".", 1000 / _g.fps, "Run" );

	_self.echo( "... Starting session ..." );

	return 0;
}

function _exit() : ( _g )
{
	if ( _g.rtmp )
		_g.rtmp.Destroy(), _g.rtmp = 0;

	if ( _g.session )
		_g.session.Destroy(), _g.session = 0;

	return 0;
}

function SetSocket( socket ) : ( _g )
{
	// Load rtmpd module
	if ( _self.load_module( "rtmpd", "" ) )
	{	_self.echo( "!!! Unable to load RTMPD module !!!" );
		_g.quit = 1;
		return 0;
	} // end if

	// Important! must do a share here
	_g.session = CSqSocket();
	_g.session.Share( socket );

	// Show remote client address
	local addr = CSqSockAddress();
	if ( _g.session.getPeerAddress( addr ) )
		_self.echo( "... Connecting to : " + addr.getDotAddress() );
	else
		_self.echo( "... FAILED TO CONNECT ..." );

	_self.echo( "... Connected" );
	
	return 1;
}

function Run() : ( _g )
{
	if ( _g.quit || !_g.session )
		return 0;

	if ( !_g.rtmp )
	{
		// Create rtmpd session object
		_g.rtmp = CRtmpdSession();

		// Initialize RTMP stuff
		if ( !_g.rtmp.Init( _g.session ) )
		{	_self.echo( "!!! RTMP Session Init() Failed : " + _g.rtmp.getLastErrorStr() );
			_g.quit = 1;
			return 0;
		} // end if

	} // end if

	local ret = _g.rtmp.ReadPacket();

	_self.echo( "... ReadPacket() : " + ret );

	// Connection closed?
	if ( 0 > ret )
	{	_g.quit = 1;
		return 0;
	} // end if

	// No packet
	else if ( !ret )
		return 0;

	_self.echo( "... Packet Type : " + ret + format( " : 0x%x", ret ) );
	_self.echo( "... Packet Size : " + _g.rtmp.getPacketSize() );

	local data = _g.rtmp.getPacketData( 0 );
	_self.echo( data.AsciiHexStr( 16, 32 ) );

	local pkt = _g.rtmp.getPacket( 0 );
	_self.echo( "\n --- PACKET ---\n" + pkt.print_r( 1 ) );

	return 1;
}

