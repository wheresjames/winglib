
_self.include( "config.nut" );

function pg_run( mRequest, mHeaders, mGet, mPost, mSession )
{
	// User wants to exit?
	if ( mGet.isset( "exit" ) && mGet[ "exit" ].len() )
	{
		// Ensure proper shutdown code
		if ( mGet[ "exit" ] == _self.get( "/", "exit_code" ) )
			_self.terminate( -1 );

		// Set new code
		local id = _self.unique();		
		_self.set( "/", "exit_code", id );
		
		return @"
				<br>
				<b><big>
					Are you sure you want to shutdown?
					&nbsp;&nbsp;&nbsp;
					<a href='?exit=" + id + @"'>YES</a>
					&nbsp;&nbsp;&nbsp;
					<a href='home'>NO</a>
				</big></b>
			";

	} // end if

	// Get list of running build scripts
	local content = @"
			<table width='95%'>
				<tr>
					<td>
						<a href='?exit=1'>Shutdown</a>
					</td>
				</tr>
				<tr>
					<td>
						" + _cfg( "obj_divider" ) + @"
					</td>
				<td>
				<tr>
					<td>
						<b>Tasks</b>
					</td>
				</tr>
			</table>
		";

	return content;
}

