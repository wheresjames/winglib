
_self.load_module( "asio", "" );

local asio = CAsioDrv();

// Show a list of available devices
_self.echo( " --- ASIO DEVICES ---\n" + asio.getDevices().getJSON() );