
_self.load_module( "ffmpeg", "" );

function _init()
{
	local frame = CSqBinary();

	test_encode( frame );

	if ( frame.getUsed() )
		test_decode( frame );

	_self.echo( "--- DONE ---" );
}

function test_encode( frame )
{
	_self.echo( "----------------------------------------" );
	_self.echo( "Testing encoder..." );

	// Load a picture
	_self.echo( "Loading picture..." );
	local img = CSqImage();
	if ( !img.Load( _self.path( "../media/car.png" ), "" ) )
	{	_self.echo( "failed to load image" );
		return;
	} // end if

	_self.echo( "Loaded picture " + img.getWidth() + "x" + img.getHeight() );

	// Create encoder
	_self.echo( "Creating encoder..." );
	local enc = CFfEncoder();
	if ( !enc.Create( CFfEncoder().CODEC_ID_MPEG4,
					  CFfConvert().PIX_FMT_YUV420P,
					  img.getWidth(), img.getHeight() ) )
	{	_self.echo( "failed to create encoder" );
		return;
	} // end if

	// Encode the frame
	local nEnc = enc.EncodeImage( img, frame, CFfConvert().SWS_FAST_BILINEAR );
	if ( !nEnc )
	{	_self.echo( "Encode() failed" );
		return;
	} // end if

//	CSqFile().put_contents_bin( _self.root( "frame_dump.avi" ), frame );

	_self.echo( "success : Encoded " + nEnc + " bytes" );
}

function test_decode( frame )
{
	_self.echo( "----------------------------------------" );
	_self.echo( "Testing decoder..." );

	local dec = CFfDecoder();

	if ( !dec.Create( CFfDecoder().CODEC_ID_MPEG4 ) )
	{	_self.echo( "failed to create decoder" );
		return;
	} // end if
	
	local img = CSqImage();
//	img.Create( 160, 120 );
	if ( !dec.DecodeImage( frame, img ) )
	{	_self.echo( "failed to decode image" );
		return;
	} // end if

	_self.echo( "success: Decoded Image " + img.getWidth() + "x" + img.getHeight() );

	img.Save( _self.root( "test.bmp" ), "" );
}

