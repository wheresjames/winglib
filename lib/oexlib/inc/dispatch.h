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


#define OexRpcRegisterPtr( p, c, f )    Register( oexT( #f ), oex::TArbDelegate< oex::CAutoStr >( (const c*)p, &c::f ) );
#define OexRpcRegister( c, f )          OexRpcRegisterPtr( this, c, f )

#define oexCall                         oex::CDispatch::Call


struct CReplyInfo
{
public:

    /// Default constructor
    CReplyInfo() {}

    /// Constructor
    CReplyInfo( CStr x_sCmd, oexUINT x_uTime = 0 )
    {   sCmd = x_sCmd; uTime = x_uTime; }

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

    /// Execution time
    oexUINT         uTime;
};


class CReply
{
public:

    /// Default constructor
    CReply()
    {
    }

    /// Reply constructor
    CReply( oexCONST TMem< CReplyInfo > &x_rRi )
    {
        m_ri.Share( x_rRi );
    }

    /// Reply constructor
    CReply( oexCONST CReply &x_rR )
    {
        m_ri.Share( x_rR.m_ri );
    }

    /// Destructor
    virtual ~CReply()
    {
    }

    /// Returns non-zero if the reply is ready
    CReply& Wait( oexUINT x_uTimeout )
    {
        // Wait for reply if we have an object
        if ( m_ri.Ptr() )
            m_ri.Ptr()->evReady.Wait( x_uTimeout ); 

        return *this;
    }

    /// Returns non-zero if the reply is ready
    oexBOOL IsDone()
    {   return m_ri.Ptr() && m_ri.Ptr()->evReady.Wait( 0 ); 
    }

    /// Returns the reply value
    CStr GetReply( oexCSTR x_pDef = oexT( "" ) )
    {
        if ( !IsDone() )
            return x_pDef;

        // Return the reply value
        return m_ri.Ptr()->sReturn;
    }

    /// Returns the reply value
    CStr operator *() 
    {   return GetReply(); }

    /// Returns the desired execution time
    oexUINT GetTime()
    {   if ( !m_ri.Ptr() )
            return 0;
        return m_ri.Ptr()->uTime;
    }

    /// Returns non-zerof if valid object
    oexBOOL IsValid()
    {   return m_ri.Size() ? oexTRUE : oexFALSE; }

    /// Returns the ref count on the underlying object
    oexUINT GetRefCount()
    {   return m_ri.GetRefCount(); }

private:
    
    /// Reply information
    TMem< CReplyInfo >      m_ri;
};

/// Holds the shared information between CDispatch objects
class CDispatchInfo
{
public:

    /// Event handler list type
    typedef TAssoList< CStr, TArbDelegate< CAutoStr > > t_EventHandlerList;

    /// Command buffer
    typedef TList< TMem< CReplyInfo > > t_CmdBufferList;
    
public:

    /// Destructor
    ~CDispatchInfo()
    {   Destroy(); }

    /// Destroys the object
    void Destroy()
    {
        { // Scope

            CTlLocalLock ll( lockPre );
            if ( ll.IsLocked() )
                lstPre.Destroy();

        } // end scope

        { // Scope

            CTlLocalLock ll( lockCmd );
            if ( ll.IsLocked() )
            {   lstCmd.Destroy();
                lstEventHandlers.Destroy();
            } // end if

        } // end scope
    }


public:

    /// Thread lock
    CTlLock                     lockCmd;

    /// Thread lock
    CTlLock                     lockPre;

    /// List of event handlers
    t_EventHandlerList          lstEventHandlers;

    /// Command buffer
    t_CmdBufferList             lstCmd;

    /// Pre queue
    t_CmdBufferList             lstPre;
    
    /// Command waiting signal
    CTlEvent                    evCmd;
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

    /// Constructor
    CDispatch() 
    {
        // Construct data object
        m_di.OexConstruct();
    }

    /// Copy constructor
    CDispatch( CDispatch &rD )
    {   m_di.Share( rD.m_di );
    }

    /// Copy operator
    CDispatch& operator = ( CDispatch &rD )
    {   m_di.Share( rD.m_di );
        return *this;
    }

    /// Destructor
    virtual ~CDispatch()
    {   Destroy();
    }

    /// Releases resources
    void Destroy()
    {
        // Lose info object
        m_di.Delete();
    }

    /// Registers a callback function
    oexBOOL Register( CStr x_sName, const TArbDelegate< CAutoStr > &x_adFunction )
    {
        // Ensure valid object
        if ( !IsValid() )
            return oexFALSE;

        // Lock the command buffer
        CTlLocalLock ll( m_di.Ptr()->lockCmd );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Add to the list
        m_di.Ptr()->lstEventHandlers[ x_sName ] = x_adFunction;

        return oexTRUE;
    }

    /// Puts a function call into the queue
    /**
        \param [in] x_pId   -   Id of the message target, interpreted by OnMap()
        \param [in] x_sCmd  -   Actual command string, use oexCall to generate.
        \param [in] x_uTime -   Time the command should be executed, zero for
                                immediate execution.
        

        Returns a CReply object
    */
    CReply Queue( oexCSTR x_pId, oexCONST CStr x_sCmd, oexUINT x_uTime = 0 )
    {
        // Ensure valid data object
        if ( !IsValid() )
            return CReply();

        // Lock the pre-queue
        CTlLocalLock ll( m_di.Ptr()->lockPre );
        if ( !ll.IsLocked() )
            return CReply();

        // Append command
        CDispatchInfo::t_CmdBufferList::iterator it = m_di.Ptr()->lstPre.Append();

        // Create command
        it.Obj().OexConstruct( x_sCmd, x_uTime );

        // Signal that a command is waiting
        m_di.Ptr()->evCmd.Set();

        // Return the reply event
        return it.Obj();
    }    

    // +++ Need to add the mapping code to Execute()
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
        // Ensure valid data object
        if ( !IsValid() )
            return 0;

        // Lock the command buffer
        CTlLocalLock ll( m_di.Ptr()->lockCmd );
        if ( !ll.IsLocked() )
            return 0;

        { // Copy commands from pre-stage buffer

            CTlLocalLock ll( m_di.Ptr()->lockPre );
            if ( ll.IsLocked() )
                m_di.Ptr()->lstCmd.Append( m_di.Ptr()->lstPre );

        } // end copy commands

        // Number of commands executed
        oexUINT uCmds = 0;

        // Count waits
        oexUINT uWaits = 0;

        // What we think the current time is
        oexCONST oexUINT uCurrentTime = CSysTime( CSysTime::eGmtTime ).GetUnixTime();

        // Execute commands
        for ( CDispatchInfo::t_CmdBufferList::iterator it; nMax-- && m_di.Ptr()->lstCmd.Next( it ); )
        {
            // Ensure valid command
            if ( it.Obj().Ptr() )
            {
                // Ensure it's time
                oexUINT uTime = it.Obj().Ptr()->uTime;
                if ( !uTime || uCurrentTime > uTime )
                {
                    // Execute the command
                    Execute( it.Obj().Ptr()->sCmd, &it.Obj().Ptr()->sReturn );

                    // Signal that reply is ready
                    it.Obj().Ptr()->evReady.Set();

                    // Count commands
                    uCmds++;

                    // Erase the command
                    it = m_di.Ptr()->lstCmd.Erase( it );

                } // end if

                // Waiting on this command
                else
                    uWaits++;

            } // end if

            // Drop this command
            else
                it = m_di.Ptr()->lstCmd.Erase( it );

        } // end for

        { // Copy commands from pre-stage buffer

            CTlLocalLock ll( m_di.Ptr()->lockPre );
            if ( ll.IsLocked() )
            {
                // Reset event if no more commands
                if ( !uWaits && !m_di.Ptr()->lstCmd.Size() && !m_di.Ptr()->lstPre.Size() )
                    m_di.Ptr()->evCmd.Reset();

                // Just to make sure
                else
                    m_di.Ptr()->evCmd.Set();

            } // end if

        } // end copy commands

        return uCmds;
    }

    /// Executes the specified function call
    oexBOOL Execute( CStr x_sCmd, CStr *x_pReturn = oexNULL )
    {
        // Ensure valid data object
        if ( !IsValid() )
            return oexFALSE;

        CTlLocalLock ll( m_di.Ptr()->lockCmd );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Deserialize the command
        CPropertyBag pb = CParser::Deserialize( x_sCmd );

        if ( !pb[ oexT( "f" ) ].IsSet() )
            return oexFALSE;

        CDispatchInfo::t_EventHandlerList::iterator it = 
            m_di.Ptr()->lstEventHandlers.Find( pb[ oexT( "f" ) ].ToString() );

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

    /// Returns a pointer to the command waiting event
    CTlEvent* GetCmdEvent()
    {   
        if ( !IsValid() )
            return oexNULL;
        
        return &m_di.Ptr()->evCmd; 
    }

    /// Returns non-zero if object is valid
    oexBOOL IsValid()
    {   return m_di.Size() ? oexTRUE : oexFALSE; }

    /// Returns the ref count on the underlying object
    oexUINT GetRefCount()
    {   return m_di.GetRefCount(); }
    
    /// Returns non-zero if there is at least one other shared object
    oexBOOL IsConnected()
    {   return 1 < m_di.GetRefCount(); }

private:

    /// Shared dispatch information
    TMem< CDispatchInfo >       m_di;

};


