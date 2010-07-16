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

	/// Size type
	typedef oexFILESIZE_T	t_size;

    /// File handle type
    typedef oexPVOID t_HFILE;

	/// Find files type
    typedef oexPVOID t_HFIND;

    /// Invalid file handle value
	static const t_HFILE c_Invalid;

	/// Invalid find files handle value
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

	/// Special folder id's
	enum
	{
		eFidNone = 0xff00,

		eFidTemp,

		eFidUserSystem,

		eFidUserOs,

		eFidCurrent,

		eFidDefRoot,

		eFidFonts
	};

private:

	/// Default constructor
	CBaseFile() {}

	/// Destructor
    ~CBaseFile() {}

public:

	// Must be called before any other file functions
	static oexBOOL InitFileSystem();

	// Must be called before program closes
	static oexBOOL FreeFileSystem();

public:

	/// Creates or opens the specified file
    static t_HFILE Create( oexCSTR x_pFile, oexUINT x_eDisposition, oexUINT x_eAccess, oexUINT x_eShare, oexUINT x_uFlags, oexINT *x_pnError );

	/// Closes the specified file handle
    static oexBOOL Close( t_HFILE x_hFile, oexINT *x_pnErr );

	/// Invalid file handle value
    static t_HFILE vInvalid()
    {   return c_Invalid; }

	/// Writes data to a file handle
    static oexBOOL Write( t_HFILE x_hFile, oexCPVOID x_pData, t_size x_llSize, t_size *x_pllWritten, oexINT *x_pnErr );

	/// Reads data from a file handle
    static oexBOOL Read( t_HFILE x_hFile, oexPVOID x_pData, t_size x_llSize, t_size *x_pllRead, oexINT *x_pnErr );

	/// Returns the size of the specified file
    static t_size Size( t_HFILE hFile );

	/// Sets the file pointer
    static t_size SetPointer( t_HFILE x_hFile, t_size llMove, oexINT nMethod );

	/// Removes the specified folder
    static oexBOOL RemoveFolder( oexCSTR x_pDir );

	/// Deletes the specified file
    static oexBOOL Delete( oexCSTR x_pFile );
    
    /// Renames the specified file
	static oexBOOL Rename( oexCSTR x_pOld, oexCSTR x_pNew );    

	/// Gets file attributes
    static oexUINT GetFileAttrib( oexCSTR x_pFile );

	/// Sets file attributes
    static oexBOOL SetFileAttrib( oexCSTR x_pFile, oexUINT x_uAttrib );

	/// Returns non-zero if the specified file or directory exists
    static oexBOOL DoesExist( oexCSTR x_pPath );

	/// Recursively creates the specified folder
    static oexBOOL CreateFolder( oexCSTR x_pPath );

	/// Returns the fully qualified path to the current module
	static CStr GetModPath( oexCSTR x_pPath = oexNULL );

	/// Returns the fully qualified file name the to the current module
	static CStr GetModFileName( oexCPVOID x_pInstance = oexNULL );

	/// Flushes file data to disk
	static oexBOOL Flush( t_HFILE x_hFile );

	/// Returns the path to the specified folder
	static CStr GetSysFolder( oexINT x_nFolderId, oexINT x_nMaxLength = oexSTRSIZE );

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
