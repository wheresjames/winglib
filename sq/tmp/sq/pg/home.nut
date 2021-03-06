
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
//	<div id='dlist'><em>...loading...</em></div>

	content += @"

			<ul id='items'></ul>

			<script type='text/javascript'>

				function addItem( id, str ) 
				{
					g_current_items[ id ] = 1;
					$('#items').prepend( '<li style=\'display:none\' id=\'item' + id + '\'>' + str + '</li>' );
     				$('#item' + id).fadeIn();
					$('#item' + id).click( function() { removeItem( id ); $(this).fadeOut(); } );
				}
							
				function removeItem( id ) 
				{
					g_current_items[ id ] = 0;
					$('#item' + id).fadeOut();
				}		

				var g_items = {};
				var g_current_items = {};
				function updateList()
				{
					for ( var key in g_items )
						if ( !g_current_items[ key ] )
						{	addItem( key, g_items[ key ] );
							return;
						} // end if
						
					for ( var key in g_current_items )
						if ( !g_items[ key ] )
						{	removeItem( key );
							return;
						} // end if
				}

				setInterval( 'updateList();', 3000 );

				function DataCallback( data )
				{
					g_items = {};
					ScsDeserialize( data, g_items );
	
					setTimeout( 'StartPolling();', 3000 );
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

