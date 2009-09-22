
_self.include( "config.nut" );
local _cfg = CConfig();

function pg_run( mParams ) : ( _cfg )
{
	// Ensure build scripts directory
	local cfg = CSqMulti();
	cfg.deserialize( CSqFile().get_contents( _cfg.cron_cfg ) );

	// Adding a build?
	if ( mParams[ "POST" ].isset( "name" ) 
		 && mParams[ "POST" ].isset( "times" )
		 && mParams[ "POST" ].isset( "jobs" ) )
	{	local id = _self.unique();
		cfg[ id ][ "name" ].set( _self.trimws( mParams[ "POST" ][ "name" ].str() ) );
		cfg[ id ][ "times" ].set( _self.trimws( mParams[ "POST" ][ "times" ].str() ) );
		cfg[ id ][ "jobs" ].set( _self.trimws( mParams[ "POST" ][ "jobs" ].str() ) );
		CSqFile().put_contents( _cfg.cron_cfg, cfg.serialize() );
		_self.echo( _cfg.cron_cfg );
	} // end if

	// Delete build?
	else if ( mParams[ "GET" ].isset( "del" ) )
	{	cfg.unset( mParams[ "GET" ][ "del" ].str() );
		CSqFile().put_contents( _cfg.cron_cfg, cfg.serialize() );
	} // end if

	else if ( mParams[ "GET" ].isset( "run" ) )
	{
		if ( cfg.isset( mParams[ "GET" ][ "run" ].str() ) )
			_self.execute2( 0, "/build_mgr", "Cron", mParams[ "GET" ][ "run" ].str(), "" );

	} // end if

	local content = @"
		<form action='/cron' method='post'>
			<table width='95%' cellspacing='0'>
				<tr>
					<td></td>
					<td>
						<nobr><b>Name</b></nobr>
					</td>
					<td>&nbsp;&nbsp;&nbsp;</td>
					<td>
						<nobr><b>Times (M,T,W,T,F,S,S)</b></nobr>
					</td>
					<td>&nbsp;&nbsp;&nbsp;</td>
					<td>
						<nobr><b>Jobs (comma separated)</b></nobr>
					</td>
					<td>
					</td>
				</tr>
		";

	content += @"
				<tr>
					<td></td>
					<td>
						<input name='name' id='name' type='text' style='width:150'>
					</td>
					<td></td>
					<td>
						<input name='times' id='times' type='text' style='width:100%'>
					</td>
					<td></td>
					<td width='100%'>
						<input name='jobs' id='jobs' type='text' style='width:100%'>
					</td>
					<td>
						<input type='submit' value='Add'>
					</td>
				</tr>
				<tr>
					<td colspan='99'>
						&nbsp;
					</td>
				</tr>
		";

	local row = 0;
	local records = "";
	foreach( k,v in cfg )
	{
		local rowcolor = ( row++ & 1 ) ? " bgcolor='" + _cfg.col_shade + "'" : "";

		records += @"
			<tr>
				<td" + rowcolor + @">
					[<a title='Delete' href='/cron?del=" + k + @"'>x</a>]
				</td>
				<td" + rowcolor + @">
					" + v.get( "name" ).str() + @"
				</td>
				<td" + rowcolor + @"></td>
				<td" + rowcolor + @">
					" + v.get( "times" ).str() + @"
				</td>
				<td" + rowcolor + @"></td>
				<td" + rowcolor + @">
					" + v.get( "jobs" ).str() + @"
				</td>
				<td" + rowcolor + @">
					[<b><a href='/cron?run=" + k + @"'>Run</a></b>]
				</td>
			</tr>
			";

	} // end foreach

	content += records;
	content += @"
				</table>
			</form>
		";

	return content;
}

