// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqFftw, CSqFftw )
	SQBIND_MEMBER_FUNCTION( CSqFftw, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqFftw, Plan )
	SQBIND_MEMBER_FUNCTION( CSqFftw, Execute )
SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_fftw( sqbind::VM x_vm )
{
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqFftw );
}

#if defined( SQBIND_STATIC )
	#include "sq_fftw.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{	SQBIND_Export_fftw( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
