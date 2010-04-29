
_self.load_module( "ffmpeg", "" );

class CGlobal
{
	rec_avi = 0;
	rec_enc = 0;
	rec_frame = 0;
};

local _g = CGlobal();

function _init() : ( _g )
{
//	local dir = _self.root( "imgs" );
//	CSqFile().mkdir( dir );

	local file = "/media/System/C2_1272399000.jem";
	local file = "/media/System/IDK_1272399600.jem";

	local data = CSqFile().get_contents_bin( file, 0 );
	_self.echo( "File size = " + data.getUsed() );

	local frame = CSqBinary(), inf = CSqMulti();
	local i = 0, start = 0, end = 0, max = data.getUsed();

	local img = CSqImage();

	while ( 1 )
	{
		// Find beginning and end of image
		start = data.findUSHORT( 0xd8ff, end, 0 );
		end = data.findUSHORT( 0xd9ff, start, 0 );

		// Did we find an image?
		if ( start == data.failed() || end == data.failed() )
		{	_self.echo( "Extracted " + i + " images." );
			return -1;
		} // end if

		end += 2;
		_self.echo( i + " : start = " + start + ", end = " + end );

		if ( data.Sub( frame, start, end - start ) )
			if ( img.Decode( "jpg", frame ) )
			{
				if ( !_g.rec_enc )
					OpenAviFile( _self.root( "Vid_" + img.getWidth() + "x" + img.getHeight() + ".avi" ),
								 img.getWidth(), img.getHeight(), 15 );

				if ( _g.rec_enc )
				{
					if ( !_g.rec_enc.EncodeImage( img, _g.rec_frame, inf ) )
						_self.echo( "Failed to encode frame" );

					else if ( !_g.rec_avi.WriteFrame( _g.rec_frame, inf ) )
						_self.echo( "Failed to write to avi file" );

				} // end if


			} // end if


//		local fname = _self.build_path( dir, "Img_" + i + ".jpg" );
//			CSqFile().put_contents_bin( fname, img );

		i++;

	} // end while

}

function OpenAviFile( file, width, height, fps ) : ( _g )
{
	_g.rec_avi = CFfContainer();
	_g.rec_enc = CFfEncoder();
	_g.rec_frame = CSqBinary();

	if ( !_g.rec_avi.Create( file, "", CSqMulti() ) )
		_self.echo( "Failed to create avi" );
	else if ( 0 > _g.rec_avi.AddVideoStream( CFfDecoder().LookupCodecId( "MP42" ), width, height, fps ) )
		_self.echo( "Failed to add video stream" );
	else if ( !_g.rec_avi.InitWrite() )
		_self.echo( "Failed to initiailze avi" );

	else if ( !_g.rec_enc.Create( _g.rec_avi.getVideoCodecId(), CFfConvert().PIX_FMT_YUV420P,
								  width, height, 10, 800000, CSqMulti() ) )
		_self.echo( "Failed to create encoder" );

}

function _idle()
{

	return 1;
}

