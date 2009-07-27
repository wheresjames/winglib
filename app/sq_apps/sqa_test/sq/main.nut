
// Print something
_self.echo( "Hello from embedded:main.nut" );

// Include another embedded file
_self.include( "inc.nut" );

// Call a function in that file
if ( inc() )
	_self.echo( "Everyting looks OK!" );
else
	_self.echo( "Something is messed up" );

