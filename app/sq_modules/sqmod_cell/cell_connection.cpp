
// cell_connection.cpp

#include "stdafx.h"

CCellConnection::CCellConnection()
{	oexZeroMemory( &m_comm, sizeof( m_comm ) );
	oexZeroMemory( &m_tagsDetails, sizeof( m_tagsDetails ) );
	m_comm.error = -1;
}

CCellConnection::CCellConnection( const CCellConnection &r )
{	CCellConnection();
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

	// Lose the tag database
	ReleaseTags();

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
	{	SetLastError( oexT( "Failed to read backplane data" ) );
		return 0;
	} // end if

	// Find out who's active
	if ( who( &m_comm, &(_rack&)m_cRack, NULL, debug ) )
	{	SetLastError( oexT( "WHO failed" ) );
		return 0;
	} // end if

	m_path.device1 = 1;
	m_path.device2 = m_cRack.Obj().cpulocation;
	m_path.device3 = -1;
	m_path.device4 = -1;

	// Do we want to load the tags?
	if ( nLoadTags )
		LoadTags();

	return 1;
}

int CCellConnection::ReleaseTags()
{
	// Lose the tag map
	m_mapTags.clear();

	// Lose the details tags
	for ( int i = 0; i < m_tagsDetails.count; i++ )
		if ( oexCHECK_PTR( m_tagsDetails.tag[ i ] ) )
			free( m_tagsDetails.tag[ i ] ),
			m_tagsDetails.tag[ i ] = 0;

	return 1;
}

int CCellConnection::LoadTags()
{
	int nStatus = 1;

	// Lose old tags
	ReleaseTags();

	// Get device tags
	if ( get_object_details_list( &m_comm, &m_path, 0, &m_tagsDetails, 0 ) )
		nStatus = 0, SetLastError( oexT( "get_object_details_list() failed" ) );
	else
		for ( int i = 0; i < m_tagsDetails.count; i++ )
			if ( !get_object_details( &m_comm, &m_path, m_tagsDetails.tag[ i ], 0 ) )
			{	m_mapTags[ (const char*)m_tagsDetails.tag[ i ]->name ] = m_tagsDetails.tag[ i ];
				m_mapSqTags.set( (const char*)m_tagsDetails.tag[ i ]->name, oexMks( i ).Ptr() );
			} // end if

	// Get program tags
	if ( get_program_list( &m_comm, &m_path, &m_prog_list, 0 ) )
		nStatus = 0, SetLastError( oexT( "get_program_list() failed" ) );

	else
	{
		// Sanity limit
		int nProgs = m_prog_list.count;
		if ( 0 < eMaxProgs && eMaxProgs < nProgs )
			nProgs = eMaxProgs;

		// Allocate memory for program tags
		if ( !m_tagsProgram.OexNew( nProgs ).Ptr() )
		{	SetLastError( oexT( "Out of Memory!" ) ); return 0; }

		// Initialize the memory
		m_tagsProgram.Zero();

		// for each program tag
		for ( int i = 0; i < nProgs; i++ )
		{
			// Get the program details
			if ( get_program_details( &m_comm, &m_path, m_prog_list.prog[ i ], 0 ) )
				nStatus = 0, SetLastError( oexT( "get_program_details() failed" ) );

			else
			{
				// Get program name
				for ( int j = 0; j < m_tagsDetails.count; j++ )
					if ( m_tagsDetails.tag[ j ]->linkid == m_prog_list.prog[ i ]->linkid )
					{	strcpy( (char*)m_prog_list.prog[ i ]->name, (char*)m_tagsDetails.tag[ j ]->name );
						j = m_tagsDetails.count;
					} // end if

				// Get program details
				if ( !get_object_details_list( &m_comm, &m_path, m_prog_list.prog[ i ]->base, &m_tagsProgram[ i ], 0 ) )
				{
					for ( int j = 0; j < m_tagsProgram[ i ].count; j++ )
					{
						if ( !get_object_details( &m_comm, &m_path, m_tagsProgram[ i ].tag[ j ], 0 ) )
						{
							oex::CStr sName = oexMks( (char*)m_prog_list.prog[ i ]->name,
													  oexT( "/" ),
													  (char*)m_tagsProgram[ i ].tag[ j ]->name );

							m_mapTags[ sName.Ptr() ] = m_tagsProgram[ i ].tag[ j ];
							m_mapSqTags.set( sName.Ptr(), oexMks( i, ".", j ).Ptr() );

						} // end if

					} // end for

				} // end if

			} // end else

		} // end for

	} // end else

	return 1;
}

int CCellConnection::IsConnected()
{	return OK == m_comm.error ? 1 : 0; }

oex::oexCSTR CCellConnection::GetTypeName( int nType )
{
#	define SWITCH_TYPE( t )	case CIP_##t		: return oexT( #t );

	// Type
	switch( nType & 0xff )
	{
		SWITCH_TYPE( PROGRAM );
		SWITCH_TYPE( MAP );
		SWITCH_TYPE( COUNTER );
		SWITCH_TYPE( TIMER );
		SWITCH_TYPE( BOOL );
		SWITCH_TYPE( SINT );
		SWITCH_TYPE( INT );
		SWITCH_TYPE( DINT );
		SWITCH_TYPE( LINT );
		SWITCH_TYPE( USINT );
		SWITCH_TYPE( UINT );
		SWITCH_TYPE( UDINT );
		SWITCH_TYPE( ULINT );
		SWITCH_TYPE( REAL );
		SWITCH_TYPE( LREAL );
		SWITCH_TYPE( STIME );
		SWITCH_TYPE( DATE );
		SWITCH_TYPE( TIME_OF_DAY );
		SWITCH_TYPE( DATE_AND_TIME );
		SWITCH_TYPE( STRING );
		SWITCH_TYPE( BYTE );
		SWITCH_TYPE( WORD );
		SWITCH_TYPE( DWORD );
		SWITCH_TYPE( LWORD );
		SWITCH_TYPE( STRING2 );
		SWITCH_TYPE( FTIME );
		SWITCH_TYPE( LTIME );
		SWITCH_TYPE( ITIME );
		SWITCH_TYPE( SHORT_STRING );
		SWITCH_TYPE( TIME );
		SWITCH_TYPE( EPATH );
		SWITCH_TYPE( ENGUNIT );

	} // end switch

	return oexT( "" );
}


int CCellConnection::GetTypeFromName( const sqbind::stdString &sType )
{
#	define CHECK_TYPE( t )	if ( sType == oexT( #t ) ) return CIP_##t

	CHECK_TYPE( PROGRAM );
	CHECK_TYPE( MAP );
	CHECK_TYPE( COUNTER );
	CHECK_TYPE( TIMER );
	CHECK_TYPE( BOOL );
	CHECK_TYPE( SINT );
	CHECK_TYPE( INT );
	CHECK_TYPE( DINT );
	CHECK_TYPE( LINT );
	CHECK_TYPE( USINT );
	CHECK_TYPE( UINT );
	CHECK_TYPE( UDINT );
	CHECK_TYPE( ULINT );
	CHECK_TYPE( REAL );
	CHECK_TYPE( LREAL );
	CHECK_TYPE( STIME );
	CHECK_TYPE( DATE );
	CHECK_TYPE( TIME_OF_DAY );
	CHECK_TYPE( DATE_AND_TIME );
	CHECK_TYPE( STRING );
	CHECK_TYPE( BYTE );
	CHECK_TYPE( WORD );
	CHECK_TYPE( DWORD );
	CHECK_TYPE( LWORD );
	CHECK_TYPE( STRING2 );
	CHECK_TYPE( FTIME );
	CHECK_TYPE( LTIME );
	CHECK_TYPE( ITIME );
	CHECK_TYPE( SHORT_STRING );
	CHECK_TYPE( TIME );
	CHECK_TYPE( EPATH );
	CHECK_TYPE( ENGUNIT );

	return 0;
}

int CCellConnection::GetTypeSize( int nType )
{
	// Type
	switch( nType & 0xff )
	{
		case CIP_BOOL 			: return 1;
		case CIP_SINT 			: return 1;
		case CIP_INT 			: return 2;
		case CIP_DINT 			: return 4;
		case CIP_LINT 			: return 8;
		case CIP_USINT			: return 1;
		case CIP_UINT			: return 2;
		case CIP_UDINT			: return 4;
		case CIP_ULINT			: return 8;
		case CIP_REAL			: return 4;
		case CIP_LREAL			: return 8;
		case CIP_BYTE			: return 1;
		case CIP_WORD			: return 2;
		case CIP_DWORD			: return 4;
		case CIP_LWORD			: return 8;
	} // end switch

	return 0;
}

oex::oexBOOL CCellConnection::GetItemValue( int nType, unsigned char *pData, int nSize, int nBit, sqbind::stdString &sRet )
{
	if ( !nSize || !oexCHECK_PTR( pData ) )
		return oex::oexFALSE;

	switch( nType & 0xff )
	{
		case CIP_BOOL :
			if ( 1 > nSize )
				return oex::oexFALSE;
			sRet = oexMks( (int)( *( (char*)pData ) ? 1 : 0 ) ).Ptr();
			break;

		case CIP_SINT :
			if ( 1 > nSize )
				return oex::oexFALSE;
			sRet = oexMks( (int)( *( (char*)pData ) ) ).Ptr();
			break;

		case CIP_INT :
			if ( 2 > nSize )
				return oex::oexFALSE;
			sRet = oexMks( (int)( *( (short*)pData ) ) ).Ptr();
			break;

		case CIP_DINT :
			if ( 4 > nSize )
				return oex::oexFALSE;
			sRet = oexMks( (int)( *( (long*)pData ) ) ).Ptr();
			break;

		case CIP_LINT :
			if ( 8 > nSize )
				return oex::oexFALSE;
			sRet = oexMks( (oex::oexINT64)( *( (oex::oexINT64*)pData ) ) ).Ptr();
			break;

		case CIP_USINT :
		case CIP_BYTE :
			if ( 1 > nSize )
				return oex::oexFALSE;
			sRet = oexMks( (unsigned int)( *( (unsigned char*)pData ) ) ).Ptr();
			break;

		case CIP_UINT :
		case CIP_WORD :
			if ( 2 > nSize )
				return oex::oexFALSE;
			sRet = oexMks( (unsigned int)( *( (unsigned short*)pData ) ) ).Ptr();
			break;

		case CIP_UDINT :
		case CIP_DWORD :
			if ( 4 > nSize )
				return oex::oexFALSE;
			sRet = oexMks( (unsigned int)( *( (unsigned long*)pData ) ) ).Ptr();
			break;

		case CIP_ULINT :
		case CIP_LWORD :
			if ( 8 > nSize )
				return oex::oexFALSE;
			sRet = oexMks( (oex::oexUINT64)( *( (oex::oexUINT64*)pData ) ) ).Ptr();
			break;

		case CIP_REAL :
			if ( 4 > nSize )
				return oex::oexFALSE;
			sRet = oexMks( (float)( *( (float*)pData ) ) ).Ptr();
			break;

		case CIP_LREAL :
			if ( 4 > nSize )
				return oex::oexFALSE;
			sRet = oexMks( (double)( *( (double*)pData ) ) ).Ptr();
			break;

		default :
			for ( int b = 0; b < nSize; b++ )
			{	if ( b && !( b & 0x0f ) ) sRet += oexT( "<br>" ), sRet += oexNL;
				sRet += oexFmt( oexT( "%02lX " ), (int)pData[ b ] ).Ptr();
			} // end for

	} // end switch

	return oex::oexTRUE;
}

sqbind::CSqMap CCellConnection::TagToMap( _tag_detail *pTd )
{
	if ( !oexCHECK_PTR( pTd ) )
		return sqbind::CSqMap();

	sqbind::CSqMap mRet;
	mRet.set( oexT( "topbase" ), 		oexMks( (unsigned int)pTd->topbase ).Ptr() );
	mRet.set( oexT( "base" ), 			oexMks( (unsigned int)pTd->base ).Ptr() );
	mRet.set( oexT( "id" ), 			oexMks( (unsigned int)pTd->id ).Ptr() );
	mRet.set( oexT( "linkid" ), 		oexMks( (unsigned int)pTd->linkid ).Ptr() );
	mRet.set( oexT( "type" ), 			oexMks( (unsigned int)( pTd->type & 0x0fff ) ).Ptr() );
	mRet.set( oexT( "dim" ),			oexMks( (unsigned int)( ( pTd->type & 0x6000 ) >> 13 ) ).Ptr() );
	mRet.set( oexT( "struct" ),			oexMks( (unsigned int)( ( pTd->type & 0x8000 ) ? 1 : 0 ) ).Ptr() );
	mRet.set( oexT( "bytes" ), 			oexMks( (unsigned int)pTd->size ).Ptr() );
	mRet.set( oexT( "memory" ), 		oexMks( (unsigned int)pTd->memory ).Ptr() );
	mRet.set( oexT( "displaytype" ), 	oexMks( (unsigned int)pTd->displaytype ).Ptr() );
	mRet.set( oexT( "name" ), 			oexMbToStrPtr( (const char*)pTd->name ) );
	mRet.set( oexT( "a1_size" ), 		oexMks( (unsigned int)pTd->arraysize1 ).Ptr() );
	mRet.set( oexT( "a2_size" ), 		oexMks( (unsigned int)pTd->arraysize2 ).Ptr() );
	mRet.set( oexT( "a3_size" ),	 	oexMks( (unsigned int)pTd->arraysize3 ).Ptr() );
	mRet.set( oexT( "type_name" ), 		GetTypeName( pTd->type ) );

	return mRet;
}

void CCellConnection::VerifyTemplate()
{
	// For each tag name
	for( sqbind::CSqMulti::t_List::iterator it = m_mapSqTemplates.list().begin();
		 it != m_mapSqTemplates.list().end(); it++ )
	{
		int i = 0, o = 0;

		// For each template entry
		for( sqbind::CSqMulti::t_List::iterator itVal = it->second.list().begin();
			 itVal != it->second.list().end(); )
		{	oex::oexBOOL bErase = oex::oexFALSE;

			// Name and type must be specified
			if ( !itVal->second.list()[ oexT( "name" ) ].str().length()
			     || !itVal->second.list()[ oexT( "type" ) ].str().length() )
				bErase = oex::oexTRUE;

			else
			{
				// Verify size field
				int nBytes = 1;

				// User structure must be at least one byte
				if ( itVal->second.list()[ oexT( "type" ) ].str() == oexT( "USER" ) )
				{	nBytes = oexStrToLong( itVal->second.list()[ oexT( "bytes" ) ].str().c_str() );
					if ( 0 > nBytes ) nBytes = 0;
				} // end if

				// Decide size based on type
				else
				{
					int nType = GetTypeFromName( itVal->second.list()[ oexT( "type" ) ].str() );
					int nSize = GetTypeSize( nType );
					if ( 0 < nSize )
						nBytes = nSize;
					else
						nBytes = oexStrToLong( itVal->second.list()[ oexT( "bytes" ) ].str().c_str() );
					if ( 0 > nBytes ) nBytes = 0;

				} // end else

				// Save size of object
				itVal->second.list()[ oexT( "bytes" ) ].str() = oexMks( nBytes ).Ptr();

				// Set the offset
				itVal->second.list()[ oexT( "offset" ) ].str() = oexMks( o ).Ptr();

				// Add size to offset
				o += nBytes;

				// Does it need to be re-indexed?
				if ( oexStrToLong( itVal->first.c_str() ) != i )
				{	it->second.list()[ oexMks( i ).Ptr() ].list() = itVal->second.list();
					bErase = oex::oexTRUE;
				} // end if

				i++;

			} // end else

			if ( bErase )
				it->second.list().erase( itVal++ );
			else
				itVal++;

		} // end for

	} // end for
}

oex::oexBOOL CCellConnection::ParseTag( const sqbind::stdString &sTag, sqbind::stdString &sName, int &nProgram, int &nTag, int &nOffset, int &nSize, int &nType, int &nBit )
{
	// Set invalid path
	nProgram = nTag = nOffset = nSize = nType = nBit = -1;

	oex::CStr sParseTag = sTag.c_str(), sTemplate, sBit;
	sName = sParseTag.Parse( oexT( "." ) ).Ptr();
	if ( *sParseTag == oexT( '.' ) )
	{	sParseTag++;
		sTemplate = sParseTag.Parse( oexT( "|" ) );
		if ( *sParseTag == oexT( '|' ) )
			sParseTag++, sBit = sParseTag;
		else
			sTemplate = sParseTag;

	} // end if

	else
	{	sName = sParseTag.Parse( oexT( "|" ) ).Ptr();
		if ( *sParseTag == oexT( '|' ) )
			sParseTag++, sBit = sParseTag;
		else
			sName = sParseTag.Ptr();

	} // end else

	// Ensure we have such a tag
	if ( !m_mapSqTags.isset( sName ) )
		return oex::oexFALSE;

	oex::CStr sT = m_mapSqTags[ sName ].c_str();
	oex::CStr sP = sT.Parse( oexT( "." ) );

	if ( *sT == oexT( '.' ) )
		sT++, nProgram = sP.ToLong();
	else
		nProgram = -1;
	nTag = sT.ToLong();

	// Validate program index
	if ( 0 <= nProgram && ( nProgram >= m_prog_list.count || nProgram >= m_tagsProgram.Size() ) )
		return oex::oexFALSE;

	// Did we get a bit length?
	if ( sBit.Length() )
		nBit = sBit.ToLong();

	// Punt if no template or bit
	if ( !sTemplate.Length() )
		return oex::oexTRUE;

	// Find the specified template key
	sqbind::CSqMulti::t_List::iterator it = m_mapSqTemplates.list().find( sName );
	if ( m_mapSqTemplates.list().end() == it )
		return oex::oexFALSE;

	// Find the specified item
	for ( sqbind::CSqMulti::t_List::iterator itTmpl = it->second.list().begin();
		  it->second.list().end() != itTmpl; itTmpl++ )
		if ( itTmpl->second.list()[ oexT( "name" ) ].str() == sTemplate.Ptr() )
		{
			// Grab the offset and size
			nOffset = oexStrToLong( itTmpl->second.list()[ oexT( "offset" ) ].str().c_str() );
			nSize = oexStrToLong( itTmpl->second.list()[ oexT( "bytes" ) ].str().c_str() );
			nType = GetTypeFromName( itTmpl->second.list()[ oexT( "type" ) ].str() );

			return oex::oexTRUE;

		} // end if

	return oex::oexFALSE;
}

sqbind::CSqMap CCellConnection::ReadTag( const sqbind::stdString &sTag )
{
	// Must be connected
	if ( m_comm.error != OK )
		return SetLastError( oexT( "err=Not connected" ) );

	sqbind::stdString sName;
	int nProgram, nTag, nOffset, nSize, nType, nBit;
	if ( !ParseTag( sTag, sName, nProgram, nTag, nOffset, nSize, nType, nBit ) )
		return SetLastError( oexMks( oexT( "err=Tag does not exist : " ), sTag.c_str() ).Ptr() );

	_tag_data *pTd = &m_tagsDetails;
	if ( 0 <= nProgram )
		pTd = &m_tagsProgram[ nProgram ];

	if ( 0 > nTag || nTag >= pTd->count )
		return SetLastError( oexMks( oexT( "err=Invalid tag index " ), nTag ).Ptr() );

	// Get object details
	if ( 0 > read_object_value( &m_comm, &m_path, pTd->tag[ nTag ], 0 ) )
		return SetLastError( oexT( "err=read_object_value() failed" ) );

	sqbind::CSqMap mRet;

	// Do they want a bit offset?
	if ( 0 <= nBit )
	{
		int nByte = nBit / 8;
		unsigned char *ptr = oexNULL;

		if ( 0 > nOffset || 0 > nSize )
		{
			if ( nByte >= pTd->tag[ nTag ]->datalen )
				return SetLastError( oexT( "err=Bit address is beyond the end of the array" ) );

			// Get item pointer and size
			ptr = pTd->tag[ nTag ]->data;

		} // end if

		else
		{
			if ( nOffset + nSize > pTd->tag[ nTag ]->datalen )
				return SetLastError( oexT( "err=Template item offset address is beyond the end of the array" ) );

			if ( nOffset + nByte > pTd->tag[ nTag ]->datalen )
				return SetLastError( oexT( "err=Bit address is beyond the end of the array" ) );

			if ( nByte >= nSize )
				return SetLastError( oexT( "err=Bit address is beyond the end of the template item" ) );

			// Get item pointer and size
			ptr = &pTd->tag[ nTag ]->data[ nOffset + nByte ];

		} // end else

		int nMask = 1;
		int i = nBit % 8;
		while ( nMask && i )
			nMask <<= 1, i--;

		// Capture the bit value
		mRet.set( oexT( "value" ), 0 != ( nMask & ptr[ nByte ] ) ? oexT( "1" ) : oexT( "0" ) );
		mRet.set( oexT( "type_name" ), oexT( "BIT" ) );
		mRet.set( oexT( "byte" ), oexMks( nByte ).Ptr() );
		mRet.set( oexT( "bit" ), oexMks( nBit % 8 ).Ptr() );

	} // end if

	// Did we get an offset and size?
	else if ( 0 > nOffset || 0 > nSize )
	{
		mRet = TagToMap( pTd->tag[ nTag ] );
		if ( mRet.isset( oexT( "err" ) ) )
			return mRet;

		sqbind::stdString sVal;
		if ( GetItemValue( pTd->tag[ nTag ]->type, pTd->tag[ nTag ]->data, pTd->tag[ nTag ]->datalen, nBit, sVal ) )
			mRet.set( oexT( "value" ), sVal.c_str() );

	} // end if

	else
	{
		// Check array bounds
		if ( nOffset + nSize > pTd->tag[ nTag ]->datalen )
			return SetLastError( oexT( "err=Template item offset address is beyond the end of the array" ) );

		sqbind::stdString sVal;
		if ( GetItemValue( nType, &pTd->tag[ nTag ]->data[ nOffset ], nSize, nBit, sVal ) )
			mRet.set( oexT( "value" ), sVal );

		mRet.set( oexT( "type" ), oexMks( nType ).Ptr() );
		mRet.set( oexT( "type_name" ), GetTypeName( nType ) );
		mRet.set( oexT( "bytes" ), oexMks( nSize ).Ptr() );
		mRet.set( oexT( "offset" ), oexMks( nOffset ).Ptr() );

	} // end else

	return mRet;
}

sqbind::stdString CCellConnection::GetBackplaneData()
{
	oex::CStr ret;

	LoadTags();

	return ret.Ptr();


/*
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

					ret << oexNL << oexT( "Device Tag " ) << i << oexNL;

					if ( i < m_tagsConfig.count && oexCHECK_PTR( m_tagsConfig.tag ) )
					{
						ret << oexT( "--- Config ---" ) << oexNL;
						ret << oexFmt( oexT( "Top Base     : %08lX" ),	m_tagsDetails.tag[ i ]->topbase ) << oexNL;
						ret << oexFmt( oexT( "Base         : %08lX" ),	m_tagsDetails.tag[ i ]->base ) << oexNL;
						ret << oexFmt( oexT( "Link ID      : %08lX" ),	m_tagsDetails.tag[ i ]->linkid ) << oexNL;
						ret << oexFmt( oexT( "Type         : %04lX" ),	m_tagsDetails.tag[ i ]->type ) << oexNL;
					} // end if

					if ( i < m_tagsDetails.count && oexCHECK_PTR( m_tagsDetails.tag ) )
					{
						ret << oexNL << oexT( "--- Details ---" ) << oexNL;
						ret << oexFmt( oexT( "Top Base     : %08lX" ),	m_tagsDetails.tag[ i ]->topbase ) << oexNL;
						ret << oexFmt( oexT( "Base         : %08lX" ),	m_tagsDetails.tag[ i ]->base ) << oexNL;
						ret << oexFmt( oexT( "ID           : %08lX" ),	m_tagsDetails.tag[ i ]->id ) << oexNL;
						ret << oexFmt( oexT( "Link ID      : %08lX" ),	m_tagsDetails.tag[ i ]->linkid ) << oexNL;
						ret << oexFmt( oexT( "Type         : %04lX" ),	m_tagsDetails.tag[ i ]->type ) << oexNL;
						ret << oexFmt( oexT( "Size         : %04lX" ),	m_tagsDetails.tag[ i ]->size ) << oexNL;
						ret << 	       oexT( "Data Length  : " ) 		<< m_tagsDetails.tag[ i ]->datalen << oexNL;
						ret << oexFmt( oexT( "Memory       : %08lX" ),	m_tagsDetails.tag[ i ]->memory ) << oexNL;
						ret << oexFmt( oexT( "Display      : %04lX" ),	m_tagsDetails.tag[ i ]->displaytype ) << oexNL;
						ret <<         oexT( "Name         : " ) 		<< (const char*)m_tagsDetails.tag[ i ]->name << oexNL;
						ret <<         oexT( "Type Name    : " ) 		<< GetTagTypeName( *m_tagsDetails.tag[ i ] ).c_str() << oexNL;
						sqbind::stdString sVal;
						if ( GetTagValue( *m_tagsDetails.tag[ i ], sVal ) )
							ret <<     oexT( "Value        : " ) 		<< sVal.c_str() << oexNL;
						ret <<         oexT( "ArraySize1   : " ) 		<< (int)m_tagsDetails.tag[ i ]->arraysize1 << oexNL;
						ret <<         oexT( "ArraySize2   : " ) 		<< (int)m_tagsDetails.tag[ i ]->arraysize2 << oexNL;
						ret <<         oexT( "ArraySize3   : " ) 		<< (int)m_tagsDetails.tag[ i ]->arraysize3 << oexNL;

						if ( m_tagsDetails.tag[ i ]->datalen && oexCHECK_PTR( m_tagsDetails.tag[ i ]->data ) )
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

	for ( int i = 0; i < m_prog_list.count; i++ )
	{
		ret << oexNL;
		ret << oexT( "--- Program " ) << i << oexT( " ---" ) << oexNL;
		ret << oexFmt( 				oexT( "Base      : %08lX" ),	m_prog_list.prog[ i ]->base ) << oexNL;

		if ( !get_program_details( &m_comm, &m_path, m_prog_list.prog[ i ], debug ) )
		{
			ret << oexFmt( 			oexT( "Link ID   : %08lX" ),	m_prog_list.prog[ i ]->linkid ) << oexNL;
			for ( int j = 0; j < m_tagsDetails.count; j++ )
				if ( m_tagsDetails.tag[ j ]->linkid == m_prog_list.prog[ i ]->linkid )
				{
					strcpy( (char*)m_prog_list.prog[ i ]->name, (char*)m_tagsDetails.tag[ j ]->name );
					ret << 			oexT( "Name      : " ) 			<< (const char*)m_tagsDetails.tag[ j ]->name << oexNL;

				} // end if

			_oexTRY
			{
				_tag_data td;
				oexZeroMemory( &td, sizeof( td ) );
				if ( !get_object_details_list( &m_comm, &m_path, m_prog_list.prog[ i ]->base, &td, debug ) )
				{
					ret << oexNL;
					ret <<                  oexT( "Program Tags : " ) <<       td.count << oexNL;
					ret << oexNL;

					for ( int j = 0; j < td.count; j++ )
					{
						ret << oexT( "Program Tag " ) << j << oexNL;
						ret << 		oexFmt( oexT( "Base         : %08lX" ),	   td.tag[ j ]->base ) << oexNL;
						ret << 		oexFmt( oexT( "ID           : %08lX" ),	   td.tag[ j ]->id ) << oexNL;

						if ( !get_object_details( &m_comm, &m_path, td.tag[ j ], debug ) )
						{
							if ( 0 > read_object_value( &m_comm, &m_path, td.tag[ j ], debug ) )
								ret << oexT( "read_object_value() failed for tag " ) << j << oexNL;

							ret << 	        oexT( "Name         : " ) 		<< (const char*)td.tag[ j ]->name << oexNL;
							ret << oexFmt(  oexT( "Type         : %04lX" ),	   td.tag[ j ]->type ) << oexNL;
							ret << 	        oexT( "Size         : " ) 		<< td.tag[ j ]->size << oexNL;
							ret << 	        oexT( "Data Length  : " ) 		<< td.tag[ j ]->datalen << oexNL;
							ret <<          oexT( "Type Name    : " ) 		<< GetTagTypeName( *td.tag[ j ] ).c_str() << oexNL;
							sqbind::stdString sVal;
							if ( GetTagValue( *td.tag[ j ], sVal ) )
								ret <<      oexT( "Value        : " ) 		<< sVal.c_str() << oexNL;
							ret << 	        oexT( "ArraySize11  : " ) 		<< (int)td.tag[ j ]->arraysize1 << oexNL;
							ret << 	        oexT( "ArraySize12  : " ) 		<< (int)td.tag[ j ]->arraysize2 << oexNL;
							ret << 	        oexT( "ArraySize13  : " ) 		<< (int)td.tag[ j ]->arraysize3 << oexNL;

							if ( td.tag[ j ]->datalen && oexCHECK_PTR( td.tag[ j ]->data ) )
							{	for ( int b = 0; b < td.tag[ j ]->datalen; b++ )
								{	if ( b && !( b & 0x0f ) ) ret << oexNL;
									ret << oexFmt( oexT( "%02lX " ), td.tag[ j ]->data[ b ] );
								} // end for
								ret << oexNL;
							} // end if

						} // end if

						// Free tag mem
						free( td.tag[ j ] );

						ret << oexNL;

					} // end for

				} // end if
				else
					ret << oexT( "!!! No Tags !!! " ) << oexNL;

			} // end try

			_oexCATCH( ... )
			{
				ret << oexMks( oexT( "!!! Assert on tag " ), (const char*)m_prog_list.prog[ i ]->name, oexNL );

			} // end catch

		} // end if

		ret << oexNL;

	} // end for

	return ret.Ptr();
*/
}

