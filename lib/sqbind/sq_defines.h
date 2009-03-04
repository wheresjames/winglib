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

#ifdef SQBIND_SQPLUS
#   include <sqplus.h>
#   define SQBIND_REGISTER_CLASS_BEGIN( c, s ) 				void __SqReg_##c( SquirrelVM *vm ) { \
                                           					SqPlus::SQClassDef< c >( *vm, oexT( s ) )
#   define SQBIND_REGISTER_CLASS_END()       				; }
#   define SQBIND_EXPORT( vm, c )            				__SqReg_##c( vm )
#   define SQBIND_MEMBER_FUNCTION( c, f )    				.func ( &c::f,        	  oexT( #f ) )
#   define SQBIND_CONST( c, e )              				.enumInt ( (int)c::##e,   oexT( #e ) )
#   define SQBIND_GLOBALCONST( e )           				.enumInt ( (int)##e,	  oexT( #e ) )
#   define SQBIND_MEMBER_VARIABLE( c, v )    				.var ( &c::v,         	  oexT( #v ) )
#   define SQBIND_STATIC_FUNCTION( c, f )    				.staticFunc ( &c::f,      oexT( #f ) )

#   define SQBIND_CLASS_CONSTRUCTOR( n, c )					.staticFunc( &_sq_construct_##c_##n, oexT( "constructor" ) )
#   define SQBIND_BEGIN_CLASS_CONSTRUCTOR( n, c, ... )		static int _sq_construct_##c_##n( __VA_ARGS__, HSQUIRRELVM v ) {
#   define SQBIND_END_CLASS_CONSTRUCTOR( c, ... )			return SqPlus::PostConstruct< c >( v, new c( __VA_ARGS__ ), SqPlus::ReleaseClassPtr< c >::release ); }

namespace sqbind { typedef SquirrelVM VM; }

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

	class CSqString
	{
	public:
		stdString* get() { return &m_str; }
		stdString* set( const stdString &x_str )
		{	m_str = x_str; return &m_str; }
		stdString& operator = ( const stdString &x_str ) { return m_str = x_str; }
		stdString& operator = ( const oex::oexTCHAR *x_str ) { if ( x_str ) m_str = x_str; return m_str; }
		static void Register( SquirrelVM &vm )
		{	SqPlus::SQClassDef< CSqString >( vm, oexT( "CSqString" ) )
					. func( &CSqString::get,		oexT( "value" ) )
					. func( &CSqString::set,		oexT( "set" ) )
				;
		}
	private:
		stdString m_str;
	};
}

// Declare type for use as squirrel parameters
DECLARE_INSTANCE_TYPE_NAME( sqbind::CSqString, CSqString )

namespace SqPlus
{
	inline void Push(HSQUIRRELVM v,const sqbind::stdString& value)
	{	sq_pushstring(v,value.c_str(),-1); }
	inline bool Match(TypeWrapper<const sqbind::stdString&>, HSQUIRRELVM v, int idx)
	{	return sq_gettype(v,idx) == OT_STRING; }
	inline sqbind::stdString Get(TypeWrapper<const sqbind::stdString&>,HSQUIRRELVM v,int idx)
	{	const SQChar * s; SQPLUS_CHECK_GET(sq_getstring(v,idx,&s)); return sqbind::stdString(s); }
}

