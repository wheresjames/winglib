
_self.load_module( "portaudio", "" );

function _init()
{
	local pa = CPaOutput();

	_self.echo( "=====================================================" );
	_self.echo( "Total Devices         : " + pa.getDeviceCount() );
	_self.echo( "Default output device : " + pa.getDefaultOutputDevice() );
	_self.echo( "=====================================================" );

	for ( local i = 0; i < pa.getDeviceCount(); i++ )
	{
		local inf = CSqMulti();
		if ( pa.getDeviceInfo( i, inf ) )
			::_self.echo( "---------------------------------------" ),
			::_self.echo( i + " : " + inf[ "name" ].str() ),
			::_self.echo( "---------------------------------------" ),
			::_self.echo( inf.print_r( 1 ) );

	} // end for

	_self.echo( "=====================================================" );
	
	TestOutput( pa );

	_self.echo( "=====================================================" );

	_self.echo( "\n...press any key...\n" );
	_self.get_key();
	return 0;
}

function TestOutput( pa )
{
	local hz = 440;
	local sps = 44100.;
	local bsize = ( sps / hz * 200 ).tointeger();

	_self.echo( "*** Opening output device : " + pa.getDefaultOutputDevice() );

	if ( !pa.Open( 1, pa.getDefaultOutputDevice(), 1, 
				   CPaOutput().paFloat32, 0.2, sps, bsize ) )
	{   _self.echo( "!!! Failed to open output stream : " + pa.getLastError() );
		return 0;
	} // end if

	_self.echo( "*** Creating sine wave" );

	local sw = CSqBinary();
	if ( !sw.AllocateFLOAT( bsize ) )
	{   _self.echo( "!!! Failed to allocate memory for audio waveform" );
		return 0;
	} // end if

	// Create sine wave
	sw.setUsed( bsize * sw.sizeFLOAT() );
	local div = sps / hz, pi2 = 3.14159 * 2;
	for( local i = 0; i < bsize; i++ )
		sw.setFLOAT( i, sin( i / div * pi2 ) );

	_self.echo( "*** Sending audio" );
	
	if ( !pa.Start() )
	{   _self.echo( "!!! Failed to start output stream : " + pa.getLastError() );
		return 0;
	} // end if

	for( local i = 0; i < 10; i++ )
		if ( !pa.Write( sw, bsize ) )
		{   _self.echo( "!!! Failed to write audio data to stream : " + pa.getLastError() );
			return 0;
		} // end if

	if ( !pa.Stop() )
	{   _self.echo( "!!! Failed to stop output stream : " + pa.getLastError() );
		return 0;
	} // end if

	_self.echo( "*** Output test successfull" );

	return 1;
}

