// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqFftw, CSqFftw )

	SQBIND_MEMBER_FUNCTION( CSqFftw, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqFftw, Allocate )
	SQBIND_MEMBER_FUNCTION( CSqFftw, Plan )
	SQBIND_MEMBER_FUNCTION( CSqFftw, Execute )
	SQBIND_MEMBER_FUNCTION( CSqFftw, getInput )
	SQBIND_MEMBER_FUNCTION( CSqFftw, getOutput )
	SQBIND_MEMBER_FUNCTION( CSqFftw, getSize )
	SQBIND_MEMBER_FUNCTION( CSqFftw, getSamples )	

	SQBIND_MEMBER_FUNCTION( CSqFftw, setInput )	
	SQBIND_MEMBER_FUNCTION( CSqFftw, getInputReal )	
	SQBIND_MEMBER_FUNCTION( CSqFftw, getInputImag )	
	SQBIND_MEMBER_FUNCTION( CSqFftw, getInputMag )	
	SQBIND_MEMBER_FUNCTION( CSqFftw, getInputPhase )	
	
	SQBIND_MEMBER_FUNCTION( CSqFftw, setOutput )	
	SQBIND_MEMBER_FUNCTION( CSqFftw, getOutputReal )	
	SQBIND_MEMBER_FUNCTION( CSqFftw, getOutputImag )	
	SQBIND_MEMBER_FUNCTION( CSqFftw, getOutputMag )	
	SQBIND_MEMBER_FUNCTION( CSqFftw, getOutputPhase )	

SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_fftw( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqFftw );
}

#if defined( SQBIND_STATIC )
	#include "sq_fftw.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_fftw( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
