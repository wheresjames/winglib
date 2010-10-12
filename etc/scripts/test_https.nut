
_self.load_module( "http", "" );
_self.load_module( "openssl", "" );

class CGlobal
{
	quit = 0;
	server = 0;
	ssl = 0;
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

	local page = "<pre>" + mParams.print_r( 1 ) + "</pre>";

	local mReply = CSqMulti();
	mReply[ "content" ] <- page;
	return mReply.serialize();
}

function CreateCertificate( certfile, keyfile )
{
	if ( CSqFile().exists( keyfile ) && CSqFile().exists( certfile ) )
		return 1;

	local key = COsslKey();

	if ( !key.CreateRsa( 512 ) )
	{	_self.echo( "CreateRsa() failed" ); return 0; }

	if ( !key.SavePrivateKey( keyfile ) )
	{	_self.echo( "SaveKeys() failed" ); return 0; }

	// Create a certificate
	local cert = COsslCert();
	if ( !cert.Create( key, 333, 365 * 24 * 60 * 60 ) )
	{	_self.echo( "Failed to create certificate" ); return 0; }

	// Set cert names
	cert.SetNameField( "C",  "US" );
	cert.SetNameField( "CN", "Company" );
	cert.SetNameField( "O",  "Organization" );

	// Set cert extensions
	cert.SetExtension( "nsCertType", "server" );
	cert.SetExtension( "nsComment", "This is a comment" );
	cert.SetExtension( "nsSslServerName", "www.server.com" );
	cert.SetExtension( "basicConstraints", "critical,CA:TRUE" );
	cert.SetExtension( "keyUsage", "critical,keyCertSign,cRLSign" );
	cert.SetExtension( "subjectKeyIdentifier", "hash" );
	cert.SetExtension( "nsCertType", "sslCA" );

	// Sign the certificate
	cert.Sign( key );

	// Save certificate to a file
	if ( !cert.SaveFile( certfile ) )
	{	_self.echo( "Failed to save certificate to file" ); return 0; }

	return 1;
}

function _init() : ( _g )
{
	local port = 1234;

	_self.echo( "Starting web server at https://localhost:" + port + "/" );

	_g.server = CSqHttpServer();

	local keyfile = _self.root( "test_ssl_key" );
	local certfile = _self.root( "test_ssl_cert" );
	if ( !CreateCertificate( certfile, keyfile ) )
	{	_g.quit = 1; return 0; }

	_g.ssl = CSqSSLPortFactory();
	if ( !_g.ssl.Initialize() || !_g.ssl.LoadCerts( certfile, keyfile ) )
	{	_self.echo( _g.ssl.getLastError() ); _g.quit = 1; return 0; }

	_g.server.setPortFactory( _g.ssl.getFactory() );

	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );

	if ( !_g.server.Start( port ) )
		_g.quit = 1,
		_self.alert( "Unable to start http server" );
}

function _idle() : ( _g )
{
//	_self.alert( "Waiting..." );

	return _g.quit;
}

function _exit() : ( _g )
{
	_g.server.Stop();
	_g.ssl.Destroy();
}

