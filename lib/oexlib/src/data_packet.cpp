/*------------------------------------------------------------------
// data_packet.cpp
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


CDataPacket::CDataPacket()
{
	m_uEncode = 0;
	m_bValidPacket = oexFALSE;
}

CDataPacket::CDataPacket( oexUINT uSize ) :
				CCircBuf( oexTRUE, uSize )
{
	m_uEncode = 0;
	m_bValidPacket = oexFALSE;
}

CDataPacket::~CDataPacket()
{
	Destroy();
}

void CDataPacket::Destroy()
{
	m_bValidPacket = oexFALSE;
    CCircBuf::Destroy();    
}

// *** Packet Writing ***

oexBOOL CDataPacket::WritePacket(oexUINT x_uPacketType, oexUINT x_uDataType, oexCPVOID x_pData, oexUINT x_uData)
{
	// Lock the buffer
	CTlLocalLock ll( *this );
	if ( !ll.IsLocked() ) return oexFALSE;

	// Initialize packet
	if ( !InitPacket( x_uPacketType, 1, x_uData ) )
		return oexFALSE;

	// Add the users data
	if ( !AddPacketData( x_uDataType, x_pData, x_uData ) )
		return oexFALSE;

	// Add end packet data
	return EndPacket();
}

oexBOOL CDataPacket::vWriteMultiPacket( oexUINT x_uPacketType, oexUINT x_uBuffers, oexPVOID *x_pArgs )
{
	// Lock the buffer
	CTlLocalLock ll( *this );
	if ( !ll.IsLocked() ) return oexFALSE;

	// Variable params
	oexPVOID *ptrExtra = x_pArgs;

	// Calculate the total size needed
	oexUINT uTotalSize = 0, i;
	for ( i = 0; i < x_uBuffers; i++ )
	{
		OEX_TRY // This could GPF if caller screws up
		{
//			oexUINT	    uType = *(oexUINT*)ptrExtra;
//			oexUCHAR	*pPtr = *(oexUCHAR**)( ptrExtra + 1 );
			oexUINT 	uSize = *(oexUINT*)( ptrExtra + 2 );

			// Track the total size
			uTotalSize += uSize;

			// Skip these parameters
			ptrExtra += 3;

		} // end try
		OEX_CATCH_ALL 
        { 
            oexASSERT( 0 );
            return oexFALSE; 
        }

	} // end for

	// Initialize packet
	if ( !InitPacket( x_uPacketType, x_uBuffers, uTotalSize ) )
		return oexFALSE;

	// Now add the data
	ptrExtra = x_pArgs;
	for ( i = 0; i < x_uBuffers; i++ )
	{
		OEX_TRY // This could GPF if caller screws up
		{
			oexUINT	    uType = *(oexUINT*)ptrExtra;
			oexUCHAR	*pPtr = *(oexUCHAR**)( ptrExtra + 1 );
			oexUINT 	uSize = *(oexUINT*)( ptrExtra + 2 );

			// Add the users data
			if ( !AddPacketData( uType, pPtr, uSize ) )
				return oexFALSE;

			// Skip these parameters
			ptrExtra += 3;

		} // end try
		OEX_CATCH_ALL 
        {
            oexASSERT( 0 );
            return oexFALSE; 
        }

	} // end for

	// Add end packet data
	return EndPacket();

}

oexBOOL CDataPacket::InitPacket( oexUINT x_uType, oexUINT x_uDataBlocks, oexUINT x_uTotalDataSize)
{
	SPacketHeader ph;

	// Initialize the check sum
    m_md5.Init();

	// Initialize the packet data
	ph.dwId = eHeaderId;

	// Calculate the total length of the packet
	ph.dwLength =	GetMinimumPacketOverhead() +
					( x_uDataBlocks * sizeof( SDataHeader ) ) + 
					x_uTotalDataSize;


	// The type of packet
	ph.dwType = x_uType;

	// Number of data blocks
	ph.dwDataBlocks = x_uDataBlocks;

	// Prepare to poke the buffer
	InitPoke();

	// Write data into the packet
	return WritePacketData( &ph, sizeof( ph ));
}

oexBOOL CDataPacket::AddPacketData( oexUINT x_uType, oexCPVOID x_pData, oexUINT x_uSize )
{
	SDataHeader dh;
	dh.dwLength = x_uSize;
	dh.dwType = x_uType;

	// Write the data block header
	if ( !WritePacketData( &dh, sizeof( dh ) ) )
		return oexFALSE;

	// Write the data
	return WritePacketData( x_pData, x_uSize, m_uEncode );
}

oexBOOL CDataPacket::WritePacketData(oexCPVOID x_pData, oexUINT x_uSize, oexUINT x_uEncode )
{
	// Write out the packet header
	if ( !Poke( x_pData, x_uSize, x_uEncode ) ) return oexFALSE;

	return oexTRUE;
}

oexBOOL CDataPacket::OnInspectWrite( oexUINT x_uBlock, oexUCHAR *x_pBuf, oexUINT x_uSize )
{
	// Update the md5 hash
    m_md5.Update( x_pBuf, x_uSize );

	return oexTRUE;
}


oexBOOL CDataPacket::EndPacket()
{
	oexGUID cs;

	// Get the md5
    m_md5.Final( &cs );

	// Write out the check sum
	if ( !Poke( &cs, sizeof( cs ) ) ) 
        return oexFALSE;

	// Commit the data
	EndPoke();

	return oexTRUE;
}

// *** Packet Reading ***

oexBOOL CDataPacket::ReadPacket(oexCPVOID x_pBuf, oexUINT x_uSize)
{
	// First write the data into the buffer
	Write( x_pBuf, x_uSize );

	// Attempt to verify a packet in the buffer
	return VerifyPacket();
}

oexBOOL CDataPacket::SkipPacket()
{
	// Do we have a packet
	if ( !m_bValidPacket )

		// Can we get one?
		if ( !VerifyPacket() ) return oexFALSE;

	// Skip over this packet
	AdvanceReadPtr( m_ph.dwLength );

	// Not valid now
	m_bValidPacket = oexFALSE;

	// Attempt to find another packet
	return VerifyPacket();
}

oexBOOL CDataPacket::FindPacket( LPSPacketHeader pPh, oexUINT *x_puAvailable )
{
	// How many bytes are availble
	oexUINT uAvailable = GetMaxRead();

	do
	{
		// Do we have enough data for a packet?
		if ( uAvailable < GetMinimumPacketOverhead() )
			return oexFALSE;

		// Take a sneak at the header
		oexUINT uRead = 0;
		if (	!Peek( pPh, sizeof( SPacketHeader ), &uRead ) || 
				uRead != sizeof( SPacketHeader ) )
			return oexFALSE;

		// Is this a valid header id?
		if ( pPh->dwId == eHeaderId )
		{	if ( x_puAvailable ) *x_puAvailable = uAvailable;
			return oexTRUE;
		} // end if

		// Should be one less byte in the buffer
		uAvailable--;

	// Skip ahead one byte
	} while ( AdvanceReadPtr( 1 ) );

	return oexFALSE;
}

oexBOOL CDataPacket::VerifyPacket()
{
	// Do we already know there is a valid packet?
	if ( m_bValidPacket ) 
        return oexTRUE;

	oexUINT uAvailable = 0;

	do
	{
		// Attempt to find a packet in the buffer
		if ( !FindPacket( &m_ph, &uAvailable ) )
			return oexFALSE;

		// Verify the length of the packet is available
		if ( m_ph.dwLength > uAvailable ) 
            return oexFALSE;

		// Enforce maximum packet size
		if ( m_ph.dwLength <= GetMaxSize() )
		{
			oexUCHAR    *pBuf = oexNULL;
			oexUINT     uSize = 0, uView = 0;

			// Initialize the check sum
			oexGUID csCur, csBuf;
            oss::CMd5 md5;

			// Calculate the md5 hash
			while ( GetReadView( uView++, 0, m_ph.dwLength - sizeof( oexGUID ), &pBuf, &uSize ) )

				// Update the md5 hash
				md5.Update( pBuf, uSize );

			// What is our calculated checksum?
            md5.Final( &csCur );

			// Read the checksum from the buffer
			Peek( &csBuf, sizeof( csBuf ), oexNULL, m_ph.dwLength - sizeof( oexGUID ) );

			// Do the checksums match?
            if ( guid::CmpGuid( &csCur, &csBuf ) )
			{	m_bValidPacket = oexTRUE;
				return oexTRUE;
			} // end if

		} // end if

	// Skip over the id
	} while ( AdvanceReadPtr( 4 ) );

	return oexFALSE;
}
                                                                 
oexBOOL CDataPacket::ReadPacketData(oexUINT x_uBlock, oexUINT x_uType, oexPVOID x_pBuf, oexUINT x_uMax, oexUINT *x_puRead, oexLONG x_lOffset)
{
	// Ensure valid packet
	if ( !m_bValidPacket ) return oexFALSE;

	// Is it a valid data block index?
	if ( x_uBlock > m_ph.dwDataBlocks ) return oexFALSE;

	// Where to start looking
	oexUINT uOffset = sizeof( SPacketHeader );

	// We must find the block
	for ( ; ; )
	{
		SDataHeader dh;

		// Peek at the First data block
		oexUINT uRead = 0;
		if ( !Peek( &dh, sizeof( dh ), &uRead, uOffset ) || uRead != sizeof( dh ) ) 
			return oexFALSE;

		// Skip over the header
		uOffset += sizeof( SDataHeader );

		// Test for unreasonable size
		if ( dh.dwLength > m_ph.dwLength ) return oexFALSE;

		// Is this is the block type we're looking for?
		if ( x_uType == 0 || dh.dwType == x_uType )
		{
			// Is this our block
			if ( !x_uBlock )
			{
				// Is the offset too much?
				if ( (oexUINT)x_lOffset > dh.dwLength ) return oexFALSE;

				// Subtrack the offset
				dh.dwLength -= x_lOffset;

				// Do they just want to know how many bytes or are there none?
				if ( x_pBuf == oexNULL || x_uMax == 0 || dh.dwLength == 0 )
				{	if ( x_puRead ) *x_puRead = dh.dwLength; return oexTRUE; }

				// Truncate length if needed
				if ( dh.dwLength > x_uMax ) dh.dwLength = x_uMax;

				// Get the data
				return Peek( x_pBuf, dh.dwLength, x_puRead, uOffset + x_lOffset, m_uEncode );

			} // end if

			// Count a block
			else x_uBlock--;

		} // end if

		// Skip the data
		uOffset += dh.dwLength;
	}

	return oexFALSE;
}

oexBOOL CDataPacket::GetPacketDataHash( oexGUID *x_pGuid, oexUINT x_uBlock, oexUINT x_uType )
{
	// Ensure guid pointer
	if ( x_pGuid == oexNULL ) return oexFALSE;

	// Ensure valid packet
	if ( !m_bValidPacket ) return oexFALSE;

	// Is it a valid data block index?
	if ( x_uBlock > m_ph.dwDataBlocks ) return oexFALSE;

	// Where to start looking
	oexUINT uOffset = sizeof( SPacketHeader );

	// We must find the block
	for ( ; ; )
	{
		SDataHeader dh;

		// Peek at the First data block
		oexUINT uRead = 0;
		if ( !Peek( &dh, sizeof( dh ), &uRead, uOffset ) || uRead != sizeof( dh ) ) 
			return oexFALSE;

		// Skip over the header
		uOffset += sizeof( SDataHeader );

		// Test for unreasonable size
		if ( dh.dwLength > m_ph.dwLength ) return oexFALSE;

		// Is this is the block type we're looking for?
		if ( x_uType == 0 || dh.dwType == x_uType )
		{
			// Is this our block
			if ( !x_uBlock )
			{
				// Initialize MD5 Hash
                oss::CMd5 md5;

				// Hash data in place
				oexUCHAR *pBuf = oexNULL;
				oexUINT uView = 0, uSize = 0;
				while ( GetReadView( uView++, 0, dh.dwLength, &pBuf, &uSize ) )

                    md5.Update( pBuf, uSize );

				// Get the hash
				md5.Final( x_pGuid );

				return oexTRUE;

			} // end if

			// Count a block
			else x_uBlock--;

		} // end if

		// Skip the data
		uOffset += dh.dwLength;

	} // end forever

	return oexFALSE;
}

oexBOOL CDataPacket::VerifyPacketData( oexUINT x_uBlock, oexUINT x_uType, oexCONST oexGUID *x_pGuid )
{
	oexGUID guid;

	// Verify the data hash
	if ( 	!x_pGuid ||
			!GetPacketDataHash( &guid, x_uBlock, x_uType ) ||
            !guid::CmpGuid( &guid, x_pGuid ) ) return oexFALSE;
	
	return oexTRUE;

}
