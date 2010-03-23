
_self.include( "config.nut" );
local _cfg = CConfig();

class CGlobal
{
	queue = [];

};
local _g = CGlobal();

function Build( id, dir ) : ( _g, _cfg )
{
	_g.queue.append( [ 1, id, dir ] );
}

function Cron( id, dir ) : ( _g, _cfg )
{
	_g.queue.append( [ 2, id, dir ] );
}

function _init() : ( _g, _cfg )
{
	_self.echo( "build manager started..." );
}

function _idle(): ( _g, _cfg )
{
	// Every second
	_self.sleep( 1000 );

	// For each build in the queue
	if ( _g.queue.len() )
	{
		// Start build
		foreach( i in _g.queue )
		{
			// Build single item
			if ( 1 == i[ 0 ] )
			{
				// Start build if not already in progress
				if ( !_self.is_path( "./" + i[ 1 ] ) )
				{	_self.spawn( 1, ".", i[ 1 ], "build.nut", 1 );
					_self.execute2( 0, i[ 1 ], "Build", i[ 1 ], i[ 2 ] );
					_self.execute( 0, i[ 1 ], "Done" );
				} // end if
				else
					_self.echo( "Already building " + i );

			} // end if

			// Build cron list
			else if ( 2 == i[ 0 ] )
			{
				// Start build if not already in progress
				if ( !_self.is_path( "./" + i[ 1 ] ) )
				{	_self.spawn( 1, ".", i[ 1 ], "build.nut", 1 );
					_self.execute2( 0, i[ 1 ], "Cron", i[ 1 ], i[ 2 ] );
					_self.execute( 0, i[ 1 ], "Done" );
				} // end if
				else
					_self.echo( "Already building " + i );

			} // end else if
		} // end foreach

		// Dump the queue
		_g.queue = [];

	} // end if

	return 0;
}

