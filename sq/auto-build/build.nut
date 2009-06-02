
dofile( _self.path( "config.nut" ) );
local _cfg = CConfig();

class CGlobal
{
	queue = [];
	done = 0;

	rt = "";
	wd = _self.root( "" );

};
local _g = CGlobal();

function Build( id, dir ) : ( _g, _cfg )
{
	_self.echo( "Starting build : " + id );

	local file = _self.build_path( _cfg.build_scripts_dir, id );
	if ( !CSqFile().exists( file ) )
	{	_self.echo( "Build config not found : " + file );
		return -1;
	} // end if

	local cfg = CSqMulti();
	cfg.deserialize( CSqFile().get_contents( file ) );

	if ( dir.len() )
		_g.rt = _self.root( _self.build_path( "BUILD", dir ) );
	else
		_g.rt = _self.root( "BUILD/code" );
	_g.wd = _g.rt;

	CSqFile().delete_path( _g.wd );
	CSqFile().mkdir( _g.wd );

	_g.done = build( cfg );
}

function build( cfg ) : ( _g, _cfg )
{
	_self.echo ( "Building : " + cfg.get( "name" ).str() );

	foreach ( k,v in cfg[ "s" ] )
	{
		_self.echo( "Step : " + k );

		switch( v[ "type" ].str() )
		{
			case "co" :

				local par = "checkout " + v[ "link" ].str() + " " + v[ "to" ].str();

				// Checkout
				_self.shell( "svn", par, _g.wd );

				break;

			case "cd" :

				_g.wd = _self.build_path( _g.rt, v[ "dir" ].str() );

				break;

			case "md" :

				CSqFile().mkdir( _self.build_path( _g.rt, v[ "dir" ].str() ) );

				break;

			case "cmd" :

				_self.shell( v[ "cmd" ].str(), "", _g.wd );

				break;

		} // end switch

	} // end foreach

	return 1;
}

function _init() : ( _g, _cfg )
{
}

function _idle(): ( _g, _cfg )
{

	return _g.done;
}

