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
	m_hFind = oexNULL;
	FindFirst( x_pDir, x_pMask );
}

CFindFiles::~CFindFiles()
{
	Destroy();
}

void CFindFiles::Destroy()
{
	// Close the find
	if ( m_hFind != oexNULL )
    {	os::CBaseFile::FindClose( m_hFind );
		m_hFind = oexNULL;
	} // end if

    m_sPath.Destroy();
    m_fd.sName.Destroy();
}

oexBOOL CFindFiles::FindFirst( oexCSTR x_pDir, oexCSTR x_pMask )
{
	// Lose previous find
	Destroy();

    // Save the path
    m_sPath = x_pDir;

    // Find the first file
    m_hFind = os::CBaseFile::FindFirst( x_pDir, x_pMask, &m_fd );
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


