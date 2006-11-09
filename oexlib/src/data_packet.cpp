/*------------------------------------------------------------------
// data_packet.cpp
// Copyright (c) 2006 
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

#include "stdafx.h"

OEXLIB_USING_NAMESPACE

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataPacket::CDataPacket()
{
	m_uEncode = 0;
	m_bValidPacket = oexFALSE;
}

CDataPacket::CDataPacket( oexUINT dwSize ) :
				CCircBuf( oexTRUE, dwSize )
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
}

// *** Packet Writing ***

oexBOOL CDataPacket::WritePacket(oexUINT x_uPacketType, oexUINT x_uDataType, oexPVOID x_pData, oexUINT x_uData)
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
		OEXLIB_TRY // This could GPF if caller screws up
		{
			oexUINT		uType = *(oexUINT*)ptrExtra;
			oexUCHAR	*pPtr = *(oexUCHAR**)( ptrExtra + 1 );
			oexUINT		uSize = *(oexUINT*)( ptrExtra + 2 );

			// Track the total size
			uTotalSize += uSize;

			// Skip these parameters
			ptrExtra += 3;

		} // end try
		OEXLIB_CATCH_ALL { return oexFALSE; }

	} // end for

	// Initialize packet
	if ( !InitPacket( x_uPacketType, x_uBuffers, uTotalSize ) )
		return oexFALSE;

	// Now add the data
	ptrExtra = x_pArgs;
	for ( i = 0; i < x_uBuffers; i++ )
	{
		OEXLIB_TRY // This could GPF if caller screws up
		{
			oexUINT		uType = *(oexUINT*)ptrExtra;
			oexUCHAR	*pPtr = *(oexUCHAR**)( ptrExtra + 1 );
			oexUINT		uSize = *(oexUINT*)( ptrExtra + 2 );

			// Add the users data
			if ( !AddPacketData( uType, pPtr, uSize ) )
				return oexFALSE;

			// Skip these parameters
			ptrExtra += 3;

		} // end try
		OEXLIB_CATCH_ALL { return oexFALSE; }

	} // end for

	// Add end packet data
	return EndPacket();

}

oexBOOL CDataPacket::InitPacket( oexUINT x_uType, oexUINT x_uDataBlocks, oexUINT x_uTotalDataSize)
{
	SPacketHeader ph;

	// Initialize the check sum
	CMd5::MD5Init( &m_md5Context );

	// Initialize the packet data
	ph.uId = DPID_HEADER;

	// Calculate the total length of the packet
	ph.uLength =	GetMinimumPacketOverhead() +
					( x_uDataBlocks * sizeof( SDataHeader ) ) + 
					x_uTotalDataSize;


	// The type of packet
	ph.uType = x_uType;

	// Number of data blocks
	ph.uDataBlocks = x_uDataBlocks;

	// Prepare to poke the buffer
	InitPoke();

	// Write data into the packet
	return WritePacketData( &ph, sizeof( ph ));
}

oexBOOL CDataPacket::AddPacketData( oexUINT x_uType, oexPVOID x_pData, oexUINT x_uSize )
{
	SDataHeader dh;
	dh.uLength = x_uSize;
	dh.uType = x_uType;

	// Write the data block header
	if ( !WritePacketData( &dh, sizeof( dh ) ) )
		return oexFALSE;

	// Write the data
	return WritePacketData( x_pData, x_uSize, m_uEncode );
}

oexBOOL CDataPacket::WritePacketData( oexPVOID x_pData, oexUINT x_uSize, oexUINT x_uEncode )
{
	// Write out the packet header
	if ( !Poke( x_pData, x_uSize, x_uEncode ) ) return oexFALSE;

	return oexTRUE;
}

oexBOOL CDataPacket::OnInspectWrite( oexUINT x_uBlock, oexUCHAR *x_pBuf, oexUINT x_uSize )
{
	// Update the md5 hash
	CMd5::MD5Update( &m_md5Context, x_pBuf, x_uSize );

	return oexTRUE;
}


oexBOOL CDataPacket::EndPacket()
{
	SCheckSum cs;

	// Get the md5
	CMd5::MD5Final( cs.md5, &m_md5Context );

	// Write out the check sum
	if ( !Poke( &cs, sizeof( cs ) ) ) return oexFALSE;

	// Commit the data
	EndPoke();

	return oexTRUE;
}

// *** Packet Reading ***

oexBOOL CDataPacket::ReadPacket( oexPVOID x_pBuf, oexUINT x_uSize )
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
	AdvanceReadPtr( m_ph.uLength );

	// Not valid now
	m_bValidPacket = oexFALSE;

	// Attempt to find another packet
	return VerifyPacket();
}

oexBOOL CDataPacket::FindPacket(LPSPacketHeader x_pPh, oexUINT *x_puAvailable)
{
	// How many bytes are availble
	oexUINT uAvailable = GetMaxRead();

	do
	{
		// Do we have enough data for a packet?
		if ( uAvailable < GetMinimumPacketOverhead() )
			return oexFALSE;

		// Take a sneak at the header
		oexUINT dwRead = 0;
		if (	!Peek( x_pPh, sizeof( SPacketHeader ), &dwRead ) || 
				dwRead != sizeof( SPacketHeader ) )
			return oexFALSE;

		// Is this a valid header id?
		if ( x_pPh->uId == DPID_HEADER )
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
	if ( m_bValidPacket ) return oexTRUE;

	oexUINT uAvailable = 0;

	do
	{
		// Attempt to find a packet in the buffer
		if ( !FindPacket( &m_ph, &uAvailable ) )
			return oexFALSE;

		// Verify the length of the packet is available
		if ( m_ph.uLength > uAvailable ) return oexFALSE;

		// Enforce maximum packet size
		if ( m_ph.uLength <= GetMaxSize() )
		{
			oexUCHAR *pBuf = NULL;
			oexUINT  uSize = 0, uView = 0;

			// Initialize the check sum
			SCheckSum csCur, csBuf;
			CMd5::MD5Init( &m_md5Context );

			// Calculate the md5 hash
			while ( GetReadView( uView++, 0, m_ph.uLength - sizeof( SCheckSum ), &pBuf, &uSize ) )

				// Update the md5 hash
				CMd5::MD5Update( &m_md5Context, pBuf, uSize );

			// What is our calculated checksum?
			CMd5::MD5Final( csCur.md5, &m_md5Context );

			// Read the checksum from the buffer
			Peek( &csBuf, sizeof( csBuf ), NULL, m_ph.uLength - sizeof( SCheckSum ) );

			// Do the checksums match?
			if ( !memcmp( &csCur, &csBuf, sizeof( SCheckSum ) ) )
			{	m_bValidPacket = oexTRUE;
				return oexTRUE;
			} // end if

		} // end if

	// Skip over the id
	} while ( AdvanceReadPtr( 4 ) );

	return oexFALSE;
}

oexBOOL CDataPacket::ReadPacketData(oexUINT x_uBlock, oexUINT x_uType, oexPVOID x_pBuf, oexUINT x_uMax, oexUINT* x_puRead, oexLONG x_lOffset)
{
	// Ensure valid packet
	if ( !m_bValidPacket ) return oexFALSE;

	// Is it a valid data block index?
	if ( x_uBlock > m_ph.uDataBlocks ) return oexFALSE;

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
		if ( dh.uLength > m_ph.uLength ) return oexFALSE;

		// Is this is the block type we're looking for?
		if ( 0 == x_uType || dh.uType == x_uType )
		{
			// Is this our block
			if ( !x_uBlock )
			{
				// Is the offset too much?
				if ( (oexUINT)x_lOffset > dh.uLength ) return oexFALSE;

				// Subtrack the offset
				dh.uLength -= x_lOffset;

				// Do they just want to know how many bytes or are there none?
				if ( x_pBuf == NULL || x_uMax == 0 || dh.uLength == 0 )
				{	if ( x_puRead ) *x_puRead = dh.uLength; return oexTRUE; }

				// Truncate length if needed
				if ( dh.uLength > x_uMax ) dh.uLength = x_uMax;

				// Get the data
				return Peek( x_pBuf, dh.uLength, x_puRead, uOffset + x_lOffset, m_uEncode );

			} // end if

			// Count a block
			else x_uBlock--;

		} // end if

		// Skip the data
		uOffset += dh.uLength;
	}

	return oexFALSE;
}

oexBOOL CDataPacket::GetPacketDataHash( oexGUID *x_pGuid, oexUINT x_uBlock, oexUINT x_uType)
{
	// Ensure guid pointer
	if ( x_pGuid == NULL ) return oexFALSE;

	// Ensure valid packet
	if ( !m_bValidPacket ) return oexFALSE;

	// Is it a valid data block index?
	if ( x_uBlock > m_ph.uDataBlocks ) return oexFALSE;

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
		if ( dh.uLength > m_ph.uLength ) return oexFALSE;

		// Is this is the block type we're looking for?
		if ( 0 == x_uType || dh.uType == x_uType )
		{
			// Is this our block
			if ( !x_uBlock )
			{
				// Initialize MD5 Hash
				CMd5::MD5_CTX ctx;
				CMd5::MD5Init( &ctx );

				// Hash data in place
				oexUCHAR *pBuf = NULL;
				oexUINT  uView = 0, uSize = 0;
				while ( GetReadView( uView++, 0, dh.uLength, &pBuf, &uSize ) )

					// Update
					CMd5::MD5Update( &ctx, pBuf, uSize );

				// Get the hash
				CMd5::MD5Final( (oexUCHAR*)x_pGuid, &ctx );

				return oexTRUE;

			} // end if

			// Count a block
			else x_uBlock--;

		} // end if

		// Skip the data
		uOffset += dh.uLength;

	} // end forever

	return oexFALSE;
}

oexBOOL CDataPacket::VerifyPacketData(oexUINT x_uBlock, oexUINT x_uType, const oexGUID *x_pGuid)
{
	oexGUID guid;

	// Verify the data hash
	if ( 	!x_pGuid ||
			!GetPacketDataHash( &guid, x_uBlock, x_uType ) ||
			!IsEqualGUID( guid, *x_pGuid ) ) return oexFALSE;
	
	return oexTRUE;

}

