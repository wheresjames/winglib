/*------------------------------------------------------------------
// data_packet.h
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

#pragma once

//==================================================================
// CDataPacket
//
/// Provides basic serial packetizing class.
/**
	Use this class to provide serial communication packetizing.
	Useful for sending data over any serial link such as Ethernet.
*/
//==================================================================
class CDataPacket : public CCircBuf
{
public:

    enum
    {
        /// Flag set if block is encrypted
        eEncrypted      = 1,

        /// Flag set if block is compressed
        eCompressed     = 2
    };

    enum
    {
        /// Header ID
        eHeaderId       = 0x57f07e13
    };

public:

	/// This structure defines a serial packet header.
	struct SPacketHeader
	{
		/// Unique ID identifying the start of packet
		oexUINT32			dwId;

		/// The total length of the packet
		oexUINT32			dwLength;

		/// Type specifier for the packet
		oexUINT32			dwType;

		/// Number of data blocks in this packet
		oexUINT32			dwDataBlocks;

	}; typedef SPacketHeader* LPSPacketHeader;


	/// This structure defines a data chunk
	struct SDataHeader
	{
		/// The total length of the packet
		oexUINT32			dwLength;

		/// Unique ID identifying the start of packet
		oexUINT32			dwType;

	}; typedef SDataHeader* LPSDataHeader; 

public:

	//==============================================================
	// VerifyPacketData()
	//==============================================================
	/// Call this function to verify the packet data
	/**
		\param [in] x_uBlock	-	Block index
		\param [in] x_uType		-	Block type
		\param [in] x_pGuid		-	Block ID
		
		\return Non-zero if verified
	
		\see 
	*/
	oexBOOL VerifyPacketData( oexUINT x_uBlock, oexUINT x_uType, oexCONST oexGUID *x_pGuid );

	//==============================================================
	// GetPacketDataHash()
	//==============================================================
	/// Returns packet data hash
	/**
		\param [in] x_pGuid		-	Buffer to hold hash
		\param [in] x_uBlock	-	Block index we're looking for
		\param [in] x_uType		-	Block type
		
		\return Non-zero if hash was retrieved.
	
		\see 
	*/
	oexBOOL GetPacketDataHash( oexGUID *x_pGuid, oexUINT x_uBlock, oexUINT x_uType );

	//==============================================================
	// ReadPacketData()
	//==============================================================
	/// This function is used to retrieve the packet data
	/**
		\param [in] x_uBlock	-	Index of block
		\param [in] x_uType		-	Block type
		\param [in] x_pBuf		-	Pointer to buffer that receives the data.
		\param [in] x_uMax		-	Size of buffer in pBuf
		\param [in] x_puRead	-	Number of bytes written to pBuf
		\param [in] x_lOffset	-	Offset from block start pointer
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL ReadPacketData( oexUINT x_uBlock, oexUINT x_uType, oexPVOID x_pBuf, oexUINT x_uMax, oexUINT *x_puRead = oexNULL, oexLONG x_lOffset = 0 );

	//==============================================================
	// ReadPacketString()
	//==============================================================
	/// Reads a string from the packet data.  Ensures no buffer overflow and NULL terminates.
	/**
		\param [in] x_uBlock	-	Block index
		\param [in] x_uType		-	Block type
		\param [in] x_pStr		-	Buffer that receives the string
		\param [in] x_uMax		-	Size of the buffer in pStr
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL ReadPacketString( oexUINT x_uBlock, oexUINT x_uType, oexSTR8 x_pStr, oexUINT x_uMax )
	{	oexUINT uLen = 0;
		oexBOOL bRes = ReadPacketData( x_uBlock, x_uType, x_pStr, x_uMax, &uLen );
		if ( uLen >= x_uMax ) uLen = x_uMax - 1; x_pStr[ uLen ] = 0; 
		return bRes;
	}

    CStr8 ReadPacketString( oexUINT x_uBlock, oexUINT x_uType )
    {
        // See how large the buffer is
        oexUINT uLen = 0;
        if ( !ReadPacketData( x_uBlock, x_uType, oexNULL, 0, &uLen ) || !uLen )
            return CStr8();

        // Allocate space
        CStr8 str;
        if ( !str.OexAllocate( uLen ) )
            return CStr8();

        // Read in the data
        ReadPacketData( x_uBlock, x_uType, str._Ptr(), uLen, &uLen );
        str.SetLength( uLen );
        return str;
    }

	//==============================================================
	// Destroy()
	//==============================================================
	/// Releases resources and readys the object for reuse
	virtual void Destroy();

	//==============================================================
	// SkipPacket()
	//==============================================================
	/// Skips the first complete and valid packet in the buffer
	/**		
		\return Non-zero if success
	
		\see 
	*/
	virtual oexBOOL SkipPacket();

	//==============================================================
	// VerifyPacket()
	//==============================================================
	/// Verifys a packet at current buffer position
	/**
		
		Searches for the packet ID, then verifys the length and checksum
		If this function returns TRUE, a valid packet awaits in the buffer.

		\return Non-zero if a valid packet is in the buffer.
	
		\see 
	*/
	virtual oexBOOL VerifyPacket();
	
	//==============================================================
	// FindPacket()
	//==============================================================
	/// Searches the buffer for the packet ID.  Does not mean it is a valid packet.
	/**
		\param [in] x_pPh			-	Receives packet header data
		\param [in] x_puAvailable	-	Number of valid bytes left in buffer
		
		\return Non-zero if valid packet header is found.
	
		\see 
	*/
	virtual oexBOOL FindPacket( LPSPacketHeader x_pPh, oexUINT *x_puAvailable );

	//==============================================================
	// ReadPacket()
	//==============================================================
	/// Adds the specified data to the buffer and checks for a valid packet
	/**
		\param [in] x_pBuf	-	Buffer containing new raw data to read
		\param [in] x_uSize	-	Size of the data in pBuf
		
		\return Non-zero if success.
	
		\see 
	*/
	virtual oexBOOL ReadPacket( oexCPVOID x_pBuf, oexUINT x_uSize );

	//==============================================================
	// ReadPacket()
	//==============================================================
	/// Adds the specified data to the buffer and checks for a valid packet
	/**
		\param [in] x_sStr	-	Buffer containing new raw data to read
		
		\return Non-zero if success.
	
		\see 
	*/
	oexBOOL ReadPacket( CStr8 x_sStr )
    {   return ReadPacket( x_sStr.Ptr(), x_sStr.Length() ); }


	//==============================================================
	// GetMinimumPacketOverhead()
	//==============================================================
	/// Returns the minimum packet size
	oexUINT GetMinimumPacketOverhead()
	{	return ( sizeof( SPacketHeader ) + sizeof( oexGUID ) ); }

	//==============================================================
	// WritePacket()
	//==============================================================
	/// Writes a complete packet to the buffer containing a single data block
	/**
		\param [in] x_uPacketType	-	Packet type id
		\param [in] x_uDataType		-	Data type id
		\param [in] x_pData			-	Pointer to data
		\param [in] x_uData			-	Number of bytes in pData
		
		Writes packet data to buffer.

		\return Non-zero if success
	
		\see 
	*/
	oexBOOL WritePacket( oexUINT x_uPacketType, oexUINT x_uDataType, oexCPVOID x_pData, oexUINT x_uData );

	//==============================================================
	// WritePacket()
	//==============================================================
	/// Writes a complete packet to the buffer containing a single data block
	/**
		\param [in] x_uPacketType	-	Packet type id
		\param [in] x_uDataType		-	Data type id
		\param [in] x_sStr			-	String containing packet data
		
		Writes packet data to buffer.

		\return Non-zero if success
	
		\see 
	*/
    oexBOOL WritePacket( oexUINT x_uPacketType, oexUINT x_uDataType, CStr8 &x_sStr )
    {   return WritePacket( x_uPacketType, x_uDataType, x_sStr.Ptr(), x_sStr.Length() ); }


	//==============================================================
	// WriteMultiPacket()
	//==============================================================
	/// Writes multiple packet buffers
	/**
		\param [in] x_uPacketType	-	Packet type
		\param [in] x_uBuffers		-	Number of buffers
		
		Writes multiple buffers add 
		( oexUINT uDataType, oexUCHAR pucBuf, oexUINT uSize, ... ) for each buffer

		\return 
	
		\see 
	*/
	oexBOOL _cdecl WriteMultiPacket( oexUINT x_uPacketType, oexUINT x_uBuffers = 0, ... )
	{	return vWriteMultiPacket( x_uPacketType, x_uBuffers, ( (oexPVOID*)&x_uBuffers ) + 1 ); }

	//==============================================================
	// vWriteMultiPacket()
	//==============================================================
	/// Writes multiple packet buffers
	/**
		\param [in] x_uPacketType	-	Packet type
		\param [in] x_uBuffers		-	Number of buffers
		\param [in] x_pArgs			-	Pointer to argument pointer list
		
		pArgs[] = { oexUINT *uDataType, oexUCHAR *pucBuf, oexUINT *uSize, ... }

		\return Non-zero if success
	
		\see 
	*/
	oexBOOL vWriteMultiPacket( oexUINT x_uPacketType, oexUINT x_uBuffers = 0, oexPVOID *x_pArgs = oexNULL );

	//==============================================================
	// EndPacket()
	//==============================================================
	/// Call to conclude a packet being written to the buffer
	/**		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL EndPacket();

	//==============================================================
	// AddPacketData()
	//==============================================================
	/// Creates a data block and writes it into the buffer
	/**
		\param [in] x_uType	-	Packet data type
		\param [in] x_pData	-	Buffer containing packet data
		\param [in] x_uSize	-	Number of bytes in pData
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL AddPacketData( oexUINT x_uType, oexCPVOID x_pData, oexUINT x_uSize );

	//==============================================================
	// AddPacketData()
	//==============================================================
	/// Creates a data block and writes it into the buffer
	/**
		\param [in] x_uType	-	Packet data type
		\param [in] x_sStr	-	Packet block data
		
		\return Non-zero if success
	
		\see 
	*/
    oexBOOL AddPacketData( oexUINT x_uType, CStr8 &x_sStr )
    {   return AddPacketData( x_uType, x_sStr.Ptr(), x_sStr.Length() ); }

	//==============================================================
	// WritePacketData()
	//==============================================================
	/// Write raw data into packet and update checksum
	/**
		\param [in] x_pData		-	Pointer to packet data
		\param [in] x_uSize		-	Number of bytes in pData
		\param [in] x_uEncode	-	Optional encoding
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL WritePacketData( oexCPVOID x_pData, oexUINT x_uSize, oexUINT x_uEncode = 0 );

	//==============================================================
	// InitPacket()
	//==============================================================
	/// Initializes the packet header and writes it to the buffer
	/**
		\param [in] x_uType				-	Packet type
		\param [in] x_uDataBlocks		-	Number of data blocks
		\param [in] x_uTotalDataSize	-	Total size of packet data
		
		\return Non-zero if success
	
		\see 
	*/
	oexBOOL InitPacket( oexUINT x_uType, oexUINT x_uDataBlocks, oexUINT x_uTotalDataSize );

	//==============================================================
	// OnInspectWrite()
	//==============================================================
	/// Over-ride to implement write block inspection
	/**
		\param [in] x_uBlock	-	Block index
		\param [in] x_pBuf		-	Pointer to write buffer
		\param [in] x_uSize		-	Size of data in pBuf
		
		\return Non-zero if success.
	
		\see 
	*/
	virtual oexBOOL OnInspectWrite( oexUINT x_uBlock, oexUCHAR *x_pBuf, oexUINT x_uSize );

	/// Constructor
	CDataPacket();

	/// Constructor
	CDataPacket( oexUINT x_uSize );

	/// Destructor
	virtual ~CDataPacket();

	//==============================================================
	// SetEncoding()
	//==============================================================
	/// Call this function to enable packet data encoding
	/**
		\param [in] dw	-	Current encoding value
	*/
	void SetEncoding( oexUINT u ) { m_uEncode = u; } 

	//==============================================================
	// GetEncoding()
	//==============================================================
	/// Returns the type of packet data encoding
	oexUINT GetEncoding() { return m_uEncode; }

private:

	/// Set to true when a valid packet has beenf found
	oexBOOL			m_bValidPacket;

	/// Packet header data
	SPacketHeader	m_ph;

	/// Running MD5 context
    oss::CMd5       m_md5;

	/// Current encode value
	oexUINT			m_uEncode;

};

