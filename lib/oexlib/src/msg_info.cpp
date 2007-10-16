/*------------------------------------------------------------------
// msg_info.cpp
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


void CMsgInfo::Destroy()
{
    m_uUsed = 0;
    m_pDst = oexNULL;
    m_pSrc = oexNULL;
    m_pBh = oexNULL;
    m_pPh = oexNULL;
    m_pData = oexNULL;
    m_sBin.Destroy();
}

oexBOOL CMsgInfo::Open( oexUINT x_uFlags, oexUINT x_uParams, oexUINT x_uTotalSize )
{   
    // Lose old message
    Destroy();

    // Add packet overhead
    x_uTotalSize += OverheadSize( x_uParams ) + sizeof( oexCHARW ) * x_uParams;

    /// Pointer to buffer
    if ( !m_sBin.SetLength( x_uTotalSize ) )
        return oexFALSE;

    // Set pointers
    SetPointers( (oexINT)x_uParams );

    // Destination address
    os::CSys::Zero( m_pDst, sizeof( CMsgAddress ) );

    // Source address
    os::CSys::Zero( m_pSrc, sizeof( CMsgAddress ) );

    // Initialize block header
    os::CSys::Zero( m_pBh, sizeof( SBlockHeader ) );
    m_pBh->uParams = x_uParams;
    m_pBh->uFlags = x_uFlags;

    // Initialize param info
    os::CSys::Zero( m_pPh, sizeof( SParamHeader ) * x_uParams );

    return oexTRUE;
}

oexBOOL CMsgInfo::AddParam( oexUINT x_uType, oexCPVOID x_pData, oexUINT x_uSize )
{
    // Sanity checks
    if ( !x_uType || !oexCHECK_PTR( x_pData ) || !oexCHECK( x_uSize ) )
        return oexFALSE;

    if ( !m_pBh || !m_pPh || !m_pData || !m_sBin.Ptr() )
        return oexFALSE;

    // Verify there is room
    if ( (oexUINT)m_sBin.Length() < x_uSize + m_uUsed + OverheadSize() )
        return oexFALSE;

    // Find param slot
    oexUINT uParam = 0;
    while ( uParam < m_pBh->uParams && m_pPh[ uParam ].uType )
        uParam++;

    // Did we get a slot?
    if ( uParam >= m_pBh->uParams )
        return oexFALSE;

    // Copy the data
    os::CSys::MemCpy( &m_pData[ m_uUsed ], x_pData, x_uSize );
    m_uUsed += x_uSize;

    // Kind of a hack, this NULL terminates everything
    for( oexUINT i = 0; i < sizeof( oexCHARW ); i++ )
        m_pData[ m_uUsed++ ] = 0;

    // Save param info
    m_pPh[ uParam ].uType = x_uType;
    m_pPh[ uParam ].uSize = x_uSize;

    return oexTRUE;
}

oexBOOL CMsgInfo::End()
{
    if ( !m_pBh || !m_pPh || !m_pData || !m_sBin.Ptr() )
        return oexFALSE;

    // How much did we use
    oexUINT uSize = OverheadSize() + m_uUsed;

    // Should match
    oexASSERT( m_sBin.Length() == uSize );
    
    // Ensure correct size
    m_sBin.SetLength( uSize );

    // Update CRC
    CCrcHash::Hash( m_pBh->ucCrc, m_pData, m_uUsed );

    return oexTRUE;
}

oexBOOL CMsgInfo::GetParamInfo( oexUINT x_uIndex, oexSTR8 *x_pData, oexUINT *x_puSize, oexUINT *x_puType )
{
    // Sanity checks
    if ( !m_pBh || !m_pPh || !m_pData || !m_sBin.Ptr() || x_uIndex >= m_pBh->uParams )
        return oexFALSE;

    // Does the caller want the size?
    if ( x_puSize )
        *x_puSize = m_pPh[ x_uIndex ].uSize;

    // Does the caller want the type?
    if ( x_puType )
        *x_puType = m_pPh[ x_uIndex ].uType;

    // How about the data?
    if ( x_pData )
    {
        // Get the data pointer
        *x_pData = m_pData;
        for ( oexUINT i = 0; i < x_uIndex; i++ )
            *x_pData += m_pPh[ i ].uSize + sizeof( oexCHARW );

        // Just ensure there really is that much data
        if ( !oexCHECK( (oexUINT)m_sBin.Length() >=   OverheadSize() 
                                                    + ( *x_pData - m_pData ) 
                                                    + m_pPh[ x_uIndex ].uSize  ) )
            return oexFALSE;

    } // end if

    return oexTRUE;
} 

oexBOOL CMsgInfo::SetPointers( oexINT x_nParams )
{
    oexSTR8 pBin = (oexSTR8)m_sBin.Ptr();
    if ( !pBin )
        return oexFALSE;

    // Destination address
    m_pDst = (CMsgAddress*)pBin;
    pBin += sizeof( CMsgAddress );

    // Source address
    m_pSrc = (CMsgAddress*)pBin;
    pBin += sizeof( CMsgAddress );

    // Block header
    m_pBh = (SBlockHeader*)pBin;
    pBin += sizeof( SBlockHeader );

    // Negative means use data from buffer
    if ( 0 > x_nParams )
        x_nParams = m_pBh->uParams;

    // Param info
    m_pPh = (SParamHeader*)pBin;
    pBin += sizeof( SParamHeader ) * x_nParams;

    // Save data pointer
    m_pData = pBin;

    return oexTRUE;

}

oexBOOL CMsgInfo::VerifyPacket()
{
    // Ensure length is correct
    if ( m_sBin.Length() != OverheadSize() + m_uUsed )
        return oexFALSE;

    // Validate the crc
    oexUCHAR ucCrc[ CCrcHash::eHashSize ];
    CCrcHash::Hash( ucCrc, m_pData, m_uUsed );
    if ( os::CSys::MemCmp( ucCrc, m_pBh->ucCrc, sizeof( ucCrc ) ) )
        return oexFALSE;

    return oexTRUE;
}

oexBOOL CMsgInfo::Deserialize( CStr8 &x_sBin )
{
    // Lose previous message
    Destroy();

    /// Set the buffer
    m_sBin.Set( x_sBin );

    if ( !SetPointers( -1 ) )
        return oexFALSE;

    m_uUsed = m_sBin.Length() - OverheadSize();

    if ( !VerifyPacket() )
    {   Destroy();
        return oexFALSE;
    } // end if

    return oexTRUE;
}

oexBOOL CMsgInfo::Deserialize( oexCSTR8 x_pBin, oexUINT x_uSize )
{
    // Lose previous message
    Destroy();

    /// Set the buffer
    m_sBin.Set( x_pBin, x_uSize );

    if ( !SetPointers( -1 ) )
        return oexFALSE;

    m_uUsed = m_sBin.Length() - OverheadSize();

    if ( !VerifyPacket() )
    {   Destroy();
        return oexFALSE;
    } // end if

    return oexTRUE;
}
