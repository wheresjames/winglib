
function _init()
{
	_self.echo( "Thread Started : " + _self.get_name() );


	// Thread 2, calls function in thread 1
	// After the default wait time ( currently 60 seconds ), 
	// Thread 2 should return with an empty string.

	_self.sleep( 3000 );
	_self.echo( "Thread 2 : Calling Thread 1" );
	_self.echo( "Thread 2 : CallMe() returned : " + _self.execute( 1, "/1", "CallMe" ) );

	return 0;
}

function _idle()
{
	return 1;
}


