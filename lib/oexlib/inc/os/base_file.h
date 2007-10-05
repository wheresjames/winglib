/*------------------------------------------------------------------
// base_file.h
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

class CBaseFile
{
public:

    /// File handle type
    typedef oexPVOID t_HFILE;

    typedef oexPVOID t_HFIND;

    /// Invalid handle value
	static const t_HFILE c_Invalid;

	static const t_HFIND c_InvalidFindHandle;

    enum
    {
        /// Share read access
        eAccessRead = 1,

        /// Share write access
        eAccessWrite = 2,

        /// Execute access
        eAccessExecute = 4

    };

    enum
    {
        /// Share delete access
        eShareDelete = 1,

        /// Share read access
        eShareRead = 2,

        /// Share write access
        eShareWrite = 4

    };

    enum
    {
        /// Creates a new file only if it does not already exist.
        eDisCreateNew = 1,

        /// Creates a new file, truncating any existing file.
        eDisCreateAlways,

        /// Opens an existing file, fails if file doesn't exist.
        eDisOpenExisting,

        /// Opens an existing file, creates a new file if non exists.
        eDisOpenAlways

    };

    enum
    {
        /// Indicates a position relative to the beginning of the file
        eFileOffsetBegin = 1,

        /// Indicates a position relative to the current file position
        eFileOffsetCurrent,

        /// Indicates a position relative to the end of the file
        eFileOffsetEnd
    };

    enum
    {
        eFileAttribReadOnly = 0x00000001,
        eFileAttribHidden = 0x00000002,
        eFileAttribSystem = 0x00000004,
        eFileAttribDirectory = 0x00000010,
        eFileAttribArchive = 0x00000020,
        eFileAttribDevice = 0x00000040,
        eFileAttribNormal = 0x00000080,
        eFileAttribTemporary = 0x00000100,
        eFileAttribSparseFile = 0x00000200,
        eFileAttribReparsePoint = 0x00000400,
        eFileAttribCompressed = 0x00000800,  
        eFileAttribOffline = 0x00001000,
        eFileAttribNotContentIndexed = 0x00002000,
        eFileAttribEncrypted = 0x00004000

    };

private:
	CBaseFile() {}
    ~CBaseFile() {}

public:

    static t_HFILE Create( oexCSTR x_pFile, oexUINT x_eDisposition, oexUINT x_eAccess, oexUINT x_eShare, oexUINT x_uFlags, oexUINT *x_puError );

    static oexBOOL Close( t_HFILE x_hFile, oexUINT *x_puErr );

    static t_HFILE vInvalid() 
    {   return c_Invalid; }

    static oexBOOL Write( t_HFILE hFile, oexCPVOID x_pData, oexUINT x_uSize, oexUINT *x_puWritten, oexUINT *x_puErr );

    static oexBOOL Read( t_HFILE hFile, oexPVOID x_pData, oexUINT x_uSize, oexUINT *x_puRead, oexUINT *x_puErr );

    static oexINT64 Size( t_HFILE hFile );

    static oexINT64 SetPointer( t_HFILE hFile, oexINT64 llMove, oexINT nMethod );

    static oexBOOL RemoveFolder( oexCSTR x_pDir );

    static oexBOOL Delete( oexCSTR x_pFile );

    static oexUINT GetFileAttrib( oexCSTR x_pFile );

    static oexBOOL SetFileAttrib( oexCSTR x_pFile, oexUINT x_uAttrib );

    static oexBOOL DoesExist( oexCSTR x_pPath );

    static oexBOOL CreateFolder( oexCSTR x_pPath );


public:

    struct SFindData
    {   oexUINT     uFileAttributes;
        oexINT64    ftCreated;
        oexINT64    ftLastAccess;
        oexINT64    ftLastModified;
        oexINT64    llSize;
        CStr        sName;
    };

    static t_HFIND FindFirst( oexCSTR x_pPath, oexCSTR x_pMask, SFindData *x_pFd );

    static oexBOOL FindNext( t_HFIND x_hFind, SFindData *x_pFd );

    static oexBOOL FindClose( t_HFIND x_hFind );

    static t_HFIND vInvalidFindHandle() 
    {   return c_InvalidFindHandle; }

};
