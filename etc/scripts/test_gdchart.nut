
_self.load_module( "http", "" );
_self.load_module( "gdchart", "" );

class CGlobal
{
	server = 0;
};

local _g = CGlobal();

function OnServerEvent( server, data )
{

}

function OnProcessRequest( params )
{
	local mParams = CSqMulti();
	mParams.deserialize( params );
	_self.echo( mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	local mReply = CSqMap();

	local l = CSqMulti();
	l.deserialize( "0=1,1=1,2=2" );

	local p = CSqMulti();
	p.deserialize( "width=320,height=240,type=0" );

	local m1 = CSqMulti(), m2 = CSqMulti();
	m1.deserialize( "0=5,1=30,2=88" );
	m2.deserialize( "0=10,1=10,2=20" );

	local data = CSqMulti();
	data[ "0" ][ "title" ].set( "Set 0" );
	for ( local i = 0; i < 21; i++ )
		data[ "0" ][ "data" ][ format( "%04d", i ) ].set( ( ( i - 10 ) * ( i - 10 ) ).tostring() );

	local gdc = CGdcChart();
	local img = gdc.GetChart( "png", "width=400,height=300,type=14", data.serialize() );
	if ( !img.isset( "img" ) )
	{	mReply.set( "content", "Error creating chart : " + ret );
		return mReply.serialize();
	} // end if

	// Write the image to disk
//	CSqFile().put_contents( "w_raw_write.png", img.get( "img" ).str() );

	mReply.set( "content", img.get( "img" ).str() );

	return mReply.serialize();
}

function _init() : ( _g )
{
	_g.server = CHttpServer();

	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );

	if ( !_g.server.Start( 1234 ) )
		_self.alert( "Unable to start http server" );
}

function _idle()
{
//	_self.alert( "Waiting..." );

	return 0;
}

