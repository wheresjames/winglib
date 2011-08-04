/*------------------------------------------------------------------
// sq_tools.h
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

template < typename T_PB, typename T_STD  >
	void SQBIND_PropertyBagToStd( T_PB &pb, T_STD &m )
	{
		for( oex::CPropertyBag::iterator it; pb.List().Next( it ); )
			m[ it.Node()->key.Ptr() ] = it->ToString().Ptr();
	}

template < typename T_PB, typename T_STD  >
	void SQBIND_PropertyBag8ToStd( T_PB &pb, T_STD &m )
	{	for( oex::CPropertyBag8::iterator it; pb.List().Next( it ); )
			m[ oexMbToStrPtr( it.Node()->key.Ptr() ) ] = oexMbToStrPtr( it->ToString().Ptr() );
	}

template < typename T_STD, typename T_PB  >
	void SQBIND_StdToPropertyBag( T_STD &m, T_PB &pb )
	{	for ( CSqMulti::iterator it = m.begin(); it != m.end(); it++ )
				pb[ it->first.c_str() ] = it->second.str().c_str();
	}

template < typename T_STD, typename T_PB  >
	void SQBIND_StdToPropertyBag8( T_STD &m, T_PB &pb )
	{	for ( CSqMulti::iterator it = m.begin(); it != m.end(); it++ )
				pb[ oexStrToMbPtr( it->first.c_str() ) ] = oexStrToMbPtr( it->second.str().c_str() );
	}

template < typename T_PB, typename T_STD  >
	void SQBIND_PropertyBagToMulti( T_PB &pb, T_STD &m )
	{
		for( oex::CPropertyBag::iterator it; pb.List().Next( it ); )
		{
			stdString sKey = it.Node()->key.Ptr();

			if ( !sKey.length() )
				sKey = oexT( "*" );

			if ( !it->Size() )
				m[ sKey ].str().assign( it->ToString().Ptr(), (stdString::size_type)it->ToString().Length() );

			else
				SQBIND_PropertyBagToMulti( *it, m[ sKey ].list() );

		} // end for
	}

template < typename T_PB, typename T_STD  >
	void SQBIND_PropertyBag8ToMulti( T_PB &pb, T_STD &m )
	{
		for( oex::CPropertyBag8::iterator it; pb.List().Next( it ); )
		{
			stdString sKey = oexMbToStrPtr( it.Node()->key.Ptr() );

			if ( !sKey.length() )
				sKey = oexT( "*" );

			if ( !it->Size() )
			{	oex::CStr s = oexMbToStr( it->ToString() );
				m[ sKey ].str().assign( s.Ptr(), (stdString::size_type)s.Length() );
			} // end if

			else
				SQBIND_PropertyBag8ToMulti( *it, m[ sKey ].list() );

		} // end for
	}

template < typename T_STD, typename T_PB  >
	void SQBIND_MultiToPropertyBag( T_STD &m, T_PB &pb )
	{
		for ( CSqMulti::iterator it = m.begin(); it != m.end(); it++ )
		{
			stdString sKey = it->first;
			if ( !sKey.length() )
				sKey = oexT( "*" );

			if ( !it->second.size() )
				pb[ sKey.c_str() ].ToString().Set( it->second.str().c_str(), it->second.str().length() );

			else
				SQBIND_MultiToPropertyBag( it->second.list(), pb[ sKey.c_str() ] );

		} // end for
	}

template < typename T_STD, typename T_PB  >
	void SQBIND_MultiToPropertyBag8( T_STD &m, T_PB &pb )
	{
		for ( CSqMulti::iterator it = m.begin(); it != m.end(); it++ )
		{
			stdString sKey = it->first;
			if ( !sKey.length() )
				sKey = oexT( "*" );

			if ( !it->second.size() )
			{	oex::CStr s( it->second.m_val.c_str(), it->second.str().length() );
				pb[ oexStrToMbPtr( sKey.c_str() ) ].ToString().Set( oexStrToMb( s ) );
			} // end if

			else
				SQBIND_MultiToPropertyBag8( it->second.list(), pb[ oexStrToMbPtr( sKey.c_str() ) ] );

		} // end for
	}

template < typename T_SL, typename T_STD, typename T_DEF >
	void SQBIND_StrListToStd( T_SL &sl, T_STD &m, T_DEF ch )
	{	for ( oex::CStrList::iterator it; sl.Next( it ); )
			m[ oex2std( *it ) ] = ch;
	}

template < typename T_M, typename T_CREG, typename T_RKEY >
	void SQBIND_CRegToPropertyBag( T_M &m, T_CREG &r )
	{	for ( CSqMulti::iterator it = m.begin(); it != m.end(); it++ )
			if ( it->first.length() && it->second.size() )
			{	T_RKEY *prk = r.GetKey( it->first.c_str() );
				if ( prk )
					for ( CSqMulti::iterator itk = it->second.begin(); itk != it->second.end(); itk++ )
						if ( itk->first.length() )
							prk->Add( 1, it->first.c_str(), itk->first.c_str(), 0 );
			} // end if
	}

template < typename T >
	T SQBIND_StrReplace( T str, const T &fstr, const T &rstr )
	{	typename T::size_type pos = 0, flen = fstr.length();
		while ( T::npos != ( pos = str.find( fstr, pos ) ) )
			str.replace( pos, flen, rstr ), pos += flen;
		return str;
	}

template < typename T >
	T prepare_inline( const T &sScript, int bFile )
	{_STT();
		// Code header / footer
		static oex::oexTCHAR szHeader[] = oexT( "{ local _p = ::_self.get_stack_params(); function echo( s ) { ::_self.append_stack_output( s.tostring() ); }" ) oexNL;
		static oex::oexTCHAR szFooter[] = oexNL oexT( "}" );
		static oex::oexTCHAR szOpenStr[] = oexNL oexT( "echo( @\"" );
		static oex::oexTCHAR szCloseStr[] = oexT( "\" );" ) oexNL;
		static oex::CStr::t_size nOverhead = (oex::CStr::t_size)
												 sizeof( szHeader ) + sizeof( szFooter )
												 + sizeof( szOpenStr ) + sizeof( szCloseStr );

		// Get the source data
		oex::CStr sSrc;
		if ( bFile )
			sSrc = oexFileGetContents( sScript.c_str() );
		else
			sSrc = std2oex( sScript );

		// Did we get anything
		oex::oexCSTR pSrc = sSrc.Ptr();
		oex::CStr::t_size szSrc = sSrc.Length(), nPos = 0, nStart = 0;
		if ( !szSrc )
			return oexT( "" );

		// Allocate space for output
		oex::CStr sDst;
		oex::CStr::t_size nDst = 0;
		oex::oexSTR pDst = sDst.OexAllocate( ( szSrc * 2 ) + nOverhead );
		if ( !pDst )
			return oexT( "" );

		// Copy header into buffer
		oexMemCpy( &pDst[ nDst ], szHeader, sizeof( szHeader ) - sizeof( oex::oexTCHAR ) );
		nDst += sizeof( szHeader ) - sizeof( oex::oexTCHAR );

		// Open tag
		oex::oexTCHAR tcOpen[] = oexT( "<?sq" );
		oex::CStr::t_size nOpen = 0, szOpen = (oex::CStr::t_size)oex::zstr::Length( tcOpen );

		// Close tag
		oex::oexTCHAR tcClose[] = oexT( "?>" );
		oex::CStr::t_size nClose = 0, szClose = (oex::CStr::t_size)oex::zstr::Length( tcClose );

		// iii Not using standard search and replace because we need speed here

		// We must have at least six characters for a complete tag
		while ( ( nPos + szOpen + szClose ) < szSrc )
		{
			// Ensure we have space
			if ( ( sDst.Size() - nDst ) < ( ( szSrc - nPos ) * 2 ) + nOverhead )
				pDst = sDst.OexAllocate( sDst.Size() * 2 );

			// Initialize
			nStart = nPos;
			nOpen = nClose = szSrc;

			// Attempt to find an open bracket
			while ( nOpen == szSrc && ( nPos + szOpen + szClose ) < szSrc )
				if ( !oexMemCmp( &pSrc[ nPos ], tcOpen, szOpen ) )
					nOpen = nPos;
				else
					nPos++;

			// Find a closing bracket
			while ( nClose == szSrc && ( nPos + szClose ) < szSrc )
				if ( !oexMemCmp( &pSrc[ nPos ], tcClose, szClose ) )
					nClose = nPos;
				else
					nPos++;

			// Did we find embedded code?
			if ( nOpen < szSrc && nClose < szSrc )
			{
				// Text data to be copied?
				if ( nStart < nOpen )
				{	oexMemCpy( &pDst[ nDst ], szOpenStr, sizeof( szOpenStr ) - sizeof( oex::oexTCHAR ) );
					nDst += sizeof( szOpenStr ) - sizeof( oex::oexTCHAR );
					while ( nStart < nOpen )
					{	if ( oexT( '"' ) == pSrc[ nStart ] )
							pDst[ nDst++ ] = oexT( '"' );
						pDst[ nDst++ ] = pSrc[ nStart++ ];
					} // end while
					oexMemCpy( &pDst[ nDst ], szCloseStr, sizeof( szCloseStr ) - sizeof( oex::oexTCHAR ) );
					nDst += sizeof( szCloseStr ) - sizeof( oex::oexTCHAR );
				} // end if

				// Any code to copy?
				nOpen += szOpen;
				if ( nOpen < nClose )
				{
					// Add new line
					oexMemCpy( &pDst[ nDst ], oexNL, sizeof( oexNL ) - sizeof( oex::oexTCHAR ) );
					nDst += sizeof( oexNL ) - sizeof( oex::oexTCHAR );

					// Copy the code
					oexMemCpy( &pDst[ nDst ], &pSrc[ nOpen ], nClose - nOpen );
					nDst += nClose - nOpen;

					// Close statement, just in case
					pDst[ nDst++ ] = oexT( ';' );

				} // endif

				// Point to data after code
				nPos += szClose;

			} // end if

		} // end while

		// Copy whatever remains
		if ( nStart < szSrc )
		{	oexMemCpy( &pDst[ nDst ], szOpenStr, sizeof( szOpenStr ) - sizeof( oex::oexTCHAR ) );
			nDst += sizeof( szOpenStr ) - sizeof( oex::oexTCHAR );
			while ( nStart < szSrc )
			{	if ( oexT( '"' ) == pSrc[ nStart ] )
					pDst[ nDst++ ] = oexT( '"' );
				pDst[ nDst++ ] = pSrc[ nStart++ ];
			} // end while
			oexMemCpy( &pDst[ nDst ], szCloseStr, sizeof( szCloseStr ) - sizeof( oex::oexTCHAR ) );
			nDst += sizeof( szCloseStr ) - sizeof( oex::oexTCHAR );
		} // end if

		// Copy footer into buffer
		oexMemCpy( &pDst[ nDst ], szFooter, sizeof( szFooter ) - sizeof( oex::oexTCHAR ) );
		nDst += sizeof( szFooter ) - sizeof( oex::oexTCHAR );

		// Set the number of bytes in the string
		sDst.SetLength( nDst );

		return oex2std( sDst );
	}

