
class CGlobal
{
	quit = 0;
	server = 0;
	ssl = 0;
};

local _g = CGlobal();

function StartVideoStream( mParams, mReply )
{
	local p = mParams[ "GET" ];
	if ( !p[ "type" ].len() )
		return 0;
	
	switch( p[ "type" ].str().tolower() )
	{
		case "mjpg" :
		case "mjpeg" :
			p.unset( "type" );
			p[ "codec" ] <- "MJPG";
			mReply[ "multi" ] <- "image/jpeg";
			break;
		
		default:
			mReply[ "mime" ] <- "video/" + p[ "type" ].str();
			break;
		
	} // end switch			

	// Valid?
	if ( !p.size() )
		return 0;
	
	// Generate a stream id
	local sid = _self.unique();
	
	// Start the streaming thread
	p[ "sid" ] <- sid;
	::_self.spawn( 1, ".", sid, "test_videostream.thread.nut", 1 );
	::_self.execute1( 0, sid, "StartStream", p.serialize() );

	// Stream shared buffer back to client
	mReply[ "share" ] <- sid;
	mReply[ "cancel_share" ] <- "1";
	
	return 1;
}

function OnProcessRequest( params )
{
	local mParams = CSqMulti(), mReply = CSqMulti();

	mParams.deserialize( params );
	_self.echo( mParams[ "REQUEST" ][ "REMOTE_ADDR" ].str() + " : " + mParams[ "REQUEST" ][ "REQUEST_STRING" ].str() );

	// What to do...
	switch( mParams[ "REQUEST" ][ "path" ].str() )
	{
		case "/video" :
		
			// Start the video
			StartVideoStream( mParams, mReply );

			break;
			
		default :
			
			mReply[ "content" ] <- @"<!DOCTYPE html>
					<html>
						<style type='text/css'>
							.vid { float : left; margin : 25px; }
						</style>
						<body>
							<div class='vid'>
								<h1>MJPEG</h1>
								<img src='/video?type=mjpeg&cap=1' />
							</div>
							<div class='vid'>
								<h1>HTML5</h1>
								<video src='/video?type=webm&codec=VP80' controls autoplay>
									Your browser does not support HTML5 video
								</video>
							</div>
						</body>
					</html>";
			break;
	
	} // end switch

	return mReply.serialize();
}

function _init() : ( _g )
{
	local port = 1234;

	_g.server = CSqHttpServer();

	// Must be multi threaded for this to work
	_g.server.EnableMultiThreading( 1 );

	// New HTTP session callback
	_g.server.SetSessionCallback( _self.queue(), "OnProcessRequest" );

	// Start the server
	if ( !_g.server.Start( port ) )
		_g.quit = 1, _self.alert( "Unable to start http server" );
		
	// Let everyone know
	else _self.echo( "Running web server at http://localhost:" + port + "/" );
}

function _idle() : ( _g )
{
//	_self.alert( "Waiting..." );

	return _g.quit;
}

function _exit() : ( _g )
{
	// Not strictly needed
	_g.server.Stop();
}

