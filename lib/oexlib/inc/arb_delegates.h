/*------------------------------------------------------------------
// arb_delegates.h
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

// 0 Parameters
#define ARBDEL_TPARAM0
#define ARBDEL_PPARAM0
#define ARBDEL_QPARAM0
#define ARBDEL_DPARAM0
#define ARBDEL_FPARAM0
#define ARBDEL_UPARAM0
#define ARBDEL_CPARAM0

// 1 Parameters
#define ARBDEL_TPARAM1      , typename T_P1
#define ARBDEL_PPARAM1      T_P1
#define ARBDEL_QPARAM1      , ARBDEL_PPARAM1
#define ARBDEL_FPARAM1      T_ARB p1
#define ARBDEL_DPARAM1      , ARBDEL_FPARAM1
#define ARBDEL_UPARAM1      p1
#define ARBDEL_CPARAM1      , ARBDEL_UPARAM1

// 2 Parameters
#define ARBDEL_TPARAM2      , typename T_P1, typename T_P2
#define ARBDEL_PPARAM2      T_P1, T_P2
#define ARBDEL_QPARAM2      , ARBDEL_PPARAM2
#define ARBDEL_FPARAM2      T_ARB p1, T_ARB p2
#define ARBDEL_DPARAM2      , ARBDEL_FPARAM2
#define ARBDEL_UPARAM2      p1, p2
#define ARBDEL_CPARAM2      , ARBDEL_UPARAM2

// 3 Parameters
#define ARBDEL_TPARAM3      , typename T_P1, typename T_P2, typename T_P3
#define ARBDEL_PPARAM3      T_P1, T_P2, T_P3
#define ARBDEL_QPARAM3      , ARBDEL_PPARAM3
#define ARBDEL_FPARAM3      T_ARB p1, T_ARB p2, T_ARB p3
#define ARBDEL_DPARAM3      , ARBDEL_FPARAM3
#define ARBDEL_UPARAM3      p1, p2, p3
#define ARBDEL_CPARAM3      , ARBDEL_UPARAM3

// 4 Parameters
#define ARBDEL_TPARAM4      , typename T_P1, typename T_P2, typename T_P3, typename T_P4
#define ARBDEL_PPARAM4      T_P1, T_P2, T_P3, T_P4
#define ARBDEL_QPARAM4      , ARBDEL_PPARAM4
#define ARBDEL_FPARAM4      T_ARB p1, T_ARB p2, T_ARB p3, T_ARB p4
#define ARBDEL_DPARAM4      , ARBDEL_FPARAM4
#define ARBDEL_UPARAM4      p1, p2, p3, p4
#define ARBDEL_CPARAM4      , ARBDEL_UPARAM4

// 5 Parameters
#define ARBDEL_TPARAM5      , typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5
#define ARBDEL_PPARAM5      T_P1, T_P2, T_P3, T_P4, T_P5
#define ARBDEL_QPARAM5      , ARBDEL_PPARAM5
#define ARBDEL_FPARAM5      T_ARB p1, T_ARB p2, T_ARB p3, T_ARB p4, T_ARB p5
#define ARBDEL_DPARAM5      , ARBDEL_FPARAM5
#define ARBDEL_UPARAM5      p1, p2, p3, p4, p5
#define ARBDEL_CPARAM5      , ARBDEL_UPARAM5

// 6 Parameters
#define ARBDEL_TPARAM6      , typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6
#define ARBDEL_PPARAM6      T_P1, T_P2, T_P3, T_P4, T_P5, T_P6
#define ARBDEL_QPARAM6      , ARBDEL_PPARAM6
#define ARBDEL_FPARAM6      T_ARB p1, T_ARB p2, T_ARB p3, T_ARB p4, T_ARB p5, T_ARB p6
#define ARBDEL_DPARAM6      , ARBDEL_FPARAM6
#define ARBDEL_UPARAM6      p1, p2, p3, p4, p5, p6
#define ARBDEL_CPARAM6      , ARBDEL_UPARAM6

#define ARBDEL_TYPEDEF_THUNK( n ) \
    typedef T_ARB (*t_Thunk##n)( void *pClass, void *pFunction ARBDEL_DPARAM##n );

#define ARBDEL_FUNCTION_OPERATOR( n ) \
    T_ARB operator ()( ARBDEL_FPARAM##n )                                   \
    {   oexASSERT( n == m_nParams );                                        \
        if ( n != m_nParams ) return 0;                                     \
        return (*((t_Thunk##n)m_pThunk))( m_pClass, 0 ARBDEL_CPARAM##n );   \
    }

#define ARBDEL_STATIC_ADDRESS( n ) \
    template < typename T_RT, class T_CLASS ARBDEL_TPARAM##n >                  \
        static void* Address##n( T_RT (T_CLASS::*pF)( ARBDEL_PPARAM##n ) )      \
        {   typedef T_RT (T_CLASS::*t_f)( ARBDEL_PPARAM##n );                   \
            static t_f s_pF = 0; if ( pF && !s_pF ) s_pF = pF; return &s_pF;    \
        }

#define ARBDEL_THUNK( n ) \
    template < typename T_RT > struct SThunk##n {                                                       \
        template < typename T_RT, class T_CLASS ARBDEL_TPARAM##n >                                      \
            static T_ARB Thunk##n( void *pClass, void *f ARBDEL_DPARAM##n )                             \
            {   typedef T_RT (T_CLASS::*t_f)( ARBDEL_PPARAM##n );                                       \
                return ( ( (T_CLASS*)pClass )->*( *(t_f*)                                               \
                            Address##n< T_RT, T_CLASS ARBDEL_QPARAM##n >( 0 ) ) )( ARBDEL_UPARAM##n );  \
        }                                                                                               \
    };                                                                                                  \
    template <> struct SThunk##n< void > {                                                              \
        template < typename T_RT, class T_CLASS ARBDEL_TPARAM##n >                                      \
            static T_ARB Thunk##n( void *pClass, void *f ARBDEL_DPARAM##n )                             \
            {   typedef T_RT (T_CLASS::*t_f)( ARBDEL_PPARAM##n );                                       \
                ( ( (T_CLASS*)pClass )->*( *(t_f*)                                                      \
                            Address##n< T_RT, T_CLASS ARBDEL_QPARAM##n >( 0 ) ) )( ARBDEL_UPARAM##n );  \
                return 0;                                                                               \
             }                                                                                          \
        };

#define ARBDEL_REGISTER( n ) \
    template < typename T_RT, class T_CLASS ARBDEL_TPARAM##n >                                              \
        void Register( T_CLASS *pClass, T_RT (T_CLASS::*pF)( ARBDEL_PPARAM##n ) )                           \
        {   m_nParams = n;                                                                                  \
            m_pClass = pClass; Address##n< T_RT, T_CLASS ARBDEL_QPARAM##n >( pF );                          \
            m_pThunk = (void*)(t_Thunk##n)&SThunk##n< T_RT >::Thunk##n< T_RT, T_CLASS ARBDEL_QPARAM##n >;   \
        }

#define ARBDEL_CONSTRUCT( c, n ) \
    template < typename T_RT, class T_CLASS ARBDEL_TPARAM##n >                          \
        c( T_CLASS *pClass, T_RT (T_CLASS::*pF)( ARBDEL_PPARAM##n ) )                   \
        {   Register( pClass, pF ); }

#define ARBDEL_PARAM( c, n )    \
    ARBDEL_TYPEDEF_THUNK( n ); ARBDEL_FUNCTION_OPERATOR( n );   \
    ARBDEL_STATIC_ADDRESS( n ); ARBDEL_THUNK( n );              \
    ARBDEL_REGISTER( n ); ARBDEL_CONSTRUCT( c, n );

/// Delegates template that invokes functions using an arbitration class
/**

    Example:

  \code

    #include <stdio.h>
    #include <tchar.h>
    #include <string.h>
    #include <comutil.h>
    #include "ArbDelegates.h"

    // Turn off Microsoft's promotional material
    #pragma warning ( disable : 4996 )

    // Define our custom delegates class
    typedef adel::TArbDelegate< _variant_t > CDelegate;

    // Class containing functions we will call
    class CMyClass
    { 
    public:    
        int Add2( int a, int b ) { return a + b; } 
        float Add3( float a, float b, float c ) { return a + b + c; } 
        void Trace( TCHAR *pMsg ) { TRACE0( pMsg ); }
        TCHAR* GetString() { return "SomeString\n\n"; }
    };

    // Test the delegates
    void test_delegates()
    {
        CMyClass mc;

        // Call CMyClass::Add1()
        CDelegate d;
        d.Register( &mc, &CMyClass::Add2 );
        int x = d( 2, 3 );

        // Call CMyClass::Add2()
        CDelegate d2( &mc, &CMyClass::Add3 );
        float y = d2( 1.5, 2.5, 3.5 );

        // Call CMyClass::Trace
        CDelegate d3( &mc, &CMyClass::Trace );
        d3( _T( "\nHello World!\n\n" ) );

        // Call CMyClass::GetString
        CDelegate d4( &mc, &CMyClass::GetString );
        TRACE0( d4() );

        return;
    }

  \endcode

*/
template < class T_ARB > class TArbDelegate
{
public:

    /// Default constructor
    TArbDelegate() 
    {   m_pClass = 0; 
        m_pThunk = 0;
        m_nParams = -1;
    }

    /// Copy Constructor
    TArbDelegate( TArbDelegate &rAd )
    {   m_pClass = rAd.m_pClass; 
        m_pThunk = rAd.m_pThunk; 
        m_nParams = rAd.m_nParams; 
    }

    // Delcare a set for each number of parameters
    ARBDEL_PARAM( TArbDelegate, 0 );
    ARBDEL_PARAM( TArbDelegate, 1 );
    ARBDEL_PARAM( TArbDelegate, 2 );
    ARBDEL_PARAM( TArbDelegate, 3 );
    ARBDEL_PARAM( TArbDelegate, 4 );
    ARBDEL_PARAM( TArbDelegate, 5 );
    ARBDEL_PARAM( TArbDelegate, 6 );

    /// Returns the number of parameters
    int GetNumParams() 
    {   return m_nParams; }
    
private:

    /// Class pointer
    void    *m_pClass;

    /// Static function containing the actual member function call
    void    *m_pThunk;

    /// The number of parameters for the wrapped function
    int     m_nParams;
};

