
function RegisterFileExtension( ext, cmd, id, name, exe )
{	_self.echo( " *** Associating " + ext + " extension with " + name );
	_self.reg_set_str( "HKCR", ext, "", id );
	_self.reg_set_str( "HKCR", id, "", name );
	_self.reg_set_str( "HKCR", id + "\\shell\\" + cmd + "\\command", "", exe + " \"%1\"" );
}

function UnregisterFileExtension( ext, id )
{	_self.echo( " *** Unassociating " + ext + " : " + id );
	_self.reg_del_key( "HKCR", ext, 1 );
	_self.reg_del_key( "HKCR", id, 1 );
}

//RegisterFileExtension( ".nut", "open", "WinglibEngine.Script", "Winglib Script", _self.module_name() );
UnregisterFileExtension( ".nut", "WinglibEngine.Script" );
