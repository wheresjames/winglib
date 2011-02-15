/*------------------------------------------------------------------
// virtual_fs.cpp
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

#include "oexlib.h"

OEX_USING_NAMESPACE

oexBOOL CVirtualFs::Open( oexCSTR8 x_pRoot )
{
    if ( !oexVERIFY_PTR( x_pRoot ) )
        return oexFALSE;

    // Create the root path
    if ( !CFile::Exists( oexStr8ToStrPtr( x_pRoot ) ) )
        if ( !CFile::CreatePath( oexStr8ToStrPtr( x_pRoot ) ) )
            return oexFALSE;

    // Attempt to open the index
    if ( !m_fIndex.OpenAlways( oexStr8ToStr( CStr8::BuildPath( x_pRoot, "top.idx" ) ).Ptr() ).IsOpen() )
        return oexFALSE;

    m_sRoot = x_pRoot;

    return oexTRUE;
}

oexBOOL CVirtualFs::Destroy()
{
    m_sRoot.Destroy();
    m_fIndex.Destroy();

    return oexTRUE;
}

oexBOOL CVirtualFs::SaveFile( oexCSTR8 x_pFile, oexCSTR8 x_pData, oexUINT x_uSize, oexUINT x_uType )
{
    if ( !m_fIndex.IsOpen() ) 
        Open( m_sRoot.Ptr() );

    // Ensure we have a file system
    if ( !m_fIndex.IsOpen() ) 
        return oexFALSE;

    // Ensure we have memory
    if ( eBlockSize != m_memTag.Size() && !m_memTag.OexNew( eBlockSize * 16 ).Ptr() )
        return oexFALSE;

    // Allocate space for the validation hash
    oexUINT uLen = sizeof( oexGUID );

    // Create id for file name
    oexGUID guidId;
    oss::CMd5::Transform( &guidId, x_pFile, zstr::Length( x_pFile ) );

    // Copy id into the tag buffer
    guid::CopyGuid( (oexGUID*)m_memTag.Ptr( uLen ), &guidId );
    uLen += sizeof( oexGUID );

    // Get the path for new data
    oexUINT uTime = CSysTime().GetLocalTime().GetUnixTime() / ( 60 * 60 );
    CStr8 sPath = CStr8::BuildPath( m_sRoot, CStr8( uTime ) );
    if ( !CFile::Exists( oexStr8ToStr( sPath ).Ptr() ) ) 
        CFile::CreatePath( oexStr8ToStr( sPath ).Ptr() );

    // While the data won't fit in the tag block
    oexUINT uBlockHeaderOffset = 0; oexUINT uBlocks = 0, uSize = x_uSize;
    while ( uSize && ( uLen >= ( eBlockSize - sizeof( SBlockHeader ) ) 
                         || uSize > ( eBlockSize - sizeof( SBlockHeader ) - uLen ) ) )
    {
        // Grab space for block header
        if ( !uBlockHeaderOffset ) 
            uBlockHeaderOffset = uLen, uLen += sizeof( SBlockHeader );

        oexGUID guid;
        oexUINT uMaxData = eBlockSize - ( sizeof( oexGUID ) * 2 + sizeof( SBlockHeader ) );
        if ( !WriteDataBlock( &guid, x_pData, ( uSize < uMaxData ) ? uSize : uMaxData, sPath, uTime ) )
            return oexFALSE;

        // Adjust data parameters
        if ( uSize <= uMaxData ) uSize = 0;
        else uSize -= uMaxData, x_pData += uMaxData;

        // Copy the file id into the tag block
        guid::CopyGuid( (oexGUID*)m_memTag.Ptr( uLen ), &guid );

        // Count one file block
        uLen += sizeof( oexGUID );

        // Count a block
        uBlocks++;

    } // end while

    // Do we have any left over data?
    if ( uSize )
    {
        // Get a pointer to the blocks
        SBlockHeader *pBh = (SBlockHeader*)m_memTag.Ptr( uLen );
        uLen += sizeof( SBlockHeader );

        // Fill in the header
        pBh->uSize = uSize;
        pBh->uType = 2;
        pBh->uFlags = 0;
        pBh->uBlocks = 0;

        // Copy the left over data
        oexPVOID pBuf = m_memTag.Ptr( uLen );
        os::CSys::MemCpy( pBuf, x_pData, uSize );

        // Update size
        uLen += uSize;

    } // end if

    // Do we need to create the block id list header?
    if ( uBlocks && uBlockHeaderOffset )
    {
        // Write block id list header
        SBlockHeader *pBh = (SBlockHeader*)m_memTag.Ptr( uBlockHeaderOffset );
        pBh->uSize = uBlocks * sizeof( oexGUID );
        pBh->uType = 1;
        pBh->uFlags = 0;
        pBh->uBlocks = uBlocks;

    } // end if

    // Calculate the MD5 hash for the block
    oss::CMd5::Transform( (oexGUID*)m_memTag.Ptr(), m_memTag.Ptr( sizeof( oexGUID ) ), uLen - sizeof( oexGUID ) );

    // Update the index
    m_fIndex.SetPtrPosBegin( guidId.Data1 & eIndexMask );
    m_fIndex.Write( &uTime, sizeof( uTime ) );

    // Create the path to the tag
    sPath = CStr8::BuildPath( sPath, CStr8().GuidToString( &guidId ) << ".dat" );

    // Write out the tag data
    CFile().OpenAlways( oexStr8ToStr( sPath ).Ptr() ).Write( m_memTag.Ptr(), uLen );

    // Link the file
    Link( x_pFile, &guidId, x_uSize, x_uType );

    return oexTRUE;
}

oexBOOL CVirtualFs::WriteDataBlock( oexGUID *x_pId, oexCPVOID x_pData, oexUINT x_uSize, CStr8 &x_sRoot, oexUINT x_uLocation )
{
    // Ensure valid block size
    oexUINT uOverhead = sizeof( oexGUID ) * 2 + sizeof( SBlockHeader );
    if ( eBlockSize < ( x_uSize + uOverhead ) )
        return oexFALSE;

    oexGUID guid;
    os::CSys::Zero( &guid, sizeof( guid ) );

    // Write the data block header
    SBlockHeader sh;
    sh.uSize = x_uSize;
    sh.uType = 2;
    sh.uFlags = 0;
    sh.uBlocks = 0;

    // Gen hash
    oss::CMd5 md5;
    md5.Update( &guid, sizeof( oexGUID ) );
    md5.Update( &sh, sizeof( sh ) );
    md5.Update( x_pData, x_uSize );
    md5.Final();

    // Does the caller want the id?
    if ( x_pId ) 
        guid::CopyGuid( x_pId, &md5.GetGuid() );

    // Attempt to open the file
    CFile f;
    if ( !f.OpenAlways( oexStr8ToStr( CStr8::BuildPath( x_sRoot, CStr8().GuidToString( md5 ) << ".dat" ) ).Ptr() ).IsOpen() )
        return oexFALSE;

    // Write out the block hash
    f.Write( md5.GetBuffer(), sizeof( oexGUID ) );

    // NULL id field
    f.SetPtrPosCur( sizeof( oexGUID ) );
//    f.Write( x_pId, sizeof( oexGUID ) );

    f.Write( &sh, sizeof( sh ) );

    // Write the data
    f.Write( x_pData, x_uSize );   

    // Update the file index
    m_fIndex.SetPtrPosBegin( md5.GetGuid().Data1 & eIndexMask );
    m_fIndex.Write( &x_uLocation, sizeof( x_uLocation ) );

    return oexTRUE;
}

CStr8 CVirtualFs::LoadFile( oexCSTR8 x_pFile, oexUINT x_uType )
{
    if ( !m_fIndex.IsOpen() ) 
        Open( m_sRoot.Ptr() );

    // Ensure we have a file system
    if ( !m_fIndex.IsOpen() ) 
        return CStr8();

    // Create id for file name
    oexGUID guidId;
    oss::CMd5::Transform( &guidId, x_pFile, zstr::Length( x_pFile ) );

    // Get the file location
    oexUINT uLocation = 0, uRead = 0;
    m_fIndex.SetPtrPosBegin( guidId.Data1 & eIndexMask );
    if ( !m_fIndex.Read( &uLocation, sizeof( uLocation ), &uRead ) 
         || uRead != sizeof( uLocation ) )
        return CStr8();

    // Ensure the path exists
    CStr8 sPath = CStr8::BuildPath( m_sRoot, CStr8( uLocation ) );
    if ( !CFile::Exists( oexStr8ToStr( sPath ).Ptr() ) ) 
        return CStr8();

    // Ensure we have memory
    if ( eBlockSize != m_memTag.Size() && !m_memTag.OexNew( eBlockSize * 16 ).Ptr() )
        return CStr8();

    CFile f;
    if ( !f.OpenExisting( oexStr8ToStr( CStr8::BuildPath( sPath, CStr8().GuidToString( &guidId ) << ".dat" ) ).Ptr() ).IsOpen() )
        return CStr8();

    // Read in the file contents
    oexUINT uLen = (oexUINT)f.Size();
    if ( !f.Read( m_memTag.Ptr(), uLen ) )
        return CStr8();

    // Verify hash
    oexGUID guid;
    oss::CMd5::Transform( &guid, m_memTag.Ptr( sizeof( oexGUID ) ), uLen - sizeof( oexGUID ) );
    if ( !guid::CmpGuid( &guid, (oexGUID*)m_memTag.Ptr() ) )
        return CStr8();

    // Verify it is the correct block
    if ( !guid::CmpGuid( &guidId, (oexGUID*)m_memTag.Ptr( sizeof( oexGUID ) ) ) )
        return CStr8();

    CStr8 str;
    uLen = sizeof( oexGUID ) * 2;
    SBlockHeader *pBh = (SBlockHeader*)m_memTag.Ptr( uLen );
    uLen += sizeof( SBlockHeader );

    if ( pBh->uType == 1 )
    {
        // Process all blocks
        while ( pBh->uBlocks-- )
        {
            // Add the block
            CStr8 sBlock = ReadDataBlock( (oexGUID*)m_memTag.Ptr( uLen ) );
            if ( !sBlock.Length() )
                return oexFALSE;

            // Append the block
            str << sBlock;

            // Skip this guid
            uLen += sizeof( oexGUID );

        } // end while

        // Point to next block header
        pBh = (SBlockHeader*)m_memTag.Ptr( uLen );
        uLen += sizeof( SBlockHeader );

    } // end if

    if ( pBh->uType == 2 )
    {
        // Add data
        str.Append( (oexCSTR8)m_memTag.Ptr( uLen ), pBh->uSize );

    } // end if

    return str;
}

CStr8 CVirtualFs::ReadDataBlock( oexGUID *x_pGuidId )
{
    // Get the file location
    oexUINT uLocation = 0, uRead = 0;
    m_fIndex.SetPtrPosBegin( x_pGuidId->Data1 & eIndexMask );
    if ( !m_fIndex.Read( &uLocation, sizeof( uLocation ), &uRead ) 
         || uRead != sizeof( uLocation ) )
        return CStr8();

    // Ensure the path exists
    CStr8 sPath = CStr8::BuildPath( m_sRoot, CStr8( uLocation ) );
    if ( !CFile::Exists( oexStr8ToStr( sPath ).Ptr() ) ) 
        return CStr8();

    CFile f;
    if ( !f.OpenExisting( oexStr8ToStr( CStr8::BuildPath( sPath, CStr8().GuidToString( x_pGuidId ) << ".dat" ) ).Ptr() ).IsOpen() )
        return CStr8();

    // Read in the file data
    CStr8 str = f.Read();
    if ( sizeof( oexGUID ) * 2 >= str.Length() ) 
        return CStr8();

    // Verify hash
    oexGUID guid;
    oss::CMd5::Transform( &guid, str.Ptr( sizeof( oexGUID ) ), str.Length() - sizeof( oexGUID ) );
    if ( !guid::CmpGuid( &guid, (oexGUID*)str.Ptr() ) )
        return CStr8();

    // Verify it's the block we're looking for
    if ( !guid::CmpGuid( &guid, x_pGuidId ) )
        return CStr8();
    
    // Return the string
    return str.Sub( sizeof( oexGUID ) * 2 + sizeof( SBlockHeader ) );
}

/// Reads the specified index
CPropertyBag8 CVirtualFs::ReadIndex( CStr8 x_sFolder )
{   
    x_sFolder.RTrim( "/\\" );

    CStr8 str = LoadFile( x_sFolder.Ptr(), 2 );

    return CParser::Deserialize( str );
}

/// Writes the specified index to disk
oexBOOL CVirtualFs::WriteIndex( oexCSTR8 x_pFolder, CPropertyBag8 &pbIdx )
{
    CStr8 str = CParser::Serialize( pbIdx );
    return SaveFile( x_pFolder, str, 2 );
}

oexBOOL CVirtualFs::Delete( oexCSTR8 x_pFile )
{   
    CStr8 sPath = CStr8( x_pFile ).GetPath();
    CStr8 sFile = CStr8( x_pFile ).GetFileName();
    
    CPropertyBag8 pb = ReadIndex( sPath );
    if ( !pb.Size() || !pb[ sFile ].IsSet() )
        return oexFALSE;

    // +++ this probably isn't working
    pb.Unset( sFile );

    WriteIndex( sPath.Ptr(), pb );

    return oexTRUE;
}

oexBOOL CVirtualFs::Link( oexCSTR8 x_pFile, oexCONST oexGUID *x_pGuid, oexUINT x_uSize, oexUINT x_uType )
{   
    // Don't link the top level
    CStr8 sPath = CStr8( x_pFile ).GetPath();
    CStr8 sFile = CStr8( x_pFile ).GetFileName();
    if ( !sPath.Length() || !sFile.Length() )
        return oexFALSE;
    
    // Read the index
    CPropertyBag8 pb = ReadIndex( sPath.Ptr() );
    
    // Save file info
    pb[ sFile ][ "type" ] = x_uType;
    if ( x_pGuid ) pb[ sFile ][ "id" ] = CStr8().GuidToString( x_pGuid );
    if ( x_uSize ) pb[ sFile ][ "size" ] = x_uSize;

    // Write the new index
    WriteIndex( sPath.Ptr(), pb );

    return oexTRUE;
}

oexBOOL CVirtualFs::MakeFolder( oexCSTR8 x_pFile )
{
    CStrList8 lst = CParser::Split( x_pFile, "\\/" );
    if ( !lst.Size() )
        return oexFALSE;

    CStr8 sPath;
    for ( CStrList8::iterator it; lst.Next( it ); )
    {   
        // Build path up to this level
        if ( !sPath.Length() ) sPath = *it;
        else 
        {
            // Build path
            sPath = CStr8::BuildPath( sPath, *it );

            // Create folder
            Link( sPath.Ptr(), oexNULL, 0, 2 );

        } // end else
        
    } // end for

    return oexTRUE;
}