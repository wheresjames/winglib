
dofile( _self.path( "config.nut" ) );
local _cfg = CConfig();

class CGlobal
{
	queue = [];

};
local _g = CGlobal();

function Build( id ) : ( _g, _cfg )
{
	_g.queue.append( id );
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
			if ( !_self.is_path( "./" + i ) )
			{	_self.spawn( 1, ".", i, _self.path( "build.nut" ), 1 );
				_self.execute1( 0, i, "Build", i );
			} // end if
			else
				_self.echo( "Already building " + i );

		} // end foreach

		// Dump the queue
		_g.queue = [];

	} // end if

	return 0;
}

