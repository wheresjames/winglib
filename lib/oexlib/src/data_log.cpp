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

oexBOOL CDataLog::SetRoot( oexCSTR x_pRoot, oexBOOL x_bChangesOnly )
{_STT();

	// Ensure valid root folder
	if ( !x_pRoot || !*x_pRoot || !oexExists( x_pRoot ) )
		return oexFALSE;

	// Save path to root folder
	m_sRoot = x_pRoot;

	m_bChangesOnly = x_bChangesOnly;

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

oexINT CDataLog::AddKey( oexCSTR x_pKey )
{_STT();

	// Ensure valid key name
	if ( !x_pKey || !*x_pKey )
		return -1;

	// Add a new logging key
	for( oexINT i = 0; i < eMaxKeys; i++ )
		if ( !m_pLogs[ i ] )
		{	m_pLogs[ i ] = OexAllocConstruct< SLogKey >();
			if ( !m_pLogs[ i ] ) return -1;
			m_pLogs[ i ]->bin.Allocate( eInitBuffer );
			m_pLogs[ i ]->sName = x_pKey;
			oex::oexGUID hash; oex::CStr8 sMb = oexStrToMb( m_pLogs[ i ]->sName );
			m_pLogs[ i ]->sHash = oexMbToStr( oex::CBase16::Encode( oex::oss::CMd5::Transform( &hash, sMb.Ptr(), sMb.Length() ), sizeof( hash ) ) );
			oexZero( m_pLogs[ i ]->hash );
			m_pLogs[ i ]->plast = 0;
			m_pLogs[ i ]->olast = 0;
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

	// First, determine if the data has changed since we last logged it
	oex::oexGUID hash;
	oex::oss::CMd5::Transform( &hash, x_pValue, x_uSize );
	if ( oex::guid::CmpGuid( &hash, &m_pLogs[ x_nKey ]->hash ) )
		return oexTRUE;

	// Copy the guid
	oex::guid::CopyGuid( &m_pLogs[ x_nKey ]->hash, &hash );

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
	vi.uNext = 0;

	// Save pointer to this structure
	m_pLogs[ x_nKey ]->olast = m_pLogs[ x_nKey ]->bin.getUsed();

	// Write the value to the memory log
	if ( !m_pLogs[ x_nKey ]->bin.AppendBuffer( (oex::CBin::t_byte*)&vi, vi.uBytes ) 
		 || !m_pLogs[ x_nKey ]->bin.AppendBuffer( (oex::CBin::t_byte*)x_pValue, x_uSize ) )
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

	// Create root folder
	CStr sRoot = m_sRoot;
	sRoot.BuildPath( x_uTime / eLogBase );
	oexCreatePath( sRoot.Ptr() );

	CStr sData = sRoot; sData.BuildPath( "data.bin" );
	oexBOOL x_bExists = oexExists( sData.Ptr() );

	oex::CFile fData;
	if ( !fData.OpenAlways( sData.Ptr() ).IsOpen() )
		return 0;

	// Don't let the file have a zero length
	if ( !x_bExists )
		fData.Write( CStr8( "Hey, you, get off of my cloud." ) );

	// Move to the end of the file
	fData.SetPtrPosEnd( 0 );

	// Flush data to disk
	for( oexINT i = 0; i < eMaxKeys; i++ )
		if ( m_pLogs[ i ] && m_pLogs[ i ]->bin.getUsed() )
		{
			// Where will this block start?
			CFile::t_size pos = (oexUINT)fData.GetPtrPos();

			// Update prev/next	pointers
			oexUINT n = 0;
			while ( m_pLogs[ i ]->bin.getUsed() >= sizeof( SValueIndex ) )
			{	
				// See if this block should go in the index
				SValueIndex *pvi = (SValueIndex*)m_pLogs[ i ]->bin.Ptr();

				// Track prev/next pointers
				if ( !n++ )
				{
					// Do we need to update a previous item?
					if ( m_pLogs[ i ]->plast )
					{	SValueIndex vi;
						oex::CFile::CRestoreFilePos rfp( &fData );
						fData.SetPtrPosBegin( m_pLogs[ i ]->plast );
						if ( fData.Read( &vi, sizeof( vi ) ) )
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
					m_pLogs[ i ]->plast = pos + m_pLogs[ i ]->bin.getOffset();
				} // end else

				// Skip this block
				m_pLogs[ i ]->bin.setOffset( m_pLogs[ i ]->bin.getOffset() + pvi->uBytes + pvi->uSize ); 

			} // end while

			// Back to zero
			m_pLogs[ i ]->bin.setOffset( 0 );

			// Write data to file
			if ( fData.Write( m_pLogs[ i ]->bin.Ptr(), m_pLogs[ i ]->bin.getUsed() ) )
			{
				// Update the index file
				CStr sIndex = sRoot; 
				sIndex.BuildPath( m_pLogs[ i ]->sHash );
				sIndex += oexT( ".bin" );
				oex::CFile fIdx;

				// Initialize the index if needed
				if ( !fIdx.OpenExisting( sIndex.Ptr() ).IsOpen() )
					if ( fIdx.OpenAlways( sIndex.Ptr() ).IsOpen() )
					{	CFile::t_size uOffset = 0;
						for ( oexINT i = 0; i < eLogBase; i += eIndexStep )
							fIdx.Write( &uOffset, sizeof( uOffset ) );
					} // end if

				// Update the index
				if ( fData.IsOpen() )
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
