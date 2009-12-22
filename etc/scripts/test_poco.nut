
_self.load_module( "poco", "" );

class CGlobal
{
};

local _g = CGlobal();

function _init() : ( _g )
{
	local smtp = CPoSmtp();
	
	local host = "";
	local user = "";
	local pass = "";

	_self.echo( "\n------ Starting ------\n" );

	if ( !smtp.Open( host ) )
	{	_self.echo( smtp.getLastError() ); return 0; }
	
	if ( !smtp.Login( "LOGIN", user, pass ) )
	{	_self.echo( smtp.getLastError() ); return 0; }

	local msg = CPoMessage();
	
	msg.setSender( "test@wheresjames.com" );
	msg.addRecipient( "test@wheresjames.com", "Test" );
	msg.setSubject( "This is a subject" );
	msg.setContent( "This is the content" );
	
	if ( !smtp.Send( msg ) )
	{	_self.echo( smtp.getLastError() ); return 0; }

	_self.echo( "\n...done...\n" );
}

function _idle() : ( _g )
{
	return -1;
}

