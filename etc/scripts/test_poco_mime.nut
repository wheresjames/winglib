
_self.load_module( "poco", "" );

class CGlobal
{
};

local _g = CGlobal();

function WaitKey()
{	_self.echo( "\n...press any key...\n" );
	_self.get_key();
}

function _init() : ( _g )
{
	_self.echo( "\n------ Starting ------\n" );

	local msg = CPoMessage();	
	msg.setSender( "test@wheresjames.com" );
	msg.addRecipient( "test@wheresjames.com", "Test" );
	msg.setSubject( "This is a subject" );

	msg.addPartStr( "content", "text/html", "This is an embedded image <img src='cid:img1'>" );

	_self.echo( "\n------ Attaching image to message ------\n" );

	// Create image
	local img = CSqImage();
	img.Create( 160, 120 );
	local pix = CSqBinary();
	if ( img.refPixels( pix ) ) 
		pix.Zero();	

	msg.addAttachmentBin( "Image.png", "image/png", img.Encode( "png" ) );
	msg.setPartHeader( "Image.png", "Content-Id", "img1" );

	_self.echo( "\n------ Encoding MIME message ------\n" );

	local bin = CSqBinary();
	msg.Encode( bin );

	CSqFile().put_contents_bin( "test.eml", bin );

	_self.echo( bin.getString() );

	_self.echo( "\n...done...\n" );

	WaitKey();
}

function _idle() : ( _g )
{
	return -1;
}

