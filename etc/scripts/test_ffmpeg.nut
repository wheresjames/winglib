
_self.load_module( "ffmpeg", "" );

function _init()
{
	test_encode();
	test_decode();
}

function test_encode()
{
	_self.echo( "----------------------------------------" );
	_self.echo( "Testing encoder..." );

	// Load a picture
	local pic = CSqImage();
	if ( !pic.Load( _self.path( "../media/car.png" ), "" ) )
	{	_self.echo( "failed to load image" );
		return;
	} // end if

//	local w = pic.getWidth();
//	local h = pic.getHeight();

	// Convert to YUV420 color space
//	local buf = CSqBinary();
//	if ( !CFfConvert().ConvertColorIB( pic, buf, CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_BICUBIC ) )
//	{	_self.echo( "ConvertColorIB() failed" );
//		return;
//	} // end if

	_self.echo( "Creating encoder..." );

	// Create encoder
	local enc = CFfEncoder();

	if ( !enc.Create( CFfEncoder().CODEC_ID_MPEG4,
					  CFfConvert().PIX_FMT_YUV420P,
					  pic.getWidth(), pic.getHeight() ) )
//	if ( !enc.Create( 0, w, h ) )
	{	_self.echo( "failed to create encoder" );
		return;
	} // end if

	// Encode the frame
	local frame = CSqBinary();
//	local nEnc = enc.Encode( buf, frame );
	local nEnc = enc.EncodeImage( pic, frame, CFfConvert().SWS_FAST_BILINEAR );
	if ( !nEnc )
	{	_self.echo( "Encode() failed" );
		return;
	} // end if

	_self.echo( "success : Encoded " + nEnc + " bytes" );
}

function test_decode()
{
	_self.echo( "----------------------------------------" );
	_self.echo( "Testing decoder..." );

	local enc = CFfDecoder();

	if ( !enc.Create( CFfDecoder().CODEC_ID_MPEG4 ) )
	{	_self.echo( "failed to create decoder" );
		return;
	} // end if


	_self.echo( "success" );
}

