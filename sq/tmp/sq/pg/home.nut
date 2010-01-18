
_self.include( "config.nut" );

function pg_home( mParams )
{
/*
	// User wants to exit?
	if ( mParams[ "GET" ].isset( "exit" ) && mParams[ "GET" ][ "exit" ].len() )
	{
		// Ensure proper shutdown code
		if ( mParams[ "GET" ][ "exit" ].str() == _self.get( "/", "exit_code" ) )
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
					<a href='/home'>NO</a>
				</big></b>
			";

	} // end if
	
*/

	// Get list of running build scripts
/*
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
				</tr>
			</table>
		";
*/
	local content = "";

//  setTimeout( '$(\'#dlist\').load( \'/data\' );', 1000 );

	content += @"	
			<div id='dlist'><em>...loading...</em></div>
			<script type='text/javascript'>

			function DataCallback( data )
			{
//				alert( data );
				$('#dlist').value = data;
			}			

			function StartPolling()
			{
				$.ajax({
					type: 'GET',
					url: '/data',
					cache:false,
					global:false,
					data: '',
					success: function( data ) { DataCallback( data ); }
				});
			}

			setTimeout( 'StartPolling();', 1000 );

			</script>
		";

	return content;
}

