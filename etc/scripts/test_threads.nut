
class CGlobal
{
	id = 0;
};

local _g = CGlobal();

function _init() : ( _g )
{
	// Demonstrates thread calling safety

	_self.echo( "Thread Started : " + _self.get_name() );

	// Create two threads
	_self.spawn( 0, "", "1", "test_threads_1.nut", 1 );
	_self.spawn( 0, "", "2", "test_threads_2.nut", 1 );			

	// Kill thread 1
	_self.sleep( 2000 );
	_self.echo( "Main : killing Thread 1" );
	_self.kill( "1" );

	// Force script to exit with thread 2 call still in the queue
	_self.echo( "Main : CallMe() returned : " + _self.execute( 1, "/1", "CallMe" ) );

	_self.echo( "Now you have to wait for Thread 2 to time out.  It shouldn't crash" );

}

function _idle()
{
	// Wait for thread 2 to timeout
	if ( _self.get_children( "." ).len() )
		return 0;

	_self.echo( "\n...press any key...\n" );
	_self.get_key();

	return 1;
}


