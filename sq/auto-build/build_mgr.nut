
dofile( _self.path( "config.nut" ) );
local _cfg = CConfig();

class CGlobal
{
	queue = [];

};
local _g = CGlobal();

function Build( id, dir ) : ( _g, _cfg )
{
	_g.queue.append( [ id, dir ] );
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
			// Start build if not already in progress
			if ( !_self.is_path( "./" + i[ 0 ] ) )
			{	_self.spawn( 1, ".", i[ 0 ], _self.path( "build.nut" ), 1 );
				_self.execute2( 0, i[ 0 ], "Build", i[ 0 ], i[ 1 ] );
			} // end if
			else
				_self.echo( "Already building " + i );

		} // end foreach

		// Dump the queue
		_g.queue = [];

	} // end if

	return 0;
}

