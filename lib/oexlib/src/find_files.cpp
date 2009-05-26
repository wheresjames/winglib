/*------------------------------------------------------------------
// find_files.cpp
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

CFindFiles::CFindFiles()
{
    m_hFind = os::CBaseFile::vInvalidFindHandle();
}

CFindFiles::CFindFiles( oexCSTR x_pDir, oexCSTR x_pMask )
{
	m_hFind = os::CBaseFile::vInvalidFindHandle();
	FindFirst( x_pDir, x_pMask );
}

CFindFiles::~CFindFiles()
{
	Destroy();
}

void CFindFiles::Destroy()
{
	// Close the find
	if ( m_hFind != os::CBaseFile::vInvalidFindHandle() )
    {  	os::CBaseFile::FindClose( m_hFind );
		m_hFind = os::CBaseFile::vInvalidFindHandle();
	} // end if

    m_sPath.Destroy();
    m_sPattern.Destroy();
    m_fd.sName.Destroy();
}

oexBOOL CFindFiles::FindFirst( oexCSTR x_pDir, oexCSTR x_pMask )
{
	// Lose previous find
	Destroy();

    // Save the path
    m_sPath = x_pDir;

	// Save pattern
    if ( oexCHECK_PTR( x_pMask ) )
    	m_sPattern = x_pMask;

    // Find the first file
    m_hFind = os::CBaseFile::FindFirst( x_pDir, oexT( "*" ), &m_fd );
    if ( os::CBaseFile::vInvalidFindHandle() == m_hFind )
        return oexFALSE;

	// Can we use this one?
	if ( BuildPaths() )
        return oexTRUE;

	// Try the next one
	return FindNext();
}

oexBOOL CFindFiles::FindNext()
{
	// Punt if no find handle
    if ( os::CBaseFile::vInvalidFindHandle() == m_hFind )
        return oexFALSE;

	do
	{
		// Get next file
        if ( !os::CBaseFile::FindNext( m_hFind, &m_fd ) )
		{	Destroy(); return oexFALSE; }

	// Build the filename
	} while ( !BuildPaths() );

	return oexTRUE;
}

oexBOOL CFindFiles::BuildPaths()
{
	// Ensure valid filename
	if ( !m_fd.sName.Length()
		 || m_fd.sName.CmpLen( oexT( "." ), 1 )
		 || m_fd.sName.CmpLen( oexT( ".." ), 2 ) )
		return oexFALSE;

	// Ok if no pattern specified
	if ( !m_sPattern.Length() )
		return oexTRUE;

	// +++ I don't think case sensitivity is usually wanted for
	//     a file pattern match.

	// Match file
//#if defined( OEX_CASE_SENSITIVE_FILES )
//	return m_fd.sName.MatchPattern( m_sPattern, oexFALSE );
//#else
	return m_fd.sName.MatchPattern( m_sPattern, oexTRUE );
//#endif
}


CStrList CFindFiles::GetFileList( oexCSTR x_pDir, oexCSTR x_pMask )
{
    CStrList lst;
	CFindFiles ff;

    // Load each file into a list
	if ( ff.FindFirst( x_pDir, x_pMask ) )
        do { lst << ff.GetFileName();
        } while ( ff.FindNext() );

    return lst;
}


