/*------------------------------------------------------------------
// dispatch.h
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


/// Event dispatch class
/**
    Event dispatching
    
  \code


  \endcode

*/
class CDispatch
{
public:

    /// Event handler list type
    typedef TAssoList< CStr, TArbDelegate< CAutoStr > > t_EventHandlerList;

    /// Command buffer
    typedef TList< CStr > t_CmdBufferList;

public:

    /// Constructor
    CDispatch() 
    {
    }

    /// Destructor
    virtual ~CDispatch()
    {   Destroy();
    }

    void Destroy()
    {
        { // Scope

            CTlLocalLock ll( m_lockPre );
            if ( ll.IsLocked() )
                m_lstPre.Destroy();

        } // end scope

        { // Scope

            CTlLocalLock ll( m_lockCmd );
            if ( ll.IsLocked() )
            {
                m_lstCmd.Destroy();
                m_lstEventHandlers.Destroy();
            } // end if

        } // end scope

    }

    /// Registers a callback function
    oexBOOL Register( CStr x_sName, TArbDelegate< CAutoStr > &x_adFunction )
    {
        CTlLocalLock ll( m_lockCmd );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Add to the list
        m_lstEventHandlers[ x_sName ] = x_adFunction;

        return oexTRUE;
    }

    /// Registers a callback function
    CAutoStr Execute( CStr x_sCmd, CStr x_sDef = oexT( "" ) )
    {
        CTlLocalLock ll( m_lockCmd );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Deserialize the command
        CPropertyBag pb = CParser::Deserialize( x_sCmd );

        if ( !pb[ oexT( "f" ) ].IsSet() )
            return x_sDef;

        t_EventHandlerList::iterator it = m_lstEventHandlers.Find( pb[ oexT( "f" ) ].ToString() );
        if ( !it.IsValid() )
            return x_sDef;

        // 0 params
        if ( !pb[ oexT( "p" ) ].IsKey( 0 ) )
        {   
            if ( !oexVERIFY( 0 == it->GetNumParams() ) )
                return x_sDef;

             return it.Obj()();

        } // end if

        // 1 params
        if ( !pb[ oexT( "p" ) ].IsKey( 1 ) )
        {   
            if ( !oexVERIFY( 1 == it->GetNumParams() ) )
                return x_sDef;

            return it.Obj()( pb[ oexT( "p" ) ][ 0 ].ToString() );

        } // end if

        // 2 params
        if ( !pb[ oexT( "p" ) ].IsKey( 2 ) )
        {   
            if ( !oexVERIFY( 2 == it->GetNumParams() ) )
                return x_sDef;

            return it.Obj()( pb[ oexT( "p" ) ][ 0 ].ToString(), pb[ oexT( "p" ) ][ 1 ].ToString() );

        } // end if

        return x_sDef;
    }

    /// Registers a callback function
    oexBOOL Queue( CStr x_sCmd )
    {
        CTlLocalLock ll( m_lockPre );
        if ( !ll.IsLocked() )
            return oexFALSE;

        m_lstPre.Append( x_sCmd );

        return oexTRUE;
    }

    template< typename T_P1 >
        static CStr Call( CStr sF, T_P1 p1 )
    {
        CPropertyBag pb;
        pb[ oexT( "f" ) ] = sF;
        pb[ oexT( "p" ) ][ 0 ] = p1;

        return CParser::Serialize( pb );
    }

    template< typename T_P1, typename T_P2 >
        static CStr Call( CStr sF, T_P1 p1, T_P2 p2 )
    {
        CPropertyBag pb;
        pb[ oexT( "f" ) ] = sF;
        pb[ oexT( "p" ) ][ 0 ] = p1;
        pb[ oexT( "p" ) ][ 1 ] = p2;

        return CParser::Serialize( pb );
    }

private:

    /// Thread lock
    CTlLock                     m_lockCmd;

    /// Thread lock
    CTlLock                     m_lockPre;

    /// List of event handlers
    t_EventHandlerList          m_lstEventHandlers;

    /// Command buffer
    t_CmdBufferList             m_lstCmd;

    /// Pre queue
    t_CmdBufferList             m_lstPre;

};



/*

    CCallbackClass cc;

//    oex::TDelegate< int, int, int > d;

    oex::TArbDelegate< oex::CAutoStr > d;

    d.Set( &cc, &CCallbackClass::Add );

    int x = d( 1, 2 );


    int y = 0;

//    void *pD = oex::CDelegate::CreateDelegate( &cc, &CCallbackClass::Add );


template < typename T_ARB >
    class TDelegate
{
public:

    /// Two param callback
    typedef T_ARB (*t_Call2)( oexPVOID, oexPVOID, oexPVOID, T_ARB, T_ARB );

public:

    /// Constructor
    TDelegate()
    {
        m_pClass = m_pFunction = oexNULL;        
    }

    /// Set delegate
    template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS > 
        void Set( T_CLASS *x_pC, T_RET(T_CLASS::*x_pF)( T_P1, T_P2 ) )
    {
        _Set< T_RET, T_P1, T_P2 >( x_pC, x_pF );
    }

public:

    /// Thunk
    template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS, typename T_FUNCTION > 
        static T_RET Thunk( void* x_pC, void* x_pF, T_P1 p1, T_P2 p2 )
    {   return ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)&x_pF ) ) )( p1, p2 ); }

    /// Sets delegate target
    template < typename T_RET, typename T_P1, typename T_P2, typename T_CLASS, typename T_FUNCTION > 
        void _Set( T_CLASS *x_pC, T_FUNCTION x_pF )
    {
        /// Thunk function type
        union UThunk { T_RET (*pThunk)( void*, void*, T_P1, T_P2 ); };
        oexSTATIC_ASSERT( sizeof( m_rawThunk ) == sizeof( UThunk ) );

        m_pClass = x_pC;
        m_pFunction = *(void**)&x_pF;

        ( (UThunk*)m_rawThunk )->pThunk = &Thunk< T_RET, T_P1, T_P2, T_CLASS, T_FUNCTION >;

        m_pCall2 = &Call< T_RET, T_P1, T_P2 >;
    }

    template < typename T_RET, typename T_P1, typename T_P2 > 
        static T_ARB Call( oexPVOID x_pT, oexPVOID x_pC, oexPVOID x_pF, T_ARB p1, T_ARB p2 )
    {
        /// Thunk function type
        union UThunk { T_RET (*pThunk)( void*, void*, T_P1, T_P2 ); };
//        oexSTATIC_ASSERT( sizeof( m_rawThunk ) == sizeof( UThunk ) );

        return ( ( (UThunk*)x_pT )->pThunk )( x_pC, x_pF, p1, p2 );
    } 

    T_ARB operator ()( T_ARB p1, T_ARB p2 )
    {
        return m_pCall2( m_rawThunk, m_pClass, m_pFunction, p1, p2 );
    }

/*
    /// Call delegate
    template < typename T_RET, typename T_P1, typename T_P2 > 
        T_RET Call( T_P1 p1, T_P2 p2 )
    {     
        /// Thunk function type
        union UThunk { T_RET (*pThunk)( void*, void*, T_P1, T_P2 ); };
        oexSTATIC_ASSERT( sizeof( m_rawThunk ) == sizeof( UThunk ) );

        return ( ( (UThunk*)m_rawThunk )->pThunk )( m_pClass, m_pFunction, p1, p2 ); 
    }
* /

protected:

    /// Class pointer
    oexPVOID        m_pClass;

    /// Function pointer
    oexPVOID        m_pFunction;

    /// Thunk
    oexUCHAR        m_rawThunk[ sizeof( void (TDelegate::*)() ) ];

    t_Call2         m_pCall2;
};


/*
template< typename T_RET, typename T_P1, typename T_P2 >
    class TDelegate : public CDelegate
{
public:

    /// Thunk function type
    typedef T_RET (*t_Thunk)( void*, void*, T_P1, T_P2 );

    /// Thunking function
    union UThunk
    {
        t_Thunk         pThunk;
    };

public:

    /// Thunk
    template < typename T_CLASS, typename T_FUNCTION > 
        static int Thunk( void* x_pC, void* x_pF, T_P1 p1, T_P2 p2 )
    {   return ( ( (T_CLASS*)x_pC )->*( *( (T_FUNCTION*)&x_pF ) ) )( p1, p2 ); }

    /// Function call operator
    T_RET operator()( T_P1 p1, T_P2 p2 )
    {     
        return ( ( (UThunk*)m_rawThunk )->pThunk )( m_pClass, m_pFunction, p1, p2 ); 

//        t_Thunk f = (t_Thunk)m_pThunk;
//        t_Thunk f = ( ( (UThunk*)m_rawThunk )->pThunk );
//        return m_pThunk( m_pClass, m_pFunction, p1, p2 ); 
    }

    /// Sets delegate target
    template < typename T_CLASS, typename T_FUNCTION > 
        void Set( T_CLASS *x_pC, T_FUNCTION x_pF )
    {   m_pClass = x_pC;
        m_pFunction = *(void**)&x_pF;
        ( (UThunk*)m_rawThunk )->pThunk = &Thunk< T_CLASS, T_FUNCTION >;
    }

private:

/*    /// Class pointer
    oexPVOID        m_pClass;

    /// Function pointer
    oexPVOID        m_pFunction;

    /// Thunking function
    union
    {
        t_Thunk         m_pThunk;

        char            m_rawThunk[ sizeof( t_Thunk ) ];
    };
* /
//    T_RET (*m_pThunk)( void*, void*, T_P1, T_P2 );

//    oexPVOID m_pThunk;
};

/*
class CDelegate
{
public:

    template< typename T_CLASS, typename T_RET, typename T_P1, typename T_P2 >
        static void* CreateDelegate( T_CLASS *x_pC, T_RET (T_CLASS::*x_pF)( T_P1, T_P2 ) )
        {
            TDelegate< T_RET, T_P1, T_P2 > *p = new TDelegate< T_RET, T_P1, T_P2 >();
            p->Set( x_pC, x_pF );
            return (void*)p;
        }

};
*/




