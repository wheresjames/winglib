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
{_STT();
    oexBOOL bRet = oexFALSE;

    // Close any open file
    if ( os::CBaseFile::vInvalid() != m_hFile )
    {   bRet = os::CBaseFile::Close( m_hFile, &m_nLastError );
        m_hFile = os::CBaseFile::vInvalid();
    } // end if

    // Lose the file name
    m_sFileName.Destroy();

    return bRet;
}

CStr8 CFile::Read( CFile::t_size x_llSize )
{_STT();
    t_size llSize = Size();
    if ( x_llSize == 0 )
	{
		if ( llSize )
        	x_llSize = (oexUINT)llSize;

		// +++ Hmmmm ...  This may mean we can't get the size
		else
			return CStr8(); // x_llSize = 16 * 1024;

	} // end if

	// Trim down if too much was asked
	else if ( llSize && x_llSize > llSize )
        x_llSize = (oexUINT)llSize;

	// Punt if no
	if ( !x_llSize )
		return CStr8();

    CStr8 str;
    if ( !str.OexAllocate( x_llSize ) )
        return CStr8();

    t_size llRead = 0;
    oexBOOL bRet = Read( str._Ptr(), x_llSize, &llRead );

    if ( !bRet ) str.Destroy();
    else str.SetLength( llRead );

    return str;
}

oexBOOL CFile::DeletePath( oexCSTR x_pPath, oexBOOL x_bEmpty )
{_STT();
	CFindFiles ff;

    // Are there any files in the directory?
	if ( !ff.FindFirst( x_pPath, oexT( "*" ) ) )
        return os::CBaseFile::RemoveFolder( x_pPath );

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
    return os::CBaseFile::RemoveFolder( x_pPath );
}

oexBOOL CFile::CreatePath( oexCSTR x_pPath )
{_STT();
	// Ensure string
	if ( !oexCHECK_PTR( x_pPath ) || !*x_pPath )
		return oexFALSE;

	// Punt if the path already exists
	if ( os::CBaseFile::DoesExist( x_pPath ) )
		return oexTRUE;

    CStrList lst = CParser::Split( x_pPath, oexT( "\\/" ) );
    if ( !lst.Size() )
        return oexFALSE;

    CStr sPath;
    for ( CStrList::iterator it; lst.Next( it ); )
    {
        // Build path up to this level
        if ( !sPath.Length() )
        {  	for ( oexINT i = 0; oexT( '/' ) == x_pPath[ i ] || oexT( '\\' ) == x_pPath[ i ]; i++ )
        		sPath << x_pPath[ i ];
        	sPath << *it;
		} // end if

        else
        	sPath = CStr::BuildPath( sPath, *it );

        // Create this part of the path if it doesn't exist
        if ( !os::CBaseFile::DoesExist( sPath.Ptr() ) )
            if ( !os::CBaseFile::CreateFolder( sPath.Ptr() ) )
                return oexFALSE;

    } // end for

    return oexTRUE;
}

CFile::t_size CFile::FindInFile( oexPVOID x_pStr, CFile::t_size x_llLen, CFile::t_size x_llMax )
{_STT();
	if ( !x_pStr || !x_llLen || ( x_llMax > 0 && x_llMax < x_llLen ) )
		return -1;

	TMem< char > buf;
	if ( !buf.OexNew( x_llLen + 1024 ).Ptr() )
		return -1;

	// Restore file position after this
	CFile::CRestoreFilePos rfp( this );

	// First just check to see if we're there already
	t_size llRead = 0;
	if ( !Read( buf.Ptr(), x_llLen, &llRead ) || llRead != x_llLen )
		return -1;

	// Current position if it matches
	if ( !oexMemCmp( x_pStr, buf.Ptr(), x_llLen ) )
		return rfp.Get();

	t_size llSize = buf.Size();
	if ( !Read( buf.Ptr( x_llLen ), llSize - x_llLen, &llRead ) || !llRead )
		return -1;

	llSize = llRead + x_llLen;
	t_size llFile = 0;
	while ( llSize >= x_llLen )
	{
		// Check for string
		t_size i;
		for ( i = 0; ( llSize - i ) >= x_llLen; i++ )
			if ( !oexMemCmp( x_pStr, buf.Ptr( i ), x_llLen ) )
				return rfp.Set( llFile + i );

		// Get more data from file
		oexMemCpy( buf.Ptr(), buf.Ptr( i ), llSize - i );
		if ( !Read( buf.Ptr( llSize - i ), i, &llRead ) || !llRead )
			return -1;

		// New buffer size
		llSize = ( llSize - i ) + llRead;
		llFile += i;

	} // end while

	return -1;

}

