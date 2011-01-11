
function WaitKey()
{	_self.echo( "\n...press any key...\n" );
	_self.get_key();
}

_self.echo( "\n... Initializing quickfix ...\n" );

// Load the quick fix module
if ( _self.load_module( "quickfix", "" ) )
{	_self.echo( "!!! Failed to load quickfix module" );
	WaitKey();
	return 0;
} // end if

local qf = CSqQuickfix();
if ( !qf.Init( "test.cfg" ) )
{	_self.echo( "!!! Failed to Initialize quickfix : " + qf.getLastError() );
	WaitKey();
	return 0;
} // end if

_self.echo( "\n... SUCCESS ...\n" );

WaitKey();


