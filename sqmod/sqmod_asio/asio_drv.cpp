
#include "stdafx.h"

#include "asiolist.h"


// CRtmpdServer
SQBIND_REGISTER_CLASS_BEGIN( CAsioDrv, CAsioDrv )

	SQBIND_MEMBER_FUNCTION( CAsioDrv, Destroy )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, setLastErrorStr )
	SQBIND_MEMBER_FUNCTION( CAsioDrv, getDevices )
//	SQBIND_MEMBER_FUNCTION( CAsioDrv,  )
//	SQBIND_MEMBER_FUNCTION( CAsioDrv,  )

SQBIND_REGISTER_CLASS_END()
DECLARE_INSTANCE_TYPE( CAsioDrv );

void CAsioDrv::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CAsioDrv );
}

CAsioDrv::CAsioDrv()
{_STT();

}

void CAsioDrv::Destroy()
{_STT();

}

#define MAX_ASIO_DRIVERS 256
#define MAX_ASIO_DRIVER_NAME_LEN 256
sqbind::CSqMulti CAsioDrv::getDevices()
{_STT();

	AsioDriverList adl;
	char name[ MAX_ASIO_DRIVER_NAME_LEN ];

	// How many drivers does the system think it has?
	long nDrivers = adl.asioGetNumDev();
	if ( 0 >= nDrivers )
		return sqbind::CSqMulti();
	else if ( MAX_ASIO_DRIVERS < nDrivers )
		nDrivers = MAX_ASIO_DRIVERS;

	// Get each driver's info
	sqbind::CSqMulti m;
	for( long i = 0; i < nDrivers; i++ )
		if ( !adl.asioGetDriverName( i, name, MAX_ASIO_DRIVER_NAME_LEN - 1 ) )
			name[ MAX_ASIO_DRIVER_NAME_LEN - 1 ] = 0, 
			m[ sqbind::oex2std( oexMks( i ) ) ][ oexT( "name" ) ].set( name );
		else
			i = nDrivers;

	return m;
}


