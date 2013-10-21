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
#ifdef SQBIND_SQBIND
#include "../../sqbind/include/sqbind.h"

#   define SQBIND_REGISTER_CLASS_BEGIN( c, s )				static void __SqReg_sqbind_##s( HSQUIRRELVM vm ) { \
																SqBind< c >::init( vm, oexT( #s ) );
#   define SQBIND_MEMBER_FUNCTION( c, f )    				sqbind_method( vm, oexT( #f ), &c::f );
#	define SQBIND_STATIC_FUNCTION( c, f )					sqbind_function( vm, #f, c::f, &SqBind< c >::get_id() );
#   define SQBIND_REGISTER_CLASS_END()						; }
#   define SQBIND_EXPORT( vm, c )            				__SqReg_sqbind_##c( vm )
#	define SQBIND_DECLARE_INSTANCE( c, n )

#	define SQBIND_CLASS_CTOR_BEGIN( c )
#	define SQBIND_CLASS_CTOR_END( c )
#	define SQBIND_CLASS_CTOR( c, n )
#	define SQBIND_CLASS_BIND_CTOR( c )

namespace sqbind { typedef HSQUIRRELVM VM; }

#else

#   define SQBIND_REGISTER_CLASS_BEGIN						_SQBIND_REGISTER_CLASS_BEGIN
#   define SQBIND_MEMBER_FUNCTION    						_SQBIND_MEMBER_FUNCTION
#   define SQBIND_REGISTER_CLASS_END						_SQBIND_REGISTER_CLASS_END
#   define SQBIND_EXPORT            						_SQBIND_EXPORT
#	define SQBIND_DECLARE_INSTANCE							_SQBIND_DECLARE_INSTANCE
#   define SQBIND_STATIC_FUNCTION							_SQBIND_STATIC_FUNCTION

#   define SQBIND_ENUM										_SQBIND_ENUM
#   define SQBIND_CONST										_SQBIND_CONST
#   define SQBIND_GLOBALCONST								_SQBIND_GLOBALCONST

#	define SQBIND_CLASS_CTOR_BEGIN							_SQBIND_CLASS_CTOR_BEGIN
#	define SQBIND_CLASS_CTOR_END 							_SQBIND_CLASS_CTOR_END
#	define SQBIND_CLASS_CTOR								_SQBIND_CLASS_CTOR
#	define SQBIND_CLASS_BIND_CTOR							_SQBIND_CLASS_BIND_CTOR

#endif

#ifdef SQBIND_SQPLUS

#   include <sqplus.h>
#   define _SQBIND_REGISTER_CLASS_BEGIN( c, s ) 			static void __SqReg_sqplus_##s( SquirrelVM *vm ) { \
															oexASSERT_PTR( vm ); \
    	                                       				SqPlus::SQClassDef< c >( *vm, oexT( #s ) ) \
															. staticFunc( &c::sq_construct_##s, oexT( "constructor" ) )
#   define _SQBIND_REGISTER_CLASS_BEGIN_NOCTOR( c, s )		static void __SqReg_sqplus_##s( SquirrelVM *vm ) { \
															oexASSERT_PTR( vm ); \
    	                                       				SqPlus::SQClassDef< c >( *vm, oexT( #s ) )
#   define _SQBIND_REGISTER_CLASS_END()       				; }
#   define _SQBIND_EXPORT( vm, c )            				__SqReg_sqplus_##c( vm )
#   define _SQBIND_MEMBER_FUNCTION( c, f )    				.func ( &c::f,        	  	oexT( #f ) )
#   define _SQBIND_CONST( c, e )              				.enumInt ( (int)c::e,   	oexT( #e ) )
#   define _SQBIND_GLOBALCONST( e )           				.enumInt ( (int)e,	  		oexT( #e ) )
#   define _SQBIND_ENUM( e, s )		           				.enumInt ( (int)e,	  	  	oexT( #s ) )
#   define _SQBIND_MEMBER_VARIABLE( c, v )    				.var ( &c::v,         	  	oexT( #v ) )
#   define _SQBIND_STATIC_FUNCTION( c, f )    				.staticFunc ( &c::f,      	oexT( #f ) )

/*
	param 1 = sa.GetFloat( 2 )

	// Example
	class CTestClass
	{
	public:

		CTestClass() {}
		CTestClass( float p1 ) {}
		CTestClass( float p1, int p2, stdString sHi ) {}

		_SQBIND_CLASS_CTOR_BEGIN( CTestClass )
			_SQBIND_CLASS_CTOR(   CTestClass, 1 )( sa.GetFloat( 2 ) )
			_SQBIND_CLASS_CTOR(   CTestClass, 3 )( sa.GetFloat( 2 ), sa.GetInt( 3 ), sa.GetString( 4 ) )
		_SQBIND_CLASS_CTOR_END(   CTestClass )

		...

	};

	// Squirrel types
	OT_NULL, OT_INTEGER, OT_FLOAT, OT_BOOL, OT_STRING, OT_TABLE, OT_ARRAY,
	OT_USERDATA, OT_CLOSURE, OT_NATIVECLOSURE, OT_GENERATOR, OT_USERPOINTER,
	OT_THREAD, OT_FUNCPROTO, OT_CLASS, OT_INSTANCE, OT_WEAKREF

*/
#	define _SQBIND_CLASS_CTOR_BEGIN( c )			static SQInteger sq_construct_##c( HSQUIRRELVM x_v ) \
													{ StackHandler sa( x_v ); c *p = 0; if( 0 )
#	define _SQBIND_CLASS_CTOR_END( c ) 				; else p = new c(); \
													return SqPlus::PostConstruct<c>( x_v, p, SqPlus::ReleaseClassPtr< c >::release); }
#	define _SQBIND_CLASS_CTOR1( c, t )				; else if ( ( 1 + 1 ) == sa.GetParamCount() && t == sa.GetType( 2 ) ) p = new c
#	define _SQBIND_CLASS_CTOR2( c, t1, t2 )			; else if ( ( 2 + 1 ) == sa.GetParamCount() && t1 == sa.GetType( 2 ) && t2 == sa.GetType( 3 ) ) p = new c
#	define _SQBIND_CLASS_CTOR3( c, t1, t2, t3 )		; else if ( ( 3 + 1 ) == sa.GetParamCount() && t1 == sa.GetType( 2 ) && t2 == sa.GetType( 3 ) && t3 == sa.GetType( 4 ) ) p = new c
#	define _SQBIND_CLASS_CTOR( c, n )				; else if ( ( n + 1 ) == sa.GetParamCount() ) p = new c
#	define _SQBIND_CLASS_CTORT1( c, n, t )			; else if ( ( n + 1 ) == sa.GetParamCount() && t == sa.GetType( 2 ) ) p = new c
#	define _SQBIND_CLASS_CTORT2( c, n, t1, t2 )		; else if ( ( n + 1 ) == sa.GetParamCount() && t1 == sa.GetType( 2 ) && t2 == sa.GetType( 3 ) ) p = new c
#	define _SQBIND_CLASS_CTORT3( c, n, t1, t2, t3 )	; else if ( ( n + 1 ) == sa.GetParamCount() && t1 == sa.GetType( 2 ) && t2 == sa.GetType( 3 ) && t3 == sa.GetType( 4 ) ) p = new c
#	define _SQBIND_CLASS_BIND_CTOR( c )				.staticFunc( &c::sq_construct_##c, oexT( "constructor" ) )

#	define _SQBIND_DECLARE_INSTANCE( c, n )			DECLARE_INSTANCE_TYPE_NAME( c, n )

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
	typedef oex::oexTCHAR stdChar;
	typedef oex::oexCHAR stdByte;
    typedef oexStdTString( stdChar ) stdString;
    typedef oexStdTString( stdByte ) stdString8;
    typedef oexStdTStream( stdChar ) stdStream;
    typedef oexStdTStream( stdByte ) stdStream8;
    typedef oexStdTIStream( stdByte ) stdIStream;
    typedef oexStdTOStream( stdByte ) stdOStream;


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
		_SQBIND_CLASS_CTOR_BEGIN( CSqString )
		_SQBIND_CLASS_CTOR_END( CSqString )
		CSqString() {}
		CSqString( const stdString &x_str ) { m_str = x_str; }
		stdString& str() { return m_str; }
		const stdString& c_str() const { return m_str; }
		stdString* ptr() { return &m_str; }
		const stdString* c_ptr() const { return &m_str; }
		stdString set( const stdString &x_str ) { m_str = x_str; return m_str; }
		stdString::size_type length() const { return (stdString::size_type)m_str.length(); }
		stdString& operator = ( const stdString &x_str ) { return m_str = x_str; }
		stdString& operator = ( const oex::oexTCHAR *x_str ) { if ( x_str ) m_str = x_str; return m_str; }
		stdString& operator = ( const oex::CStr &x_str ) { m_str.assign( x_str.Ptr(), (stdString::size_type)x_str.Length() ); return m_str; }
		void clear() { m_str.clear(); }
		bool empty() { return m_str.empty(); }
		static void Register( sqbind::VM vm );
	private:
		stdString m_str;
	};

	template < typename T >
		static sqbind::stdString oex2std( const T &s )
		{	return sqbind::stdString( s.Ptr(), (sqbind::stdString::size_type)s.Length() ); }

	template < typename T >
		static sqbind::stdString8 oex2std8( const T &s )
		{	return sqbind::stdString8( s.Ptr(), (sqbind::stdString::size_type)s.Length() ); }

	template < typename T >
		static sqbind::stdString oex82std( const T &s )
		{	oex::CStr t = oexMbToStr( s );
			return sqbind::stdString( t.Ptr(), (sqbind::stdString::size_type)t.Length() );
		}

	template < typename T >
		static oex::CStr std2oex( const T &s )
		{	return oex::CStr( s.c_str(), s.length() ); }

	template < typename T >
		static oex::CStr8 std82oex8( const T &s )
		{	return oex::CStr8( s.c_str(), s.length() ); }

	template < typename T >
		static oex::CStr8 std2oex8( const T &s )
		{	return oexStrToMb( oex::CStr( s.c_str(), s.length() ) ); }

	template < typename T >
		static oex::CBin std2bin( const T &s )
		{	return oex::CBin( oexStrToMb( std2oex( s ) ) ); }

	template < typename T >
		static oex::CStr bin2std( const T &b )
		{	return oex2std( oexMbToStr( oex::CStr8( b.Mem(), b.getUsed() ) ) ); }

	template< typename T >
		stdString obj2str( const T &_o )
		{
			T &o = (T&)_o;
			switch( sq_type( o.GetObjectHandle() ) )
			{
				case OT_STRING :
				{	SQInteger nLen = o.Len();
					const SQChar *pStr = o.ToString();
					if ( nLen && pStr )
						return stdString( pStr, (sqbind::stdString::size_type)nLen );
				} break;

				case OT_INTEGER :
					return oex2std( oexMks( o.ToInteger() ) );
					break;

				case OT_FLOAT :
					return oex2std( oexMks( o.ToFloat() ) );
					break;

				case OT_BOOL :
					return o.ToBool() ? oexT( "1" ) : oexT( "0" );
					break;

				default:
					break;

			} // end switch

			return oexT( "" );
		}
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
		SQInteger sz = sq_getsize(v,idx);
		SQPLUS_CHECK_GET(sq_getstring(v,idx,&s));
		return sqbind::stdString(s,(sqbind::stdString::size_type)sz);
	}
}
#endif

#ifdef SQBIND_SQBIND

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
		SQInteger sz = sq_getsize(v,p_idx);
		sq_getstring(v,p_idx,&str);
		return sqbind::stdString(str,(sqbind::stdString::size_type)sz);
	}

	static void push(HSQUIRRELVM v, const sqbind::stdString& p_value) {
		sq_pushstring(v,p_value.c_str(),p_value.length());
	}
};
#endif
