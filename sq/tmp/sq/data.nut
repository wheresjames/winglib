


function ProcessData( mParams, loggedin )
{
	local mReply = CSqMap();
	local content = "Content Denied";

	mReply.set( "session", mParams[ "SESSION" ].serialize() );
	mReply.set( "content", content );
	return mReply.serialize();
}


