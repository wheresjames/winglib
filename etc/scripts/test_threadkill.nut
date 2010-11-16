
// Demonstrates thread killing
_self.echo( "\n--- STARTING THREAD KILL TEST ---\n" );

// Create a locked up thread
_self.echo( "Creating a locked up thread..." );
_self.spawn( 0, "", "1", "function _init() { while ( 1 ) { _self.sleep( 1000 ); } }", 0 );

_self.echo( "Sending kill message to thread..." );

if ( _self.kill_wait( "1", 5, 0 ) )
	_self.echo( "Opps, the thread died, that shouldn't have happened..." );
{
	_self.echo( "Sent kill and the thread didn't die, now we'll terminate it." );

	// Kill thread 1
	if ( !_self.kill_wait( "1", 5, 1 ) )
		_self.echo( "Failed to kill thread" );
	else
		_self.echo( "Thread was terminated, everything went as expected :)" );

} // end if

// End
_self.echo( "\n...press any key...\n" );
_self.get_key();
