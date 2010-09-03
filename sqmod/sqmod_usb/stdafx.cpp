// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CSqUsb, CSqUsb )

	SQBIND_MEMBER_FUNCTION( CSqUsb, Init )
	SQBIND_MEMBER_FUNCTION( CSqUsb, Destroy )
	SQBIND_MEMBER_FUNCTION( CSqUsb, getDeviceList )
	
SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CSqUsb );

// Export classes
static void SQBIND_Export_usb( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CSqUsb );
}

#if defined( SQBIND_STATIC )
	#include "sq_usb.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_usb( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif

