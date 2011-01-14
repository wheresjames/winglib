
_self.load_module( "mimetic", "" );

class CGlobal
{
};

local _g = CGlobal();

function _init() : ( _g )
{
	_self.echo( "\n------ Encoding MIME message ------\n" );

	local mime = CSqMimetic();

	mime.setFrom( "from <from@domain.com>" );
	mime.setTo( "to <to@domain.com>" );
	mime.setSubject( "Subject Line" );
	mime.setBody( "This is the body" );

	local enc = mime.Encode();

	_self.echo( enc );

	_self.echo( "\n------ Decoding MIME message ------\n" );

	local dec = CSqMimetic();

	_self.echo( "Decoded : " + dec.Decode( enc ) );

	_self.echo( "From    : " + dec.getFrom() );
	_self.echo( "To      : " + dec.getTo() );
	_self.echo( "Subject : " + dec.getSubject() );
	_self.echo( "Body    : " + dec.getBody() );

	// Create image
	local img = CSqImage();
	img.Create( 320, 240 );
	local pix = CSqBinary();
	if ( img.refPixels( pix ) ) 
		pix.Zero();	

	_self.echo( "\n------ Adding MIME attachments ------\n" );

	// Add image as attachment
//	mime.setAttachment( "Image.png", "image/png", img.Encode( "png" ) );

	_self.echo( "\n------ MIME Structure ------\n" );

	_self.echo( mime.getStructure().print_r( 1 ) );

	_self.echo( "\n------ Encoding MIME message ------\n" );

	_self.echo( mime.Encode() );

	_self.echo( "\n...done...\n" );
}

function _idle() : ( _g )
{
	return -1;
}

