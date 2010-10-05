
_self.load_module( "openssl", "" );

class CGlobal
{
	ossl = 0;
	key = 0;
};

local _g = CGlobal();

function _init() : ( _g )
{
	_self.echo( "\n------ Starting ------\n" );

	_g.ossl = CSqOpenSSL();

	// Create test keys
	if ( !CreateKeys( "TEST_KEYS", "challenge" ) )
	{	_self.echo( "CreateKeys() failed" ); return 0; }

	// Sign with key
	if ( !Sign( _g.key, "Alice loves Bob" ) )
	{	_self.echo( "Sign() failed" ); return 0; }

	// From file
	if ( !Sign2( "TEST_KEYS", "Alice loves Bob" ) )
	{	_self.echo( "Sign2() failed" ); return 0; }

	// Create a certificate
	if ( !CreateCertificate( _g.key ) )
	{	_self.echo( "CreateCertificate() failed" ); return 0; }

	_self.echo( "\n...press any key...\n" );
	_self.get_key();
}

function CreateCertificate( key )
{
	// Create a certificate
	local cert = COsslCert();
	if ( !cert.Create( key, 512, 0, 365 * 24 * 60 * 60 ) )
	{	_self.echo( "Failed to create certificate" ); return 0; }

	// Set cert names
	cert.SetNameField( "C",  "US" );
	cert.SetNameField( "CN", "Company" );
	cert.SetNameField( "O",  "Organization" );

	// Set cert extensions
	cert.SetExtension( "nsCertType", "server" );
	cert.SetExtension( "nsComment", "This is a comment" );
	cert.SetExtension( "nsSslServerName", "www.myserver.com" );
	cert.SetExtension( "basicConstraints", "critical,CA:TRUE" );
	cert.SetExtension( "keyUsage", "critical,keyCertSign,cRLSign" );
	cert.SetExtension( "subjectKeyIdentifier", "hash" );
	cert.SetExtension( "nsCertType", "sslCA" );

	// Sign the certificate
	cert.Sign( key );

	// Save certificate to a file
	if ( !cert.SaveFile( _self.root( "cert.pem" ) ) )
	{	_self.echo( "Failed to save certificate to file" ); return 0; }

	_self.echo( "Certificate Created!" );

	return 1;
}

function CreateKeys( name, challenge ) : ( _g )
{
	_self.echo( "Creating keys : " + name );

	_g.key = COsslKey();

	if ( !_g.key.CreateRsa( name, 512 ) )
	{	_self.echo( "CreateRsa() failed" ); return 0; }

	if ( !_g.key.SetChallenge( challenge ) )
	{	_self.echo( "SetChallenge() failed" ); return 0; }

	if ( !_g.key.SaveKeys( name, name + ".key", name + ".pub" ) )
	{	_self.echo( "SaveKeys() failed" ); return 0; }

	_self.echo( "Verifying keys : " + name );

	local prv = COsslKey();
	if ( !prv.LoadPrivateKey( name + ".key", name ) )
	{	_self.echo( "LoadPrivateKey() failed" ); return 0; }

	local pub = COsslKey();
	if ( !pub.LoadPublicKey( name + ".pub", name ) )
	{	_self.echo( "LoadPublicKey() failed" ); return 0; }

	return 1;
}

function Sign( key, data ) : ( _g )
{
	_self.echo( "Signing with " + key.getName() + " : " + data );

	local sig = CSqBinary();
	if ( !_g.ossl.Sign( key, data, sig ) )
	{	_self.echo( "Sign() failed" ); return 0; }

	_self.echo( "Signature length = " + sig.getUsed() );
//	_self.echo( sig.AsciiHexStr( 16, 0 ) );
	_self.echo( sig.Fingerprint( 32, 10, 1 ) );

	// Attempt to create fingerprint image
	local img = CSqImage();
	if ( img.Create( 32 * 6, 10 * 6 ) )
	{	sig.FingerprintImage( img, CSqBinary(), 6 );
		img.Save( "fingerprint.png", "" );
	} // end if

	if ( !_g.ossl.Verify( key, data, sig ) )
	{	_self.echo( "Verify() failed" ); return 0; }

	_self.echo( "Signature verified!" );

	return 1;

}

function Sign2( name, data ) : ( _g )
{
	_self.echo( "Signing with " + name + " : " + data );

	local prv = COsslKey();
	if ( !prv.LoadPrivateKey( name + ".key", name ) )
	{	_self.echo( "LoadPrivateKey() failed" ); return 0; }

	local sig = CSqBinary();
	if ( !_g.ossl.Sign( prv, data, sig ) )
	{	_self.echo( "Sign() failed" ); return 0; }

	_self.echo( "Signature length = " + sig.getUsed() );
//	_self.echo( sig.AsciiHexStr( 16, 0 ) );
	_self.echo( sig.Fingerprint( 32, 10, 1 ) );

	local pub = COsslKey();
	if ( !pub.LoadPublicKey( name + ".pub", name ) )
	{	_self.echo( "LoadPublicKey() failed" ); return 0; }

	if ( !_g.ossl.Verify( pub, data, sig ) )
	{	_self.echo( "Verify() failed" ); return 0; }

	_self.echo( "Signature verified!" );
	
	return 1;

}

function _idle() : ( _g )
{
	return -1;
}

