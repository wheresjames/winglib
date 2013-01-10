
/*
<html>
<body>
	<script src='http://releases.flowplayer.org/js/flowplayer-3.2.11.min.js'></script>
	<link rel='stylesheet' type='text/css' href='http://releases.flowplayer.org/5.2.1/skin/minimalist.css' />
	<div id='rtmpplayer' style='width:320px;height:240px;margin:0 auto;text-align:center'>
		<img src='default.jpg' width='320' height='240' />
	</div>
	<script type='text/javascript'>		
		$f('rtmpplayer', 'http://releases.flowplayer.org/swf/flowplayer-3.2.15.swf', 
		{	clip: 
			{	url: 'livestream',
				scaling: 'fit',
				provider: 'test'
			},
			plugins: 
			{	test: 
				{	url: 'flowplayer.rtmp-3.2.11.swf',
					netConnectionUrl: 'rtmp://localhost:1935/test'
				}
			},
			canvas: 
			{	backgroundGradient: 'none'
			}
		});
	</script>
</body>
</html>
*/

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
			session.setScript( _self.queue(), _self.unique(), "test_rtmpd_session.nut", "SetSocket" );

		} // end else

	} // end while

	return 0;
}

