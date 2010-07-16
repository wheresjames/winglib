
_self.load_module( "freetype2", "" );

class CGlobal
{
};

local _g = CGlobal();

function _init() : ( _g )
{
	local font_name = "Arial.ttf";

	_self.echo( "\n=== STARTING ===\n" );

	local ft = CFtLibrary();
	if ( ft.getLastError() )
	{	_self.echo( "freetype2 failed to initialize : " + ft.getLastError() );
		return 0;
	} // end if

	_self.echo( "Fonts folder : '" + _self.get_sys_folder( "fonts" ) + "'" );

	local face = CFtFace();
	if ( !ft.LoadFont( face, font_name, 0 ) )
	{	_self.echo( "freetype2 failed to load font '" + font_name + "' : " + ft.getLastError() );
		return 0;
	} // end if

	// Set font size
	face.setCharSize( 50 * 64, 0, 100, 0 );

	// Set pen position
	face.setPen( 300 * 64, ( 480 - 200 ) * 64 );

	// local font = 

	_self.echo( "\n=== DONE ===\n" );

	return 1;
}

function _idle() : ( _g )
{
	return -1;
}

