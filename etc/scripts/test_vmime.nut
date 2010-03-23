
_self.load_module( "vmime", "" );

class CGlobal
{
};

local _g = CGlobal();

function _init() : ( _g )
{
	local vmime = CSqVMime();

	vmime.setUsername( "" );
	vmime.setPassword( "" );

	_self.echo( "\n------ Starting ------\n" );
/*
	local m = CVmMsg();

	m.setField( "From",			"test@wheresjames.com" );
	m.setField( "To",			"test@wheresjames.com" );
	m.setField( "Subject",		"Subject" );

	m.setField( "Date",			"Fri, 18 Dec 2009 10:04:21 -0500" );
	m.setField( "Sender",		"test@wheresjames.com" );
	m.setField( "User-Agent",	"vmime" );
	m.setField( "Content-Type",	"text/plain; charset=UTF-8" );

	m.setBody( "This is the message body" );

	_self.echo( m.Generate() );

//	if ( !vmime.Send( "smtp://smtp.powweb.com", m ) )
//	{	_self.echo( "failed to send message : " + vmime.getLastError() ); _self.sleep( 3000 ); return 0; }

	return;
*/
	_self.echo( "\n------ Connecting ------\n" );

	if ( !vmime.Open( "imap://imap.powweb.com" ) )
	{	_self.echo( "failed to connect : " + vmime.getLastError() ); return 0; }

	local count = vmime.getMessageCount();
	_self.echo( count + " message(s) in your inbox\n" );

	_self.echo( "\n---- Folders ----\n" );

	local folders = CSqMulti();
	vmime.getFolders( folders );
	_self.echo( vmime.getLastError() );

	_self.echo( folders.print_r( 1 ) );

	if ( count )
	{
		_self.echo( "--- First Message ---\n" );

		local msg = vmime.getMessage( 1 );
		if ( !msg.isValid() )
			_self.echo( "invalid message object" );
		else
		{
//			_self.echo( "Header: \n" + msg.getHeader() );

			_self.echo( "From: "	+ msg.getField( "From" ) );
			_self.echo( "To: "		+ msg.getField( "To" ) );
			_self.echo( "Date: "	+ msg.getField( "Date" ) );
			_self.echo( "Subject: " + msg.getField( "Subject" ) );

			_self.echo( "\n --- Body ---\n\n" + msg.getBody() );

			_self.echo( "\n --- Structure ---\n\n" + msg.getStructure() );

//			_self.echo( "\n---------------------------------------------------\n" 
//						+ msg.Generate() );

		} // end if

	} // end if

	local m = CVmMsg();

	m.setField( "From",			"test@wheresjames.com" );
	m.setField( "To",			"test@wheresjames.com" );
	m.setField( "Subject",		"Subject" );

//	m.setField( "Date",			"Fri, 18 Dec 2009 10:04:21 -0500" );
//	m.setField( "Sender",		"test@wheresjames.com" );
//	m.setField( "User-Agent",	"vmime" );
//	m.setField( "Content-Type",	"text/plain; charset=UTF-8" );

	m.setBody( "This is the message body" );

	_self.echo( "\n--- Sending message ---\n" + m.Generate() );

	// If imap outbox is supported
//	if ( !vmime.setFolder( "OUTBOX" ) )
//	{	_self.echo( vmime.getLastError() ); return 0; }
//	if ( !vmime.addMessage( m ) )
//	{	_self.echo( vmime.getLastError() ); return 0; }

	// Send via SMTP
	if ( !vmime.Send( "smtp://smtp.powweb.com", m ) )
	{	_self.echo( "failed to send message : " + vmime.getLastError() ); _self.sleep( 3000 ); return 0; }

	_self.echo( "\n...done...\n" );
}

function _idle() : ( _g )
{
	return -1;
}

