
// cell_connection.cpp

#include "stdafx.h"

int CCellConnection::Destroy()
{
	// Close the connection
	if ( m_comm.file_handle )
	{	closesocket( m_comm.file_handle );
		m_comm.file_handle = 0;
	} // end if

	m_comm.hostname = 0;
	m_sIp = "";

	return 1;
}

int CCellConnection::Connect( const std::string &sIp )
{
	// Lose old connection
	Destroy();

	m_sIp = sIp;
	m_comm.hostname = (byte*)m_sIp.c_str();

	int m_nDebug = 0;
	establish_connection( &m_comm, &(_services&)m_cServices, m_nDebug );

	if ( m_comm.error != OK )
	{	SetLastError( oexT( "Unable to connect to device" ) );
		Destroy();
		return 0;
	} // end if

	return 1;
}

std::string CCellConnection::GetBackplaneData()
{
	if ( m_comm.error != OK )
		return SetLastError( oexT( "Not connected" ) );

	m_path.device1 = -1;
	m_path.device2 = -1;
	m_path.device3 = -1;
	m_path.device4 = -1;
	m_path.device5 = -1;
	m_path.device6 = -1;
	m_path.device7 = -1;
	m_path.device8 = -1;

	int debug = 0;
	if ( get_backplane_data( &m_comm, &(_backplane_data&)m_cBackplane,
						     &(_rack&)m_cRack, &m_path, debug ) )
		return SetLastError( oexT( "Failed to read backplane data" ) );

	oex::CStr ret;

	ret << oexT( "RX_Bad_m : " ) 						<< m_cBackplane.Obj().rx_bad_m 			<< oexNL;
	ret << oexT( "ERR+Threshold : " ) 					<< m_cBackplane.Obj().err_threshold 	<< oexNL;
	ret << oexT( "RX_Bad_CRC : " ) 						<< m_cBackplane.Obj().rx_bad_crc 		<< oexNL;
	ret << oexT( "RX_Bus_Timeout : " ) 					<< m_cBackplane.Obj().rx_bus_timeout 	<< oexNL;
	ret << oexT( "TX_Retry : " ) 						<< m_cBackplane.Obj().tx_retry 			<< oexNL;
	ret << oexFmt( oexT( "Status : %X" ), 				   m_cBackplane.Obj().status )	 		<< oexNL;
	ret << oexFmt( oexT( "Address : %04X" ), 			   m_cBackplane.Obj().address ) 		<< oexNL;
	ret << oexT( "Major Rev : " ) 						<< m_cBackplane.Obj().rev_major 		<< oexNL;
	ret << oexT( "Minor Rev : " ) 						<< m_cBackplane.Obj().rev_minor 		<< oexNL;
	ret << oexFmt( oexT( "Serial # : %08lX" ), 			   m_cBackplane.Obj().serial_number )	<< oexNL;
	ret << oexT( "Backplane size : " ) 					<< m_cBackplane.Obj().rack_size 		<< oexNL;
	ret << oexNL;

	// Find out who's active
	if ( who( &m_comm, &(_rack&)m_cRack, NULL, debug ) )
		return SetLastError( oexT( "WHO faild" ) );

	for ( int i = 0; i < m_cBackplane.Obj().rack_size; i++ )
	{
		ret << oexNL << oexT( "Rack : " ) 			<< i << oexNL;
		ret << oexT( "ID : " ) 						<< m_cRack.Obj().identity[ i ]->ID						<< oexNL;
		ret << oexT( "Type : " )					<< ( ( 32 > m_cRack.Obj().identity[ i ]->type )
													     ? oexMks( PROFILE_Names[ m_cRack.Obj().identity[ i ]->type ] )
													     : oexMks( m_cRack.Obj().identity[ i ]->type ) ) << oexNL;
		ret << oexT( "ProductCode : " )				<< m_cRack.Obj().identity[ i ]->product_code << oexNL;
		ret << oexT( "Revision : " )				<< m_cRack.Obj().identity[ i ]->rev_lo << oexNL;
		ret << oexFmt( oexT( "Status : %X" ), 	       m_cRack.Obj().identity[ i ]->status )	<< oexNL;
		ret << oexFmt( oexT( "Serial # : %08lX" ), 	   m_cRack.Obj().identity[ i ]->serial )	<< oexNL;

	} // end for

	return ret.Ptr();
}

