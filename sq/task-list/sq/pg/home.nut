
_self.include( "config.nut" );

function pg_run( mRequest, mHeaders, mGet, mPost, mSession )
{
	// Get list of running build scripts

	local content = @"
			<table width='95%'>
				<tr>
					<td>
						<b>Tasks</b>
					</td>
				</tr>
				<tr>
					<td>
					</td>
				</tr>
			</table>
		";

	return content;
}

