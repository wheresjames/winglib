
// _self.echo( _self.get_system_drive_info( _self.get_sys_folder( 0, "default_drive" ) ).print_r( 1 ) );

function row( a, b, c, d )
{	_self.echo( format( "%-20s %7s %7s  %s", a, b, c, d ) );
}

function show_folder( name, path )
{	row( name, 
		 CSqFile().get_dirlist( path, "*", 1, 0 ).size().tostring(), 
		 CSqFile().get_dirlist( path, "*", 0, 1 ).size().tostring(), 
		 path );
}

_self.echo( "\n" );
_self.echo( "\n------------------------------------------------------" );
row( "Description", "Files", "Folders", "Path" );

local specific_folders = 
{
	[ "temp" ]					= "Temporary",
	[ "current" ]				= "Current",
	[ "root" ]					= "Root",
	[ "default_drive" ]			= "Default Drive",
	[ "sys" ]					= "System",
	[ "os" ]					= "Operating System",
	[ "fonts" ]					= "Fonts",
	[ "recycle" ]				= "Recycle",
	[ "cookies" ]				= "Cookies",
	[ "network" ]				= "Network",
	[ "printers" ]				= "Printers",
	[ "recent" ]				= "Recent",
	[ "history" ]				= "History"
}

_self.echo( "\n------------------- SPECIFIC PATHS -------------------" );
foreach( k,v in specific_folders )
	show_folder( v, _self.get_sys_folder( 0, k ) );

// These have both shared and non shared versions
local special_folders = 
{
	[ "settings" ] 		= "Settings",
	[ "desktop" ] 		= "Desktop",
	[ "downloads" ] 	= "Downloads",
	[ "templates" ] 	= "Templates",
	[ "public" ] 		= "Public",
	[ "documents" ] 	= "Documents",
	[ "music" ] 		= "Music",
	[ "pictures" ] 		= "Pictures",
	[ "video" ] 		= "Video",
	[ "start_menu" ] 	= "Start Menu",
	[ "startup" ] 		= "Startup"
}

_self.echo( "\n------------------- SHARED PATHS -------------------" );
foreach( k,v in special_folders )
	show_folder( v, _self.get_sys_folder( 1, k ) );

_self.echo( "\n------------------- USER PATHS -------------------" );
foreach( k,v in special_folders )
	show_folder( v, _self.get_sys_folder( 0, k ) );

_self.echo( "\n...press any key...\n" );
_self.get_key();

