
_self.load_module( "ffmpeg", "" );

function _init()
{
	local ffmpeg_root = _self.root( "_ffmpeg_" )
	CSqFile().mkdir( ffmpeg_root );

	local test_img = _self.path( "../media/wall_street.jpg" );
	local test_avi = _self.path( "../media/nurse_shark.avi" );
//	local test_avi = "/home/landshark/bag_test.avi";
//	local test_avi = _self.root( "TestClip.avi" );

	// **************************************************
	// Encode / Decode tests

	// Load a picture
	local img = CSqImage();
	if ( !img.Load( test_img, "" ) )
	{	_self.echo( "failed to load image" );
		return;
	} // end if

	_self.echo( "Loaded picture " + img.getWidth() + "x" + img.getHeight() );

	run_test( ffmpeg_root, "MJPEG", img, CFfEncoder().CODEC_ID_MJPEG, -1,
			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_BILINEAR );

//	run_test( ffmpeg_root, "WMV1", img, CFfEncoder().CODEC_ID_WMV1, 0,
//			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_FAST_BILINEAR );

//	run_test( ffmpeg_root, "WMV2", img, CFfEncoder().CODEC_ID_WMV2, 0,
//			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_FAST_BILINEAR );

	run_test( ffmpeg_root, "MSMPEG4V2", img, CFfEncoder().CODEC_ID_MSMPEG4V2, 0,
			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_FAST_BILINEAR );

	run_test( ffmpeg_root, "MPEG4", img, CFfEncoder().CODEC_ID_MPEG4, 0,
			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_FAST_BILINEAR );

	run_test( ffmpeg_root, "FLV1", img, CFfEncoder().CODEC_ID_FLV1, 0,
			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_FAST_BILINEAR );

	img.Resample( 352, 288, 0 );
	run_test( ffmpeg_root, "H263", img, CFfEncoder().CODEC_ID_H263, 0,
			  CFfConvert().PIX_FMT_YUV420P, CFfConvert().SWS_FAST_BILINEAR );


	// **************************************************
	// AVI test

	test_avi_read( ffmpeg_root, test_avi );

	test_avi_write( ffmpeg_root, _self.build_path( ffmpeg_root, "_a_ffmpeg_write_test.avi" ) );

	// **************************************************
	// RTSP test

//	test_rtsp( ffmpeg_root, "rtsp://192.168.2.130/Mediainput/mpeg4", 30 );
//	test_rtsp( ffmpeg_root, "rtsp://prug.rtsp-youtube.l.google.com/ChoLENy73wIaEQmJv18x7xfevhMYESARFEgGDA==/0/0/0/1/video.3gp", 15 );
//	test_rtsp( ffmpeg_root, "rtsp://video2.multicasttech.com/AFTVSciFi3GPP296.sdp", 15 );
//	test_rtsp( ffmpeg_root, "rtsp://a1352.l1857053128.c18570.g.lq.akamaistream.net/D/1352/18570/v0001/reflector:53128", 15 );

	_self.echo( "" );
}

function test_rtsp( root, url, fps )
{
	_self.echo( "" );
	_self.echo( "----------------------------------------------" );
	_self.echo( "-     RTSP Test : " + url );
	_self.echo( "----------------------------------------------" );

	local vid_in = CFfContainer();
	local file_info = CSqMulti();
	if ( !vid_in.Open( url, file_info ) )
	{	_self.echo( "failed to open file" );
		return;
	} // end if

	_self.echo( "Video File : " + vid_in.getWidth() + "x" + vid_in.getHeight() );
	_self.echo( " -> " + file_info.serialize() );

	local dec = CFfDecoder();
	if ( !dec.Create( vid_in.getVideoCodecId(), vid_in.getVideoFormat(),
					  vid_in.getWidth(), vid_in.getHeight(), 0 ) )
	{	_self.echo( "failed to create decoder" );
		return;
	} // end if

	local vid_out = CFfContainer();
	if ( !vid_out.Create( _self.build_path( root, "_ffmpeg_rtsp.avi" ), "", file_info ) )
	{	_self.echo( "failed to create file" );
		return;
	} // end if

	if ( 0 > vid_out.AddVideoStream( vid_in.getVideoCodecId(), vid_in.getWidth(), vid_in.getHeight(), fps ) )
	{	_self.echo( "failed to add video stream" );
		return;
	} // end if

	if ( !vid_out.InitWrite() )
	{	_self.echo( "failed to initialize file" );
		return;
	} // end if

	local i = 0;
	local stream = -1;
	local frame = CSqBinary();
	local frame_info = CSqMulti();
	while ( i < ( fps * 10 ) && 0 <= ( stream = vid_in.ReadFrame( frame, frame_info ) ) )
	{
		if ( vid_in.getVideoStream() == stream )
		{
//			_self.sleep( 1000 / fps );

			vid_out.WriteFrame( frame, frame_info );

			_self.print( "\r" + i++ );
			_self.flush();

		} // end if
		else
			_self.echo( "non video frame" );

	} // end while

	_self.echo( "\rsuccess : Frames written = " + i );
}

function test_avi_write( root, file )
{
	_self.echo( "" );
	_self.echo( "----------------------------------------------" );
	_self.echo( "-      AVI Write Test" );
	_self.echo( "----------------------------------------------" );

	local img = CSqImage();
	if ( !img.Load( _self.build_path( root, "_z_avi_0.jpg" ), "" ) )
	{	_self.echo( "failed to find avi input image" );
		return;
	} // end if

	local vid = CFfContainer();
	if ( !vid.Create( file, "", CSqMulti() ) )
	{	_self.echo( "failed to create file" );
		return;
	} // end if

	if ( 0 > vid.AddVideoStream( CFfEncoder().CODEC_ID_MPEG4, img.getWidth(), img.getHeight(), 30 ) )
	{	_self.echo( "failed to add video stream" );
		return;
	} // end if

	if ( !vid.InitWrite() )
	{	_self.echo( "failed to initialize file" );
		return;
	} // end if

	local enc = CFfEncoder();
	if ( !enc.Create( vid.getVideoCodecId(), CFfConvert().PIX_FMT_YUV420P,
					  img.getWidth(), img.getHeight(), 0 ) )
	{	_self.echo( "failed to create encoder" );
		return;
	} // end if

	local i = 0;
	local frame = CSqBinary();
	do
	{
		local nEnc = enc.EncodeImage( img, frame, CSqMulti() );
		if ( !nEnc )
		{	_self.echo( "Encode() failed" );
			return;
		} // end if

		if ( !vid.WriteFrame( frame, CSqMulti() ) )
		{	_self.echo( "failed to write frame to avi" );
			return;
		} // end if

		_self.print( "\r" + i );
		_self.flush();

	} while ( img.Load( _self.build_path( root, "_z_avi_" + ++i + ".jpg" ), "" ) );

	_self.echo( "\rsuccess : Frames written = " + i );
}

function test_avi_read( root, file )
{
	_self.echo( "" );
	_self.echo( "----------------------------------------------" );
	_self.echo( "-      AVI Read Test" );
	_self.echo( "----------------------------------------------" );

	local vid = CFfContainer();
	if ( !vid.Open( file, CSqMulti() ) )
	{	_self.echo( "failed to open file" );
		return;
	} // end if

	_self.echo( "Video File : " + vid.getWidth() + "x" + vid.getHeight() );

	local dec = CFfDecoder();
	if ( !dec.Create( vid.getVideoCodecId(), vid.getVideoFormat(),
					  vid.getWidth(), vid.getHeight(), 0 ) )
	{	_self.echo( "failed to create decoder" );
		return;
	} // end if

	local i = 0;
	local stream = -1;
	local frame = CSqBinary();
	while ( 0 <= ( stream = vid.ReadFrame( frame, CSqMulti() ) ) )
	{
		if ( vid.getVideoStream() == stream )
		{
			local img = CSqImage();
			if ( !dec.DecodeImage( frame, img, CFfConvert().SWS_FAST_BILINEAR ) )
			{	_self.echo( "failed to decode image" );
				return;
			} // end if

			// Save this frame
			img.Save( _self.build_path( root, "_z_avi_" + i++ + ".jpg" ), "" );

			_self.print( "\r" + i );
			_self.flush();

		} // end if

	} // end while

	_self.echo( "\rsuccess : Frames read " + i );

	return;
}

function run_test( root, name, img, fmt, cmp, cs, alg )
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

	CSqFile().put_contents_bin( _self.build_path( root, "_ffmpeg_" + name + ".raw" ), frame );

	cimg.Save( _self.build_path( root, "_ffmpeg_" + name + ".png" ), "" );
}


function test_encode( frame, img, fmt, cmp, cs, alg )
{
	_self.echo( "\nTesting encoder..." );

	local enc = CFfEncoder();
	if ( !enc.Create( fmt, cs, img.getWidth(), img.getHeight(), cmp ) )
	{	_self.echo( "failed to create encoder" );
		return;
	} // end if

	local nEnc = enc.EncodeImage( img, frame, CSqMulti() );
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
