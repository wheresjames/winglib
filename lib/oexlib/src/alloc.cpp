/*------------------------------------------------------------------
// alloc.cpp
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

#if defined( _DEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

/// Underrun padding
oexUCHAR CAlloc::m_ucUnderrunPadding[ 4 ] = 
{
	'U',  'N',  'D',  'R'
};

/// Overrun padding {20981ECD-F285-4da0-9552-A929C5CD7971}
oexUCHAR CAlloc::m_ucOverrunPadding[ 24 ] = 
{
	'O',  'V',  'E',  'R',
	'R',  'U',  'N',  '!',
	0x20, 0x98, 0x1e, 0xcd,
	0xf2, 0x85, 0x4d, 0xa0,
	0x95, 0x52, 0xa9, 0x29,
	0xc5, 0xcd, 0x00, 0x00
};

void CAlloc::ReportBlock( oexPVOID x_pMem, oexUINT uSize )
{
    if ( !oexVERIFY_PTR( x_pMem ) )
        return;

    oexUCHAR *pBuf = (oexUCHAR*)x_pMem;

    oexUINT uBlockSize = *(oexUINT*)pBuf;
    pBuf += sizeof( oexUINT );

    // Allocation information
    SAllocInfo *pAi = (SAllocInfo*)pBuf;
    pBuf += sizeof( SAllocInfo ) * eMaxAllocTrail;

    // Size of protected area
	oexUINT uProtected = *(oexUINT*)pBuf;

    oexTCHAR szMsg[ 1024 ] = oexEMPTY_STRING;
    os::CSys::StrFmt( szMsg, sizeof( szMsg ), "Total: %lu, Block: %lu, Protected: %lu\n",
                                              uSize, uBlockSize, uProtected );

    if ( pAi[ 0 ].pFile )
        os::CSys::StrFmt( zstr::EndOfString( szMsg ), sizeof( szMsg ) - zstr::Length( szMsg ), 
                          "%s(%lu) : Allocated\n", pAi[ 0 ].pFile, pAi[ 0 ].uLine );

    if ( pAi[ 1 ].pFile )
        os::CSys::StrFmt( zstr::EndOfString( szMsg ), sizeof( szMsg ) - zstr::Length( szMsg ), 
                          "%s(%lu) : Resized\n", pAi[ 1 ].pFile, pAi[ 1 ].uLine );

    if ( pAi[ 2 ].pFile )
        os::CSys::StrFmt( zstr::EndOfString( szMsg ), sizeof( szMsg ) - zstr::Length( szMsg ), 
                          "%s(%lu) : Freed\n", pAi[ 2 ].pFile, pAi[ 2 ].uLine );

    oexTRACE( szMsg );

}

oexPVOID CAlloc::Alloc( oexUINT x_uSize, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex )
{
    // Allocate memory in powers of two
    oexUINT uBlockSize = cmn::NextPower2( sizeof( oexUINT ) + x_uSize + ProtectAreaSize() );

    oexUCHAR *pBuf = (oexUCHAR*)os::CMem::New( uBlockSize, x_uLine, x_pFile );
    if ( !pBuf )
        return oexNULL;

    // Save block size
    *(oexUINT*)pBuf = uBlockSize;
    pBuf += sizeof( oexUINT );

    // Initialize protected memory variables
    os::CSys::Zero( pBuf, sizeof( SAllocInfo ) * eMaxAllocTrail + sizeof( oexUINT ) );

    // Return protected memory area
    return CAlloc::ProtectMem( pBuf, x_uSize, oexTRUE, x_uLine, x_pFile, x_uInfoIndex );
}

void CAlloc::Free( oexPVOID x_pBuf, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex )
{
    // Delete the memory
    os::CMem::Delete( (oexUCHAR*)CAlloc::VerifyMem( x_pBuf, oexTRUE ) - sizeof( oexUINT ) );
}

oexPVOID CAlloc::Resize( oexPVOID x_pBuf, oexUINT x_uNewSize, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex )
{
    // Do we have the space to resize?
    oexUINT uBlockSize = BlockSize( x_pBuf );
    if ( uBlockSize < x_uNewSize ) 
        return oexNULL;

    // Resize the protected memory area
    return ProtectMem( VerifyMem( x_pBuf, oexTRUE ), x_uNewSize, oexTRUE, x_uLine, x_pFile, x_uInfoIndex );
}

oexUINT CAlloc::BlockSize( oexPVOID x_pBuf )
{
    if ( !oexVERIFY( x_pBuf ) )
        return 0;

    // Grab the size of the allocated buffer
    return *(oexUINT*)( ( (oexUCHAR*)x_pBuf ) - sizeof( oexUINT )
                                              - sizeof( m_ucUnderrunPadding )
                                              - ( sizeof( SAllocInfo ) * eMaxAllocTrail )
                                              - sizeof( oexUINT ) );
}

oexPVOID CAlloc::VerifyMem( oexPVOID x_pBuf, oexBOOL x_bUpdate, oexUINT *x_puSize, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex )
{
    // Convert to byte pointer
    oexUCHAR *pBuf = (oexUCHAR*)x_pBuf;

    // Pull out this pointer so we can see it from the debugger
    SAllocInfo *pAi = (SAllocInfo*)( pBuf - sizeof( oexUINT ) 
                                          - sizeof( m_ucUnderrunPadding )
                                          - ( sizeof( SAllocInfo ) * eMaxAllocTrail ) );

    // Grab the size of the allocated buffer
    oexUINT uSize = *(oexUINT*)( pBuf - sizeof( oexUINT )
                                      - sizeof( m_ucUnderrunPadding ) );

    if ( x_puSize )
	    *x_puSize = *( (oexUINT*)pBuf );

    // Valid info index?
    if ( x_bUpdate && x_uInfoIndex < eMaxAllocTrail )
    {
        // Save allocation information
        pAi[ x_uInfoIndex ].pFile = x_pFile;
        pAi[ x_uInfoIndex ].uLine = x_uLine;

    } // end if

	// Ensure overrun buffers match
	// *** If this asserts, you probably over-ran the buffer!
	//     i.e. buffer overflow
	if ( !oexVERIFY( !os::CSys::MemCmp( &pBuf[ uSize ], m_ucOverrunPadding, sizeof( m_ucOverrunPadding ) ) ) )
		return oexNULL;

	// Clear so it can't haunt us later
    if ( x_bUpdate )
	    os::CSys::MemSet( &pBuf[ uSize ], 0, sizeof( m_ucOverrunPadding ) );

    // Skip back over the under run buffer
	pBuf -= sizeof( m_ucUnderrunPadding );

	// Ensure underrun buffers match / underflow protection
	// *** If this asserts, you likely either under-ran the buffer, 
    //     or the block before this one over ran
    if ( !oexVERIFY( !os::CSys::MemCmp( pBuf, m_ucUnderrunPadding, sizeof( m_ucUnderrunPadding ) ) ) )
		return oexNULL;

	// Clear so it can't haunt us later
    if ( x_bUpdate )
    	os::CSys::MemSet( pBuf, 0, sizeof( m_ucUnderrunPadding ) );

	// Skip the size
	pBuf -= sizeof( oexUINT );

	// We don't want this value comming back to haunt us
    if ( x_bUpdate )
    	*( (oexUINT*)pBuf ) = 0;

    // Skip back over allocation information
    pBuf -= sizeof( SAllocInfo ) * eMaxAllocTrail;

	// Return original buffer pointer
	return pBuf;
}

oexUINT CAlloc::ProtectAreaSize()
{
	return  ( sizeof( SAllocInfo ) * eMaxAllocTrail ) 
            + sizeof( oexUINT )
            + sizeof( m_ucUnderrunPadding )
            + sizeof( m_ucOverrunPadding );
}

oexUINT CAlloc::UsedSize( oexPVOID x_pBuf )
{
    if ( !oexVERIFY( x_pBuf ) )
        return 0;

    // Grab the size of the usable space
    return *(oexUINT*)( ( (oexUCHAR*)x_pBuf ) - sizeof( oexUINT )
                                              - sizeof( m_ucUnderrunPadding ) );
}

oexPVOID CAlloc::ProtectMem( oexPVOID x_pBuf, oexUINT x_uSize, oexBOOL x_bUpdate, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex )
{
    // Convert to byte pointer
    oexUCHAR *pBuf = (oexUCHAR*)x_pBuf;

    // Get pointer to allocation information
    SAllocInfo *pAi = (SAllocInfo*)pBuf;

    // Valid info index?
    if ( x_bUpdate && x_uInfoIndex < eMaxAllocTrail )
    {
        // Save allocation information
        pAi[ x_uInfoIndex ].pFile = x_pFile;
        pAi[ x_uInfoIndex ].uLine = x_uLine;

    } // end if

    // Skip the allocation info
    pBuf += sizeof( SAllocInfo ) * eMaxAllocTrail;

    // Save size of buffer for later
    if ( x_bUpdate )
    	*( (oexUINT*)pBuf ) = x_uSize;

	// Adjust to new start point
	pBuf += sizeof( oexUINT );

    // Copy under-run padding
    if ( x_bUpdate )
    	os::CSys::MemCpy( pBuf, m_ucUnderrunPadding, sizeof( m_ucUnderrunPadding ) );

    // Skip under run padding
    pBuf += sizeof( m_ucUnderrunPadding );

	// Copy overrun padding
    if ( x_bUpdate )
    	os::CSys::MemCpy( &pBuf[ x_uSize ], m_ucOverrunPadding, sizeof( m_ucOverrunPadding ) );

	// New protected memory area pointer
	return pBuf;
}

#else

oexPVOID CAlloc::Alloc( oexUINT x_uSize )
{
    // Allocate memory in powers of two
    oexUINT uBlockSize = cmn::NextPower2( sizeof( oexUINT ) + x_uSize );

    oexUCHAR *pBuf;

    // Allocate buffer
    try
    {
        pBuf = oexNEW oexUCHAR[ uBlockSize ];
        if ( !pBuf )
            return oexNULL;

    } // end try
    catch( ... )
    {
        return oexNULL;

    } // end catch

    // Save block size
    *(oexUINT*)pBuf = uBlockSize;
    pBuf += sizeof( oexUINT );

    // Return protected memory area
    return pBuf;
}

void CAlloc::Free( oexPVOID x_pBuf )
{
    try
    {
        oexDELETE_ARR( (oexUCHAR*)pBuf - sizeof( oexUINT ) );

    } // end try
    catch( ... )
    {
        return;

    } // end catch
}

oexPVOID CAlloc::Resize( oexPVOID x_pBuf, oexUINT x_uNewSize )
{
    // Do we have the space to resize?
    oexUINT uBlockSize = BlockSize( x_pBuf );
    if ( uBlockSize < x_uNewSize ) 
        return oexNULL;

    // Return pointer if there is enough room
    return x_pBuf;
}

oexUINT CAlloc::BlockSize( oexPVOID x_pBuf )
{
    // Grab the size of the allocated buffer
    return *(oexUINT*)( ( (oexUCHAR*)x_pBuf ) - sizeof( oexUINT ) );
}

#endif
