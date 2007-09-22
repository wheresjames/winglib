/*------------------------------------------------------------------
// file.h
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
// CFile
//
/// File access
/**
	
*/
//==================================================================
class CFile
{
public:

    /// Default constructor
    CFile()
    {
        // Start with an invalid file handle
        m_hFile = os::CBaseFile::vInvalid();

        m_uLastError = 0;

    }

    /// Destructor
    ~CFile()
    {   Destroy(); }

    /// Creates a new file, fails if file already exists
    CFile& CreateNew(      oexCSTR x_pFile, 
                            oexUINT x_eAccess = os::CBaseFile::eAccessRead | os::CBaseFile::eAccessWrite, 
                            oexUINT x_eShare = os::CBaseFile::eShareRead | os::CBaseFile::eShareWrite )
    {   return Create( x_pFile, os::CBaseFile::eDisCreateNew, x_eAccess, x_eShare ); }

    /// Creates a new file, truncates any existing file
    CFile& CreateAlways(   oexCSTR x_pFile, 
                            oexUINT x_eAccess = os::CBaseFile::eAccessRead | os::CBaseFile::eAccessWrite, 
                            oexUINT x_eShare = os::CBaseFile::eShareRead | os::CBaseFile::eShareWrite )
    {   return Create( x_pFile, os::CBaseFile::eDisCreateAlways, x_eAccess, x_eShare ); }

    /// Opens an existing file, fails if file doesn't exist
    CFile& OpenExisting(   oexCSTR x_pFile, 
                            oexUINT x_eAccess = os::CBaseFile::eAccessRead | os::CBaseFile::eAccessWrite, 
                            oexUINT x_eShare = os::CBaseFile::eShareRead | os::CBaseFile::eShareWrite )
    {   return Create( x_pFile, os::CBaseFile::eDisOpenExisting, x_eAccess, x_eShare ); }

    /// Opens an existing file, creats if file doesn't exist
    CFile& OpenAlways(     oexCSTR x_pFile, 
                            oexUINT x_eAccess = os::CBaseFile::eAccessRead | os::CBaseFile::eAccessWrite, 
                            oexUINT x_eShare = os::CBaseFile::eShareRead | os::CBaseFile::eShareWrite )
    {   return Create( x_pFile, os::CBaseFile::eDisOpenAlways, x_eAccess, x_eShare ); }

    /// Creates file
    CFile& Create(     oexCSTR x_pFile, oexUINT x_eDisposition,
                            oexUINT x_eAccess = os::CBaseFile::eAccessRead | os::CBaseFile::eAccessWrite, 
                            oexUINT x_eShare = os::CBaseFile::eShareRead | os::CBaseFile::eShareWrite )
    {   Destroy(); 
        m_hFile = os::CBaseFile::Create( x_pFile, x_eDisposition, x_eAccess, x_eShare, 0, &m_uLastError );
        if ( os::CBaseFile::vInvalid != m_hFile ) m_sFileName = x_pFile;
        return *this;
    }


    /// Closes the file
    oexBOOL Close() { return Destroy(); }

    /// Closes the file
    oexBOOL Destroy();

    /// Returns the file size
    oexINT64 Size() { return os::CBaseFile::Size( m_hFile ); }

	//==============================================================
	// Read()
	//==============================================================
	/// Reads data from file
	/**
		\param [out] x_pBuf			-	Receives data read from file
		\param [in]  x_uSize		-	Size of buffer in x_pBuf
		\param [out] x_puRead		-	Receives number of bytes read
		
		\return Non-zero if success
	
		\see 
	*/
    oexBOOL Read( oexPVOID x_pBuf, oexUINT x_uSize, oexUINT *x_puRead = oexNULL )
    {   if ( os::CBaseFile::vInvalid() == m_hFile ) return oexFALSE;
        return os::CBaseFile::Read( m_hFile, x_pBuf, x_uSize, x_puRead, &m_uLastError ); 
    }

	//==============================================================
	// Read()
	//==============================================================
	/// Reads data from file
	/**
		\param [in]  x_uSize		-	Number of bytes to read
		
		\return CStr object containing the data
	
		\see 
	*/
    CStr8 Read( oexUINT x_uSize = 0 );

	//==============================================================
	// Write()
	//==============================================================
	/// Writes data to file
	/**
		\param [in]  x_pBuf			-	Data to write
		\param [in]  x_uSize		-	Number of bytes to write
		\param [out] x_puWritten    -	Receives number of bytes written
		
		\return Non-zero if success
	
		\see 
	*/
    oexBOOL Write( oexCPVOID x_pBuf, oexUINT x_uSize, oexUINT *x_puWritten = oexNULL )
    {   if ( os::CBaseFile::vInvalid() == m_hFile ) return oexFALSE;
        return os::CBaseFile::Write( m_hFile, x_pBuf, x_uSize, x_puWritten, &m_uLastError ); 
    }

	//==============================================================
	// Write()
	//==============================================================
	/// Writes a string to the file
	/**
		\param [in] x_sStr			-	Data to write
		
		\return Non-zero if success
	
		\see 
	*/
    oexBOOL Write( CStr8 x_sStr, oexUINT *x_puWritten = oexNULL )
    {   return Write( x_sStr.Ptr(), x_sStr.LengthInBytes(), x_puWritten ); 
    }

    /// File write operator
    CFile& operator << ( CStr8 x_sStr ) { Write( x_sStr ); return *this; }

    /// Returns the file name
    CStr GetFileName() { return m_sFileName; }


	//==============================================================
	// GetPtrPos()
	//==============================================================
	/// Returns the current 64-bit file pointer position
	/**
		\return 64-bit file pointer position
	
		\see 
	*/
	oexINT64 GetPtrPos()
    {   if ( os::CBaseFile::vInvalid() == m_hFile ) return -1;
        return os::CBaseFile::SetPointer( m_hFile, 0, os::CBaseFile::eFileOffsetCurrent ); 
    }

	//==============================================================
	// SetPtrPos()
	//==============================================================
	/// Sets the 64-bit file pointer position
	/**
		\param [in] x_llPos	    -	64-bit file offset pointer
		\param [in] x_nMethod	-	Relative specifier for offset
		
		\return Non-zero if success
	
		\see 
	*/
    oexINT64 SetPtrPos( oexINT64 x_llPos, oexINT x_nMethod )
    {   if ( os::CBaseFile::vInvalid() == m_hFile ) return -1;
        return os::CBaseFile::SetPointer( m_hFile, x_llPos, x_nMethod ); 
    }

    //==============================================================
	// SetPtrPosBegin()
	//==============================================================
	/// Sets the 64-bit file pointer position relative to the beginning of the file
	/**
		\param [in] offset	-	64-bit file offset pointer
		
		\return Non-zero if success
	
		\see 
	*/
    oexINT64 SetPtrPosBegin( oexINT64 x_llPos )
    {   return SetPtrPos( x_llPos, os::CBaseFile::eFileOffsetBegin ); }

	//==============================================================
	// SetPtrPosEnd()
	//==============================================================
	/// Sets the 64-bit file pointer position relative to the end of the file
	/**
		\param [in] x_llPos	-	64-bit file offset pointer
		
		\return Non-zero if success
	
		\see 
	*/
    oexINT64 SetPtrPosEnd( oexINT64 x_llPos )
    {   return SetPtrPos( x_llPos, os::CBaseFile::eFileOffsetEnd ); }

	//==============================================================
	// SetPtrPosCur()
	//==============================================================
	/// Sets the 64-bit file pointer position relative to the current file pointer
	/**
		\param [in] x_llPos	-	64-bit file offset pointer
		
		\return Non-zero if success
	
		\see 
	*/
    oexINT64 SetPtrPosCur( oexINT64 x_llPos )
    {   return SetPtrPos( x_llPos, os::CBaseFile::eFileOffsetCurrent ); }

    /// Returns non-zero if the specified file is open
    oexBOOL IsOpen() { return os::CBaseFile::vInvalid() != m_hFile; } 

    /// Closes and deletes the current file
    oexBOOL Delete() 
    {   if ( !IsOpen() )
            return oexFALSE;
        CStr s = GetFileName();
        Destroy();
        return Delete( s.Ptr() );
    }

public:

    /// Concatinates two strings into a path
    static CStr BuildPath( CStr x_sRoot, CStr x_sPath, oexTCHAR tSep = oexT( '/' ) )
    {   return x_sRoot.RTrim( oexT( "\\/" ) ) << tSep << x_sPath.LTrim( oexT( "\\/" ) ); }

    /// Deletes the specified file
    static oexBOOL Delete( oexCSTR x_pFile )
    {   return os::CBaseFile::Delete( x_pFile ); }

    /// Deletes a path
    static oexBOOL DeletePath( oexCSTR x_pPath, oexBOOL x_bEmpty = oexFALSE );

    /// Creates a path
    static oexBOOL CreatePath( oexCSTR x_pPath );

    /// Returns non-zero if the path exists
    static oexBOOL Exists( oexCSTR x_pPath )
    {   return os::CBaseFile::DoesExist( x_pPath ); }

    /// Returns the root path of str
    static CStr RootPath( CStr str )
    {   str.RParse( oexT( "\\/" ) ); 
        return str;
    }

    /// Returns the root path of str
    static CStr ChopFileName( CStr str )
    {   return str.RParse( oexT( "\\/" ) )++; 
    }

private:
    
    /// File handle
    os::CBaseFile::t_HFILE              m_hFile;

    /// Name of the open file
    CStr                                m_sFileName;

    /// Last error value
    oexUINT                             m_uLastError;

};

