
// sq_fftw.h

#pragma once

class CSqFftw
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqFftw )
	_SQBIND_CLASS_CTOR_END( CSqFftw )

	/// Default constructor
	CSqFftw();

	/// Destructor
	~CSqFftw();

	/// Releases resources
	void Destroy();

	/// Allocate memory for fft
	int Allocate( int sz );

	/// Plans the fft
	int Plan( int samples, int reverse, int measure );

	/// Executes the fft
	int Execute();

	/// Returns the input buffer
	sqbind::CSqBinary getInput()
	{	return sqbind::CSqBinary( (sqbind::CSqBinary::t_byte*)m_in, m_size ); }

	/// Returns the output buffer
	sqbind::CSqBinary getOutput()
	{	return sqbind::CSqBinary( (sqbind::CSqBinary::t_byte*)m_out, m_size ); }

	/// Returns the size of the input/output buffers in bytes
	int getSize() { return m_size; }

	/// Returns the number of samples in the input/output buffers
	int getSamples() { return m_samples; }

	/// Sets an input value
	void setInput( int i, double real, double imag )
	{	getInput().setDOUBLE( i * 2, real );
		getInput().setDOUBLE( i * 2 + 1, imag );
	}

	/// Returns the output real component
	double getInputReal( int i ) { return getInput().getDOUBLE( i * 2 ); }

	/// Returns the output imaginary component
	double getInputImag( int i ) { return getInput().getDOUBLE( i * 2 + 1 ); }

	/// Gets the output magnitude
	double getInputMag( int i ) 
	{	double real = getInput().getDOUBLE( i * 2 );
		double imag = getInput().getDOUBLE( i * 2 + 1 );
		if ( !imag ) return real; if ( !real ) return imag;
		return sqrt( real * real + imag * imag );
	}

	/// Gets the output phase
	double getInputPhase( int i ) 
	{	double real = getInput().getDOUBLE( i * 2 );
		if ( !real ) return 0;
		double imag = getInput().getDOUBLE( i * 2 + 1 );
		if ( !imag ) return 0;
//		return atan( imag / real );
		return atan2( imag, real );
	}

	/// Sets an input value
	void setOutput( int i, double real, double imag )
	{	getOutput().setDOUBLE( i * 2, real );
		getOutput().setDOUBLE( i * 2 + 1, imag );
	}

	/// Returns the output real component
	double getOutputReal( int i ) { return getOutput().getDOUBLE( i * 2 ); }

	/// Returns the output imaginary component
	double getOutputImag( int i ) { return getOutput().getDOUBLE( i * 2 + 1 ); }

	/// Gets the output magnitude
	double getOutputMag( int i ) 
	{	double real = getOutput().getDOUBLE( i * 2 );
		double imag = getOutput().getDOUBLE( i * 2 + 1 );
		if ( !imag ) return real; if ( !real ) return imag;
		return sqrt( real * real + imag * imag );
	}

	/// Gets the output phase
	double getOutputPhase( int i ) 
	{	double real = getOutput().getDOUBLE( i * 2 );
		if ( !real ) return 0;
		double imag = getOutput().getDOUBLE( i * 2 + 1 );
		if ( !imag ) return 0;
//		return atan( imag / real );
		return atan2( imag, real );
	}

private:

	/// Non-zero if the plan is valid
	int						m_plan_valid;

	/// The fftw plan
	fftw_plan				m_plan;

	/// Size of the input / output buffer
	int						m_size;

	/// Number of samples to use
	int						m_samples;

	/// Input buffer
	fftw_complex*			m_in;

	/// Output buffer
	fftw_complex*			m_out;

};
SQBIND_DECLARE_INSTANCE( CSqFftw, CSqFftw );

