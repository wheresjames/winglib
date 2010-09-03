
_self.load_module( "usb", "" );

_self.echo( "\n--- USB Test --\n" );

// Initialize usb library
local usb = CSqUsb();			  
if ( 0 > usb.Init() )
{	_self.echo( "Init() failed" );
} // end if

// Show the list of usb devices
local list = usb.getDeviceList();
_self.echo( list.print_r( 1 ) );

_self.echo( "\n--- DONE --\n" );
