
function _init()
{
	_self.alert( "sqmod_test_d.dll" );
	_self.load_module( "sqmod_test_d.dll", 0 );

//	local tc = CTestClass();
//	_self.alert( tc.Add( 2, 2 ).tostring() );

	// Spawn child
	_self.spawn( "child_0", "_self.alert( \"This is another thread\" );", 0 );

	// Spawn child
//	_self.spawn( "child_1", "function ShowMsg( msg ) { _self.execute1( \"..\", \"Talk\", msg ); }", 0 );

	// Send the child a message
//	_self.execute1( "child_1", "ShowMsg", "Thread #2" );

}

function _idle()
{
	_self.alert( "Waiting..." );

	return 1;
}

function Talk( msg )
{
    _self.alert( msg );

    _self.quit( 0 );
}
