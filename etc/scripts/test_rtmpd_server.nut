
class CGlobal
{
	server = CSqSocket();
};

local _g = CGlobal();

function _init() : ( _g )
{
	if ( _self.load_module( "rtmpd", "" ) )
	{	_self.echo( "\n!!! FAILED TO LOAD RTMP MODULE !!!\n" );
		return -1;
	} // end if

	_self.echo( "\n... Starting RTMP server ...\n" );

	local ts = CSqTime(); ts.GetLocalTime();
	local rtmp = CRtmpdSession();
	local ver = rtmp.getLibVersion();
	_self.echo( "... LIB VERSION : " 
					+ ( ( ver & 0xffff0000 ) >> 16 ) 
					+ "." + ( ( ver & 0xff00 ) >> 8 ) 
					+ "." + ( ver & 0xff ) + "\n" );

	CSqFile().mkdir( _self.root( "rtmplogs" ) );
	rtmp.StartDebugLog( _self.root( "rtmplogs/rtmp-" + ts.FormatTime( "%Y%c%d-%g%m%s" ) ) );

	if ( !_g.server.Bind( 1935 ) )
	{	_self.echo( "Bind() : " + _g.server.getLastError() );
		return 0;
	} // end if

	if ( !_g.server.Listen( 16 ) )
	{	_self.echo( "Listen() : " + _g.server.getLastError() );
		return 0;
	} // end if

	_self.echo( "server listening on : rtmp://localhost:1935" );

	return 0;
}

function _idle() : ( _g )
{
	// Wait for connect request
	while ( _g.server.WaitEvent( CSqSocket().EVT_ACCEPT, 100 ) )
	{
		// Create a new socket to handle this session
		local session = CSqSocket();

		// Accept the session
		if ( !_g.server.Accept( session ) )
			_self.echo( "Accept() : " + _g.server.getLastError() );

		else
		{
			// Show connection info
			local addr = CSqSockAddress();
			_g.server.getPeerAddress( addr );
			_self.echo( "Connection from : " + addr.getDotAddress() );

			// Set the script that will handle the session
			session.setScript( _self.queue(), "session", "test_rtmpd_session.nut", "SetSocket" );

		} // end else

	} // end while

	return 0;
}

