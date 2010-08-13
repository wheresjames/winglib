/*------------------------------------------------------------------
// data_log.cpp
//
// Copyright (c) 1997
// Robert Umbehant
// winglib@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted for commercial and
// non-commercial purposes, provided that the following
// conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to
//   endorse or promote products derived from this software without
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#include "../oexlib.h"

OEX_USING_NAMESPACE

CDataLog::CDataLog()
{_STT();

	m_bChangesOnly = oexTRUE;

	oexZero( m_pLogs );
}

CDataLog::~CDataLog()
{_STT();
	Destroy();
}

void CDataLog::Destroy()
{_STT();

	// Write remaining data to disk
	Flush( 0 );

	// Lose logging objects
	for ( oexINT i = 0; i < eMaxKeys; i++ )
		if ( m_pLogs[ i ] )
			OexAllocDelete( m_pLogs[ i ] ),
			m_pLogs[ i ] = oexNULL;

	// Forget the root
	m_sRoot.Destroy();
}

oexBOOL CDataLog::SetRoot( oexCSTR x_pRoot )
{_STT();

	// Ensure valid root folder
	if ( !x_pRoot || !*x_pRoot )
		return oexFALSE;

	// Create folder if it doesn't exist
	if ( !oexExists( x_pRoot ) )
		oexCreatePath( x_pRoot );

	// Punt if that failed
	if ( !oexExists( x_pRoot ) )
		return oexFALSE;

	// Save path to root folder
	m_sRoot = x_pRoot;

	return oexTRUE;
}

oexINT CDataLog::FindKey( oexCSTR x_pKey )
{_STT();

	// Ensure valid key name
	if ( !x_pKey || !*x_pKey )
		return -1;

	// Find logging key with the same name
	for( oexINT i = 0; i < eMaxKeys; i++ )
		if ( m_pLogs[ i ] && m_pLogs[ i ]->sName == x_pKey )
			return i;

	return -1;
}

oexINT CDataLog::AddKey( oexCSTR x_pKey, oexUINT x_uTime )
{_STT();

	// Ensure valid key name
	if ( !x_pKey || !*x_pKey )
		return -1;

	// Add a new logging key
	for( oexINT i = 0; i < eMaxKeys; i++ )
		if ( !m_pLogs[ i ] )
		{
			// Create logging object
			m_pLogs[ i ] = OexAllocConstruct< SLogKey >();
			if ( !m_pLogs[ i ] ) 
				return -1;

			// Allocate some initial space
			m_pLogs[ i ]->bin.Allocate( eInitBuffer );

			// Save key name
			m_pLogs[ i ]->sName = x_pKey;

			// Name hash, this will be the file names
			CStr8 sMb = oexStrToMb( x_pKey );
			m_pLogs[ i ]->sHash = oexMbToStr( CBase16::Encode( oss::CMd5::Transform( &m_pLogs[ i ]->hash, sMb.Ptr(), sMb.Length() ), sizeof( m_pLogs[ i ]->hash ) ) );

			// Init vars
			m_pLogs[ i ]->valid = 0;			
			m_pLogs[ i ]->plast = 0;
			m_pLogs[ i ]->olast = 0;

			if ( m_sRoot.Length() )
			{ 	
				// Save key name to file
				if ( !x_uTime )
					x_uTime = oexGetUnixTime();
				CStr sRoot = m_sRoot;
				sRoot.BuildPath( x_uTime / eLogBase );
				oexCreatePath( sRoot.Ptr() );
				CFile().CreateAlways( ( sRoot.BuildPath( m_pLogs[ i ]->sHash ) << oexT( ".txt" ) ).Ptr() )
					.Write( oexStrToMbPtr( x_pKey ) );
/*
				// Get root index time
				x_uTime = x_uTime - ( x_uTime % eIndexStep );

				// Resume existing index
				SIterator it; it.Init( m_sRoot, x_pKey );
				if ( FindValue( it, x_uTime, 0, eDtNone, eMethodNone ) )
				{	while ( it.vi.uNext > it.pos )
					{	it.pos = it.vi.uNext;
						it.fData.SetPtrPosBegin( it.vi.uNext );
						if ( !it.fData.Read( &it.vi, sizeof( it.vi ) ) || it.hash.Data1 != it.vi.uHash )
							it.vi.uNext = 0;
					} // end while
					m_pLogs[ i ]->plast = it.pos;
				} // end if
*/
			} // end write name
			
			return i;

		} // end if

	return -1;
}

oexINT CDataLog::RemoveKey( oexINT x_nKey )
{_STT();

	// Ensure valid key
	if ( 0 > x_nKey || eMaxKeys <= x_nKey || !m_pLogs[ x_nKey ] )
		return -1;

	// Delete key buffer
	OexAllocDelete( m_pLogs[ x_nKey ] );
	m_pLogs[ x_nKey ] = oexNULL;

	return x_nKey;
}

oexBOOL CDataLog::Log( oexINT x_nKey, oexCPVOID x_pValue, oexUINT x_uSize, oexUINT x_uTime, oexUINT x_uTimeMs )
{
	// Ensure valid key
	if ( 0 > x_nKey || eMaxKeys <= x_nKey || !m_pLogs[ x_nKey ] )
		return oexFALSE;

	// Grab time
	if ( !x_uTime )
		x_uTime = oexGetUnixTime();

	// First, determine if the data has changed since we last logged it
	oexGUID hash;
	oss::CMd5::Transform( &hash, x_pValue, x_uSize );
	if ( m_pLogs[ x_nKey ]->valid > x_uTime
		 && guid::CmpGuid( &hash, &m_pLogs[ x_nKey ]->changed ) )
		return oexTRUE;

	// Set timeout
	m_pLogs[ x_nKey ]->valid = x_uTime + eMaxValid;

	// Copy the guid
	guid::CopyGuid( &m_pLogs[ x_nKey ]->changed, &hash );

	// Did user provide a time?
	if ( !x_uTime )
	{	CSysTime st( CSysTime::eGmtTime );
		x_uTime = st.GetUnixTime();
		x_uTimeMs = st.GetMilliSecond();
	} // end if

	// Prepare index header
	SValueIndex vi;
	vi.uBytes = sizeof( vi );
	vi.uTime = x_uTime;
	vi.uTimeMs = x_uTimeMs;
	vi.uSize = x_uSize;
	vi.uPrev = m_pLogs[ x_nKey ]->olast;
	vi.uHash = m_pLogs[ x_nKey ]->hash.Data1;
	vi.uNext = 0;

	// Update last pointer
	if ( m_pLogs[ x_nKey ]->bin.getUsed() )
	{	SValueIndex *viLast = (SValueIndex*)m_pLogs[ x_nKey ]->bin.Ptr( m_pLogs[ x_nKey ]->olast );
		if ( viLast ) viLast->uNext = m_pLogs[ x_nKey ]->bin.getUsed();
	} // end if

	// Save pointer to this structure
	m_pLogs[ x_nKey ]->olast = m_pLogs[ x_nKey ]->bin.getUsed();

	// Write the value to the memory log
	if ( !m_pLogs[ x_nKey ]->bin.AppendBuffer( (CBin::t_byte*)&vi, vi.uBytes ) 
		 || !m_pLogs[ x_nKey ]->bin.AppendBuffer( (CBin::t_byte*)x_pValue, x_uSize ) )
		return oexFALSE;

	return oexTRUE;
}

oexBOOL CDataLog::Flush( oexUINT x_uTime )
{_STT();

	// Punt if no root folder
	if ( !m_sRoot.Length() )
		return oexFALSE;

	// Get timestamp if needed
	if ( !x_uTime )
		x_uTime = oexGetUnixTime();

	// Open the database
	CFile fData;
	if ( !OpenDb( oexTRUE, m_sRoot, oexT( "" ), x_uTime, oexNULL, &fData ) )
		return oexFALSE;

	// Flush data to disk
	for( oexINT i = 0; i < eMaxKeys; i++ )
		if ( m_pLogs[ i ] && m_pLogs[ i ]->bin.getUsed() )
		{
			// Where will this block start?
			CFile::t_size pos = (oexUINT)fData.GetPtrPos();

			// +++ Setting this to zero causes more overhead, but allows 'fixing'
			//     currupt file entries, which I currently seem to be seeing very occasionally.
//			m_pLogs[ i ]->plast = 0;

			// Validate last entry
			if ( m_pLogs[ i ]->plast )
			{	SValueIndex vi;
				CFile::CRestoreFilePos rfp( &fData );
				fData.SetPtrPosBegin( m_pLogs[ i ]->plast );
				if ( !fData.Read( &vi, sizeof( vi ) ) || m_pLogs[ i ]->hash.Data1 != vi.uHash )
					m_pLogs[ i ]->plast = 0;
			} // end if

			// Should we resume?
			if ( !m_pLogs[ i ]->plast )
			{
				// Get root index time
				oexUINT uRootTime = x_uTime - ( x_uTime % eIndexStep );

				// Resume existing index
				SIterator itR; itR.Init( m_sRoot, m_pLogs[ i ]->sName.Ptr() );
				if ( FindValue( itR, uRootTime, 0, eDtNone, eMethodNone ) )
				{	m_pLogs[ i ]->plast = itR.pos;
					while ( itR.vi.uNext > itR.pos )
					{	itR.pos = itR.vi.uNext;
						itR.fData.SetPtrPosBegin( itR.vi.uNext );
						if ( !itR.fData.Read( &itR.vi, sizeof( itR.vi ) ) || itR.hash.Data1 != itR.vi.uHash )
							itR.vi.uNext = 0;
						else
							m_pLogs[ i ]->plast = itR.pos;
					} // end while
				} // end if

			} // end if

			// Update prev/next	pointers
			oexUINT n = 0;
			while ( m_pLogs[ i ]->bin.getUsed() >= sizeof( SValueIndex ) )
			{	
				// Get block info
				SValueIndex *pvi = (SValueIndex*)m_pLogs[ i ]->bin.Ptr();

				// Track prev/next pointers
				if ( !n++ )
				{
					// Do we need to update a previous item?
					if ( m_pLogs[ i ]->plast )
					{	SValueIndex vi;
						CFile::CRestoreFilePos rfp( &fData );
						fData.SetPtrPosBegin( m_pLogs[ i ]->plast );
						if ( fData.Read( &vi, sizeof( vi ) ) && m_pLogs[ i ]->hash.Data1 == vi.uHash )
						{	vi.uNext = pos;
							fData.SetPtrPosBegin( m_pLogs[ i ]->plast );
							fData.Write( &vi, sizeof( vi ) );
						} // end if
					} // end if
					
					pvi->uPrev = m_pLogs[ i ]->plast;
					pvi->uNext += pos;

				} // end if
				else
				{	pvi->uPrev += pos;
					if ( pvi->uNext ) pvi->uNext += pos;
				} // end else

				// Save last index
				m_pLogs[ i ]->plast = pos + m_pLogs[ i ]->bin.getOffset();

				// Skip this block
				m_pLogs[ i ]->bin.setOffset( m_pLogs[ i ]->bin.getOffset() + pvi->uBytes + pvi->uSize ); 

			} // end while

			// Back to zero
			m_pLogs[ i ]->bin.setOffset( 0 );

			// Write data to file
			if ( fData.Write( m_pLogs[ i ]->bin.Ptr(), m_pLogs[ i ]->bin.getUsed() ) )
			{
				// Update the index
				CFile fIdx;
				if ( OpenDb( oexTRUE, m_sRoot, m_pLogs[ i ]->sHash, x_uTime, &fIdx, oexNULL ) )
				{
					// While we have blocks
					CFile::t_size ex = 0;
					oexUINT uLast = oexMAXUINT;
					while ( m_pLogs[ i ]->bin.getUsed() >= sizeof( SValueIndex ) )
					{	
						// See if this block should go in the index
						SValueIndex *pvi = (SValueIndex*)m_pLogs[ i ]->bin.Ptr();
						oexUINT uI = ( pvi->uTime % eLogBase ) / eIndexStep;
						if ( oexMAXUINT == uLast || uLast < uI )
						{
							uLast = uI;

							// Point to index position
							fIdx.SetPtrPosBegin( uI * sizeof( pos ) );

							// Update the index if blank
							if ( fIdx.Read( &ex, sizeof( ex ) ) && !ex )
								ex = pos + m_pLogs[ i ]->bin.getOffset(), 
								fIdx.SetPtrPosBegin( uI * sizeof( pos ) ),
								fIdx.Write( &ex, sizeof( ex ) );

						} // end if

						// Skip this block
						m_pLogs[ i ]->bin.setOffset( m_pLogs[ i ]->bin.getOffset() + pvi->uBytes + pvi->uSize ); 

					} // end while

				} // end if

			} // end if

			// Clear memory buffer
			m_pLogs[ i ]->bin.setOffset( 0 );
			m_pLogs[ i ]->bin.setUsed( 0 );
			m_pLogs[ i ]->olast = 0;

		} // end if

	return oexTRUE;
}

CPropertyBag CDataLog::GetKeyList( oexUINT x_uTime )
{_STT();

	// Ensure we have a root
	if ( !m_sRoot.Length() )
		return CPropertyBag();

	// Use current time if not specified
	if ( !x_uTime )
		x_uTime = oexGetUnixTime();

	// Build root to data based on timestamp
	CStr sRoot = m_sRoot;
	sRoot.BuildPath( x_uTime / eLogBase );
	if ( !oexExists( sRoot.Ptr() ) )
		return CPropertyBag();

	CPropertyBag pb;

	CFindFiles ff;
	if ( ff.FindFirst( sRoot.Ptr(), oexT( "*.txt" ) ) )
		do
		{
			// Read key name into property bag
			if ( !ff.IsDirectory() )
				pb[ ff.GetFileName() ] = CFile()
					.OpenExisting( ( CStr( sRoot ).BuildPath( ff.GetFileName() ) ).Ptr() ).Read();

		} while ( ff.FindNext() );

	return pb;
}

oexBOOL CDataLog::IsKeyData( CStr x_sRoot, CStr x_sHash, oexUINT x_uTime )
{
	if ( !x_uTime )
		x_uTime = oexGetUnixTime();

	// Build root to data based on starting timestamp
	if ( !oexExists( ( x_sRoot.BuildPath( x_uTime / eLogBase ).BuildPath( x_sHash ) << oexT( ".bin" ) ).Ptr() ) )
		return oexFALSE;
	
	return oexTRUE;
}

oexBOOL CDataLog::OpenDb( oexBOOL x_bCreate, CStr x_sRoot, CStr x_sHash, oexUINT x_uTime, CFile *x_pIdx, CFile *x_pData )
{
	// Build root to data based on starting timestamp
	x_sRoot.BuildPath( x_uTime / eLogBase );
	if ( !oexExists( x_sRoot.Ptr() ) )
	{	if ( !x_bCreate )
			return oexFALSE;
		else
			oexCreatePath( x_sRoot.Ptr() );
	} // end if

	// Data file?
	if ( x_pData )
	{
		// Data file path
		CStr sData = x_sRoot; sData.BuildPath( "data.bin" );
		oexBOOL bExists = oexExists( sData.Ptr() );
		if ( !bExists && !x_bCreate )
			return oexFALSE;

		// Open the data file
		if ( x_bCreate 
			 ? !x_pData->OpenAlways( sData.Ptr() ).IsOpen() 
			 : !x_pData->OpenExisting( sData.Ptr() ).IsOpen() )
			return oexFALSE;

		// Initialize?
		if ( x_bCreate && !bExists )
			x_pData->Write( CStr8( "Hey, you, get off of my cloud." ) );

		// Move to the end of the file
		x_pData->SetPtrPosEnd( 0 );

	} // end if

	// Index file?
	if ( x_pIdx )
	{
		// Open key index
		CStr sIndex( x_sRoot ); sIndex.BuildPath( x_sHash ) << oexT( ".bin" );
		oexBOOL bExists = oexExists( sIndex.Ptr() );
		if ( !bExists && !x_bCreate )
			return oexFALSE;

		// Open the index file
		if ( x_bCreate 
			 ? !x_pIdx->OpenAlways( sIndex.Ptr() ).IsOpen() 
			 : !x_pIdx->OpenExisting( sIndex.Ptr() ).IsOpen() )
			return oexFALSE;

		// Initialize index file if needed
		if ( x_bCreate && !bExists )
		{	CFile::t_size uOffset = 0;
			for ( oexINT i = 0; i < eLogBase; i += eIndexStep )
				x_pIdx->Write( &uOffset, sizeof( uOffset ) );
		} // end if

	} // end if

	return oexTRUE;
}

oexBOOL CDataLog::FindValue( SIterator &x_it, oexUINT x_uTime, oexUINT x_uTimeMs, oexINT x_nDataType, oexINT x_nMethod )
{
	// Get db metrics
	oexUINT _uB = x_uTime / eLogBase;
	oexUINT _uI = ( x_uTime % eLogBase ) / eIndexStep;
	oexBOOL bNew = oexFALSE;

	// Ensure we have the correct database open
	if ( oexMAXUINT == x_it.uB || _uB != x_it.uB )
	{	
		// Save open db index
		x_it.uB = _uB;

		// Open the database containing data for this time
		if ( !OpenDb( oexFALSE, x_it.sRoot, x_it.sHash, x_uTime, &x_it.fIdx, &x_it.fData ) )
			return oexFALSE;

		// No good
		x_it.vi.uHash = 0;
		bNew = oexTRUE;

	} // end if

	// Time to index?
	if ( oexMAXUINT == _uI || _uI != x_it.uI )
	{
		// Save index offset
		x_it.uI = _uI;

		// Header invalid now
		x_it.vi.uTime = 0;

		// Point to index position
		x_it.fIdx.SetPtrPosBegin( x_it.uI * sizeof( CFile::t_size ) );

		// Update the index if blank
		CFile::t_size ex;
		if ( x_it.fIdx.Read( &ex, sizeof( ex ) ) && ex )
			x_it.pos = ex;

		// No good
		x_it.vi.uHash = 0;
		bNew = oexTRUE;

	} // end if

	// Do we have a data position?
	if ( !x_it.pos )
		return oexFALSE;

	// Do we need new data?
	if ( x_it.vi.uTime < x_uTime || ( x_it.vi.uTime == x_uTime && x_it.vi.uTimeMs < x_uTimeMs ) )
	{
		x_it.nCount = 0;
		x_it.vi.uNext = x_it.pos;
		do
		{
			// Do we have a previous valid data?
			if ( x_it.hash.Data1 == x_it.vi.uHash )
			{
				// Point to data
				x_it.fData.SetPtrPosBegin( x_it.pos + x_it.vi.uBytes );

				// Get string value
				if ( x_it.getValue( x_it.sValue ) )
				{
					// Average values if needed
					if ( !bNew && eDtString < x_nDataType && eMethodAverage == x_nMethod )
					{
						// Reset 
						if ( !x_it.nCount++ )
							x_it.fValue = 0, x_it.nValue = 0;

						if ( eDtInt == x_nDataType )
							x_it.nValue += x_it.sValue.ToInt();

						else if ( eDtFloat == x_nDataType )
							x_it.fValue += x_it.sValue.ToFloat();

					} // end if

				} // end if

			} // end if

			// Save last valid position
			x_it.pos = x_it.vi.uNext;

			// Read the header
			x_it.fData.SetPtrPosBegin( x_it.vi.uNext );
			if ( !x_it.fData.Read( &x_it.vi, sizeof( x_it.vi ) ) || x_it.hash.Data1 != x_it.vi.uHash )
				return oexFALSE;

		} while ( ( x_it.vi.uTime < x_uTime || ( x_it.vi.uTime == x_uTime && x_it.vi.uTimeMs < x_uTimeMs ) ) && x_it.vi.uNext > x_it.pos );

		// Scale averages
		if ( !bNew && eDtString < x_nDataType && eMethodAverage == x_nMethod )
		{	if ( 0 < x_it.nCount )
				x_it.fValue /= x_it.nCount,
				x_it.nValue /= x_it.nCount;
			x_it.nCount = 0;
		} // end if

		else if ( eDtInt == x_nDataType )
			x_it.nValue = x_it.sValue.ToInt();

		else if ( eDtFloat == x_nDataType )
			x_it.fValue = x_it.sValue.ToFloat();

	} // end if

	return oexTRUE;
}

CPropertyBag CDataLog::GetLog( oexCSTR x_pKey, oexUINT x_uStart, oexUINT x_uEnd, oexUINT x_uInterval, oexINT x_nDataType, oexINT x_nMethod )
{
	// Sanity checks
	if ( !x_pKey || !*x_pKey || !m_sRoot.Length() || x_uStart > x_uEnd )
		return CPropertyBag();

	SIterator it;
	if ( !it.Init( m_sRoot, x_pKey ) || !it.IsData( x_uStart ) )
		return CPropertyBag();
	
	// Align with the interval
	oexUINT uAlign = x_uInterval / 1000;
	if ( uAlign )
		x_uStart -= ( x_uStart % uAlign ),
		x_uEnd -= ( x_uEnd % uAlign );

	// Create data
	CPropertyBag pb;
	oexUINT uTime = x_uStart, uTimeMs = 0;
	while ( uTime <= x_uEnd )
	{
		// Find the value for this time
		if ( FindValue( it, uTime, uTimeMs, x_nDataType, x_nMethod ) )
		{	if ( uTimeMs )
				pb[ oexFmt( oexT( "%u.%u" ), uTime, uTimeMs ) ].ToString() = it.sValue;
			else
				pb[ uTime ].ToString() = it.sValue;
		} // end if
			
		else if( uTimeMs )
			pb[ oexFmt( oexT( "%u.%u" ), uTime, uTimeMs ) ] = oexT( "" );

		else
			pb[ uTime ].ToString() = oexT( "" );

		// Update time
		uTimeMs += x_uInterval;
		uTime += uTimeMs / 1000;
		uTimeMs %= 1000;

	} // end for

	return pb;
}

CStr CDataLog::GetLogBin( oexCSTR x_pKey, oexUINT x_uStart, oexUINT x_uEnd, oexUINT x_uInterval, oexINT x_nDataType, oexINT x_nMethod, float x_fScale )
{
	// Sanity checks
	if ( !x_pKey || !*x_pKey || !m_sRoot.Length() || x_uStart > x_uEnd )
		return oexT( "" );

	// Set defaults
	if ( eDtNone == x_nDataType ) 
		x_nDataType = eDtFloat;
	if ( eMethodNone == x_nMethod ) 
		x_nMethod = eMethodAverage;
	if ( !x_fScale )
		x_fScale = 1.f;

	SIterator it;
	if ( !it.Init( m_sRoot, x_pKey ) || !it.IsData( x_uStart ) )
		return oexT( "" );

	// Align with the interval
	oexUINT uAlign = x_uInterval / 1000;
	if ( uAlign )
		x_uStart -= ( x_uStart % uAlign ),
		x_uEnd -= ( x_uEnd % uAlign );

	// Allocate space for binary data
	oexUINT nItems = ( x_uEnd - x_uStart ) * 1000 / x_uInterval;
	CBin bin( nItems * sizeof( float ), nItems * sizeof( float ) );

	// Create data
	CPropertyBag pb;
	oexUINT uTime = x_uStart, uTimeMs = 0, i = 0;
	while ( i < nItems && uTime <= x_uEnd )
	{
		// Find the value for this time
		if ( FindValue( it, uTime, uTimeMs, x_nDataType, x_nMethod ) )
			bin.setFLOAT( i++, it.fValue * x_fScale );
		else
			bin.setFLOAT( i++, 0.f );

		// Update time
		uTimeMs += x_uInterval;
		uTime += uTimeMs / 1000;
		uTimeMs %= 1000;

	} // end for

	// Set the binary share
	CStr sId = oexGuidToString();
	oexSetBin( sId, &bin );

	return sId;
}
