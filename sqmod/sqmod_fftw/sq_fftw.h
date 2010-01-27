
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

	/// Plans the fft
	int Plan( sqbind::CSqBinary *in, sqbind::CSqBinary *out, int reverse, int measure );

	/// Executes the fft
	int Execute();

private:

	/// Non-zero if the plan is valid
	int						m_plan_valid;

	/// The fftw plan
	fftw_plan				m_plan;

	/// Input buffer
	sqbind::CSqBinary		m_in;

	/// Output buffer
	sqbind::CSqBinary		m_out;

};
SQBIND_DECLARE_INSTANCE( CSqFftw, CSqFftw );

