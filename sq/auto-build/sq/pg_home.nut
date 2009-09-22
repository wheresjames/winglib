
_self.include( "config.nut" );
local _cfg = CConfig();

function pg_run( mParams ) : ( _cfg )
{
	// Get list of running build scripts
	local running_scripts = "<table>";
	local children = split( _self.get_children( "/build_mgr" ), "," );
	if ( children.len() )
		foreach( child in children )
			running_scripts += "<tr><td>" + child + "</td></tr>";
	else
		running_scripts += "<tr><td>None</td></tr>";
	running_scripts += "</table>";

	local content = @"
			<table width='95%'>
				<tr>
					<td>
						<b>Running Build Scripts</b>
					</td>
				</tr>
				<tr>
					<td>
						" + running_scripts + @"
					</td>
				</tr>
			</table>
		";

	return content;
}

