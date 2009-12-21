
_self.load_module( "poco", "" );

class CGlobal
{
};

local _g = CGlobal();

function _init() : ( _g )
{
	local vmime = CPoSmtp();

	_self.echo( "\n------ Starting ------\n" );

	_self.echo( "\n...done...\n" );
}

function _idle() : ( _g )
{
	return -1;
}

