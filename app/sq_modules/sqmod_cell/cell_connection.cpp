
// cell_connection.cpp

#include "stdafx.h"

CCellConnection::CCellConnection()
{	oexZeroMemory( &m_comm, sizeof( m_comm ) );
	m_comm.error = -1;
}


int CCellConnection::Destroy()
{
	// Close the connection
	if ( m_comm.file_handle )
	{	closesocket( m_comm.file_handle );
		m_comm.file_handle = 0;
	} // end if

	m_comm.hostname = 0;
	m_comm.error = -1;
	m_sIp = "";

	return 1;
}

int CCellConnection::Connect( const sqbind::stdString &sIp, int nLoadTags )
{
	// Lose old connection
	Destroy();

	m_sIp = sIp;
	m_comm.hostname = (byte*)m_sIp.c_str();

	int m_nDebug = 0;
	establish_connection( &m_comm, &m_cServices.Obj(), m_nDebug );

	if ( m_comm.error != OK )
	{	SetLastError( oexT( "Unable to connect to device" ) );
		Destroy();
		return 0;
	} // end if

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
		SetLastError( oexT( "Failed to read backplane data" ) );

	// Find out who's active
	if ( who( &m_comm, &(_rack&)m_cRack, NULL, debug ) )
		SetLastError( oexT( "WHO failed" ) );

	m_path.device1 = 1;
	m_path.device2 = m_cRack.Obj().cpulocation;
	m_path.device3 = -1;
	m_path.device4 = -1;

	// Do we want to load the tags?
	if ( nLoadTags )
		LoadTags();

	return 1;
}

int CCellConnection::LoadTags()
{
	if ( get_object_config_list( &m_comm, &m_path, 0, &m_tagsConfig, 0 ) )
		SetLastError( oexT( "get_object_config_list() failed" ) );

	if ( get_object_details_list( &m_comm, &m_path, 0, &m_tagsDetails, 0 ) )
		SetLastError( oexT( "get_object_details_list() failed" ) );

	if ( get_program_list( &m_comm, &m_path, &m_prog_list, 0 ) )
		SetLastError( oexT( "get_program_list() failed" ) );

	int nTags = m_tagsDetails.count;
	if ( nTags < m_tagsConfig.count )
		nTags = m_tagsConfig.count;
	for ( int i = 0; i < nTags; i++ )
		if ( i < m_tagsDetails.count )
			if ( !get_object_details( &m_comm, &m_path, m_tagsDetails.tag[ i ], 0 ) )
				m_mapTags.set( (const char*)m_tagsDetails.tag[ i ]->name, oexMks( i ).Ptr() );

	return 1;
}

int CCellConnection::IsConnected()
{	return OK == m_comm.error ? 1 : 0; }

sqbind::stdString CCellConnection::ReadTag( const sqbind::stdString &sProgram, const sqbind::stdString &sTag )
{
	if ( m_comm.error != OK )
		return SetLastError( oexT( "ERR: Not connected" ) );

	if ( m_mapTags.size() && !m_mapTags.isset( sTag ) )
		return SetLastError( oexT( "ERR: Tag not found" ) );

	_tag_detail td;
	oexZeroMemory( &td, sizeof( td ) );
	if ( read_tag( &m_comm, &m_path, 0, (char*)sTag.c_str(), &td, 0 ) )
		return SetLastError( oexT( "ERR: read_tag() failed" ) );

	int nVal = 0;
	if ( 1 == td.datalen )
		nVal = *( (char*)td.data );
	if ( 2 == td.datalen )
		nVal = *( (short*)td.data );
	if ( 4 == td.datalen )
		nVal = *( (long*)td.data );

	if ( 0xC1 == ( td.type & 0xff ) )
		return nVal ? oexT( "true" ) : oexT( "false" );

	return oexFmt( oexT( "type:%04lX" ), td.type ).Ptr();
}

sqbind::stdString CCellConnection::GetBackplaneData()
{
	int debug = 0;

	if ( m_comm.error != OK )
		return SetLastError( oexT( "Not connected" ) );

	// Refresh tags
	LoadTags();

	oex::CStr ret;

	ret << oexNL << oexT( "--- Backplane Data ---" ) << oexNL;
	ret <<         oexT( "RX_Bad_m       : " )			<< m_cBackplane.Obj().rx_bad_m 			<< oexNL;
	ret <<         oexT( "ERR+Threshold  : " ) 			<< m_cBackplane.Obj().err_threshold 	<< oexNL;
	ret <<         oexT( "RX_Bad_CRC     : " )			<< m_cBackplane.Obj().rx_bad_crc 		<< oexNL;
	ret <<         oexT( "RX_Bus_Timeout : " ) 			<< m_cBackplane.Obj().rx_bus_timeout 	<< oexNL;
	ret <<         oexT( "TX_Retry       : " )			<< m_cBackplane.Obj().tx_retry 			<< oexNL;
	ret << oexFmt( oexT( "Status         : %X" ), 		   m_cBackplane.Obj().status )	 		<< oexNL;
	ret << oexFmt( oexT( "Address        : %04X" ), 	   m_cBackplane.Obj().address ) 		<< oexNL;
	ret <<         oexT( "Major Rev      : " ) 			<< m_cBackplane.Obj().rev_major 		<< oexNL;
	ret <<         oexT( "Minor Rev      : " ) 			<< m_cBackplane.Obj().rev_minor 		<< oexNL;
	ret << oexFmt( oexT( "Serial #       : %08lX" ),	   m_cBackplane.Obj().serial_number )	<< oexNL;
	ret <<         oexT( "Backplane size : " ) 			<< m_cBackplane.Obj().rack_size 		<< oexNL;
	ret << oexNL;

	ret << oexNL << oexT( "--- Rack Data ---" ) << oexNL;
	for ( int i = 0; i < m_cBackplane.Obj().rack_size; i++ )
	{
		ret << oexNL;
		ret <<		   oexT( "Rack        : " )			<< i << oexNL;
		ret <<		   oexT( "ID          : " )			<< m_cRack.Obj().identity[ i ]->ID						<< oexNL;
		ret <<		   oexT( "Type        : " )			<< ( ( 32 > m_cRack.Obj().identity[ i ]->type )
													     ? oexMks( PROFILE_Names[ m_cRack.Obj().identity[ i ]->type ] )
													     : oexMks( m_cRack.Obj().identity[ i ]->type ) ) << oexNL;
		ret <<		   oexT( "ProductCode : " )			<< m_cRack.Obj().identity[ i ]->product_code << oexNL;
		ret <<		   oexT( "Revision    : " )			<< m_cRack.Obj().identity[ i ]->rev_lo << oexNL;
		ret << oexFmt( oexT( "Status      : %X" ), 	       m_cRack.Obj().identity[ i ]->status )	<< oexNL;
		ret << oexFmt( oexT( "Serial #    : %08lX" ), 	   m_cRack.Obj().identity[ i ]->serial )	<< oexNL;
		ret << oexNL;

	} // end for

	if ( get_object_config_list( &m_comm, &m_path, 0, &m_tagsConfig, debug ) )
		ret << oexT( "get_object_config_list() failed" ) << oexNL;

	if ( get_object_details_list( &m_comm, &m_path, 0, &m_tagsDetails, debug ) )
		ret << oexT( "get_object_details_list() failed" ) << oexNL;

	if ( get_program_list( &m_comm, &m_path, &m_prog_list, debug ) )
		ret << oexT( "get_program_list() failed" ) << oexNL;

	int nTags = m_tagsDetails.count;
	if ( nTags < m_tagsConfig.count )
		nTags = m_tagsConfig.count;

	ret << oexNL << oexT( "--- Tags ---" ) << oexNL;
	ret << oexT( "Tags Found : " ) << nTags << oexNL;

	int c1 = 0, c68 = 0, c69 = 0;
	for ( int i = 0; i < nTags; i++ )
	{
		if ( i < m_tagsDetails.count )
		{
			if ( get_object_details( &m_comm, &m_path, m_tagsDetails.tag[ i ], debug ) )
				ret << oexT( "get_object_details() failed for tag " ) << i << oexNL;

			if ( i < m_tagsConfig.count )
				if ( get_object_config( &m_comm, &m_path, m_tagsConfig.tag[ i ], debug ) )
					ret << oexT( "get_object_config() failed for tag " ) << i << oexNL;

			switch ( m_tagsDetails.tag[ i ]->type & 255 )
			{
				case 0x68 :
					c68++;
					break;

				case 0x69 :
					c69++;
					break;

				default :

					if ( 0 > read_object_value( &m_comm, &m_path, m_tagsDetails.tag[ i ], debug ) )
						ret << oexT( "read_object_value() failed for tag " ) << i << oexNL;

					if ( i < m_tagsConfig.count && oexCHECK_PTR( m_tagsConfig.tag ) )
					{
						ret << oexNL << oexT( "--- Config ---" ) << oexNL;
						ret << oexFmt( oexT( "Top Base  : %08lX" ),	m_tagsDetails.tag[ i ]->topbase ) << oexNL;
						ret << oexFmt( oexT( "Base      : %08lX" ),	m_tagsDetails.tag[ i ]->base ) << oexNL;
						ret << oexFmt( oexT( "Link ID   : %08lX" ),	m_tagsDetails.tag[ i ]->linkid ) << oexNL;
						ret << oexFmt( oexT( "Type      : %04lX" ),	m_tagsDetails.tag[ i ]->type ) << oexNL;
					} // end if

					if ( i < m_tagsDetails.count && oexCHECK_PTR( m_tagsDetails.tag ) )
					{
						ret << oexNL << oexT( "--- Details ---" ) << oexNL;
						ret << oexFmt( oexT( "Top Base  : %08lX" ),	m_tagsDetails.tag[ i ]->topbase ) << oexNL;
						ret << oexFmt( oexT( "Base      : %08lX" ),	m_tagsDetails.tag[ i ]->base ) << oexNL;
						ret << oexFmt( oexT( "ID        : %08lX" ),	m_tagsDetails.tag[ i ]->id ) << oexNL;
						ret << oexFmt( oexT( "Link ID   : %08lX" ),	m_tagsDetails.tag[ i ]->linkid ) << oexNL;
						ret << oexFmt( oexT( "Type      : %04lX" ),	m_tagsDetails.tag[ i ]->type ) << oexNL;
						ret << oexFmt( oexT( "Size      : %04lX" ),	m_tagsDetails.tag[ i ]->size ) << oexNL;
						ret << oexFmt( oexT( "Memory    : %08lX" ),	m_tagsDetails.tag[ i ]->memory ) << oexNL;
						ret << oexFmt( oexT( "Display   : %04lX" ),	m_tagsDetails.tag[ i ]->displaytype ) << oexNL;
						ret <<         oexT( "Name      : " ) 		<< (const char*)m_tagsDetails.tag[ i ]->name << oexNL;
						ret <<         oexT( "ArraySize1: " ) 		<< (oex::oexUINT)m_tagsDetails.tag[ i ]->arraysize1 << oexNL;
						ret <<         oexT( "ArraySize2: " ) 		<< (oex::oexUINT)m_tagsDetails.tag[ i ]->arraysize2 << oexNL;
						ret <<         oexT( "ArraySize3: " ) 		<< (oex::oexUINT)m_tagsDetails.tag[ i ]->arraysize3 << oexNL;

						if ( m_tagsDetails.tag[ i ]->datalen )
							for ( int b = 0; b < m_tagsDetails.tag[ i ]->datalen; b++ )
							{	if ( b && !( b & 0x0f ) ) ret << oexNL;
								ret << oexFmt( oexT( "%02lX " ), m_tagsDetails.tag[ i ]->data[ b ] );
							} // end for

						ret << oexNL;

					} // end if

					break;

			} // end switch

		} // end if

	} // end for

	ret << oexNL << oexT( "--- Programs ---" ) << oexNL;
	ret << oexT( "Programs found : " ) << m_prog_list.count << oexNL;

	return ret.Ptr();
}

