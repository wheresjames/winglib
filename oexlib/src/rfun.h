/*------------------------------------------------------------------
// rfun.h
// Copyright (c) 2006 
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
// TRFun
//
/// Provides splitting of a class into interface and implementation
/**

  \code

  ///////////////////////////////////////////////
  // *** RfTest.h

	#pragma once

	#include "RFun.h"

	class CRfContext
	{	UINT uValue;
	};


	// {266B2BAD-710D-4a3c-B65F-0465D47D6E56}
	static const GUID IDD_CRfTest = 
	{ 0x266b2bad, 0x710d, 0x4a3c, { 0xb6, 0x5f, 0x4, 0x65, 0xd4, 0x7d, 0x6e, 0x56 } };

	class CRfTest : public TRFun< TCHAR, CRfContext >
	{
	public:

		/// Default constructor
		CRfTest() { InitFunctionMap(); }

		/// Destructor
		virtual ~CRfTest() {}


		// Show message function
		HRESULT ShowMessage( CRfContext *pContext );	


		// Interface map
		START_RFINTERFACE( IDD_CRfTest )
			
			RFINTERFACE_FUNCTION( ShowMessage )

		END_RFINTERFACE()


		// Function map
		START_RFMAP( CRfTest )
			
			RFMAP_FUNCTION( ShowMessage )

		END_RFMAP()

	};

  ///////////////////////////////////////////////
  // *** RfTest.cpp

	#include "RfTest.h"

	HRESULT CRfTest::ShowMessage(CRfContext *pContext)
	{
		MessageBox( NULL, "ShowMessage()", "Function", MB_OK );
		return 0;
	}

  ///////////////////////////////////////////////
  // main()
  
	CRfContext rc;

	CRfTest implRt;

	CRfTest::Interface inRt;

	inRt.SetImpl( &implRt );

	inRt.ShowMessage( &rc );

  \endcode

*/
//==================================================================
template < class T, class CONTEXT > class TRFun
{
public:

    //==================================================================
    // CAutoMem
    //
    /// Just a simple auto pointer
    /**
        This class is a simple auto pointer.  It has properties that I
        particularly like for this type of job.  I'll quit making my
        own when boost comes with VC...     
    */
    //==================================================================
    template < class T > class CAutoMem
    {
        public:

            /// Default constructor
            CAutoMem() { m_p = NULL; }

			/// Assignment constructor
			CAutoMem( T* p ) { m_p = p; }

			/// Assignment
			CAutoMem* operator =( T* p )
			{ release(); m_p = p; return this; }

            /// Destructor
            ~CAutoMem() { release(); }

            /// Release allocated object
            void release() 
			{ if ( m_p ) 
				{ delete m_p; m_p = NULL; } 
			}

            /// Returns a pointer to encapsulated object
            T& Obj() { return *m_p; }

            /// Returns a pointer to the encapsulated object
            operator T&() { return Obj(); }

			/// Returns non-zero if valid object
			oexBOOL IsValid() { return NULL != m_p; }

        private:

            /// Contains a pointer to the controlled object
            T       *m_p;
            
    };

	/// TRfCall
	template< class T_RETURN, typename T_ARG1 > class TRfCall
	{
	public:

		/// Make the function call
		virtual T_RETURN Call( T_ARG1 arg1 ) = 0;
	};

	/// TRfBind
	template < class T_CLASS, typename T_RETURN, typename T_ARG1 > class TRfBind : 
		public TRfCall< T_RETURN, T_ARG1 >
	{
	public:

		/// Constructor binds to the member function
		TRfBind( T_CLASS *pClass, T_RETURN ( T_CLASS::*pFunction )( T_ARG1 ) )
		{	m_pClass = pClass; m_pFunction = pFunction; }

		/// Makes the member function call
		virtual T_RETURN Call( T_ARG1 arg1 ) { return ( m_pClass->*m_pFunction )( arg1 ); }

	private:

		/// Class object
		T_CLASS		*m_pClass;

		/// Function pointer
		T_RETURN ( T_CLASS::*m_pFunction )( T_ARG1 );
	};

	typedef T			__t_T;

	/// Context type
	typedef CONTEXT		__t_Context;

public:

	/// Default constructor
	TRFun() { }

	/// Destructor
	virtual ~TRFun() { __m_lstFunctionMap.clear(); }

	/// Initializes the function map
	void InitFunctionMap()
	{ 	__RegisterClassId(); 
		__RegisterCallbacks(); 
	}

	/// Calls the specified function
	oexRESULT Call( oexCSTR pName, __t_Context *pContext )
	{	__t_FunctionMap::iterator it = __m_lstFunctionMap.find( pName );
		if ( __m_lstFunctionMap.end() == it || !it->second.IsValid() ) return oexERR;
		return it->second.Obj().Call( pContext );
	}

	/// Registers the callback functions
	virtual void __RegisterCallbacks() {}

	/// Registers the class ID
	virtual void __RegisterClassId() {}

protected:

	/// Registers the specified function
	virtual void __RegisterFunction( oexCSTR pName, TRfCall< oexRESULT, __t_Context* >* fn )
	{	__m_lstFunctionMap[ pName ] = fn; }

	/// The class ID
	oexGUID __m_guidClassId;

private:

	/// Function map node type
	typedef CAutoMem< TRfCall< oexRESULT, __t_Context* > > __t_FunctionMapNode;

	/// Function map type
	typedef std::map< std::basic_string< T >, __t_FunctionMapNode > __t_FunctionMap;
		
	/// Function map
	__t_FunctionMap	__m_lstFunctionMap;
};

#define START_RFINTERFACE( __guid )								\
	virtual void __RegisterClassId()								\
	{	memcpy( &__m_guidClassId, &__guid, sizeof( OEXLIB_NAMESPACE::oexGUID ) ); }		\
	class Interface		{	public:									\
	Interface() { __m_pRFun = NULL; }								\
	OEXLIB_NAMESPACE::oexRESULT __CallFunction( OEXLIB_NAMESPACE::oexCSTR pFn, __t_Context *pContext )		\
	{	if ( __m_pRFun ) return __m_pRFun->Call( pFn, pContext );	\
		return E_NOTIMPL; }											\
		OEXLIB_NAMESPACE::TRFun< __t_T, __t_Context > *__m_pRFun;								\
	void SetImpl( OEXLIB_NAMESPACE::TRFun< __t_T, __t_Context > *p ) { __m_pRFun = p; }

#define RFINTERFACE_FUNCTION( __f )									\
	OEXLIB_NAMESPACE::oexRESULT __f( __t_Context *pContext ) {							\
		return __CallFunction( _T( #__f ), pContext ); }		

#define END_RFINTERFACE()					};

#define START_RFMAP( __c )											\
	typedef __c __t_ClassType;										\
	virtual void __RegisterCallbacks()		{

#define RFMAP_FUNCTION( __f )										\
	__RegisterFunction( _T( #__f ), new TRfBind< __t_ClassType, OEXLIB_NAMESPACE::oexRESULT, __t_Context* >( this, __t_ClassType::__f ) );

#define END_RFMAP()		}




