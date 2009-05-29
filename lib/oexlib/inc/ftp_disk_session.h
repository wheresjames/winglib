/*------------------------------------------------------------------
// ftp_disk_session.h
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
// CFtpDiskSession
//
/// Disk file backed FTP
/**

*/
//==================================================================
class CFtpDiskSession : public CFtpSession
{

public:

    /// Constructor
    CFtpDiskSession()
    {
        // Root folder
        SetRoot( "c:/ftp" );
    }

    /// Destructor
    virtual ~CFtpDiskSession()
    {
    }

public:

    /// Return non-zero if folder is valid
    virtual oexBOOL OnValidateFolder( oexCSTR8 x_pFolder )
    {   if ( !x_pFolder || !*x_pFolder )
            return oexFALSE;
        return CFile::Exists( oexStr8ToStrPtr( x_pFolder  ) );
    }

    /// Called when the specified file needs deleting
    virtual oexBOOL OnDeleteFile( oexCSTR8 x_pFile )
    {   CStr sFile( oexStr8ToStrPtr( x_pFile ) );
        if ( !CFile::Exists( sFile.Ptr() ) )
            return oexFALSE;
        return CFile::Delete( sFile.Ptr() );
    }

    /// Called to create the specified folder
    virtual oexBOOL OnCreateFolder( oexCSTR8 x_pFolder )
    {   CStr sFile( oexStr8ToStrPtr( x_pFolder ) );
        if ( CFile::Exists( sFile.Ptr() ) )
            return oexTRUE;
        return os::CBaseFile::CreateFolder( sFile.Ptr() );
    }

    /// Called to remove the specified folder
    virtual oexBOOL OnRemoveFolder( oexCSTR8 x_pFolder )
    {   CStr sFile( oexStr8ToStrPtr( x_pFolder ) );
        if ( !CFile::Exists( sFile.Ptr() ) )
            return oexTRUE;
        return os::CBaseFile::RemoveFolder( sFile.Ptr() );
    }

    /// Returns the file list
    virtual CStr8 OnGetFileList( oexCSTR8 x_pFolder )
    {
        CStr8 sStr;
   	    CFindFiles ff;

        // Build file list
        if ( ff.FindFirst( oexStr8ToStrPtr( x_pFolder ) ) )
        {
            do
            {
                // Format file information
                sStr << CStr8().Fmt(  "%crwxrwxrwx    1 user     group %u %s %s\n",
                                      ( ff.GetFileAttributes() & os::CBaseFile::eFileAttribDirectory ) ? 'd' : '-',
                                      (oexUINT)ff.FileData().llSize,
                                      oexStrToStr8Ptr(
                                        CSysTime( CSysTime::eFmtFile, ff.FileData().ftLastModified )
                                            .FormatTime( oexT( "%b %d %Y" ) ) ),
                                      oexStrToStr8Ptr( ff.GetFileName() ) );

            } while ( ff.FindNext() );

        } // end if

        return sStr;
    }

    /// Called to remove the specified folder
    virtual oexBOOL OnReadStart( oexCSTR8 x_pFile )
    {   return m_file.OpenExisting( oexStr8ToStrPtr( x_pFile ) ).IsOpen(); }

    /// Called to remove the specified folder
    virtual CStr8 OnReadData( oexINT64 x_llBytes )
    {   return m_file.Read( 16 * 1024 ); }

    /// Called to complete the read
    virtual oexBOOL OnReadEnd()
    {   return m_file.Destroy(); }

    /// Called to remove the specified folder
    virtual oexBOOL OnWriteStart( oexCSTR8 x_pFile )
    {   return m_file.CreateAlways( oexStr8ToStrPtr( x_pFile ) ).IsOpen(); }

    /// Called to remove the specified folder
    virtual CStr8 OnWriteData( CStr8 x_sData )
    {   return m_file.Write( x_sData ); }

    /// Called to complete the read
    virtual oexBOOL OnWriteEnd()
    {   return m_file.Destroy(); }

private:

    /// File
    CFile   m_file;
};

