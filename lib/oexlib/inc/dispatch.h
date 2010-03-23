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

// This stuff is like CORBA in a bottle ;)

#define OexRpcRegisterPtr( p, c, f )    Register( oexT( #f ), OEX_NAMESPACE::TArbDelegate< OEX_NAMESPACE::CAutoStr >( (const c*)p, &c::f ) );
#define OexRpcRegister( c, f )          OexRpcRegisterPtr( this, c, f )

#define oexCall                         OEX_NAMESPACE::CDispatch::Call


template< typename T_DISPATCH >
    struct CMessageInfo
{
public:

    /// Default constructor
    CMessageInfo() { uTime = 0; }

    /// Constructor
    CMessageInfo( CStr x_sCmd, oexUINT x_uTime = 0 )
    {   sCmd = x_sCmd; uTime = x_uTime; }

    /// Copy constructor
    CMessageInfo( CMessageInfo &x_rRi )
    {   sCmd = x_rRi.sCmd; 
        sReturn = x_rRi.sReturn; 
        evReady = x_rRi.evReady; 
        cDispatch = x_rRi.cDispatch; 
    }

    /// Copy operator
    CMessageInfo& operator = ( CMessageInfo &x_rRi )
    {   sCmd = x_rRi.sCmd; 
        sReturn = x_rRi.sReturn; 
        evReady = x_rRi.evReady; 
        cDispatch = x_rRi.cDispatch; 
        return *this; 
    }

    /// Command id, usually describes the destination
    CStr            sMap;

    /// Command string
    CStr            sCmd;

    /// Set when the reply is ready
    CTlEvent        evReady;

    /// Return event id
    CStr            sReturn;

    /// Priority
    oexUINT         uPriority;

    /// Priority
    oexUINT         uAccess;

    /// Execution time
    oexUINT         uTime;

    /// Dispatch object
    T_DISPATCH      cDispatch;
};

/// Holds reply information
template< typename T_DISPATCH >
    class CMessageImpl
{
public:

    /// Default constructor
    CMessageImpl()
    {   m_mi.OexConstruct();
    }

    /// Reply constructor
    CMessageImpl( oexCONST TMem< CMessageInfo< T_DISPATCH > > &x_rRi )
    {
        m_mi.Share( x_rRi );
    }

    /// Reply constructor
    CMessageImpl( oexCONST CMessageImpl &x_rR )
    {
        m_mi.Share( x_rR.m_mi );
    }

    /// Destructor
    virtual ~CMessageImpl()
    {
    }

    /// Returns non-zero if the reply is ready
    CMessageImpl& Wait( oexUINT x_uTimeout )
    {
        // Wait for reply if we have an object
        if ( m_mi.Ptr() )
            m_mi.Ptr()->evReady.Wait( x_uTimeout ); 

        return *this;
    }

    /// Returns non-zero if the reply is ready
    oexBOOL IsDone()
    {   return m_mi.Ptr() && m_mi.Ptr()->evReady.Wait( 0 ); 
    }

    /// Returns the reply value
    CStr GetReply( oexCSTR x_pDef = oexT( "" ) )
    {
        if ( !IsDone() )
            return x_pDef;

        // Return the reply value
        return m_mi.Ptr()->sReturn;
    }

    /// Returns the reply value
    CStr operator *() 
    {   return GetReply(); }

    /// Returns the desired execution time
    oexUINT GetTime()
    {   if ( !m_mi.Ptr() )
            return 0;
        return m_mi.Ptr()->uTime;
    }

    /// Returns non-zerof if valid object
    oexBOOL IsValid()
    {   return m_mi.Size() ? oexTRUE : oexFALSE; }

    /// Returns the ref count on the underlying object
    oexUINT GetRefCount()
    {   return m_mi.GetRefCount(); }

    /// Returns reply dispatch pointer
    T_DISPATCH* GetDispatch()
    {   return &m_mi.Ptr()->cDispatch;
    }

    /// Returns reply info object
    CMessageInfo< T_DISPATCH >* Mi()
    {   return m_mi.Ptr(); }

private:
    
    /// Reply information
    TMem< CMessageInfo< T_DISPATCH > >      m_mi;
};

/// Holds the shared information between CDispatch objects
template< typename T_MSGINFO >
    class CDispatchInfo
{
public:

    /// Event handler list type
    typedef TAssoList< CStr, TArbDelegate< CAutoStr > > t_EventHandlerList;

    /// Command buffer
    typedef TList< T_MSGINFO > t_CmdBufferList;
    
public:

    /// Destructor
    ~CDispatchInfo()
    {   Destroy(); }

    /// Destroys the object
    void Destroy()
    {
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

    /// Command buffer
    t_CmdBufferList             lstCmd;

    /// Thread lock
    CTlLock                     lockEventHandlers;

    /// List of event handlers
    t_EventHandlerList          lstEventHandlers;

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

    /// Reply type
    typedef CMessageImpl< CDispatch >     CMessage;

public:

    /// Constructor
    CDispatch() 
    {
        // +++ Here's the deal, I really don't want to call Create in the
        //     constructor because many times, the memory won't be used.
        //     However, it's the only thread safe place to do it.  Another
        //     alternative would be to create a lock in this class to
        //     synchronized createion.  This would be better for the 
        //     objects that didn't use the memory, but worse for the
        //     ones that did.
        Create();
    }

    /// Copy constructor
    CDispatch( CDispatch &rD )
    {
        if ( !rD.IsValid() )
            rD.Create();
        
        m_di.Share( rD.m_di );
    }

    /// Copy operator
    CDispatch& operator = ( CDispatch &rD )
    {
        if ( !rD.IsValid() )
            rD.Create();

        m_di.Share( rD.m_di );

        return *this;
    }

    /// Destructor
    virtual ~CDispatch()
    {   Destroy();
    }

    /// Creates the memory object
    oexBOOL Create()
    {
        // Construct data object
        m_di.OexConstruct();

        return IsValid();
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
//            if ( !Create() )
                return oexFALSE;

        // Lock the command buffer
        CTlLocalLock ll( m_di.Ptr()->lockEventHandlers );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Add to the list
        m_di.Ptr()->lstEventHandlers[ x_sName ] = x_adFunction;

        return oexTRUE;
    }

    /// Puts a function call into the queue
    /**
        \param [in] x_pMap      -   Describes the message target, interpreted by OnMap()
        \param [in] x_sCmd      -   Actual command string, use oexCall to generate.
        \param [in] x_uPriority -   Priority level of the command.  It will be queued
                                    ahead of lower priority messages.
        \param [in] x_uAccess   -   Access level.  Must be at or above executing objects
                                    level.
        \param [in] x_uTime     -   Time the command should be executed, zero for
                                    immediate execution.        

        Returns a CMessage object
    */
    CMessage Queue( oexCSTR x_pMap, oexCONST CStr x_sCmd, oexUINT x_uPriority = 0, oexUINT x_uAccess = 0, oexUINT x_uTime = 0 )
    {
        // Ensure valid data object
        if ( !IsValid() )
//            if ( !Create() )
                return CMessage();

        // Lock the queue
        CTlLocalLock ll( m_di.Ptr()->lockCmd );
        if ( !ll.IsLocked() )
            return CMessage();

        // Append command
        CDispatchInfo< CMessage >::t_CmdBufferList::iterator it;
        
        // Lowest priority?
        if ( !x_uPriority )
            it = m_di.Ptr()->lstCmd.Append();

        else
        {
            // Priority based insert
            oexBOOL bInserted = oexFALSE;
            for ( CDispatchInfo< CMessage >::t_CmdBufferList::iterator itInsert; 
                  !bInserted && m_di.Ptr()->lstCmd.Next( itInsert ); )
            {
                // Look for a lower priority object
                if ( itInsert.Obj().Mi()->uPriority < x_uPriority )
                {
                    // Put the command ahead of this item
                    it = m_di.Ptr()->lstCmd.Append();
                    
                    // Did we get a valid object?
                    if ( it.IsValid() )
                    {
                        // Move to correct location
                        m_di.Ptr()->lstCmd.MoveBefore( it, itInsert );

                        // Check the result
                        bInserted = oexTRUE;

                    } // end if

                } // end if

            } // end for

            // Just stick it on the end if we couldn't find a spot
            if ( !bInserted )
                it = m_di.Ptr()->lstCmd.Append();

        } // end else

        if ( !it.IsValid() )
            return CMessage();

        // Set mapping information
        if ( x_pMap )
            it.Obj().Mi()->sMap = x_pMap;

        // Set command string
        it.Obj().Mi()->sCmd = x_sCmd;

        // Save time string
        it.Obj().Mi()->uPriority = x_uPriority;

        // Save time string
        it.Obj().Mi()->uAccess = x_uAccess;

        // Save time string
        it.Obj().Mi()->uTime = x_uTime;

        // Signal that a command is waiting
        m_di.Ptr()->evCmd.Set();

        // Return the reply event
        return it.Obj();
    }    

    /// Over-ride to provide message mapping or handling
    /**
        \param [in] x_pMsg   -   The message information
    */
    virtual oexBOOL OnMap( CMessage* x_pMsg )
    {   return oexFALSE; 
    }

    /// Processes the function queue
    /**
        \param [in] uMax    -   Maximum number of commands to execute,
                                zero for all.

        \return Number of commands executed.
    */
    oexUINT ProcessQueue( oexINT nMax = 0 )
    {
        // Ensure valid data object
        if ( !IsValid() )
            return 0;

        // Number of commands executed
        oexUINT uCmds = 0;

        // Count waits
        oexUINT uWaits = 0;

        // What we think the current time is
        oexCONST oexUINT uCurrentTime = CSysTime( CSysTime::eGmtTime ).GetUnixTime();
         
        oexBOOL bErase = oexFALSE;
        CDispatchInfo< CMessage >::t_CmdBufferList::iterator it;

        // Process through the messages
        do
        {           
            { // Scope, don't leave the buffer locked while we execute commands

                // Lock the command buffer
                CTlLocalLock ll( m_di.Ptr()->lockCmd );
                if ( !ll.IsLocked() )
                    return 0;

                // Do we want to erase?
                if ( bErase && it.IsValid() )

                    // Erase message and get the next one
                    it = m_di.Ptr()->lstCmd.Erase( it );

                // Clear erase flag
                bErase = oexFALSE;

                // Grab the next message
                m_di.Ptr()->lstCmd.Next( it );

            } // end scope  

            // Did we get a command?
            if ( it.IsValid() )
            {
                // Attempt to map it
                if ( OnMap( it.Ptr() ) )

                    bErase = oexTRUE;

                else
                {
                    // Ensure it's time
                    oexUINT uTime = it.Obj().Mi()->uTime;
                    if ( !uTime || uCurrentTime > uTime )
                    {
                        // Execute the command locally
                        Execute( it.Obj().Mi()->sCmd, &it.Obj().Mi()->sReturn );

                        // Signal that reply is ready
                        it.Obj().Mi()->evReady.Set();

                        // Count commands
                        uCmds++;

                        // Erase this command
                        bErase = oexTRUE;

                    } // end if

                    // Waiting on this command
                    else
                        uWaits++;

                } // end else

            } // end if

        } while( it.IsValid() && --nMax );

        // Reset command event
        if ( !uWaits && !m_di.Ptr()->lstCmd.Size() )
            m_di.Ptr()->evCmd.Reset();

        // Just to make sure
        else
            m_di.Ptr()->evCmd.Set();

        return uCmds;
    }

    /// Executes the specified function call
    oexBOOL Execute( CStr x_sCmd, CStr *x_pReturn = oexNULL )
    {
        // Ensure valid data object
        if ( !IsValid() || !x_sCmd.Length() )
            return oexFALSE;

        // Deserialize the command
        CPropertyBag pb = CParser::Deserialize( x_sCmd );

        if ( !pb.IsKey( oexT( "f" ) ) )
            return oexFALSE;

        // Event handler
        CDispatchInfo< CMessage >::t_EventHandlerList::iterator it;

        { // Scope

            // Lock the event handlers
            CTlLocalLock ll( m_di.Ptr()->lockEventHandlers );
            if ( !ll.IsLocked() )
                return oexFALSE;
            
            // Grab the event handler
            it = m_di.Ptr()->lstEventHandlers.Find( pb[ oexT( "f" ) ].ToString() );

        } // end scope

        // How did it go
        if ( !it.IsValid() )
            return oexFALSE;

        CStr ret;

        // 0 params
        if ( !pb.IsKey( oexT( "p" ) ) || !pb[ oexT( "p" ) ].IsKey( 0 ) )
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
    TMem< CDispatchInfo< CMessage > >       m_di;

};


