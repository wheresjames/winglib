
	local font_name = "arial.ttf";
//	local font_name = "comic.ttf";
//	local font_name = "cour.ttf";
//	local font_name = "webdings.ttf";

	_self.echo( "\n=== STARTING ===\n" );
	_self.flush();

	if ( _self.load_module( "freetype2", "" ) )
	{	_self.echo( "Failed to load freetype2 module" );
		return 0;
	} // end if

	local ft = CFtLibrary();
	if ( ft.getLastError() )
	{	_self.echo( "freetype2 failed to initialize : " + ft.getLastError() );
		_self.flush();
		face.Destroy();
		ft.Destroy();
		return 0;
	} // end if

	_self.echo( "Fonts folder : '" + _self.get_sys_folder( 0, "fonts" ) + "'" );
	_self.flush();

	local face = CFtFace();
	if ( !ft.LoadFont( face, font_name, 0 ) )
	{	_self.echo( "freetype2 failed to load font '" + font_name + "' : " + ft.getLastError() );
		_self.flush();
		face.Destroy();
		ft.Destroy();
		return 0;
	} // end if

	_self.echo( "Font Loaded" );
	_self.flush();
	
	// Set font size
	face.setCharSize( 25 * 64, 15 * 64, 100, 0 );
//	face.setCharSize( 25 * 64, 0, 100, 0 );

//	face.setAngle( 180. );

	local sz = CSqSize();
	face.CalcSize( "hi", sz );
	_self.echo( "Size of 'hi' = " + sz.getX() + " x " + sz.getY() + "\n" );
	_self.echo( face.Str2Ascii( 0, 0, "hi", " .o#@", "\n" ) );

	// Create image
	local img = CSqImage();
	img.Create( 320, 100 );

	local pix = CSqBinary();
	if ( img.refPixels( pix ) ) pix.Zero();	

	local str = "Hello World!";
	face.CalcSize( str, sz );
	face.setColor( 128, 0, 0 );
	face.DrawImg( str, CSqPos( 22, 22 + sz.getY() ), img, 1 );

	face.setColor( 255, 255, 255 );
	face.DrawImg( str, CSqPos( 20, 20 + sz.getY() ), img, 1 );

	face.Destroy();
	ft.Destroy();
	
	img.Save( "fonttest.jpg", "" );

	_self.echo( "\n=== DONE ===\n" );

	_self.echo( "\n...press any key...\n" );
	_self.flush();
//	_self.get_key();


