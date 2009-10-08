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
				m[ sKey ].str().assign( it->ToString().Ptr(), it->ToString().Length() );

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
				m[ sKey ].str().assign( s.Ptr(), s.Length() );
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

