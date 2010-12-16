
class CGlobal
{
	file = "";
	raw = "";
	port = CSqSerialPort();
	buf = CSqBinary();
};

local _g = CGlobal();

function _init() : ( _g )
{
	local port = "COM1";
	local dir = "c:/serial_data";

	_self.echo( "\n===== Begin Serial Test =====\n" );

	// Where to put stuff
	CSqFile().mkdir( dir );

	local t = CSqTime(); t.GetSystemTime();
	_g.raw = dir + "/" + port + "_" + t.FormatTime( "%Y%c%d_%g%m%s" ) + ".raw";
	_g.file = dir + "/" + port + "_" + t.FormatTime( "%Y%c%d_%g%m%s" ) + ".txt";

	_g.port.setBaudRate( 9600 );
//	_g.port.setBaudRate( 115200 );

	// Show port params
	_self.echo( "baud = " + _g.port.getBaudRate()
                + ", stop bits = " + _g.port.getStopBits()
                + ", parity = " + _g.port.getParity()
				);

	// Open first serial port
	if ( _g.port.OpenNamed( port ) )
		_self.echo( "Opened port : " + _g.port.getIndex() + " : " + _g.port.getName() );
	else
		_self.echo( "Failed to open port : " + _g.port.getIndex() + " : " + _g.port.getName() );

//	_g.port.Write( "Hello World!\r\n" );

}

function _idle() : ( _g )
{
	// Attempt to read data
	if ( !_g.port.ReadBin( _g.buf, 1000 ) )
		return 0;

	// Append data to file
	local t = CSqTime(); t.GetLocalTime();
	CSqFile().append_contents_bin( _g.raw, _g.buf );
	CSqFile().append_contents( _g.file,	  "Time : " + t.FormatTime( "%Y/%c/%d %g:%m:%s" ) + " - "
										+ "Read : " + _g.buf.getUsed() + "\r\n"
										+ _g.buf.AsciiHexStr( 16, 1000 ) + "\r\n\r\n" );

//	_self.echo( "Read : " + _g.buf.getUsed() );
//	_self.echo( _g.buf.AsciiHexStr( 16, 32 ) );
	
//	_self.echo( "\n...done...\n" );

	return 0;
}

