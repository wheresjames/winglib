
_self.load_module( "vmime", "" );

class CGlobal
{
};

local _g = CGlobal();

function _init() : ( _g )
{
	_self.echo( "\n------ Starting ------\n" );

	local vmime = CSqVMime();


	_self.echo( "\n...done...\n" );
}

function _idle() : ( _g )
{
	return -1;
}

