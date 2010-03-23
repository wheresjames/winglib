
local _g_cfg =
{
	// Server TCP port
	tcp_port	= 4405,

	// Databases
	db_name		= "tmp.db",
	db_tables	= {
					users =
					[	"CREATE TABLE users ( uid INTEGER NOT NULL, user CHAR(64), display CHAR(64), title CHAR(64), PRIMARY KEY(uid) )",
					]
				  },

	// Scheme colors
	col_bg		= "#ffffff",
	col_fg 		= "#000000",

	col_text	= "#202000",
	col_link 	= "#000000",
	col_vlink	= "#000000",
	col_slink	= "#000080",
	col_shade 	= "#80a0ff",
	col_dkline 	= "#000020",

//	col_text 	= "#800000",
//	col_link 	= "#ff0000",
//	col_shade 	= "#100000",
//	col_dkline 	= "#200000",

//	col_text 	= "#000080",
//	col_link 	= "#0000ff",
//	col_shade 	= "#000010",

	// Directory that holds the build scritps
	build_scripts_dir = _self.root( "build_scripts" ),

	// Cron jobs config file
	cron_cfg = _self.root( "cron.cfg" ),

	obj_divider		=
@"
	<div width='100%' style='border-bottom:solid 1px'>&nbsp;</div>
	<div width='100%'>&nbsp;</div>
",

	obj_border		= "border-left:solid 1px;border-top:solid 1px;border-right:solid 1px;border-bottom:solid 1px;",

	_last_ = 0
};

function _cfg( k ) : ( _g_cfg )
{	if ( k in _g_cfg ) return _g_cfg[ k ]; return ""; }
