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

#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

// Ensure alignement
#	if defined( OEX_ALIGNEDMEM ) && 0 != OEX_ALIGNEDMEM
		oexSTATIC_ASSERT( 0 == ( OEX_MEMBLOCKPADDING & ( OEX_ALIGNEDMEM - 1 ) ) );
		oexSTATIC_ASSERT( OEX_MEMBLOCKPADDING >= sizeof( oexUINT ) );
		oexSTATIC_ASSERT( OEX_SIZE_VAR >= sizeof( oexUINT ) );
#	endif

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

#endif

void CAlloc::ReportBlock( oexCPVOID x_pMem, oexUINT uSize )
{
    // Could get an exception if it's not our memory block
    // But we can't force people to use our allocation routines ;)
    oexTRY
    {
        if ( !oexVERIFY_PTR( x_pMem ) )
            return;

        oexUCHAR *pBuf = (oexUCHAR*)x_pMem;

        oexUINT uBlockSize = *(oexUINT*)pBuf;
        pBuf += OEX_SIZE_VAR;

        SBlockHeader *pBh = (SBlockHeader*)pBuf;
        pBuf += sizeof( SBlockHeader );

        oexTCHAR szMsg[ 1024 ] = oexEMPTY_STRING;
        os::CSys::StrFmt( szMsg, sizeof( szMsg ), oexT( "Total: %u, Block: %u, Protected: %u\n" ),
                                                  uSize, uBlockSize, pBh->uSize );

        if ( pBh->ai[ 0 ].pFile )
            os::CSys::StrFmt( zstr::eos( szMsg ), sizeof( szMsg ) - zstr::Length( szMsg ),
                              oexT( "%s(%u) : Allocated\n" ), pBh->ai[ 0 ].pFile, pBh->ai[ 0 ].uLine );

        if ( pBh->ai[ 1 ].pFile )
            os::CSys::StrFmt( zstr::eos( szMsg ), sizeof( szMsg ) - zstr::Length( szMsg ),
                              oexT( "%s(%u) : Resized\n" ), pBh->ai[ 1 ].pFile, pBh->ai[ 1 ].uLine );

        if ( pBh->ai[ 2 ].pFile )
            os::CSys::StrFmt( zstr::eos( szMsg ), sizeof( szMsg ) - zstr::Length( szMsg ),
                              oexT( "%s(%u) : Freed\n" ), pBh->ai[ 2 ].pFile, pBh->ai[ 2 ].uLine );

        oexTRACE( szMsg );

    } // end try

    oexCATCH_ALL()
    {
        oexTRACE( oexT( "!!! Asserted while trying to interpret memory block.  Perhaps it's some other _CLIENT_BLOCK?\n" ) );

    } // end catch

}

oexBOOL CAlloc::GetBlockReport( oexCPVOID x_pMem, oexUINT uSize, oexSTR pMsg, oexUINT uBuf )
{
	if ( !pMsg )
		return oexFALSE;

	*pMsg = 0;

    // Could get an exception if it's not our memory block
    // But we can't force people to use our allocation routines ;)
    oexTRY
    {
        if ( !oexVERIFY_PTR( x_pMem ) )
            return oexFALSE;

        oexUCHAR *pBuf = (oexUCHAR*)x_pMem;

        oexUINT uBlockSize = *(oexUINT*)pBuf;
        pBuf += OEX_SIZE_VAR;

        SBlockHeader *pBh = (SBlockHeader*)pBuf;
        pBuf += sizeof( SBlockHeader );

#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
        pBuf += sizeof( m_ucUnderrunPadding );
#endif

		if( uSize )
			os::CSys::StrFmt( pMsg, uBuf, oexT( "Total: %u, Block: %u, Protected: %u, Refs: %u, Flags: %u\n" ),
		                                              uSize, uBlockSize, pBh->uSize, pBh->uRef, pBh->uFlags );
		else
			os::CSys::StrFmt( pMsg, uBuf, oexT( "Block: %u, Protected: %u, Refs: %u, Flags: %u\n" ),
													  uBlockSize, pBh->uSize, pBh->uRef, pBh->uFlags );

        if ( pBh->ai[ 0 ].pFile )
            os::CSys::StrFmt( zstr::eos( pMsg ), uBuf - zstr::Length( pMsg ),
                              oexT( "%s(%u) : Allocated\n" ), pBh->ai[ 0 ].pFile, pBh->ai[ 0 ].uLine );

        if ( pBh->ai[ 1 ].pFile )
            os::CSys::StrFmt( zstr::eos( pMsg ), uBuf - zstr::Length( pMsg ),
                              oexT( "%s(%u) : Resized\n" ), pBh->ai[ 1 ].pFile, pBh->ai[ 1 ].uLine );

        if ( pBh->ai[ 2 ].pFile )
            os::CSys::StrFmt( zstr::eos( pMsg ), uBuf - zstr::Length( pMsg ),
                              oexT( "%s(%u) : Freed\n" ), pBh->ai[ 2 ].pFile, pBh->ai[ 2 ].uLine );

#if defined( OEX_MAX_LINES_IN_MEMDUMP )

		oexSIZE_T i = 0;
		oexSIZE_T b, sz = uBuf - zstr::Length( pMsg );
		oexSTR p = zstr::eos( pMsg );
		for ( oexSIZE_T l = 0; 0 < sz && i < pBh->uSize && l < OEX_MAX_LINES_IN_MEMDUMP; l++, i += 16 )
		{
			for ( b = 0; 0 < sz && ( i + b ) < pBh->uSize && b < 16; b++ )
				os::CSys::StrFmt( p, sz,oexT( "%0.2X " ), (oexUINT)pBuf[ i + b ] ),
				p += 3, sz -= 3;

			while ( 0 < sz && b < 16 )
				zstr::Copy( p, sz, oexT( "   " ) ),
				p += 3, sz -= 3, b++;

			// Append space
			zstr::Copy( p, sz, oexT( "  " ) ),
			p += 2, sz -= 2;

			for ( b = 0; 0 < sz && ( i + b ) < pBh->uSize && b < 16; b++ )
				if ( pBuf[ i + b ] >= oexT( ' ' ) && pBuf[ i + b ] <= oexT( '~' ) )
					*( p++ ) = pBuf[ i + b ], sz--;
				else
					*( p++ ) = oexT( '.' ), sz--;

			// Append new line
			zstr::Copy( p, sz, oexT( "\r\n" ) ),
			p += 2, sz -= 2;

		} // end for

#endif

		return oexTRUE;

    } // end try

    oexCATCH_ALL()
    {

    } // end catch

	zstr::Copy( pMsg, oexT( " Assert while trying to interpret memory block.  Perhaps it's someone else's memory block?\n" ) );

	return oexFALSE;
}

oexPVOID CAlloc::Alloc( oexUINT x_uSize, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex, oexBOOL x_bUseFullBlock )
{
    // If this asserts you didn't use the logging macro
    // Instead of Allocate, use OexAllocate
    // Instead of New use OexNew
    // Construct to OexConstruct
    // ConstructArray to OexConstructArray
    // etc...
    oexASSERT( x_pFile );

    // Allocate memory in powers of two
    oexUINT uBlockSize = cmn::NextPower2( OEX_SIZE_VAR + x_uSize + ProtectAreaSize() );

    // Does user want all allocated space?
    if ( x_bUseFullBlock )

        // Maximum useable space for the block size
        x_uSize = uBlockSize - ( OEX_SIZE_VAR + ProtectAreaSize() );

    // Ok, get the memory
	oexUCHAR *pBuf = (oexUCHAR*)CMem::New( uBlockSize, x_uLine, x_pFile );

    if ( !pBuf )
        return oexNULL;

    // Save block size
    *(oexUINT*)pBuf = uBlockSize;
    pBuf += OEX_SIZE_VAR;

    // Initialize block header
    oexZeroMemory( pBuf, sizeof( SBlockHeader ) );

    // Return protected memory area
    pBuf = (oexUCHAR*)CAlloc::ProtectMem( pBuf, x_uSize, oexTRUE, x_uLine, x_pFile, x_uInfoIndex );

    // Increment the reference count on this memory block
    AddRef( pBuf );

    return pBuf;
}

oexINT CAlloc::Free( oexPVOID x_pBuf, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex )
{
    // Decrement the reference count
	oexINT nRef;
    if ( 0 != ( nRef = DecRef( x_pBuf ) ) )
        return nRef;

    // Verify the memory
    oexPVOID pBuf = (oexUCHAR*)CAlloc::VerifyMem( x_pBuf, oexTRUE ) - OEX_SIZE_VAR;

	// Delete memory
    CMem::Delete( pBuf );

    return 0;
}

oexPVOID CAlloc::ReAlloc( oexPVOID x_pBuf, oexUINT x_uNewSize, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex, oexBOOL x_bAllowMove, oexBOOL x_bUseFullBlock )
{
    // Do we have the space in the current block?
    oexUINT uBlockSize = BlockSize( x_pBuf );
    if ( uBlockSize >= ( x_uNewSize + ProtectAreaSize() + OEX_SIZE_VAR ) )
	    return ProtectMem( VerifyMem( x_pBuf, oexTRUE ), x_uNewSize, oexTRUE, x_uLine, x_pFile, x_uInfoIndex );

	// Will the caller allow the block to be moved?
   	if ( !x_bAllowMove )
   		return oexNULL;

	// New block size
	uBlockSize = cmn::NextPower2( OEX_SIZE_VAR + x_uNewSize + ProtectAreaSize() );

	// Attempt resize
	oexUCHAR *pBuf = (oexUCHAR*)CMem::Resize( (oexUCHAR*)VerifyMem( x_pBuf, oexTRUE ) - OEX_SIZE_VAR, uBlockSize, x_uLine, x_pFile );

	// Ensure the resize worked
	if( !pBuf )
		return oexNULL;

	// Save new block size
	*(oexUINT*)pBuf = uBlockSize;
	pBuf += OEX_SIZE_VAR;

	// Return new memory block
	return (oexUCHAR*)ProtectMem( pBuf, x_uNewSize, oexTRUE, x_uLine, x_pFile, x_uInfoIndex );
}

oexPVOID CAlloc::VerifyMem( oexPVOID x_pBuf, oexBOOL x_bUpdate, oexUINT *x_puSize, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex )
{
    // Validate pointer
    if ( !oexVERIFY_PTR( x_pBuf ) )
        return 0;

    // Convert to byte pointer
    oexUCHAR *pBuf = (oexUCHAR*)x_pBuf;

    // Grab a pointer to the block header
    SBlockHeader *pBh = (SBlockHeader*)( pBuf - sizeof( SBlockHeader )
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )
                                         - sizeof( m_ucUnderrunPadding )
#endif
                                         );

    // Grab the size of the allocated buffer
    oexUINT uSize = pBh->uSize;

    if ( x_puSize )
	    *x_puSize = *( (oexUINT*)pBuf );

    // Valid info index?
    if ( x_bUpdate && x_uInfoIndex < eMaxAllocTrail )
    {
        // Save allocation information
        pBh->ai[ x_uInfoIndex ].pFile = x_pFile;
        pBh->ai[ x_uInfoIndex ].uLine = x_uLine;

    } // end if

#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

	// Ensure overrun buffers match
	// *** If this asserts, you probably over-ran the buffer!
	//     i.e. buffer overflow
	//     Or maybe you're freeing a bad pointer?
	if ( os::CSys::MemCmp( &pBuf[ uSize ], m_ucOverrunPadding, sizeof( m_ucOverrunPadding ) ) )
	{
#if defined( oexDEBUG )
		oexTCHAR szMsg[ 1024 * 16 ] = { 0 };
		if ( GetBlockReport( x_pBuf, 0, szMsg, sizeof( szMsg ) ) )
			oexEcho( szMsg );
		oexASSERT( 0 != "Overrun padding has been currupted" );
#endif
		return oexNULL;
	} // end if

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

#endif

	// Skip the block header
	pBuf -= sizeof( SBlockHeader );

	// We don't want this value comming back to haunt us
    if ( x_bUpdate )
    	pBh->uSize = 0;

	// Return original buffer pointer
	return pBuf;
}

oexPVOID CAlloc::ProtectMem( oexPVOID x_pBuf, oexUINT x_uSize, oexBOOL x_bUpdate, oexUINT x_uLine, oexCSTR x_pFile, oexUINT x_uInfoIndex )
{
    // Convert to byte pointer
    oexUCHAR *pBuf = (oexUCHAR*)x_pBuf;

    // Grab a pointer to the block header
    SBlockHeader *pBh = (SBlockHeader*)pBuf;

    // Skip the allocation info
    pBuf += sizeof( SBlockHeader );

    // Valid info index?
    if ( x_bUpdate && x_uInfoIndex < eMaxAllocTrail )
    {
        // Save allocation information
        pBh->ai[ x_uInfoIndex ].pFile = x_pFile;
        pBh->ai[ x_uInfoIndex ].uLine = x_uLine;

    } // end if

    // Save size of usable space for later
    if ( x_bUpdate )
        pBh->uSize = x_uSize;

#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

    // Copy under-run padding
    if ( x_bUpdate )
    	os::CSys::MemCpy( pBuf, m_ucUnderrunPadding, sizeof( m_ucUnderrunPadding ) );

    // Skip under run padding
    pBuf += sizeof( m_ucUnderrunPadding );

	// Copy overrun padding
    if ( x_bUpdate )
    	os::CSys::MemCpy( &pBuf[ x_uSize ], m_ucOverrunPadding, sizeof( m_ucOverrunPadding ) );

#endif

	// New protected memory area pointer
	return pBuf;
}

