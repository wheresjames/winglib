
_self.include( "config.nut" );

function pg_run( mParams )
{
	local db = CSqSQLite();
//	db.DebugMode( 1 );
	if ( !db.Open( _cfg( "db_name" ) ) )
	{	self.error( db.GetLastError() );
		return "Database unavailable";
	} // end if

	local content = "";

	// Delete user if needed
	if ( mParams[ "GET" ].isset( "del" ) )
		db.Exec( "DELETE FROM users WHERE `uid`='" + mParams[ "GET" ][ "del" ].str() + "'" );

	if ( mParams[ "POST" ].isset( "new_user" ) )
	{
		mParams[ "POST" ].unset( "new_user" );

		// Insert the user into the database
		if ( !db.Insert( "users", mParams[ "POST" ] ) )
			_self.error( db.GetLastError() );

	} // end if

	// Show database contents
	if ( !db.Exec( "SELECT * FROM users ORDER BY user" ) )
		_self.error( 0, db.GetLastError() );

	local users = db.Result();
	local user_list = @"<table align='center' width='80%'>
							<tr>
								<td>
								</td>
							<td>
								<b>Username</b>
							</td>
							<td>
								<b>Display</b>
							</td>
							<td>
								<b>Title</b>
							</td>
						</tr>";

	foreach( k,v in users )
	{
		user_list += "<tr><td>[<a href='?del=" + v[ "uid" ].str() + "'>X</a>]"
					 + "</td><td>" + v[ "user" ].str()
					 + "</td><td>" + v[ "display" ].str()
					 + "</td><td>" + v[ "title" ].str()
					 + "</td></tr>";

	} // end foreach
	user_list += "</table>";

//	content += "<pre>" + mParams[ "POST" ].serialize() + "</pre>";

	// Get list of running build scripts
	content += _cfg( "obj_divider" ) + @"
		<form method='POST'>
			<input type='hidden' id='new_user' name='new_user' value=''>
			<table width='95%' align='center'>
				<tr>
					<td width='50%'>
					</td>
					<td width='50%'>
					</td>
				<td>
				<tr>
					<td>
						<b>Username</b> (letters and numbers only)
					</td>
					<td>
						<b>Display Name</b> (Bob Smith)
					</td>
				</tr>
				<tr>
					<td>
						<input id='user' name='user' type='text' style='width:100%'>
					</td>
					<td>
						<input id='display' name='display' type='text' style='width:100%'>
					</td>
				</tr>
				<tr>
					<td colspan='99'>
						<b>Title</b>
					</td>
				</tr>
				<tr>
					<td>
						<input id='title' name='title' type='text' style='width:100%'>
					</td>
				</tr>
				<tr>
					<td colspan='99'>
						<input type='submit' value='Create User'>
					</td>
				<td>
			</table>
		</form>
		";

	content += _cfg( "obj_divider" ) + user_list;

	return content;
}

