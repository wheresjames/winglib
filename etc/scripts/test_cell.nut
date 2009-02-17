
function _init()
{
	_self.load_module( "cell", "" );

	local tc = CCellConnection();

	if ( !tc.Connect( "172.17.2.20" ) )
		_self.alert( "Failed to connect to device" );

	_self.alert( tc.GetBackplaneData() );

}

function _idle()
{
	_self.alert( "Waiting..." );

	return 1;
}

