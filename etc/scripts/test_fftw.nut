
_self.load_module( "fftw", "" );

class CGlobal
{
};

local _g = CGlobal();

function _init() : ( _g )
{
	local vmime = CSqFftw();

	_self.echo( "\n------ Starting ------\n" );

	local size = 256;
	local fftw = CSqFftw();

	_self.echo( "... Creating a plan for a 256 sample 1D FFT ..." );
	fftw.Plan( size, 0, 0 );

	_self.echo( "... Filling the input buffer with a sine wave ..." );
	for ( local i = 0; i < size; i++ )
		fftw.setInput( i, 10. * cos( i * 0.1 ), 0. );

	_self.echo( "... Input Samples ..." );
	for ( local i = 0; i < size; i++ )
		_self.echo( i  
					+ ": Real : " + fftw.getInputReal( i ).tostring() 
					+ ", Imag : " + fftw.getInputImag( i ).tostring() 
					+ ", Mag  : " + fftw.getInputMag( i ).tostring()
					+ ", Phase: " + fftw.getInputPhase( i ).tostring()
					);

	_self.echo( "... Executing FFT ..." );
	fftw.Execute();	

	_self.echo( "... Output Samples ..." );
	for ( local i = 0; i < size; i++ )
		_self.echo( i 
					+ ": Real : " + fftw.getOutputReal( i ).tostring() 
					+ ", Imag : " + fftw.getOutputImag( i ).tostring() 
					+ ", Mag  : " + fftw.getOutputMag( i ).tostring()
					+ ", Phase: " + fftw.getOutputPhase( i ).tostring()
					);

	_self.echo( "... Now for a speed check ..." );

	// Sync to clock
	local t = _self.gmt_time();
	while ( t == _self.gmt_time() );

	// Run the test
	local l = 0;
	t = _self.gmt_time() + 10;
	while ( t > _self.gmt_time() )
		l++, fftw.Execute();

	_self.echo( "About " + l / 10 + " FFT's per second with " + size + " samples" );

	_self.echo( "\n...done...\n" );
}

function _idle() : ( _g )
{
	return -1;
}

