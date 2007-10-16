/*------------------------------------------------------------------
// msg_mgr.h
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

/// Manages the message network
class CMsgMgr
{

public:

    /// Event handler list type
    typedef TAssoList< oexGUID, CMsgCom* > t_MsgComList;

    /// Event handler list type
    typedef TAssoList< oexGUID, CMsg >     t_MsgReplyList;

public:

    /// Default constructor
    CMsgMgr()
    {
        os::CSys::CreateGuid( &m_guidId );
    }

    /// Destructor
    virtual ~CMsgMgr()
    {        
        Destroy();
    }

    /// Releases all resources
    oexBOOL Destroy();

    /// Registers a communication port
    oexBOOL Register( CMsgCom *x_pMc );

    /// Registers a communication port
    oexBOOL Unregister( CMsgCom *x_pMc );

    /// Registers a communication port
    oexBOOL Unregister( oexGUID &x_rGuid );

    /// Routes the message to a remote or local destination
    CMsg Send( CMsg &x_rMsg );

    /// Routes the message to a local destination
    oexBOOL Recv( CMsg &x_rMsg );

    /// Route to other process
    virtual oexBOOL OnRouteProcess( CMsg &x_rMsg );

    /// Route to other network
    virtual oexBOOL OnRouteNetwork( CMsg &x_rMsg );

    /// Handle reply
    virtual oexBOOL OnReply( CMsg &x_rMsg );

    /// Registers the object to be signaled when a reply is recieved
    oexBOOL RegisterReply( CMsg &x_rMsg );

    /// Cancel the reply object
    oexBOOL CancelReply( oexGUID &guidId );

    /// Call to signal that a relpy was recieved
    oexBOOL SignalReply( oexGUID &guidId );

    /// Returns the object id
    oexGUID& GetId()
    {   return m_guidId; }

public:

    /// Returns the static instance
    static CMsgMgr& Mm()
    {   return m_cMsgMgr; }

private:

    /// Thread lock
    CTlLock                     m_lock;

    /// Our address
    oexGUID                     m_guidId;

    /// List of event handlers
    t_MsgComList                m_lstMsgCom;

    /// Reply objects
    t_MsgReplyList              m_lstReply;

private:

    /// The single instance
    static CMsgMgr              m_cMsgMgr;
};

