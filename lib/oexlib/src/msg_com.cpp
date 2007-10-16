/*------------------------------------------------------------------
// msg_com.cpp
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

CMsgCom::CMsgCom()
{
    os::CSys::CreateGuid( &_m_guidId );

    CMsgMgr::Mm().Register( this );
}

/// Destructor
CMsgCom::~CMsgCom()
{
    CMsgMgr::Mm().Unregister( this );
}


oexBOOL CMsgCom::msgRegister( oexGUID &x_rGuid, oexCONST CMsgTarget &x_adFunction )
{
    // Lock the command buffer
    CTlLocalLock ll( _m_lockMsgHandlers );
    if ( !ll.IsLocked() )
        return oexFALSE;

    // Add to the list
    _m_lstMsgHandlers[ x_rGuid ] = x_adFunction;

    return oexTRUE;
}

oexBOOL CMsgCom::msgUnregister( oexGUID &x_rGuid )
{
    // Lock the command buffer
    CTlLocalLock ll( _m_lockMsgHandlers );
    if ( !ll.IsLocked() )
        return oexFALSE;

    // Erase address
    _m_lstMsgHandlers.Unset( x_rGuid );

    return oexTRUE;
}

/// Routes the message to a remote or local destination
CMsg CMsgCom::msgSend( CMsg &x_rMsg )
{
    // Sanity check
    if ( !x_rMsg.IsValid() )
        return CMsg();

    // Can't be routed without an id
    if ( guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetId() ) )
        return CMsg();

    // Set our id in the return address
    x_rMsg.Mi().Src()->SetInstance( &_m_guidId );

    // Is it for us?
    if ( guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetInstance() )
         || guid::CmpGuid( &_m_guidId, &x_rMsg.Mi().Dst()->GetInstance() ) )
    {   
        // Short circuit the reply queue if needed
        if ( x_rMsg.Mi().WantReply() )
        {
            // Create reply signal
            x_rMsg.EnableReplyEvent( oexTRUE );

            // Set direct reply flag
            x_rMsg.SetDirectReply( oexTRUE );

        } // end if

        // Receive the message
        msgRecv( x_rMsg );
        
        return x_rMsg;

    } // end if

    // Send through the network
    return oexNet.Send( x_rMsg );
}

oexBOOL CMsgCom::msgRecv( CMsg &x_rMsg )
{
    // Sanity check
    if ( !x_rMsg.IsValid() )
        return oexFALSE;

    // Can't route without an id
    if ( guid::CmpGuid( &IID_ZEROS, &x_rMsg.Mi().Dst()->GetId() ) )
        return oexFALSE;

    // Lock the handler list
    CTlLocalLock ll( _m_lockMsgQueue );
    if ( !ll.IsLocked() )
        return oexFALSE;

    // Grab the priority
    oexUINT uPriority = x_rMsg.GetPriority();

    // Lowest priority?
    if ( !uPriority )
        _m_lstMsgQueue.Append( x_rMsg );

    else
    {
        // Priority based insert
        oexBOOL bInserted = oexFALSE;
        for ( t_MsgQueue::iterator itInsert; 
              !bInserted && _m_lstMsgQueue.Next( itInsert ); )
        {
            // Look for a lower priority object
            if ( itInsert.Obj().GetPriority() < uPriority )
            {
                // Put the command ahead of this item
                t_MsgQueue::iterator it = _m_lstMsgQueue.Append( x_rMsg );
                
                // Did we get a valid object?
                if ( it.IsValid() )
                {
                    // Move to correct location
                    _m_lstMsgQueue.MoveBefore( it, itInsert );

                    // Check the result
                    bInserted = oexTRUE;

                } // end if

            } // end if

        } // end for

        // Just stick it on the end if we couldn't find a spot
        if ( !bInserted )
            _m_lstMsgQueue.Append( x_rMsg );

    } // end else

    // Set event if message is in the queue
    if ( _m_lstMsgQueue.Size() )
        _m_evMsgWaiting.Set();

    // Hand down
    return oexTRUE;
}

oexUINT CMsgCom::msgProcessQueue( oexINT nMax )
{
    // Number of commands executed
    oexUINT uCmds = 0;

    // Count waits
    oexUINT uWaits = 0;

    // What we think the current time is
    oexCONST oexUINT uCurrentTime = CSysTime( CSysTime::eGmtTime ).GetUnixTime();
     
    oexBOOL bErase = oexFALSE;
    t_MsgQueue::iterator it;

    // Process through the messages
    do
    {           
        { // Scope, don't leave the buffer locked while we execute commands

            // Lock the command buffer
            CTlLocalLock ll( _m_lockMsgQueue );
            if ( !ll.IsLocked() )
                return 0;

            // Do we want to erase?
            if ( bErase && it.IsValid() )

                // Erase message and get the next one
                it = _m_lstMsgQueue.Erase( it );

            // Clear erase flag
            bErase = oexFALSE;

            // Grab the next message
            _m_lstMsgQueue.Next( it );

        } // end scope  

        // Did we get a command?
        if ( it.IsValid() )
        {
            // Ensure it's time
            oexUINT uTime = it.Obj().GetTime();
            if ( !uTime || uCurrentTime > uTime )
            {
                // Execute the message
                CMsg msg = msgExecute( it.Obj() );

                // Send reply if needed
                if ( it.Obj().WantReply() )
                {
                    // Is it a direct reply?
                    if ( !it.Obj().IsDirectReply() )
                        msgSend( msg );

                    // Short circuit the reply loop
                    else if ( it.Obj().IsConnected() )
                    {   it.Obj().Mi().Deserialize( msg.Serialize() );
                        it.Obj().SetReadyEvent( oexTRUE );
                    } // end else

                } // end if

                // Count commands
                uCmds++;

                // Erase this command
                bErase = oexTRUE;

            } // end if

            // Waiting on this command
            else
                uWaits++;

        } // end if

    } while( it.IsValid() && --nMax );

    // Set event
    if ( !uWaits && !_m_lstMsgQueue.Size() )
        _m_evMsgWaiting.Reset();
    else
        _m_evMsgWaiting.Set();

    return uCmds;
}

CMsg CMsgCom::msgExecute( CMsg &x_rMsg )
{
    // Lock the handler list
    CTlLocalLock ll( _m_lockMsgHandlers );
    if ( !ll.IsLocked() )
        return CMsg();

    // See if we have a destination for this address
    CMsgTarget mt = _m_lstMsgHandlers[ x_rMsg.Mi().Dst()->GetId() ];
    if ( !mt.IsValid() )
        return CMsg();

    return mt( x_rMsg );
}
