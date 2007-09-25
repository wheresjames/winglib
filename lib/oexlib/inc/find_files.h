/*------------------------------------------------------------------
// find_files.h
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
// CFindFiles
//
/// Simple wrapper for the Windows Find Files API
/**
	Use this class to search for files and directories.

	To search sub-folders as well, use TRecursiveFindFiles.

	Typical example:

  \code

	CFindFiles ff;

	if ( ff.FindFirst( "C:\\", "*.*" ) )
		do
		{

			if ( ff.IsDirectory() ) ; // Use folder

			else ; // Use file

		} while ( ff.FindNext() );

  \endcode

*/
//==================================================================
class CFindFiles  
{
public:

	//==============================================================
	// Destroy()
	//==============================================================
	/// Releases all resources used by this class instance
	void Destroy();

	//==============================================================
	// BuildPaths()
	//==============================================================
	/// Builds paths to current file
	/**		
		\return Non-zero if success
	*/
	oexBOOL BuildPaths()
    {
	    // Ensure valid filename
	    return ( m_fd.sName.Length() &&
             !m_fd.sName.CmpLen( oexT( "." ), 1 ) &&
             !m_fd.sName.CmpLen( oexT( ".." ), 2 ) );
    }


	//==============================================================
	// FindNext()
	//==============================================================
	/// Finds the next matching file
	/**		
		\return Non-zero if another file is found
	*/
	oexBOOL FindNext();

	//==============================================================
	// FindFirst()
	//==============================================================
	/// Finds the first matching file
	/**
		\param [in] x_pDir		-	Directory to start search
		\param [in] x_pMask		-	File search mask

		\return Non-zero if a matching file was found
	
		\see 
	*/
	oexBOOL FindFirst( oexCSTR x_pDir, oexCSTR x_pMask = oexT( "*.*" ) );

	/// Constructor
	CFindFiles();

	//==============================================================
	// FindFirst()
	//==============================================================
	/// Construcs object and finds the first matching file
	/**
		\param [in] pDir		-	Directory to start search
		\param [in] pMask		-	File search mask
		
		\return Non-zero if a matching file was found
	
		\see 
	*/
	CFindFiles( oexCSTR x_pDir, oexCSTR x_pMask = oexT( "*.*" ) );

	/// Destructor
	virtual ~CFindFiles();

	//==============================================================
	// GetPath()
	//==============================================================
	/// Returns the path to the found file
	CStr GetPath() 
    {   if ( !IsValid() ) return oexT( "" ); return m_sPath; }

	//==============================================================
	// GetFullPath()
	//==============================================================
	/// Returns the complete filename for the found file
    CStr GetFullPath() 
    {   if ( !IsValid() ) return oexT( "" ); return CStr::BuildPath( m_sPath.Ptr(), m_fd.sName ); }

	//==============================================================
	// GetFileName()
	//==============================================================
	/// Returns the filename for the current matching file
	CStr GetFileName() 
    {   if ( !IsValid() ) return oexT( "" ); return m_fd.sName; }

	//==============================================================
	// GetFileAttributes()
	//==============================================================
	/// Returns the file attributes of the current matching file
	oexUINT GetFileAttributes() 
    {   if ( !IsValid() ) return 0; return m_fd.uFileAttributes; }

	//==============================================================
	// IsDirectory()
	//==============================================================
	/// Returns non-zero if the current match is a directory
	oexBOOL IsDirectory() 
    {	if ( !IsValid() ) return oexFALSE; 
        return ( 0 != ( os::CBaseFile::eFileAttribDirectory & m_fd.uFileAttributes ) ); 
    }

    /// Returns non-zero if a find is in progress
    oexBOOL IsValid()
    {   return os::CBaseFile::vInvalidFindHandle() != m_hFind; }

    /// Returns extended file information
    os::CBaseFile::SFindData& FileData() { return m_fd; }

    /// Returns a list of all matching files
    static CStrList GetFileList( oexCSTR x_pDir, oexCSTR x_pMask = oexT( "*.*" ) );

private:

	/// Currently open find handle
    os::CBaseFile::t_HFIND				m_hFind;

	/// Buffer containing path to file
	CStr				                m_sPath;

    /// Find data
    os::CBaseFile::SFindData            m_fd;

};


//==================================================================
// TRecursiveFindFiles
//
/// Creates an array of CFindFiles to support recursive searches
/**

	The functions in this class, by design, exactly match CFindFiles
	so it is a drop in replacement.

	This is a template class because I didn't want to use dynamic 
	memory allocation since this could slow down the searches.  I 
	also don't know the maximum folder depth you may want to parse.  
	Using a 'large enough' number like 32 or 64 would result
	in a pretty large amount of memory.  So I allow (force) you to
	decide for yourself.

	I also failed to supply a default to avoid causing any pain
	to those that may not otherwise realize that the depth is 
	limited.

	At the time I am writing this, each depth will cost you 
	112 bytes.  sizeof( CFindFiles )

	Typical example:

  \code

	TRecursiveFindFiles< 16 > rff;

	if ( rff.FindFirst( "C:\\", "*.*" ) )
		do
		{

			if ( rff.IsDirectory() ) ; // Use folder

			else ; // Use file

		} while ( rff.FindNext() );

  \endcode

*/
//==================================================================
template < oexCONST oexLONG c_lMaxDepth > class TRecursiveFindFiles  
{
public:

	/// Default constructor
	TRecursiveFindFiles()  { m_uDepth = 0; }
	
	/// Destructor
	virtual ~TRecursiveFindFiles() {}

	//==============================================================
	// TRecursiveFindFiles()
	//==============================================================
	/// Construcs object and finds the first matching file
	/**
		\param [in] x_pDir		-	Directory to start search
		\param [in] x_pMask		-	File search mask
		
		\return Non-zero if a matching file was found
	
		\see 
	*/
	TRecursiveFindFiles( oexCSTR x_pDir, oexCSTR x_pMask = oexT( "*.*" ) )
	{	m_uDepth = 0; FindFirst( x_pDir, x_pMask ); }

	//==============================================================
	// Destroy()
	//==============================================================
	/// Releases all resources used by this class instance
	void Destroy()
	{
		// Validate depth
		if ( c_lMaxDepth <= m_dwDepth )
			m_uDepth = c_lMaxDepth;

		do 
		{				
			// Destroy this depth
			m_ff[ m_uDepth ].Destroy();

			// Next depth
			if ( m_uDepth ) m_uDepth--;

		} while ( m_uDepth );
	}

	//==============================================================
	// BuildPaths()
	//==============================================================
	/// Builds paths to current file
	/**		
		\return Non-zero if success
	*/
	oexBOOL BuildPaths() { return m_ff[ m_uDepth ].BuildPath(); }

	//==============================================================
	// FindNext()
	//==============================================================
	/// Finds the next matching file
	/**		
		\return Non-zero if another file is found
	*/
	oexBOOL FindNext()
	{
		// Ensure valid depth
		if ( c_lMaxDepth <= m_dwDepth ) 
			m_uDepth = c_lMaxDepth - 1;

		// Recurse into directory if this is one
		if ( m_ff[ m_uDepth ].IsDirectory() )
		{
			// Do we have another valid depth
			oexUINT uNext = m_uDepth + 1;
			if ( c_lMaxDepth > uNext )
			{
				// See if we can find any files in this folder
				if ( m_ff[ uNext ].FindFirst( m_ff[ m_uDepth ].GetFullPath().Ptr(), m_sSearchMask.Ptr() ) );
				{
					// Accept this depth as current
					m_uDepth = uNext;

					return oexTRUE;

				} // end if

			} // end if

		} // end if

		// Try to find another file
		for( ; ; )
		{
			// Is there another file at this depth?
			if ( m_ff[ m_uDepth ].FindNext() )
				return oexTRUE;

			// Any higher ground?
			if ( !m_uDepth ) 
                return oexFALSE;

			// Next depth
			m_uDepth--;

		} // end for

		// Unreachable...
		return oexFALSE;
	}

	//==============================================================
	// FindFirst()
	//==============================================================
	/// Finds the first matching file
	/**
		\param [in] x_pDir		-	Directory to start search
		\param [in] x_pMask		-	File search mask
		
		\return Non-zero if a matching file was found
	
		\see 
	*/
	oexBOOL FindFirst( oexCSTR x_pDir, oexCSTR x_pMask = oexT( "*.*" ) )
	{
        if ( !oexVERIFY_PTR( x_pDir ) || !oexVERIFY_PTR( x_pMask ) )
            return oexFALSE;

		// We have to save the search mask for later
		m_sSearchMask = x_pMask;

		// Pass it on
		return m_ff[ m_uDepth ].FindFirst( x_pDir, x_pMask ); 
	}


	//==============================================================
	// GetPath()
	//==============================================================
	/// Returns the path to the found file
	CStr GetPath() { return m_ff[ m_uDepth ].GetPath(); }

	//==============================================================
	// GetFullPath()
	//==============================================================
	/// Returns the complete filename for the found file
	CStr GetFullPath() { return m_ff[ m_uDepth ].GetFullPath(); }

	//==============================================================
	// GetFileName()
	//==============================================================
	/// Returns the filename for the current matching file
	CStr GetFileName() { return m_ff[ m_uDepth ].GetFileName(); }

	//==============================================================
	// GetFileAttributes()
	//==============================================================
	/// Returns the file attributes of the current matching file
	oexUINT GetFileAttributes() { return m_ff[ m_uDepth ].GetFileAttributes(); }

	//==============================================================
	// IsDirectory()
	//==============================================================
	/// Returns non-zero if the current match is a directory
	oexBOOL IsDirectory() { return m_ff[ m_uDepth ].IsDirectory(); }

    /// Returns extended file information
    os::CBaseFile::SFindData& FileData() { return m_ff[ m_uDepth ].FileData(); }

private:

	/// Current folder depth
	oexUINT				m_uDepth;

	/// CFindFiles object array
	CFindFiles			m_ff[ c_lMaxDepth ];

	/// Search mask
	CStr                m_sSearchMask;

};


