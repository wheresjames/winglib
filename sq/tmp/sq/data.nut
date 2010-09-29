


function ProcessData( mParams, loggedin )
{
	local mReply = CSqMap();
	local content = "Content Denied";

	mReply[ "session" ] <- mParams[ "SESSION" ].serialize();
	mReply[ "content" ] <- content;
	return mReply.serialize();
}


