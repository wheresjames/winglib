
class CGlobal
{
	done = 0;
	session = 0;
};

local _g = CGlobal();

function SetSocket( socket ) : ( _g )
{
	_self.echo( "... SetSocket() ..." );

	// Important! must do a share here
	_g.session = CSqSocket();
	_g.session.Share( socket );
}

function _init() : ( _g )
{
	_self.echo( "\n... Starting session ..." );

}

function Do() : ( _g )
{
	local data = "Asynchronous!";
	local write = "HTTP/1.1 200\r\nContent-Length: " + data.len() + "\r\n\r\n" + data;
	if ( !_g.session.Write( write, 0 ) )
	{	_self.echo( "Write() : " + _g.session.getLastError() );
		return;
	} // end if

	_g.session.Destroy();
	_self.echo( "... data written ..." );

	_g.done = 1;
}

function _idle() : ( _g )
{
	if ( !_g.session )
		return 0;

	if ( !_g.done )
	{	Do();
		return 0;
	} // end if

	if ( _g.session.isRunning( 1 ) )
		return 0;

	_self.echo( "... Session ending ..." );

	return 1;
}

