
_self.load_module( "http", "" );

class CGlobal
{
	server = 0;
	cap = 0;
};

local _g = CGlobal();

function OnServerEvent( server, data )
{

}

function OnProcessRequest( params ) : ( _g )
{
	local mReply = CSqMap();

	local mParams = CSqMulti();
	mParams.deserialize( params );
	_self.echo( mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

//	local img = CSqImage();
//	img.Load( _self.path( "../media/car.png" ), "" );

	local img = _g.cap.Capture( "jpg" );

	mReply.set( "content", img ); //.Encode( "jpg" ) );

	return mReply.serialize();
}

function _init() : ( _g )
{
	_g.server = CHttpServer();

	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );

	if ( !_g.server.Start( 1234 ) )
		_self.echo( "Unable to start http server" );

	_g.cap = CSqCapture();
	if ( 0 > _g.cap.Init( 0, 0, 320, 240, 15, "JPEG", 1 ) )
		_self.echo( "Unable to start capture device" );

	local disp = "";
	local formats = split( _g.cap.GetSupportedFormats(), "," );
	foreach( f in formats )
		disp += f + " - " + _g.cap.GetFormatDescription( f ) + "\n";

	_self.echo( disp );

//	_self.echo( _g.cap.GetSupportedFormats() );
}

function _idle()
{
//	_self.alert( "Waiting..." );

	return 0;
}

