// pa_output.cpp

#include "stdafx.h"


CSqUsb::CSqUsb()
{_STT();

	m_nInit = -1;

}

CSqUsb::~CSqUsb()
{_STT();
	Destroy();
}

void CSqUsb::Destroy()
{_STT();

//	if ( 0 <= m_nInit )
//		libusb_exit( 0 ),
//		m_nInit = -1;
}

int CSqUsb::Init()
{_STT();

	// Initialize USB library
	if ( m_nInit < 0 )
		m_nInit = libusb_init( 0 );

	return m_nInit;
}

sqbind::CSqMulti CSqUsb::getDeviceList()
{_STT();

	if ( 0 > m_nInit )
		return sqbind::CSqMulti();

	// Get USB device list
	libusb_device **devs = 0;
	ssize_t cnt = libusb_get_device_list( 0, &devs );
	if ( !devs )
		return sqbind::CSqMulti();

	int i;
	sqbind::CSqMulti mb;
	for ( int i = 0; devs[ i ]; i++ )
	{
		struct libusb_device_descriptor desc;
		if ( 0 <= libusb_get_device_descriptor( devs[ i ], &desc ) )
		{
			sqbind::stdString k = sqbind::oex2std( oexFmt( oexT( "%04x:%04x" ), desc.idVendor, desc.idProduct ) );
			mb[ k ][ oexT( "vendor_id" ) ].set( sqbind::oex2std( oexMks( desc.idVendor ) ) );
			mb[ k ][ oexT( "product_id" ) ].set( sqbind::oex2std( oexMks( desc.idProduct ) ) );
			mb[ k ][ oexT( "bus_number" ) ].set( sqbind::oex2std( oexMks( libusb_get_bus_number( devs[ i ] ) ) ) );
			mb[ k ][ oexT( "dev_addr" ) ].set( sqbind::oex2std( oexMks( libusb_get_device_address( devs[ i ] ) ) ) );
			mb[ k ][ oexT( "descriptor_type" ) ].set( sqbind::oex2std( oexMks( desc.bDescriptorType ) ) );
			mb[ k ][ oexT( "bcd_usb" ) ].set( sqbind::oex2std( oexMks( desc.bcdUSB ) ) );
			mb[ k ][ oexT( "device_class" ) ].set( sqbind::oex2std( oexMks( desc.bDeviceClass ) ) );
			mb[ k ][ oexT( "device_sub_class" ) ].set( sqbind::oex2std( oexMks( desc.bDeviceSubClass ) ) );
			mb[ k ][ oexT( "device_protocol" ) ].set( sqbind::oex2std( oexMks( desc.bDeviceProtocol ) ) );
			mb[ k ][ oexT( "max_packet_size_0" ) ].set( sqbind::oex2std( oexMks( desc.bMaxPacketSize0 ) ) );
			mb[ k ][ oexT( "manufacturer_id" ) ].set( sqbind::oex2std( oexMks( desc.iManufacturer ) ) );
			mb[ k ][ oexT( "serial_number" ) ].set( sqbind::oex2std( oexMks( desc.iSerialNumber ) ) );
			mb[ k ][ oexT( "num_configurations" ) ].set( sqbind::oex2std( oexMks( desc.bNumConfigurations ) ) );

		} // end if

	} // end while

	libusb_free_device_list( devs, 1 );

	return mb;
}

