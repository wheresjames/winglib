
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
	_self.echo( "\n=== FIELDS ===\n" );

	_self.echo( "Fonts folder : '" + _self.get_sys_folder( "fonts" ) + "'" );
	
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
	
	// Draw title
	local title = "PDF Test", tw = pdf.getTextWidth( title );

	if ( !pdf.SetFont( _self.get_sys_path( "fonts", "arial.ttf" ), 24 ) )
	{	_self.echo( "Error setting font : " + pdf.getLastError() ); WaitKey(); return; }

	pdf.BeginText();

	if ( !pdf.TextOut( w - tw, h - 50, title ) )
	{	_self.echo( "Error writing text : " + pdf.getLastError() ); WaitKey(); return; }

	pdf.EndText();	
	
	if ( !pdf.Save( _self.root( "test.pdf" ) ) )
	{	_self.echo( "Error saving to file : " + pdf.getLastError() ); WaitKey(); return; }

	_self.echo( "\n=== DONE ===\n" );
	
	WaitKey();
}

function _idle()
{

	return 0;
}

