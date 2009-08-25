
_self.include( "config.nut" );

function pg_run( mRequest, mHeaders, mGet, mPost, mSession )
{
	local content = "";
	if ( mPost.isset( "new_user" ) )
	{
		content += "Adding User";

		mPost.unset( "new_user" );

		// Insert the user into the database
		local db = CSqSQLite();
		if ( db.Open( _cfg( "db_name" ) ) )
			if ( !db.Insert( "users", mPost ) )
				_self.echo( db.GetLastError() );

		// Show database contents
		if ( db.Exec( "SELECT * FROM users" ) )
			_self.echo( db.Result().print_r( 1 ) );

	} // end if
	
	content += "<pre>" + mPost.serialize() + "</pre>";

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

	return content;
}

