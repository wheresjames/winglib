
if ( _self.load_module( "crtmp", "" ) )
{	_self.echo( "\n!!! FAILED TO LOAD CRTMP MODULE !!!\n" );
	return -1;
} // end if

class CGlobal
{
//	server = CSqSocket();

	quit = 0;
	crtmp = CRtmpServer();
};

local _g = CGlobal();

function _init() : ( _g )
{
	_self.echo( "\n... Starting RTMP server ...\n" );
	_self.get_key();

//	if ( !_g.server.Bind( 1935 ) )
	if ( !_g.crtmp.Init() )
	{	_self.echo( "Init() failed : " + _g.crtmp.getLastErrorStr() );
		_g.quit = 1;
		return 0;
	} // end if

	_self.echo( " --- PROTOCOL MAP --- \n" + _g.crtmp.getProtocolMap() + "\n\n" );

	if ( !_g.crtmp.Start( 1935 ) )
	{	_self.echo( "Start() failed : " + _g.crtmp.getLastErrorStr() );
		_g.quit = 1;
		return 0;
	} // end if

	_self.echo( "server listening on : rtmp://localhost:1935" );

	return 0;
}

function _idle() : ( _g )
{
	return _g.quit;
}

