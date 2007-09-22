/*------------------------------------------------------------------
// file.cpp
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


oexBOOL CFile::Destroy()
{
    oexBOOL bRet = oexFALSE;

    // Close any open file
    if ( os::CBaseFile::vInvalid() != m_hFile )
    {   bRet = os::CBaseFile::Close( m_hFile, &m_uLastError );
        m_hFile = os::CBaseFile::vInvalid();
    } // end if

    // Lose the file name
    m_sFileName.Destroy();

    return bRet;
}

CStr CFile::Read( oexUINT x_uSize )
{   
    oexINT64 llSize = Size();
    if ( x_uSize == 0 || x_uSize > llSize )
        x_uSize = (oexUINT)llSize;

    CStr str;
    if ( !str.OexAllocate( x_uSize ) )
        return oexFALSE;

    oexUINT uRead = 0;
    oexBOOL bRet = Read( str._Ptr(), x_uSize, &uRead );
    
    if ( !bRet ) str.Destroy();
    else str.SetLength( uRead );

    return str;        
}

oexBOOL CFile::DeletePath( oexCSTR x_pPath, oexBOOL x_bEmpty )
{
	CFindFiles ff;

    // Are there any files in the directory?
	if ( !ff.FindFirst( x_pPath, oexT( "*.*" ) ) )
        return os::CBaseFile::DeleteDirectory( x_pPath );

    // Dump everything in the directory
	do
	{
        if ( ff.IsDirectory() ) 
            DeletePath( ff.GetFullPath().Ptr(), oexFALSE );
		else 
            os::CBaseFile::Delete( ff.GetFullPath().Ptr() );

	} while ( ff.FindNext() );

    // Do we just want to empty the directories?
    if ( x_bEmpty )
        return oexTRUE;

    // Delete the directory
    return os::CBaseFile::DeleteDirectory( x_pPath );
}

oexBOOL CFile::CreatePath( oexCSTR x_pPath )
{
    CStrList lst = CParser::Split( x_pPath, oexT( "\\/" ) );
    if ( !lst.Size() )
        return oexFALSE;

    CStr sPath;
    for ( CStrList::iterator it; lst.Next( it ); )
    {   
        // Build path up to this level
        if ( !sPath.Length() ) sPath = *it;
        else sPath = CFile::BuildPath( sPath, *it );

        // Create this part of the path if it doesn't exist
        if ( !os::CBaseFile::DoesExist( sPath.Ptr() ) )
            if ( !os::CBaseFile::CreateFolder( sPath.Ptr() ) )
                return oexFALSE;
        
    } // end for

    return oexTRUE;
}

