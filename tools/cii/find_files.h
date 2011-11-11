// find_files.h

#pragma once

#include "typecnv.h"

namespace ff
{
	template< typename T, typename T_STR >
		T_STR& RTrim( T_STR &s, T c )
		{	return s.erase( s.find_last_not_of( c ) + 1 ); }

	template< typename T, typename T_STR >
		T_STR& LTrim( T_STR &s, T c )
		{	return s.erase( 0, s.find_first_not_of( c ) ); }

	template< typename T, typename T_STR >
		T_STR BuildPath( T_STR s1, T_STR s2 )
		{
#if defined( _WIN32 )
			T sep = tcTC( T, '\\' );
#else
			T sep = tcTC( T, '/' );
#endif
			return RTrim( RTrim( s1, tcTC( T, '\\' ) ), tcTC( T, '/' ) )
				   + sep
				   + LTrim( LTrim( s1, tcTC( T, '\\' ) ), tcTC( T, '/' ) );
		}

	/// Find data structure
	struct SFindData
	{	
		tcnv::tc_int64	uFileAttributes;
		tcnv::tc_int64	ftCreated;
		tcnv::tc_int64	ftLastAccess;
		tcnv::tc_int64	ftLastModified;
		tcnv::tc_int64	llSize;
		char			*szName[ 1024 ];
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