
_self.load_module( "irrlicht", "" );
_self.load_module( "freenect", "" );

class CGlobal
{
	quit = 1;
	irr = 0;
	fn = 0;
	video = 0;
	tex = 0;
};

local _g = CGlobal();

function WaitKey()
{	_self.echo( "\n...press any key...\n" );
	_self.get_key();
}

function _init() : ( _g )
{
	_g.fn = CSqFreenect();

	if ( !_g.fn.Init() )
	{	_self.echo( "Failed to initialize freenect library" );
		WaitKey();
		return 0;
	} // end if

	_self.echo( "...freenect library initialized...\n" );

	local ndev = _g.fn.getNumDevices();
	_self.echo( "Number of devices : " + ndev + "\n" );
	
//	if ( !ndev )
//	{	_self.echo( "Exiting since no devices were found\n" );
//		WaitKey();
//		return 0;
//	} // end if

	if ( !_g.fn.OpenDevice( 0 ) )
	{	_self.echo( "Error opening device\n" );
		WaitKey();
		return 0;
	} // end if
	
	_g.irr = CSqIrrlicht();

//	_g.irr.SetStereo( 1 );
	_self.echo( "...Initializing...\n" );
	_g.irr.Init( "libfreenect Test", 640, 480, 1 );

	_self.echo( "...setting ambient light...\n" );
	_g.irr.SetAmbientLight( CSqirrColorf( 0.5, 0.5, 0.5 ) );

	_self.echo( "...adding lights...\n" );
	_g.irr.AddLight( CSqirrVector3d( 0., 100., 0. ), CSqirrColorf( 1., 1., 1. ), 100. );
	_g.irr.AddLight( CSqirrVector3d( 0., 100., 50. ), CSqirrColorf( 1., 1., 1. ), 100. );

//	_self.echo( "adding sky...\n" );
//	_g.irr.AddSkyDome( _self.path( "../imgs/sky.png" ), 16, 16, 100., 100. );

	_self.echo( "...adding camera...\n" );
	local cam = _g.irr.AddCamera( CSqirrVector3d( 0, 0, 100 ), CSqirrVector3d( 0, 0, 0 ) );
//	cam.SetLens( 1., 2.4, 3.2 );

	_self.echo( "...adding video plane...\n" );
    _g.video = _g.irr.AddGrid( 100., 100., 1, 1, 0., 2, CSqirrColor( 255, 255, 255 ), 2 );
	_g.video.SetPosition( CSqirrVector3d( -50, -50, 0 ) );

//	_g.tex = _g.irr.CreateTexture( .getWidth(), .getHeight(), 0 );
//	_g.video.SetTexture( 0, _g.tex );

	// Set draw function
	_self.set_timer( ".", 30, "OnDraw" )

	_g.quit = 0;
}

function _idle() : ( _g )
{
	return _g.quit;
}

function OnDraw() : ( _g )
{
	_g.quit = _g.irr.Draw( CSqirrColor( 100, 100, 100 ) );
	return 0;
}

