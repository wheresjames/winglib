
class CGlobal
{
	port = CSqSerialPort();
	buf = CSqBinary();
};

local _g = CGlobal();

function _init() : ( _g )
{
	_self.echo( "\n===== Begin Serial Test =====\n" );

	_g.port.setBaudRate( 57600 );

	// Show port params
	_self.echo( "baud = " + _g.port.getBaudRate()
                + ", stop bits = " + _g.port.getStopBits()
                + ", parity = " + _g.port.getParity()
				);

	// Open first serial port
	if ( _g.port.Open( 0 ) )
		_self.echo( "Opened port : " + _g.port.getIndex() + " : " + _g.port.getName() );
	else
		_self.echo( "Failed to open port : " + _g.port.getIndex() + " : " + _g.port.getName() );

	_g.port.Write( "Hello World!\r\n" );

}

function _idle() : ( _g )
{
	// Attempt to read data
	if ( !_g.port.ReadBin( _g.buf, 1000 ) )
	{	_self.print( "." ), _self.flush();
		return 0;
	} // end if

	_self.echo( "Read : " + _g.buf.getUsed() );
	_self.echo( _g.buf.AsciiHexStr( 16, 32 ) );

//	_self.echo( "\n...done...\n" );

	return 0;
}

