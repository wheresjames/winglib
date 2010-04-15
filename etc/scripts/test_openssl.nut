
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

	if ( !CreateKeys( "TEST_KEYS", "challenge" ) )
	{	_self.echo( "CreateKeys() failed" ); return 0; }
	
	Sign( _g.key, "Alice loves Bob" );

	_self.echo( "\n...done...\n" );
}

function CreateKeys( name, challenge ) : ( _g )
{
	_self.echo( "Creating keys : " + name );

	_g.key = COsslKey();

	if ( !_g.key.CreateRsa( name, 512 ) )
	{	_self.echo( "CreateRsa() failed" ); return 0; }
	
	if ( !_g.key.SetChallenge( challenge ) )
	{	_self.echo( "SetChallenge() failed" ); return 0; }
	
	if ( !_g.key.SaveKeys( name, name + ".key", name + ".pki" ) )
	{	_self.echo( "SaveKeys() failed" ); return 0; }

	_self.echo( "Verifying keys : " + name );

	local prv = COsslKey();	
	if ( !prv.LoadPrivateKey( name + ".key", name ) )
	{	_self.echo( "LoadPrivateKey() failed" ); return 0; }
	
	local pub = COsslKey();
	if ( !pub.LoadPublicKey( name + ".pki", name ) )
	{	_self.echo( "LoadPublicKey() failed" ); return 0; }

	return 1;
}

function Sign( key, data ) : ( _g )
{
	_self.echo( "Signing : " + key + " : " + data );

	local sig = CSqBinary();
	if ( !_g.ossl.Sign( key, "Hello World!", sig ) )
	{	_self.echo( "LoadPublicKey() failed" ); return 0; }


	_self.echo( "Signature length = " + sig.getUsed() );
	
	return 1;
		
}

function _idle() : ( _g )
{
	return -1;
}

