
_self.load_module( "ffmpeg", "" );

function _init()
{
//	local test_image = "../media/car.png";
	local test_image = "../media/wall_street.jpg";
//	local test_image = "../media/mars.jpg";

	// Load a picture
	local img = CSqImage();
	if ( !img.Load( _self.path( test_image ), "" ) )
	{	_self.echo( "failed to load image" );
		return;
	} // end if

	_self.echo( "Loaded picture " + img.getWidth() + "x" + img.getHeight() );

	run_test( "MJPEG", img, CFfEncoder().CODEC_ID_MJPEG, -1,
			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_BILINEAR );

//	run_test( "WMV1", img, CFfEncoder().CODEC_ID_WMV1, 0,
//			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_FAST_BILINEAR );

//	run_test( "WMV2", img, CFfEncoder().CODEC_ID_WMV2, 0,
//			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_FAST_BILINEAR );

	run_test( "MSMPEG4V2", img, CFfEncoder().CODEC_ID_MSMPEG4V2, 0,
			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_FAST_BILINEAR );

	run_test( "MPEG4", img, CFfEncoder().CODEC_ID_MPEG4, 0,
			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_FAST_BILINEAR );

	run_test( "FLV1", img, CFfEncoder().CODEC_ID_FLV1, 0,
			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_FAST_BILINEAR );

	img.Resample( 352, 288, 1 );
	run_test( "H263", img, CFfEncoder().CODEC_ID_H263, 0,
			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_FAST_BILINEAR );

}

function run_test( name, img, fmt, cmp, cs, alg )
{
	_self.echo( "" );
	_self.echo( "----------------------------------------------" );
	_self.echo( "-      Codec : " + name );
	_self.echo( "----------------------------------------------" );

	local frame = CSqBinary();
	local cimg = CSqImage();

	cimg.CopyImage( img );

	test_encode( frame, cimg, fmt, cmp, cs, alg );

	if ( !frame.getUsed() )
		return;

	test_decode( frame, cimg, fmt, cmp, cs, alg );

	CSqFile().put_contents_bin( _self.root( "_ffmpeg_" + name + ".jpg" ), frame );

	cimg.Save( _self.root( "_ffmpeg_" + name + ".png" ), "" );
}


function test_encode( frame, img, fmt, cmp, cs, alg )
{
	_self.echo( "\nTesting encoder..." );

	local enc = CFfEncoder();
	if ( !enc.Create( fmt, cs, img.getWidth(), img.getHeight(), cmp ) )
	{	_self.echo( "failed to create encoder" );
		return;
	} // end if

	local nEnc = enc.EncodeImage( img, frame, alg );
	if ( !nEnc )
	{	_self.echo( "Encode() failed" );
		return;
	} // end if

	_self.echo( "success : Encoded " + nEnc + " bytes" );
}

function test_decode( frame, img, fmt, cmp, cs, alg )
{
	_self.echo( "\nTesting decoder..." );

	local dec = CFfDecoder();
	if ( !dec.Create( fmt, cs, img.getWidth(), img.getHeight(), cmp ) )
	{	_self.echo( "failed to create decoder" );
		return;
	} // end if

	if ( !dec.DecodeImage( frame, img, alg ) )
	{	_self.echo( "failed to decode image" );
		return;
	} // end if

	_self.echo( "success: Decoded Image " + img.getWidth() + "x" + img.getHeight() );

}
