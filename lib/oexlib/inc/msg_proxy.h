/*------------------------------------------------------------------
// msg_proxy.h
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

/// Creates a guid by hashing the parameter
#define msgCreateMsgId( s, g )                  OEX_NAMESPACE::oss::CMd5::Transform( g, OEX_NAMESPACE::obj::Ptr( s ), OEX_NAMESPACE::obj::Size( s ) )
#define msgCreateTempMsgId( s )                 OEX_NAMESPACE::oss::CMd5::Transform( &OEX_NAMESPACE::oexGUID(), OEX_NAMESPACE::obj::Ptr( s ), OEX_NAMESPACE::obj::Size( s ) )

/// Registers a class pointer and function 
#define msgRegisterFunction( p, c, f )          msgRegister( *msgCreateTempMsgId( oexT( #f ) ), \
                                                             OEX_NAMESPACE::CMsgTarget( (const c*)p, &c::f ) );

/// Registers a function using the 'this' pointer
#define msgRegisterThisFunction( c, f )         msgRegisterFunction( this, c, f )

/// Use to create a CMsg object
#define msgCreate                               OEX_NAMESPACE::CMsg::Msg

/// Creates a CMsgAddress address object
#define msgTo                                   &OEX_NAMESPACE::CMsgAddress

/// Object manager
#define msgOrb                                  oex::CMsgObjectMgr::Mm()

/// 
class CMsgObjectInfo
{
public:

    /// Event handler list type
    typedef TAssoList< oexGUID, CMsgTarget >    t_MsgHandlerList;

    /// Message queue
    typedef TList< CMsg >                       t_MsgQueue;

public:

    /// Default constructor
    CMsgObjectInfo()
    {
    }

    /// Destructor
    virtual ~CMsgObjectInfo()
    {
    }

    /// Returns the object id
    oexGUID& msgId()
    {   return m_guidId; 
    }

    oexBOOL Register( oexGUID &x_rGuid, oexCONST CMsgTarget &x_adFunction )
    {
        // Lock the command buffer
        CTlLocalLock ll( m_lockMsgHandlers );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Add to the list
        m_lstMsgHandlers[ x_rGuid ] = x_adFunction;

        return oexTRUE;
    }

    oexBOOL Unregister( oexGUID &x_rGuid )
    {
        // Lock the command buffer
        CTlLocalLock ll( m_lockMsgHandlers );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Erase address
        m_lstMsgHandlers.Unset( x_rGuid );

        return oexTRUE;
    }

    /// Routes the message to a remote or local destination
    CMsg Send( CMsg &x_rMsg );

    /// Routes the message to a local destination
    oexBOOL Recv( CMsg &x_rMsg );

    /// Processes the function queue
    /**
        \param [in] uMax    -   Maximum number of commands to execute,
                                zero for all.

        \return Number of commands executed.
    */
    oexUINT ProcessQueue( oexINT nMax = 0 );

    /// Executes a message
    CMsg Execute( CMsg &x_rMsg );

public:

    /// Thread lock
    CTlLock                     m_lockMsgHandlers;

    /// Thread lock
    CTlLock                     m_lockMsgQueue;

    /// Our address
    oexGUID                     m_guidId;

    /// List of event handlers
    t_MsgHandlerList            m_lstMsgHandlers;

    /// Message queue
    t_MsgQueue                  m_lstMsgQueue;

    /// Message queue status event
    CTlEvent                    m_evMsgWaiting;
};

/// Message object
class CMsgObject
{
public:

    /// Default constructor
    CMsgObject();

    /// Destructor
    virtual ~CMsgObject();

    /// Register a function
    oexBOOL msgRegister( oexGUID &x_rGuid, oexCONST CMsgTarget &x_adFunction )
    {   return __m_lstMoi.Ptr()->Register( x_rGuid, x_adFunction ); }

    /// Unregister a function
    oexBOOL msgUnregister( oexGUID &x_rGuid )
    {   return __m_lstMoi.Ptr()->Unregister( x_rGuid ); }

    /// Unregister a function
    CMsg msgSend( CMsg &x_rMsg )
    {   return __m_lstMoi.Ptr()->Send( x_rMsg ); }

    /// Unregister a function
    oexBOOL msgRecv( CMsg &x_rMsg )
    {   return __m_lstMoi.Ptr()->Recv( x_rMsg ); }

    /// Unregister a function
    oexUINT msgProcessQueue()
    {   return __m_lstMoi.Ptr()->ProcessQueue(); }

    /// Returns message event object
    CTlEvent& msgGetEvent()
    {   return __m_lstMoi.Ptr()->m_evMsgWaiting; }

    /// Returns the object id
    oexGUID& msgId()
    {   return __m_lstMoi.Ptr()->m_guidId; }
    
private:

    /// Allow CMsgProxy access
    friend class CMsgProxy;

    /// Function registration information
    TMem< CMsgObjectInfo >      __m_lstMoi;
};


/// 
/**
*/
class CMsgProxy
{
public:

    /// Default constructor
    CMsgProxy()
    {}

    /// Copy constructor
    CMsgProxy( CMsgProxy &x_rMp )
    {   __m_lstMoi.Share( x_rMp.__m_lstMoi ); 
    }
    
    /// Constructor
    CMsgProxy( CMsgObject &x_rMo )
    {   __m_lstMoi.Share( x_rMo.__m_lstMoi ); 
    }
    
    /// Destructor
    virtual ~CMsgProxy()
    {}

private:

    /// Shared object
    TMem< CMsgObjectInfo >      __m_lstMoi;
};



/// 
/**
    
*/
class CMsgObjectMgr
{
public:

    /// Event handler list type
    typedef TAssoList< oexGUID, CMsgObject* > t_MsgObjectList;

    /// Event handler list type
    typedef TAssoList< oexGUID, CMsg >     t_MsgReplyList;

public:

    CMsgObjectMgr()
    {
    }

    virtual ~CMsgObjectMgr()
    {
    }

    oexBOOL Destroy()
    {
        // Lock the handler list
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Lose the lists
        m_lstMsgObject.Destroy();

        return oexTRUE;
    }

    oexBOOL Register( CMsgObject *x_pMc )
    {
        if ( !oexCHECK_PTR( x_pMc ) )
            return oexFALSE;

        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Save address
        m_lstMsgObject[ x_pMc->msgId() ] = x_pMc;

        return oexTRUE;
    }

    oexBOOL Unregister( CMsgObject *x_pMc )
    {
        if ( !oexCHECK_PTR( x_pMc ) )
            return oexFALSE;

        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Erase address
        m_lstMsgObject.Unset( x_pMc->msgId() );

        return oexTRUE;
    }

    oexBOOL Unregister( oexGUID &x_rGuid )
    {
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Erase address
        m_lstMsgObject.Unset( x_rGuid );

        return oexTRUE;
    }

    CMsg Send( CMsg &x_rMsg )
    {
        // Sanity check
        if ( !x_rMsg.IsValid() )
            return CMsg();

        // Can't be routed without an id
        if ( guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetId() ) )
            return CMsg();

        // Can't route without an instance id
        if ( guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetInstance() ) )
            return CMsg();

        // Register reply object if needed
        if ( x_rMsg.WantReply() )
            RegisterReply( x_rMsg );

        // Bound for somewhere else?
        if ( !guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetProcess() )
             && !guid::CmpGuid( &m_guidId, &x_rMsg.Mi().Dst()->GetProcess() ) )
        {
            // Attempt to route
            if ( !OnRouteProcess( x_rMsg ) )
            {   CancelReply( x_rMsg.Mi().Src()->GetId() );
                return CMsg();
            } // end if

        } // end if

        // Route back down
        else if ( !Recv( x_rMsg ) )
        {   CancelReply( x_rMsg.Mi().Src()->GetId() );
            return CMsg();
        } // end if

        return x_rMsg;
    }

    oexBOOL Recv( CMsg &x_rMsg )
    {
        // Sanity check
        if ( !x_rMsg.IsValid() )
            return oexFALSE;

        // Can't route without an instance id
        if ( guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetInstance() ) )
            return oexFALSE;

        // Lock the handler list
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // First, check to see if it's a reply
        if ( OnReply( x_rMsg ) )
            return oexTRUE;

        // Find a message reciever by that id
        t_MsgObjectList::iterator it = m_lstMsgObject.Find( x_rMsg.Mi().Dst()->GetInstance() );
        if ( !it.IsValid() )
            return oexFALSE;

        // See if we have a destination for this address
        CMsgObject *pMo = it.Obj();            
        if ( !oexCHECK_PTR( pMo ) )
            return oexFALSE;

        // Hand down
        return pMo->msgRecv( x_rMsg );
    }

    oexBOOL OnRouteProcess( CMsg &x_rMsg )
    {
        // Is this packet bound for another machine?
        if ( !guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetNetwork() ) )
            return OnRouteNetwork( x_rMsg );

        return oexFALSE;
    }

    oexBOOL OnRouteNetwork( CMsg &x_rMsg )
    {

        return oexFALSE;
    }

    oexBOOL OnReply( CMsg &x_rMsg )
    {
        // Lock the handler list
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Is it a reply?
        t_MsgReplyList::iterator itReply = m_lstReply.Find( x_rMsg.Mi().Dst()->GetId() );
        if ( !itReply.IsValid() )
            return oexFALSE;

        // Signal reply if anyone is listening
        if ( itReply.Obj().IsConnected() )
        {
            // Copy the data
            itReply.Obj().Mi().Deserialize( x_rMsg.Mi().Serialize() );

            // Signal that a reply was received
            itReply.Obj().SetReadyEvent( oexTRUE );

        } // end if

        // Remove the reply object
        m_lstReply.Erase( itReply );

        return oexTRUE;
    }

    oexBOOL RegisterReply( CMsg &x_rMsg )
    {
        // Ensure valid message
        if ( !x_rMsg.IsValid() )
            return oexFALSE;

        // New message object
//        x_rMsg.Create( 0 );

        // Create reply signal
        x_rMsg.EnableReplyEvent( oexTRUE );

        // Create unique id for reply
        oexGUID guid;
        x_rMsg.Mi().Src()->SetId( os::CSys::CreateGuid( &guid ) );

        // Set the instance id
        x_rMsg.Mi().Src()->SetInstance( &m_guidId );
        
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Save away the reply object
        m_lstReply[ guid ] = x_rMsg;

        return oexTRUE;
    }

    oexBOOL CancelReply( oexGUID &guidId )
    {
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Erase reply object
        m_lstReply.Unset( guidId );

        return oexTRUE;
    }

    oexBOOL SignalReply( oexGUID &guidId )
    {
        CTlLocalLock ll( m_lock );
        if ( !ll.IsLocked() )
            return oexFALSE;

        // Signal that a reply was received
        t_MsgReplyList::iterator it = m_lstReply.Find( guidId );
        if ( !it.IsValid() )
            return oexFALSE;

        // Signal that a reply was received
        it.Obj().Mi().SetReadyEvent( oexTRUE );

        // Erase reply object
        m_lstReply.Erase( it );

        return oexTRUE;
    }

private:

    /// Thread lock
    CTlLock                     m_lock;

    /// Our address
    oexGUID                     m_guidId;

    /// List of event handlers
    t_MsgObjectList             m_lstMsgObject;

    /// Reply objects
    t_MsgReplyList              m_lstReply;

public:

    /// Returns the static instance
    static CMsgObjectMgr& Mm()
    {   return m_cMsgObjectMgr; }

private:

    /// The single instance
    static CMsgObjectMgr        m_cMsgObjectMgr;
};

