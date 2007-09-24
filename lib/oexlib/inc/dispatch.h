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


#define OexRpcRegisterPtr( p, c, f )    Register( oexT( #f ), oex::TArbDelegate< oex::CAutoStr >( p, &c::f ) );
#define OexRpcRegister( c, f )          OexRpcRegisterPtr( (c*)this, c, f )

#define oexCall                         oex::CDispatch::Call


struct CReplyInfo
{
public:

    /// Default constructor
    CReplyInfo() {}

    /// Constructor
    CReplyInfo( CStr x_sCmd )
    {   sCmd = x_sCmd; }

    /// Copy constructor
    CReplyInfo( CReplyInfo &x_rRi )
    {   sCmd = x_rRi.sCmd; sReturn = x_rRi.sReturn; evReady = x_rRi.evReady; }

    /// Copy operator
    CReplyInfo& operator = ( CReplyInfo &x_rRi )
    {   sCmd = x_rRi.sCmd; sReturn = x_rRi.sReturn; evReady = x_rRi.evReady; return *this; }

    /// Command string
    CStr            sCmd;

    /// Set when the reply is ready
    CTlEvent        evReady;

    /// Return event id
    CStr            sReturn;
};



class CReply
{
public:

    /// Default constructor
    CReply()
    {
    }

    /// Reply constructor
    CReply( TMem< CReplyInfo > &x_rRi )
    {
        m_ri.Share( x_rRi );
    }

    /// Destructor
    virtual ~CReply()
    {
    }

    /// Returns non-zero if the reply is ready
    oexBOOL Wait( oexUINT x_uTimeout )
    {   return m_ri.Ptr() && m_ri.Ptr()->evReady.Wait( x_uTimeout ); }

    /// Returns non-zero if the reply is ready
    oexBOOL IsReady()
    {   return Wait( 0 ); }

    /// Returns the reply value
    CStr GetReply()
    {
        if ( !IsReady() )
            return oexT( "" );

        // Return the reply value
        return m_ri.Ptr()->sReturn;
    }

    /// Returns the reply value
    CStr operator *() 
    {   return GetReply(); }

    /// Reply information
    TMem< CReplyInfo >      m_ri;
};

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
    typedef TList< TMem< CReplyInfo > > t_CmdBufferList;

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
            {   m_lstCmd.Destroy();
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

    /// Puts a function call into the queue
    /**
        Returns a unique id for the return event
    */
    CReply Queue( oexCSTR x_pId, CStr x_sCmd )
    {
        CTlLocalLock ll( m_lockPre );
        if ( !ll.IsLocked() )
            return CReply();

        // Attempt to map the message
        CReply reply;
        if ( OnMap( x_pId, x_sCmd, reply ) )
            return reply;

        // Append command
        t_CmdBufferList::iterator it = m_lstPre.Append();

        // Create command
        it.Obj().OexConstruct( x_sCmd );

        // Signal that a command is waiting
        m_evCmd.Set();

        // Return the event id
        return it.Obj();
    }    

    /// Over-ride to provide message mapping
    virtual oexBOOL OnMap( oexCSTR x_pId, CStr x_sCmd, CReply &x_rReply )
    {   return oexFALSE; }

    /// Processes the function queue
    /**
        \param [in] uMax    -   Maximum number of commands to execute

        \return Number of commands executed.
    */
    oexUINT ProcessQueue( oexINT nMax = -1 )
    {
        // Lock the command buffer
        CTlLocalLock ll( m_lockCmd );
        if ( !ll.IsLocked() )
            return 0;

        { // Copy commands from pre-stage buffer

            CTlLocalLock ll( m_lockPre );
            if ( ll.IsLocked() )
                m_lstCmd.Append( m_lstPre );

        } // end copy commands

        oexUINT uCmds = 0;

        // Execute commands
        for ( t_CmdBufferList::iterator it; nMax-- && m_lstCmd.Next( it ); )
        {
            // Ensure valid command
            if ( it.Obj().Ptr() )
            {
                // Execute the command
                Execute( it.Obj().Ptr()->sCmd, &it.Obj().Ptr()->sReturn );

                // Signal that reply is ready
                it.Obj().Ptr()->evReady.Set();

                // Count commands
                uCmds++;

            } // end if

            // Drop this command
            it = m_lstCmd.Erase( it );      

        } // end for

        { // Copy commands from pre-stage buffer

            CTlLocalLock ll( m_lockPre );
            if ( ll.IsLocked() )
            {
                // Reset event if no more commands
                if ( !m_lstCmd.Size() && !m_lstPre.Size() )
                    m_evCmd.Reset();

                // Just to make sure
                else
                    m_evCmd.Set();

            } // end if

        } // end copy commands

        return uCmds;
    }

    /// Executes the specified function call
    oexBOOL Execute( CStr x_sCmd, CStr *x_pReturn = oexNULL )
    {
        CTlLocalLock ll( m_lockCmd );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Deserialize the command
        CPropertyBag pb = CParser::Deserialize( x_sCmd );

        if ( !pb[ oexT( "f" ) ].IsSet() )
            return oexFALSE;

        t_EventHandlerList::iterator it = m_lstEventHandlers.Find( pb[ oexT( "f" ) ].ToString() );
        if ( !it.IsValid() )
            return oexFALSE;

        CStr ret;

        // 0 params
        if ( !pb[ oexT( "p" ) ].IsKey( 0 ) )
        {
            if ( oexVERIFY( 0 == it->GetNumParams() ) )
                ret = it.Obj()();
        } // end else if

        // 1 params
        else if ( !pb[ oexT( "p" ) ].IsKey( 1 ) )
        {
            if ( oexVERIFY( 1 == it->GetNumParams() ) )
                ret = it.Obj()( pb[ oexT( "p" ) ][ 0 ].ToString() );
        } // end else if

        // 2 params
        else if ( !pb[ oexT( "p" ) ].IsKey( 2 ) )
        {
            if ( oexVERIFY( 2 == it->GetNumParams() ) )
                ret = it.Obj()( pb[ oexT( "p" ) ][ 0 ].ToString(), pb[ oexT( "p" ) ][ 1 ].ToString() );
        } // end else if

        // 3 params
        else if ( !pb[ oexT( "p" ) ].IsKey( 3 ) )
        {
            if ( oexVERIFY( 3 == it->GetNumParams() ) )
                ret = it.Obj()( pb[ oexT( "p" ) ][ 0 ].ToString(), pb[ oexT( "p" ) ][ 1 ].ToString(),
                                pb[ oexT( "p" ) ][ 2 ].ToString() );
        } // end else if

        // 4 params
        else if ( !pb[ oexT( "p" ) ].IsKey( 4 ) )
        {
            if ( oexVERIFY( 4 == it->GetNumParams() ) )
                ret = it.Obj()( pb[ oexT( "p" ) ][ 0 ].ToString(), pb[ oexT( "p" ) ][ 1 ].ToString(),
                                pb[ oexT( "p" ) ][ 2 ].ToString(), pb[ oexT( "p" ) ][ 3 ].ToString() );
        } // end else if

        // 5 params
        else if ( !pb[ oexT( "p" ) ].IsKey( 5 ) )
        {
            if ( oexVERIFY( 5 == it->GetNumParams() ) )
                ret = it.Obj()( pb[ oexT( "p" ) ][ 0 ].ToString(), pb[ oexT( "p" ) ][ 1 ].ToString(),
                                pb[ oexT( "p" ) ][ 2 ].ToString(), pb[ oexT( "p" ) ][ 3 ].ToString(),
                                pb[ oexT( "p" ) ][ 4 ].ToString() );
        } // end else if

        // 6 params
        else if ( !pb[ oexT( "p" ) ].IsKey( 6 ) )
        {
            if ( oexVERIFY( 6 == it->GetNumParams() ) )
                ret = it.Obj()( pb[ oexT( "p" ) ][ 0 ].ToString(), pb[ oexT( "p" ) ][ 1 ].ToString(),
                                pb[ oexT( "p" ) ][ 2 ].ToString(), pb[ oexT( "p" ) ][ 3 ].ToString(),
                                pb[ oexT( "p" ) ][ 4 ].ToString(), pb[ oexT( "p" ) ][ 5 ].ToString() );
        } // end else if

        else 
            return oexFALSE;

        // Does the caller want the return value?
        if ( x_pReturn )
            *x_pReturn = ret;

        return oexTRUE;
    }

    static CStr Call( CStr sF )
    {
        CPropertyBag pb;
        pb[ oexT( "f" ) ] = sF;

        return CParser::Serialize( pb );
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

    template< typename T_P1, typename T_P2, typename T_P3 >
        static CStr Call( CStr sF, T_P1 p1, T_P2 p2, T_P3 p3 )
    {
        CPropertyBag pb;
        pb[ oexT( "f" ) ] = sF;
        pb[ oexT( "p" ) ][ 0 ] = p1;
        pb[ oexT( "p" ) ][ 1 ] = p2;
        pb[ oexT( "p" ) ][ 2 ] = p3;

        return CParser::Serialize( pb );
    }

    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4 >
        static CStr Call( CStr sF, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4 )
    {
        CPropertyBag pb;
        pb[ oexT( "f" ) ] = sF;
        pb[ oexT( "p" ) ][ 0 ] = p1;
        pb[ oexT( "p" ) ][ 1 ] = p2;
        pb[ oexT( "p" ) ][ 2 ] = p3;
        pb[ oexT( "p" ) ][ 3 ] = p4;

        return CParser::Serialize( pb );
    }

    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5 >
        static CStr Call( CStr sF, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5 )
    {
        CPropertyBag pb;
        pb[ oexT( "f" ) ] = sF;
        pb[ oexT( "p" ) ][ 0 ] = p1;
        pb[ oexT( "p" ) ][ 1 ] = p2;
        pb[ oexT( "p" ) ][ 2 ] = p3;
        pb[ oexT( "p" ) ][ 3 ] = p4;
        pb[ oexT( "p" ) ][ 4 ] = p5;

        return CParser::Serialize( pb );
    }

    template< typename T_P1, typename T_P2, typename T_P3, typename T_P4, typename T_P5, typename T_P6 >
        static CStr Call( CStr sF, T_P1 p1, T_P2 p2, T_P3 p3, T_P4 p4, T_P5 p5, T_P6 p6 )
    {
        CPropertyBag pb;
        pb[ oexT( "f" ) ] = sF;
        pb[ oexT( "p" ) ][ 0 ] = p1;
        pb[ oexT( "p" ) ][ 1 ] = p2;
        pb[ oexT( "p" ) ][ 2 ] = p3;
        pb[ oexT( "p" ) ][ 3 ] = p4;
        pb[ oexT( "p" ) ][ 4 ] = p5;
        pb[ oexT( "p" ) ][ 5 ] = p6;

        return CParser::Serialize( pb );
    }

    /// Returns a reference to the command waiting event
    CTlEvent& GetCmdEvent()
    {   return m_evCmd; }

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
    
    /// Command waiting signal
    CTlEvent                    m_evCmd;

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




