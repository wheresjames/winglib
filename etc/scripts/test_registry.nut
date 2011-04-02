
function WaitKey()
{	_self.echo( "\n...press any key...\n" );
	_self.get_key();
}

_self.echo( "\n=== BEGIN ===\n" );

local rkey = "SOFTWARE\\Microsoft\\Internet Explorer";
_self.echo( "REG: " + rkey + ":Version = " + _self.reg_get_str( "HKLM", rkey, "Version" ) );

rkey = "SOFTWARE\\WinglibScriptEngine_x86";
_self.echo( "REG: " + rkey + ":Install_Dir = " + _self.reg_get_str( "HKLM", rkey, "Install_Dir" ) );

rkey = "SOFTWARE\\WinglibScriptEngine_x64";
_self.echo( "REG: " + rkey + ":Install_Dir = " + _self.reg_get_str( "HKLM", rkey, "Install_Dir" ) );

_self.echo( "\n=== DONE ===\n" );

WaitKey();
	

