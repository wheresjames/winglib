
function _init()
{
	_self.echo( "Thread Started : " + _self.get_name() );

	_self.sleep( 4000 );
	_self.echo( "Thread 1 : Done sleeping, probably going to process kill() from main now" );
}

function _idle()
{
	return 0;
}

function CallMe()
{
	return "hi";
}


