// find_files.h

#pragma once

#include "typecnv.h"

/**
	Platform independent file find

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
namespace ff
{
	template< typename T, typename T_STR >
		T_STR& RTrim( T_STR &s, T c )
		{	return s.erase( s.find_last_not_of( c ) + 1 ); }

	template< typename T, typename T_STR >
		T_STR& LTrim( T_STR &s, T c )
		{	return s.erase( 0, s.find_first_not_of( c ) ); }

	template < typename T >
		T ReplaceStr( const T s, const T a, const T b )
		{	T _s( s ); T::size_type i = 0;
			while( T::npos != ( i = _s.find_first_of( a, i ) ) )
				_s.replace( i, a.length(), b ), i += b.length();
			return _s;
		}

	template < typename T_STR, typename T >
		T_STR& ReplaceChar( T_STR &s, const T a, const T b )
		{	T_STR::size_type i = 0;
			while( T_STR::npos != ( i = s.find_first_of( a, i ) ) )
				s[ i++ ] = b;
			return s;
		}

	template< typename T, typename T_STR >
		T_STR GetPath( T_STR s )
		{
			T_STR::size_type lb = s.find_last_of( tcTC( T, '\\' ) );
			T_STR::size_type lf = s.find_last_of( tcTC( T, '/' ) );
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
			T_STR::size_type lb = s.find_last_of( tcTC( T, '\\' ) );
			T_STR::size_type lf = s.find_last_of( tcTC( T, '/' ) );
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
		tcnv::tc_int64	uFileAttributes;
		tcnv::tc_int64	ftCreated;
		tcnv::tc_int64	ftLastAccess;
		tcnv::tc_int64	ftLastModified;
		tcnv::tc_int64	llSize;
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

}; // namespace ff