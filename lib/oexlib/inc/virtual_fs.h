/*------------------------------------------------------------------
// virtual_fs.h
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
// CVirtualFs
//
/// Virtual file system
/**
*/
//==================================================================
class CVirtualFs  
{
public:

    enum
    {
        eIndexMask = 0xffffff
    };

    enum
    {
        // Block size
        eBlockSize = 64 * 1024
    };

#pragma pack( push, 1 )

    struct SBlockHeader
    {   oexINT32        uSize;
        oexINT32        uType;
        oexINT32        uFlags;
        oexINT32        uBlocks;
    };

#pragma pack( pop )
        
public:

    /// Default constructor
    CVirtualFs() 
    {   m_sRoot = "e:/nobck/vfs";
        m_fIndex.OpenAlways( oexStr8ToStr( m_sRoot ).Ptr() );
    }

    /// Destructor
    virtual ~CVirtualFs() { Destroy(); }

    /// Closes the virtual file system
    oexBOOL Destroy();

    //==============================================================
    // Open
    //==============================================================
    /// Opens the virtual file system at the specified root
    /**
        \param [in] x_pRoot     -   Root folder in which to open
                                    a virtual file system.  If the
                                    file system does not already
                                    exist, it is created.
    */
    oexBOOL Open( oexCSTR8 x_pRoot );    

    /// Saves a file into the virtual file system
    oexBOOL SaveFile( oexCSTR8 x_pFile, oexCSTR8 x_pData, oexUINT x_uSize, oexUINT x_uType = 1 );

    /// Saves a file into the virtual file system
    oexBOOL SaveFile( oexCSTR8 x_pFile, CStr8 &x_sStr, oexUINT x_uType = 1 )
    {   return SaveFile( x_pFile, x_sStr.Ptr(), x_sStr.Length(), x_uType ); }

    /// Writes out a single data block
    oexBOOL WriteDataBlock( oexGUID *x_pId, oexCPVOID x_pData, oexUINT x_uSize, CStr8 &x_sRoot, oexUINT x_uLocation );

    /// Loads the specified file from virtual storage
    CStr8 LoadFile( oexCSTR8 x_pFile, oexUINT x_uType = 0 );

    /// Loads a file block
    CStr8 ReadDataBlock( oexGUID *x_pGuidId );

    /// Reads the specified index
    CPropertyBag8 ReadIndex( CStr8 x_sFolder );

    /// Writes the specified index to disk
    oexBOOL WriteIndex( oexCSTR8 x_pFolder, CPropertyBag8 &lstIndex );

    /// Returns non-zero if a VFS is currently open
    oexBOOL IsOpen() { return m_fIndex.IsOpen(); }

    /// Returns the root
    CStr8& GetRoot() { return m_sRoot; }

    /// Deletes the specified file record
    oexBOOL Delete( oexCSTR8 x_pFile );

    /// Adds a link to the file
    oexBOOL Link( oexCSTR8 x_pFile, oexCONST oexGUID *x_pGuid, oexUINT x_uSize, oexUINT x_uType );

    /// Creates a folder
    oexBOOL MakeFolder( oexCSTR8 x_pFile );


private:

    /// The root path of the virtual file system
    CStr8                        m_sRoot;

    /// Find data
    CFile                       m_fIndex;

    /// Tag memory
    TMem< oexUCHAR >            m_memTag;

};

