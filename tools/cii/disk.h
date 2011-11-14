// disk.h

#pragma once

#include "str.h"

/**
	Platform independent file functions

	File find
	@code

		const char *pRoot = "c:\\Temp";

		ff::SFindData fd; ff::HFIND hFind;
		if ( ff::c_invalid_hfind != ( hFind = ff::FindFirst( pRoot, "*", &fd ) ) )
		{
			do { printf( "%s\n", ff::FilePath< char, std::basic_string< char > >( pRoot, fd.szName ).c_str() );
			} while ( ff::FindNext( hFind, &fd ) );

			ff::FindClose( hFind );

		} // end if

	@endcode

*/
namespace disk
{
	typedef unsigned long t_size;

#if defined( _WIN32 )
		typedef void* HFILE;
#else
		typedef int HFILE;
#endif

	const HFILE c_invalid_hfile = (HFILE)-1;

	template< typename T, typename T_STR >
		T_STR& RTrim( T_STR &s, T c )
		{	return s.erase( s.find_last_not_of( c ) + 1 ); }

	template< typename T, typename T_STR >
		T_STR& LTrim( T_STR &s, T c )
		{	return s.erase( 0, s.find_first_not_of( c ) ); }

	template < typename T >
		T ReplaceStr( const T s, const T a, const T b )
		{	T _s( s ); 
			typename T::size_type i = 0;
			while( T::npos != ( i = _s.find_first_of( a, i ) ) )
				_s.replace( i, a.length(), b ), i += b.length();
			return _s;
		}

	template < typename T_STR, typename T >
		T_STR& ReplaceChar( T_STR &s, const T a, const T b )
		{	typename T_STR::size_type i = 0;
			while( T_STR::npos != ( i = s.find_first_of( a, i ) ) )
				s[ i++ ] = b;
			return s;
		}

	template< typename T, typename T_STR >
		T_STR GetPath( T_STR s )
		{
			typename T_STR::size_type lb = s.find_last_of( tcTC( T, '\\' ) );
			typename T_STR::size_type lf = s.find_last_of( tcTC( T, '/' ) );
			if ( T_STR::npos == lb && T_STR::npos == lf )
				return T_STR();
			if ( T_STR::npos == lb )
				return T_STR( s.c_str(), 0, lf );
			else if ( T_STR::npos == lf )
				return T_STR( s.c_str(), 0, lb );
			else if ( lf > lb )
				return T_STR( s.c_str(), 0, lf );
			return T_STR( s.c_str(), 0, lb );
		}

	template< typename T, typename T_STR >
		T_STR GetName( T_STR s )
		{
			typename T_STR::size_type lb = s.find_last_of( tcTC( T, '\\' ) );
			typename T_STR::size_type lf = s.find_last_of( tcTC( T, '/' ) );
			if ( T_STR::npos == lb && T_STR::npos == lf )
				return s;
			if ( T_STR::npos == lb )
				return T_STR( s.c_str(), lf + 1, T_STR::npos );
			else if ( T_STR::npos == lf )
				return T_STR( s.c_str(), lb + 1, T_STR::npos );
			else if ( lf > lb )
				return T_STR( s.c_str(), lf + 1, T_STR::npos );
			return T_STR( s.c_str(), lb + 1, T_STR::npos );
		}

	template< typename T, typename T_STR >
		T_STR WebPath( T_STR s1, T_STR s2 )
		{
			T sep = tcTC( T, '/' );
			ReplaceChar( s1, tcTC( T, '\\' ), sep );
			ReplaceChar( s2, tcTC( T, '\\' ), sep );
			return RTrim( RTrim( s1, tcTC( T, '\\' ) ), tcTC( T, '/' ) )
				   + sep
				   + LTrim( LTrim( s2, tcTC( T, '\\' ) ), tcTC( T, '/' ) );
		}

	template< typename T, typename T_STR >
		T_STR FilePath( T_STR s1, T_STR s2 )
		{
#if defined( _WIN32 )
			T sep = tcTC( T, '\\' );
			ReplaceChar( s1, tcTC( T, '/' ), sep );
			ReplaceChar( s2, tcTC( T, '/' ), sep );
			return RTrim( RTrim( s1, tcTC( T, '\\' ) ), tcTC( T, '/' ) )
				   + sep
				   + LTrim( LTrim( s2, tcTC( T, '\\' ) ), tcTC( T, '/' ) );
#else
			return WebPath( s1, s2 );
#endif
		}

	// File flags
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

	/// Find data structure
	struct SFindData
	{
		str::tc_int64	uFileAttributes;
		str::tc_int64	ftCreated;
		str::tc_int64	ftLastAccess;
		str::tc_int64	ftLastModified;
		str::tc_int64	llSize;
		char			szName[ 1024 ];
	};

	/// Find handle
	typedef void* HFIND;

	/// Invalid find handle value
	const HFIND c_invalid_hfind = (HFIND)-1;

	/// Finds the first file matching the specified requirements
	/**
		@param[in]	x_pPath		- Root path for search
		@param[in]	x_pMask		- File mask to search for
		@param[in]	x_pFd		- Pointer to find data structure
		
		@return Returns a handle to an open find
	*/
	HFIND FindFirst( const char *x_pPath, const char *x_pMask, SFindData *x_pFd );

	/// Finds the next file matching the specified requirements
	/**
		@param[in]	x_hFind		- Find handle opened with ff_FindFirst()
		@param[in]	x_pFd		- Pointer to find data structure
		
		@return Returns a handle to an open find
	*/
	bool FindNext( HFIND x_hFind, SFindData *x_pFd );

	/// Closes a find handle and releases resources
	/**
		@param[in]	x_hFind		- Find handle opened with ff_FindFirst()
	*/
	bool FindClose( HFIND x_hFind );

	/// Opens the specified file and returns a handle
	HFILE Open( const char *x_pFile, const char *x_pMode );

	/// Opens the specified file and returns a handle
	t_size Write( const void *x_pData, t_size x_nSize, t_size x_nCount, HFILE x_hFile );

	/// Opens the specified file and returns a handle
	t_size Read( void *x_pData, t_size x_nSize, t_size x_nCount, HFILE x_hFile );

	/// Closes the specified file handle
	t_size Close( HFILE x_hFile );

	/// Returns the size of the file opened by the specified handle
	str::tc_int64 Size( HFILE hFile );

	/// Returns the size of the named file
	str::tc_int64 Size( const char *x_pFile );

	/// Creates the specified directory
	bool mkdir( const void *x_pData );

	/// Returns non-zero if the specified path exists
	bool exists( const char *x_pPath );

	/// Deletes the specified file
	bool unlink( const char *x_pFile );

	/// Removes the sepecified directory
	bool rmdir( const char *x_pPath );

	/// Writes the data to the named file
	/**
		@param [in] sFile	- Name of file
		@param [in] sData	- Data to write to file
	*/
	template< typename T, typename T_STR >
		t_size WriteFile( T_STR sFile, T_STR sData )
		{	HFILE hOut = Open( sFile.c_str(), "wb" );
			if ( c_invalid_hfile == hOut )
				return 0;
			t_size n = Write( sData.data(), sizeof( T ), sData.length(), hOut );
			Close( hOut );
			return n;
		}

	template< typename T, typename T_STR >
		t_size WriteFile( T_STR sFile, T_STR sData1, T_STR sData2 )
		{	HFILE hOut = Open( sFile.c_str(), "wb" );
			if ( c_invalid_hfile == hOut )
				return 0;
			t_size n = Write( sData1.data(), sizeof( T ), sData1.length(), hOut )
					   + Write( sData2.data(), sizeof( T ), sData2.length(), hOut );
			Close( hOut );
			return n;
		}

	/// Appends the data to the named file
	/**
		@param [in] sFile	- Name of file
		@param [in] sData	- Data to append to file
	*/
	template< typename T, typename T_STR >
		t_size AppendFile( T_STR sFile, T_STR sData )
		{	HFILE hOut = Open( sFile.c_str(), "ab" );
			if ( c_invalid_hfile == hOut )
				return 0;
			t_size n = Write( sData.data(), sizeof( T ), sData.length(), hOut );
			Close( hOut );
			return n;
		}

	/// Reads data from the specified file into a string 
	/**
		@param [in] sFile	- Name of file
		@param [in] sData	- Data to write to file
	*/
	template< typename T, typename T_STR >
		T_STR ReadFile( T_STR sFile, str::tc_int64 nMax = 0 )
		{
			// Open the file
			HFILE hIn = Open( sFile.c_str(), "rb" );
			if ( c_invalid_hfile == hIn )
				return T_STR();

			// How much space do we need?
			str::tc_int64 sz = Size( hIn ) / sizeof( T );
			if ( !sz )
				return T_STR();

			// Is it too much?
			if ( 0 < nMax && nMax < sz )
				sz = nMax;

			// Attempt to allocate memory
			T_STR sData;
//			try { sData.reserve( sz );
//			} catch( ... ) { return T_STR(); }

			// Set size
			try { sData.resize( sz );
			} catch( ... ) { return T_STR(); }

			// Read in the data
			sz = Read( (T*)sData.data(), sizeof( T ), sz, hIn );
			if ( 0 >= sz )
				return T_STR();

			// Close the file
			Close( hIn );

			// Return the data
			return sData;
		}

}; // namespace ff