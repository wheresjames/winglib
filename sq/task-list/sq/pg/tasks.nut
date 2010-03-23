
_self.include_once( "config.nut" );

function raw_tasks( mParams )
{
	local db = CSqSQLite();
//	db.DebugMode( 1 );
	if ( !db.Open( _cfg( "db_name" ) ) )
	{	_self.echo( db.GetLastError() );
		return "";
	} // end if

	// Add?
	if ( mParams[ "GET" ].isset( "_add" ) && mParams[ "GET" ].isset( "_dat" ) )
	{
		local inf = CSqMulti();
		inf.deserialize( mParams[ "GET" ][ "_dat" ].str() );
//		_self.echo( inf.print_r( 1 ) );

		// Insert the user into the database
		if ( !db.Insert( "tasks", inf ) )
			_self.echo( db.GetLastError() );

	} // end if

	if ( !db.Exec( "SELECT * FROM tasks" ) )
	{	_self.echo( db.GetLastError() );
		return "";
	} // end if

//	_self.echo( db.Result().serialize() );

	return db.Result().serialize();

//	local tasks = db.Result();
//	return "1=hello,2=world";
}

function pg_tasks( mParams )
{
	local content = "";

	content +=
		@"
			<hr>

			<table width='95%' align='center'>
				<tr>
					<td>
						<b>Text</b>
					</td>
					<td style='width:100%'>
						<input id='title' type='text' style='width:100%'/>
					</td>
					<td>
						<button onclick='submitItem()'>Create</button>
					</td>
				<td>
			</table>

			<hr>

			<ul id='items'></ul>

			<script type='text/javascript'>

				function getLength( a )
				{	var l = 0; for( var i in a ) l++; return l; }

				var g_submit_items = {};
				function submitItem()
				{
					g_submit_items[ 'title' ] = document.getElementById( 'title' ).value;
					document.getElementById( 'title' ).value = '';
				}

				function addItem( id, arr ) 
				{
					g_current_items[ id ] = 1;
					$('#items').prepend( '<li style=\'display:none;list-style-type:none\' id=\'item' + id + '\'>' + arr[ 'title' ] + '</li>' );
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
						} // end if
						
					for ( var key in g_current_items )
						if ( !g_items[ key ] )
						{	removeItem( key );
						} // end if
				}

				setInterval( 'updateList();', 3000 );
				
				function DataCallback( data )
				{
					if ( data.length )
					{	g_items = {};
						ScsDeserialize( data, g_items );
					} // end if
	
					setTimeout( 'StartPolling();', 3000 );
				}			

				function StartPolling()
				{
					var getp = '_rid=0';
					if ( getLength( g_submit_items ) )
					{	getp += '&_add=1&_dat=' + escape( ScsSerialize( g_submit_items ) );
						g_submit_items = {};
					} // end if

					$.ajax({
						type: 'GET',
						url: '/tasks',
						cache:false,
						global:false,
						data: getp,
						success: function( data ) { DataCallback( data ); }
					});
				}

				setTimeout( 'StartPolling();', 1000 );
//				setInterval( 'StartPolling();', 3000 );

			</script>
		";

	return content;
}

