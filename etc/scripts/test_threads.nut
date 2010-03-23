
class CGlobal
{
	id = 0;
};

local _g = CGlobal();

function _init() : ( _g )
{
	_self.echo( "Thread Started : " + _self.get_name() );

	switch( _self.get_name() )
	{
		// Main Thread
		case "" :

			// Create two threads
			_self.spawn( 0, "", "1", "test_threads.nut", 1 );
			_self.spawn( 0, "", "2", "test_threads.nut", 1 );			

			// Kill thread 1
			_self.sleep( 2000 );
			_self.echo( "Main : killing Thread 1" );
			_self.kill( "1" );

			// Force script to exit with thread 2 call still in the queue
			_self.echo( "Main : CallMe() returned : " + _self.execute( 1, "/1", "CallMe" ) );

			_self.echo( "Now you have to wait 60 seconds for Thread 2 to time out.  It shouldn't crash" );

			break;

		// Thread 1, contains function that will get called
		case "1" : 
			_self.sleep( 4000 );
			_self.echo( "Thread 1 : Done sleeping, probably going to process kill() from main now" );
			break;

		// Thread 2, calls function in thread 1
		// After the default wait time ( currently 60 seconds ), 
		// Thread 2 should return with an empty string.
		case "2" : 
			_self.sleep( 3000 );
			_self.echo( "Thread 2 : Calling Thread 1" );
			_self.echo( "Thread 2 : CallMe() returned : " + _self.execute( 1, "/1", "CallMe" ) );
			break;

		default: break;
	};

}

function _idle()
{
//	_self.alert( "Waiting..." );

	if ( !_self.get_name().len() || _self.get_children.len() )
		return 1;

	else if ( _self.get_name() == "2" )
		return 1;

	return 0;
}

function CallMe()
{
	return "hi";
}


