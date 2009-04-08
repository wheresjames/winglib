/*------------------------------------------------------------------
// sq_defines.h
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

//#	define SQBIND_NEW( m_type ) new m_type
//#	define SQBIND_DELETE( m_instance) delete m_instance
#include "../../sqbind/include/sqbind.h"
#ifdef SQBIND_SQBIND

#   define SQBIND_REGISTER_CLASS_BEGIN( c, s )				static void __SqReg_sqbind_##s( HSQUIRRELVM vm ) { \
																SqBind< c >::init( vm, oexT( #s ) );
#   define SQBIND_MEMBER_FUNCTION( c, f )    				sqbind_method( vm, oexT( #f ), &c::f );
#   define SQBIND_REGISTER_CLASS_END()						; }
#   define SQBIND_EXPORT( vm, c )            				__SqReg_sqbind_##c( vm )
#	define SQBIND_DECLARE_INSTANCE( c, n )

namespace sqbind { typedef HSQUIRRELVM VM; }

#else

#   define SQBIND_REGISTER_CLASS_BEGIN						_SQBIND_REGISTER_CLASS_BEGIN
#   define SQBIND_MEMBER_FUNCTION    						_SQBIND_MEMBER_FUNCTION
#   define SQBIND_REGISTER_CLASS_END						_SQBIND_REGISTER_CLASS_END
#   define SQBIND_EXPORT            						_SQBIND_EXPORT
#	define SQBIND_DECLARE_INSTANCE							_SQBIND_DECLARE_INSTANCE
#   define SQBIND_STATIC_FUNCTION							_SQBIND_STATIC_FUNCTION

#endif

#ifdef SQBIND_SQPLUS

#   include <sqplus.h>
#   define _SQBIND_REGISTER_CLASS_BEGIN( c, s ) 				static void __SqReg_sqplus_##s( SquirrelVM *vm ) { \
																oexVERIFY_PTR( vm ); \
    	                                       					SqPlus::SQClassDef< c >( *vm, oexT( #s ) )
#   define _SQBIND_REGISTER_CLASS_END()       				; }
#   define _SQBIND_EXPORT( vm, c )            				__SqReg_sqplus_##c( vm )
#   define _SQBIND_MEMBER_FUNCTION( c, f )    				.func ( &c::f,        	  oexT( #f ) )
#   define _SQBIND_CONST( c, e )              				.enumInt ( (int)c::##e,   oexT( #e ) )
#   define _SQBIND_GLOBALCONST( e )           				.enumInt ( (int)##e,	  oexT( #e ) )
#   define _SQBIND_ENUM( e, s )		           				.enumInt ( (int)e,	  	  oexT( s ) )
#   define _SQBIND_MEMBER_VARIABLE( c, v )    				.var ( &c::v,         	  oexT( #v ) )
#   define _SQBIND_STATIC_FUNCTION( c, f )    				.staticFunc ( &c::f,      oexT( #f ) )

#   define _SQBIND_CLASS_CONSTRUCTOR( n, c )					.staticFunc( &_sq_construct_##c_##n, oexT( "constructor" ) )
#   define _SQBIND_BEGIN_CLASS_CONSTRUCTOR( n, c, ... )		static int _sq_construct_##c_##n( __VA_ARGS__, HSQUIRRELVM v ) {
#   define _SQBIND_END_CLASS_CONSTRUCTOR( c, ... )			return SqPlus::PostConstruct< c >( v, new c( __VA_ARGS__ ), SqPlus::ReleaseClassPtr< c >::release ); }

#	define _SQBIND_DECLARE_INSTANCE( c, n )					DECLARE_INSTANCE_TYPE_NAME( c, n )

#	if !defined( SQBIND_SQBIND )
namespace sqbind { typedef SquirrelVM *VM; }
#endif

#elif SQBIND_JKBIND

#   define jkUNICODE 1
#   include <script/VMCore.h>
#   include <script/Declarator.h>
#   define SQBIND_REGISTER_CLASS_BEGIN( c )  SCRIPT_CLASS_REGISTER_BEGIN( c ) \
                                            .declareClass< c >( xSTRING( #c ) )
#   define SQBIND_REGISTER_CLASS_END()       SCRIPT_CLASS_REGISTER_END( c )
#   define SQBIND_EXPORT( vm, c )            script::ClassTraits< c >::bind( vm )
#   define SQBIND_MEMBER_FUNCTION( c, f )    .method< alreadyAsThis >( xSTRING( #f ), &##c::##f )
namespace sqbind { typedef script::VMCore VM; }

#endif

namespace sqbind
{
	typedef oexStdTString( oex::oexTCHAR ) stdString;

	class CSqParam
	{
	public:

		// String type
		typedef stdString						t_SqStr;

	public:

		// Serializes the object to a string
		virtual t_SqStr serialize() = 0;

		// Deserializes the object from a string
		virtual void deserialize( const t_SqStr &s ) = 0;

		// Merges object data / does not have to be implemented
		virtual void merge( const t_SqStr &s ) {}
	};

	class CSqString
	{
	public:
		CSqString() {}
		CSqString( const stdString &x_str ) { m_str = x_str; }
		stdString& str() { return m_str; }
		const stdString* c_str() const { return &m_str; }
		stdString set( const stdString &x_str )
		{	m_str = x_str; return m_str; }
		stdString& operator = ( const stdString &x_str ) { return m_str = x_str; }
		stdString& operator = ( const oex::oexTCHAR *x_str ) { if ( x_str ) m_str = x_str; return m_str; }

		static void Register( sqbind::VM vm );
	private:
		stdString m_str;
	};
}

#if defined( SQBIND_SQPLUS )

// Declare type for use as squirrel parameters
_SQBIND_DECLARE_INSTANCE( sqbind::CSqString, CSqString )

namespace SqPlus
{

	inline void Push(HSQUIRRELVM v,const sqbind::stdString& value)
	{	sq_pushstring(v,value.c_str(),value.length()); }
	inline bool Match(TypeWrapper<const sqbind::stdString&>, HSQUIRRELVM v, int idx)
	{	return sq_gettype(v,idx) == OT_STRING; }
	inline sqbind::stdString Get(TypeWrapper<const sqbind::stdString&>,HSQUIRRELVM v,int idx)
	{	const SQChar * s = 0;
		int sz = sq_getsize(v,idx);
		SQPLUS_CHECK_GET(sq_getstring(v,idx,&s));
		return sqbind::stdString(s,sz);
	}
}
#endif

template<>
class SqBind<sqbind::stdString> {
public:
	struct Getter {
		SQBIND_INLINE sqbind::stdString get(HSQUIRRELVM v, int p_idx) {
			return SqBind<sqbind::stdString>::get(v,p_idx);
		}
	};
	struct GetterPtr {
		sqbind::stdString temp;
		SQBIND_INLINE sqbind::stdString* get(HSQUIRRELVM v, int p_idx) {
			temp=SqBind<sqbind::stdString>::get(v,p_idx);
			return &temp;
		}
	};
	static sqbind::stdString get(HSQUIRRELVM v, int p_idx) {
		if (sq_gettype(v,p_idx)!=OT_STRING) {
			sqbind_throwerror(v,_SC("Type is not string!"));
			return sqbind::stdString();
		}
		const SQChar * str;
		int sz = sq_getsize(v,p_idx);
		sq_getstring(v,p_idx,&str);
		return sqbind::stdString(str,sz);
	}

	static void push(HSQUIRRELVM v, const sqbind::stdString& p_value) {
		sq_pushstring(v,p_value.c_str(),p_value.length());
	}
};
