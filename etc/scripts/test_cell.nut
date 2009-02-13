
function _init()
{
	_self.load_module( "sqmod_cell_d.dll", 0 );

	local tc = CCellConnection();

	if ( !tc.Connect( "127.0.0.1" ) )
		_self.alert( "Unable to connect" );


//	local tc = CTestClass();
//	_self.alert( tc.Add( 2, 2 ).tostring() );



}



function _idle()
{
	_self.alert( "Waiting..." );

	return 1;
}

