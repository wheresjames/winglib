
_self.load_module( "haru", "" );

class CGlobal
{
};

local _g = CGlobal();

function WaitKey()
{	_self.echo( "\n...press any key...\n" );
	_self.get_key();
}

function _init()
{
	_self.echo( "\n=== BEGIN ===\n" );

	_self.echo( "Fonts folder : '" + _self.get_sys_folder( 0, "fonts" ) + "'" );
	
	local pdf = CHaruPdf();
	
	if ( !pdf.New() )
	{	_self.echo( "Error creating new document : " + pdf.getLastError() ); WaitKey(); return; }
	
	if ( !pdf.AddPage() )
	{	_self.echo( "Error adding page : " + pdf.getLastError() ); WaitKey(); return; }
	
	local w = pdf.getPageWidth(), h = pdf.getPageHeight();	
	_self.echo( "Created document : " + w + " x " + h );
	
	// Draw box
	pdf.setLineWidth( 1 );
	pdf.Rectangle( 50, 50, w - 100, h - 100 );
	pdf.Stroke();
	
	// Set font
//	if ( !pdf.SetTtfFont( _self.get_sys_path( 0, "fonts", "arial.ttf" ), 24 ) )
	if ( !pdf.SetFont( "Helvetica", 24 ) )
	{	_self.echo( "Error setting font : " + pdf.getLastError() ); WaitKey(); return; }

	// Draw title
	local title = "PDF Test";
	local tw = pdf.getTextWidth( title );
	pdf.BeginText();
	if ( !pdf.TextOut( ( w - tw ) / 2, h - 40, title ) )
	{	_self.echo( "Error writing text : " + pdf.getLastError() ); WaitKey(); return; }
	pdf.EndText();	

	local text = "abcdefgABCDEFG12345!#$%&+-@?";
	local fonts = 
	[
			"Courier",
			"Courier-Bold",
			"Courier-Oblique",
			"Courier-BoldOblique",
			"Helvetica",
			"Helvetica-Bold",
			"Helvetica-Oblique",
			"Helvetica-BoldOblique",
			"Times-Roman",
			"Times-Bold",
			"Times-Italic",
			"Times-BoldItalic",
			"Symbol",
			"ZapfDingbats"
		];

	pdf.BeginText();
	pdf.MoveTextPos( 60, h - 105 );

	foreach( f in fonts )
	{
		_self.echo( "Printing : " + f );

		pdf.SetFont( f, 9 );
		pdf.ShowText( f );
		pdf.MoveTextPos( 0, -18 );

		pdf.SetFont( f, 20 );
		pdf.ShowText( text );
		pdf.MoveTextPos( 0, -20 );

	} // end foreach

	pdf.EndText();

	if ( !pdf.Save( "test.pdf" ) )
	{	_self.echo( "Error saving to file : " + pdf.getLastError() ); WaitKey(); return; }

	_self.echo( "\nCreated file : test.pdf" );
	
	local bin = CSqBinary();
	if ( pdf.getBin( bin ) )
		CSqFile().put_contents_bin( "test_bin.pdf", bin ),
		_self.echo( "\nCreated file : test_bin.pdf" );

	_self.echo( "\n=== DONE ===\n" );
	
	WaitKey();
	
	return -1;
}

function _idle()
{

	return 0;
}

