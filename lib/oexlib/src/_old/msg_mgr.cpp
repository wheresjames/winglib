/*------------------------------------------------------------------
// msg_mgr.cpp
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

#include "../oexlib.h"
OEX_USING_NAMESPACE


CMsgMgr CMsgMgr::m_cMsgMgr;

oexBOOL CMsgMgr::Destroy()
{
    // Lock the handler list
    CTlLocalLock ll( m_lock );
    if ( !ll.IsLocked() )
        return oexFALSE;

    // Lose the lists
    m_lstMsgCom.Destroy();
    m_lstReply.Destroy();

    return oexTRUE;
}

oexBOOL CMsgMgr::Register( CMsgCom *x_pMc )
{
    if ( !oexCHECK_PTR( x_pMc ) )
        return oexFALSE;

    CTlLocalLock ll( m_lock );
    if ( !ll.IsLocked() )
        return oexFALSE;

    // Save address
    m_lstMsgCom[ x_pMc->msgId() ] = x_pMc;

    return oexTRUE;
}

oexBOOL CMsgMgr::Unregister( CMsgCom *x_pMc )
{
    if ( !oexCHECK_PTR( x_pMc ) )
        return oexFALSE;

    CTlLocalLock ll( m_lock );
    if ( !ll.IsLocked() )
        return oexFALSE;

    // Erase address
    m_lstMsgCom.Unset( x_pMc->msgId() );

    return oexTRUE;
}

oexBOOL CMsgMgr::Unregister( oexGUID &x_rGuid )
{
    CTlLocalLock ll( m_lock );
    if ( !ll.IsLocked() )
        return oexFALSE;

    // Erase address
    m_lstMsgCom.Unset( x_rGuid );

    return oexTRUE;
}

CMsg CMsgMgr::Send( CMsg &x_rMsg )
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

oexBOOL CMsgMgr::Recv( CMsg &x_rMsg )
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
    t_MsgComList::iterator it = m_lstMsgCom.Find( x_rMsg.Mi().Dst()->GetInstance() );
    if ( !it.IsValid() )
        return oexFALSE;

    // See if we have a destination for this address
    CMsgCom *pMc = it.Obj();            
    if ( !oexCHECK_PTR( pMc ) )
        return oexFALSE;

    // Hand down
    return pMc->msgRecv( x_rMsg );
}

oexBOOL CMsgMgr::OnRouteProcess( CMsg &x_rMsg )
{
    // Is this packet bound for another machine?
    if ( !guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetNetwork() ) )
        return OnRouteNetwork( x_rMsg );

    return oexFALSE;
}

oexBOOL CMsgMgr::OnRouteNetwork( CMsg &x_rMsg )
{

    return oexFALSE;
}

oexBOOL CMsgMgr::OnReply( CMsg &x_rMsg )
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

oexBOOL CMsgMgr::RegisterReply( CMsg &x_rMsg )
{
    // Ensure valid message
    if ( !x_rMsg.IsValid() )
        return oexFALSE;

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

oexBOOL CMsgMgr::CancelReply( oexGUID &guidId )
{
    CTlLocalLock ll( m_lock );
    if ( !ll.IsLocked() )
        return oexFALSE;

    // Erase reply object
    m_lstReply.Unset( guidId );

    return oexTRUE;
}

oexBOOL CMsgMgr::SignalReply( oexGUID &guidId )
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
