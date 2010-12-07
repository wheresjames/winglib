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

	SetLogParams( eLogBase, eIndexStep );

	m_bChangesOnly = oexTRUE;

	oexZero( m_pLogs );

	m_uLimit = eDefaultFetchLimit;
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

void CDataLog::SetLogParams( t_time tBase, t_time tStep )
{_STT();
	if ( 0 < tBase )
		m_tLogBase = tBase;
	if ( 0 < tStep )
	{	m_tIndexStep = tStep;
		m_tMaxValid = tStep >> 2;
	} // end if
	if ( !m_tMaxValid ) 
		m_tMaxValid = 1;
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

oexINT CDataLog::AddKey( oexCSTR x_pKey, t_time x_tTime )
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
				if ( !x_tTime )
					x_tTime = oexGetUnixTime();
				CStr sRoot = m_sRoot;
				sRoot.BuildPath( x_tTime / m_tLogBase );
				oexCreatePath( sRoot.Ptr() );
				m_pLogs[ i ]->sFile = sRoot.BuildPath( m_pLogs[ i ]->sHash ) << oexT( ".txt" );
				CFile().CreateAlways( m_pLogs[ i ]->sFile.Ptr() ).Write( oexStrToMbPtr( x_pKey ) );

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

oexINT CDataLog::GetBufferSize( oexINT x_nKey )
{_STT();

	// Ensure valid key
	if ( 0 > x_nKey || eMaxKeys <= x_nKey || !m_pLogs[ x_nKey ] )
		return -1;

	return m_pLogs[ x_nKey ]->bin.getUsed();
}

oexBOOL CDataLog::Log( oexINT x_nKey, oexCPVOID x_pValue, oexUINT x_uSize, t_time x_tTime, t_time x_tTimeMs, oexBOOL bBuffering )
{
	// Ensure valid key
	if ( 0 > x_nKey || eMaxKeys <= x_nKey || !m_pLogs[ x_nKey ] )
		return oexFALSE;

	// Grab time
	t_time tTime = x_tTime;
	if ( !tTime )
		tTime = oexGetUnixTime();

	// First, determine if the data has changed since we last logged it
	oexGUID hash;
	oss::CMd5::Transform( &hash, x_pValue, x_uSize );
	t_time uRi = ( x_tTime % m_tLogBase ) / m_tIndexStep;

	// We don't log if nothing has changed
	if ( uRi == m_pLogs[ x_nKey ]->uRi
		 && m_pLogs[ x_nKey ]->valid > tTime
		 && guid::CmpGuid( &hash, &m_pLogs[ x_nKey ]->changed ) )
		return oexTRUE;

	// Set timeout
	m_pLogs[ x_nKey ]->valid = tTime + m_tMaxValid;

	// Copy the data state hash and root index
	m_pLogs[ x_nKey ]->uRi = uRi;
	guid::CopyGuid( &m_pLogs[ x_nKey ]->changed, &hash );

	// Did user provide a time?
	if ( !x_tTime )
	{	CSysTime st( CSysTime::eGmtTime );
		x_tTime = st.GetUnixTime();
		x_tTimeMs = st.GetMilliSecond();
	} // end if

	// Prepare index header
	SValueIndex vi;
	vi.uBytes = sizeof( vi );
	vi.uTime = x_tTime;
	vi.uTimeMs = x_tTimeMs;
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

	if ( !bBuffering )
		return FlushBuffer( x_nKey, &vi, x_pValue, x_uSize );

	// Write the value to the memory log
	if ( !m_pLogs[ x_nKey ]->bin.AppendBuffer( (CBin::t_byte*)&vi, vi.uBytes ) 
		 || !m_pLogs[ x_nKey ]->bin.AppendBuffer( (CBin::t_byte*)x_pValue, x_uSize ) )
		return oexFALSE;

	return oexTRUE;
}

oexBOOL CDataLog::FlushBuffer( oexINT x_nKey, SValueIndex *pVi, oexCPVOID pBuf, oexUINT uSize )
{_STT();
	// Ensure valid key
	if ( 0 > x_nKey || eMaxKeys <= x_nKey || !m_pLogs[ x_nKey ] )
		return oexFALSE;

	// Punt if no root folder
	if ( !m_sRoot.Length() )
		return oexFALSE;

	// Get timestamp if needed
	if ( !pVi->uTime )
		pVi->uTime = oexGetUnixTime();

	// Open the database
	CFile fData; oexBOOL bNewDb = oexFALSE;
	if ( !OpenDb( oexTRUE, m_sRoot, oexT( "" ), pVi->uTime, oexNULL, &fData, m_tLogBase, m_tIndexStep, bNewDb ) )
		return oexFALSE;

	// Invalidate timers if new database
	if ( bNewDb )
		for( oexINT i = 0; i < eMaxKeys; i++ )
			if ( m_pLogs[ i ] ) m_pLogs[ i ]->valid = 0;

	// Where will this block start?
	CFile::t_size pos = (oexUINT)fData.GetPtrPos();

	// Validate last entry
	if ( m_pLogs[ x_nKey ]->plast )
	{	SValueIndex vi;
		CFile::CRestoreFilePos rfp( &fData );
		fData.SetPtrPosBegin( m_pLogs[ x_nKey ]->plast );
		if ( !fData.Read( &vi, sizeof( vi ) ) || m_pLogs[ x_nKey ]->hash.Data1 != vi.uHash )
			m_pLogs[ x_nKey ]->plast = 0;
	} // end if

	// Should we resume?
	if ( !m_pLogs[ x_nKey ]->plast )
	{
		// Get root index time
		t_time tRootTime = pVi->uTime - ( pVi->uTime % m_tIndexStep );

		// Resume existing index
		SIterator itR; itR.Init( m_sRoot, m_pLogs[ x_nKey ]->sName.Ptr() );
		if ( FindValue( itR, tRootTime, 0, 0, eDtNone, eMethodNone, m_tLogBase, m_tIndexStep ) )
		{	m_pLogs[ x_nKey ]->plast = itR.pos;
			while ( itR.viNext.uNext > itR.pos )
			{	itR.pos = itR.viNext.uNext;
				itR.fData.SetPtrPosBegin( itR.viNext.uNext );
				if ( !itR.fData.Read( &itR.viNext, sizeof( itR.viNext ) ) || itR.hash.Data1 != itR.viNext.uHash )
					itR.viNext.uNext = 0;
				else
					m_pLogs[ x_nKey ]->plast = itR.pos;
			} // end while
		} // end if

	} // end if

	// Write data to file
	if ( !fData.Write( pVi, sizeof( SValueIndex ) ) || !fData.Write( pBuf, uSize ) )
		return oexFALSE;

	// Update the index
	CFile fIdx;
	if ( OpenDb( oexTRUE, m_sRoot, m_pLogs[ x_nKey ]->sHash, pVi->uTime, &fIdx, oexNULL, m_tLogBase, m_tIndexStep, bNewDb ) )
	{
		t_time tI = ( pVi->uTime % m_tLogBase ) / m_tIndexStep;

		// Point to index position
		fIdx.SetPtrPosBegin( tI * sizeof( pos ) );

		// Update the index if blank
		CFile::t_size ex = 0;
		if ( fIdx.Read( &ex, sizeof( ex ) ) && !ex )
			ex = pos + m_pLogs[ x_nKey ]->bin.getOffset(), 
			fIdx.SetPtrPosBegin( tI * sizeof( pos ) ),
			fIdx.Write( &ex, sizeof( ex ) );

	} // end if

	// Ensure key name file
	if ( m_sRoot.Length() )
	{ 	t_time tTime = pVi->uTime;
		CStr sRoot = m_sRoot;
		sRoot.BuildPath( tTime / m_tLogBase );
		m_pLogs[ x_nKey ]->sFile = sRoot.BuildPath( m_pLogs[ x_nKey ]->sHash ) << oexT( ".txt" );
		if ( m_pLogs[ x_nKey ]->sFile.Length() && !oexExists( m_pLogs[ x_nKey ]->sFile.Ptr() ) )
			CFile().CreateAlways( m_pLogs[ x_nKey ]->sFile.Ptr() ).Write( oexStrToMbPtr( m_pLogs[ x_nKey ]->sName ) );
	} // end write name

	return oexTRUE;
}

oexBOOL CDataLog::Flush( t_time x_tTime )
{_STT();

	// Punt if no root folder
	if ( !m_sRoot.Length() )
		return oexFALSE;

	// Get timestamp if needed
	if ( !x_tTime )
		x_tTime = oexGetUnixTime();

	// Open the database
	CFile fData; oexBOOL bNewDb = oexFALSE;
	if ( !OpenDb( oexTRUE, m_sRoot, oexT( "" ), x_tTime, oexNULL, &fData, m_tLogBase, m_tIndexStep, bNewDb ) )
		return oexFALSE;

	// Invalidate timers if new database
	if ( bNewDb )
		for( oexINT i = 0; i < eMaxKeys; i++ )
			if ( m_pLogs[ i ] ) m_pLogs[ i ]->valid = 0;

	// Flush data to disk
	for( oexINT i = 0; i < eMaxKeys; i++ )
		if ( m_pLogs[ i ] && m_pLogs[ i ]->bin.getUsed() )
		{
			// Where will this block start?
			CFile::t_size pos = (oexUINT)fData.GetPtrPos();

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
				t_time tRootTime = x_tTime - ( x_tTime % m_tIndexStep );

				// Resume existing index
				SIterator itR; itR.Init( m_sRoot, m_pLogs[ i ]->sName.Ptr() );
				if ( FindValue( itR, tRootTime, 0, 0, eDtNone, eMethodNone, m_tLogBase, m_tIndexStep ) )
				{	m_pLogs[ i ]->plast = itR.pos;
					while ( itR.viNext.uNext > itR.pos )
					{	itR.pos = itR.viNext.uNext;
						itR.fData.SetPtrPosBegin( itR.viNext.uNext );
						if ( !itR.fData.Read( &itR.viNext, sizeof( itR.viNext ) ) || itR.hash.Data1 != itR.viNext.uHash )
							itR.viNext.uNext = 0;
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
				CFile fIdx; oexBOOL bNewDb = oexFALSE;
				if ( OpenDb( oexTRUE, m_sRoot, m_pLogs[ i ]->sHash, x_tTime, &fIdx, oexNULL, m_tLogBase, m_tIndexStep, bNewDb ) )
				{
					// While we have blocks
					CFile::t_size ex = 0;
					t_time tLast = oexMAXLONG;
					while ( m_pLogs[ i ]->bin.getUsed() >= sizeof( SValueIndex ) )
					{	
						// See if this block should go in the index
						SValueIndex *pvi = (SValueIndex*)m_pLogs[ i ]->bin.Ptr();
						t_time tI = ( pvi->uTime % m_tLogBase ) / m_tIndexStep;
						if ( oexMAXLONG == tLast || tLast < tI )
						{
							tLast = tI;

							// Point to index position
							fIdx.SetPtrPosBegin( tI * sizeof( pos ) );

							// +++ Change to only read the index if we haven't written it

							// Update the index if blank
							if ( fIdx.Read( &ex, sizeof( ex ) ) && !ex )
								ex = pos + m_pLogs[ i ]->bin.getOffset(), 
								fIdx.SetPtrPosBegin( tI * sizeof( pos ) ),
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

			// Ensure key name file
			if ( m_sRoot.Length() )
			{ 	CStr sRoot = m_sRoot;
				sRoot.BuildPath( x_tTime / m_tLogBase );
				m_pLogs[ i ]->sFile = sRoot.BuildPath( m_pLogs[ i ]->sHash ) << oexT( ".txt" );
				if ( m_pLogs[ i ]->sFile.Length() && !oexExists( m_pLogs[ i ]->sFile.Ptr() ) )
					CFile().CreateAlways( m_pLogs[ i ]->sFile.Ptr() ).Write( oexStrToMbPtr( m_pLogs[ i ]->sName ) );
			} // end write name

		} // end if
	return oexTRUE;
}

CPropertyBag CDataLog::GetKeyList( t_time x_tTime )
{_STT();

	// Ensure we have a root
	if ( !m_sRoot.Length() )
		return CPropertyBag();

	// Use current time if not specified
	if ( !x_tTime )
		x_tTime = oexGetUnixTime();

	// Build root to data based on timestamp
	CStr sRoot = m_sRoot;
	sRoot.BuildPath( x_tTime / m_tLogBase );
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

oexBOOL CDataLog::IsKeyData( CStr x_sRoot, CStr x_sHash, t_time x_tTime, t_time uLogBase )
{_STT();
	if ( !x_tTime )
		x_tTime = oexGetUnixTime();

	// Build root to data based on starting timestamp
	if ( !oexExists( ( x_sRoot.BuildPath( x_tTime / uLogBase ).BuildPath( x_sHash ) << oexT( ".bin" ) ).Ptr() ) )
		return oexFALSE;
	
	return oexTRUE;
}

oexBOOL CDataLog::OpenDb( oexBOOL x_bCreate, CStr x_sRoot, CStr x_sHash, t_time x_tTime, CFile *x_pIdx, CFile *x_pData, t_time uLogBase, t_time uIndexStep, oexBOOL &bNew )
{_STT();

	// Assume existing
	bNew = oexFALSE;

	// Build root to data based on starting timestamp
	x_sRoot.BuildPath( x_tTime / uLogBase );
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
			bNew = oexTRUE, x_pData->Write( CStr8( "Hey, you, get off of my cloud." ) );

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
		{	bNew = oexTRUE;
			CFile::t_size uOffset = 0;
			for ( oexUINT i = 0; i < uLogBase; i += uIndexStep )
				x_pIdx->Write( &uOffset, sizeof( uOffset ) );
		} // end if

	} // end if

	return oexTRUE;
}

oexBOOL CDataLog::FindValue( SIterator &x_it, t_time x_tTime, t_time x_tTimeMs, t_time x_tInterval, oexINT x_nDataType, oexINT x_nMethod, t_time tLogBase, t_time tIndexStep )
{_STT();
	// Get db metrics
	t_time _tB = x_tTime / tLogBase;
	t_time _tI = ( x_tTime % tLogBase ) / tIndexStep;
	oexBOOL bNew = oexFALSE;

	// Ensure we have the correct database open
	if ( oexMAXLONG == x_it.uB || _tB != x_it.uB )
	{	
		// Save open db index
		x_it.uB = _tB;
		x_it.uI = oexMAXLONG;

		// Open the database containing data for this time
		oexBOOL bNewDb = oexFALSE;
		if ( !OpenDb( oexFALSE, x_it.sRoot, x_it.sHash, x_tTime, &x_it.fIdx, &x_it.fData, tLogBase, tIndexStep, bNewDb ) )
			return oexFALSE;

	} // end if

	// Time to index?
	if ( oexMAXLONG == x_it.uI || _tI != x_it.uI )
	{
		// Save index offset
		x_it.uI = _tI;

		// New read
		bNew = oexTRUE;

		// Point to index position
		x_it.fIdx.SetPtrPosBegin( x_it.uI * sizeof( CFile::t_size ) );

		// Update the index if blank
		CFile::t_size ex;
		if ( x_it.fIdx.Read( &ex, sizeof( ex ) ) && ex )
			x_it.npos = ex;

	} // end if

	// Do we have more data?
	if ( !x_it.npos )
		return ( !x_tInterval ) ? oexFALSE : ( x_it.pos ? oexTRUE : oexFALSE );

	// Is our last value still valid?
	if ( x_tInterval && x_it.pos )
		if ( x_it.viNext.uTime > x_tTime || ( x_it.viNext.uTime == x_tTime && x_it.viNext.uTimeMs > x_tTimeMs ) )
			return oexTRUE;

	// Do we need new data?
	CFile::t_size p = 0;
	t_time tMin = x_tTime - ( x_tInterval / 1000 );
	while ( p < x_it.npos )
	{	
		// Read the header
		p = x_it.npos;
		x_it.fData.SetPtrPosBegin( p );
		if ( !x_it.fData.Read( &x_it.viNext, sizeof( x_it.viNext ) ) || x_it.hash.Data1 != x_it.viNext.uHash )
			p = x_it.npos = 0;

		// Is it after the time we're looking for?
		else if ( x_it.viNext.uTime > x_tTime || ( x_it.viNext.uTime == x_tTime && x_it.viNext.uTimeMs > x_tTimeMs ) )
			x_it.npos = p;

		else
		{
			// Copy structure
			oexMemCpy( &x_it.vi, &x_it.viNext, sizeof( x_it.vi ) );

			// Save position
			x_it.pos = p;
			x_it.npos = x_it.viNext.uNext;

			// Average values if needed
			if ( !bNew && tMin <= x_it.vi.uTime && eDtString < x_nDataType && eMethodAverage & x_nMethod )
			{
				// Point to the data
				x_it.fData.SetPtrPosBegin( x_it.pos + x_it.vi.uBytes );

				// Do we want to read values?
				if ( !( oex::CDataLog::eMethodNoRead & x_nMethod ) )
				{
					// Get string value
					if ( x_it.getValue( x_it.sValue ) )
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

		} // end if

	} // end while

	// Scale averages
	if ( 0 < x_it.nCount )
			x_it.fValue /= x_it.nCount,
			x_it.nValue /= x_it.nCount,
			x_it.nCount = 0;

	else if ( x_it.pos )
	{
		// Point to the data
		x_it.fData.SetPtrPosBegin( x_it.pos + x_it.vi.uBytes );

		// Do we want to read the value?
		if ( !( oex::CDataLog::eMethodNoRead & x_nMethod ) )
		{
			// Read the value
			if ( x_it.getValue( x_it.sValue ) )
			{
				if ( eDtInt == x_nDataType )
					x_it.nValue = x_it.sValue.ToInt();

				else if ( eDtFloat == x_nDataType )
					x_it.fValue = x_it.sValue.ToFloat();

			} // end if

		} // end if

	} // end else

	return oexTRUE;
}

CPropertyBag CDataLog::GetLog( oexCSTR x_pKey, t_time x_tStart, t_time x_tEnd, t_time x_tInterval, oexINT x_nDataType, oexINT x_nMethod )
{_STT();

	// Unbounded?
	oexBOOL bUnbounded = ( !x_tEnd ) ? oex::oexTRUE : oex::oexFALSE;

	// Calculate proper start / stop times
	CSysTime::CalculateTimes( &x_tStart, &x_tEnd, 60 );

	// Sanity checks
	if ( !x_pKey || !*x_pKey || !m_sRoot.Length() || x_tStart > x_tEnd )
		return CPropertyBag();

	// Don't read data if we're just returning the size
	if ( eDtSize == x_nDataType )
		x_nMethod |= eMethodNoRead;

	SIterator it;
	if ( !it.Init( m_sRoot, x_pKey ) || !it.IsData( m_tLogBase, x_tStart ) )
		return CPropertyBag();

	// Align with the interval
	t_time tAlign = x_tInterval / 1000;
	if ( tAlign )
		x_tStart -= ( x_tStart % tAlign ),
		x_tEnd -= ( x_tEnd % tAlign );

	// Just so the averaging is correct, use one interval before the start
	if ( x_tInterval && eMethodAverage & x_nMethod )
	{	oexINT64 t = (oexINT64)x_tStart * 1000ll - (oexINT64)x_tInterval;
		t_time s = t / 1000;
		t_time ms = t % 1000;
		FindValue( it, s, ms, x_tInterval, x_nDataType, x_nMethod, m_tLogBase, m_tIndexStep );
	} // end if

	// Create data
	CPropertyBag pb;
	oexUINT uN = 0;
	t_time tTime = x_tStart, tTimeMs = 0;
	while ( ( bUnbounded || tTime <= x_tEnd ) && uN < m_uLimit )
	{
		// Count a sample
		uN++;

		// Find the value for this time
		if ( FindValue( it, tTime, tTimeMs, x_tInterval, x_nDataType, x_nMethod, m_tLogBase, m_tIndexStep ) )
		{
			if ( !x_tInterval )
			{
				// Ensure it falls within our time range
				if ( it.vi.uTime >= x_tStart &&
					 ( bUnbounded || ( it.vi.uTime < x_tEnd || ( it.vi.uTime == x_tEnd && !it.vi.uTimeMs ) ) ) )
				{
					if ( eDtSize == x_nDataType )
						it.sValue = it.vi.uSize;

					if ( it.vi.uTimeMs )
						pb[ oexFmt( oexT( "%u.%.3u" ), it.vi.uTime, it.vi.uTimeMs ) ].ToString() = it.sValue;
					else
						pb[ it.vi.uTime ].ToString() = it.sValue;
				} // end if

			} // end if
			
			else if ( tTimeMs )
				pb[ oexFmt( oexT( "%u.%.3u" ), tTime, tTimeMs ) ].ToString() = it.sValue;

			else
				pb[ tTime ].ToString() = it.sValue;

		} // end if

		else if ( bUnbounded )
			return pb;
		
		else if ( !x_tInterval )
			;

		else if( tTimeMs )
			pb[ oexFmt( oexT( "%u.%.3u" ), tTime, tTimeMs ) ] = oexT( "" );

		else
			pb[ tTime ].ToString() = oexT( "" );

		// Calculate next timestamp
		if ( !x_tInterval )
		{
			// Next index?
			if ( it.npos <= it.pos )
			{	tTime += m_tIndexStep;
				tTime -= tTime % m_tIndexStep;
				tTimeMs = 0;
			} // end if

			else
			{	tTime = it.viNext.uTime;
				tTimeMs = it.viNext.uTimeMs;
			} // end else

		} // end if

		else
		{	// Update time
			tTimeMs += x_tInterval;
			tTime += tTimeMs / 1000;
			tTimeMs %= 1000;
		} // end else

	} // end for

	return pb;
}

CStr CDataLog::GetLogBin( oexCSTR x_pKey, t_time x_tStart, t_time x_tEnd, t_time x_tInterval, oexINT x_nDataType, oexINT x_nMethod, float x_fScale )
{_STT();

	// Unbounded?
	oexBOOL bUnbounded = ( !x_tEnd ) ? oex::oexTRUE : oex::oexFALSE;

	// Calculate proper start / stop times
	CSysTime::CalculateTimes( &x_tStart, &x_tEnd, 60 );

	// Sanity checks
	if ( !x_pKey || !*x_pKey || !m_sRoot.Length() || x_tStart > x_tEnd || !x_tInterval )
		return oexT( "" );

	// Set defaults
	if ( eDtNone == x_nDataType ) 
		x_nDataType = eDtFloat;
	if ( eMethodNone == x_nMethod ) 
		x_nMethod = eMethodAverage;
	if ( !x_fScale )
		x_fScale = 1.f;

	SIterator it;
	if ( !it.Init( m_sRoot, x_pKey ) || !it.IsData( m_tLogBase, x_tStart ) )
		return oexT( "" );

	// Align with the interval
	t_time tAlign = x_tInterval / 1000;
	if ( tAlign )
		x_tStart -= ( x_tStart % tAlign ),
		x_tEnd -= ( x_tEnd % tAlign );

	// Just so the averaging is correct, use one interval before the start
	if ( eMethodAverage & x_nMethod )
	{	oexINT64 t = (oexINT64)x_tStart * 1000ll - (oexINT64)x_tInterval;
		t_time s = t / 1000;
		t_time ms = t % 1000;
		FindValue( it, s, ms, x_tInterval, x_nDataType, x_nMethod, m_tLogBase, m_tIndexStep );
	} // end if

	// Calculate number of positions needed
	oexUINT nItems = ( x_tEnd - x_tStart ) * 1000 / x_tInterval;
	if ( !nItems )
		return oexT( "" );

	// Ensure we don't go over the maximum limit
	if ( nItems > m_uLimit )
		nItems = m_uLimit;

	// Allocate space for binary data
	CBin bin( nItems * sizeof( float ), nItems * sizeof( float ) );
	if ( !bin.Ptr() )
		return oexT( "" );

	// Create data
	CPropertyBag pb;
	oexUINT uN = 0;
	t_time tTime = x_tStart, tTimeMs = 0, i = 0;
	while ( i < nItems && ( bUnbounded || tTime <= x_tEnd ) )
	{
		// Count a sample
		uN++;

		// Find the value for this time
		if ( FindValue( it, tTime, tTimeMs, x_tInterval, x_nDataType, x_nMethod, m_tLogBase, m_tIndexStep ) )
			bin.setFLOAT( i++, it.fValue * x_fScale );
		else
			bin.setFLOAT( i++, 0.f );

		// Update time
		tTimeMs += x_tInterval;
		tTime += tTimeMs / 1000;
		tTimeMs %= 1000;

	} // end for

	// The number of valid items	
	bin.setUsed( i ? ( i - 1 ) * sizeof( float ) : 0 );

	// Set the binary share
	CStr sId = oexGuidToString();
	oexSetBin( sId, &bin );

	return sId;
}
