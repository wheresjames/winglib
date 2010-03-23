/*------------------------------------------------------------------
// msg_com.h
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

/// Message communication port
class CMsgCom
{
public:

    /// Event handler list type
    typedef TAssoList< oexGUID, CMsgTarget >    t_MsgHandlerList;

    /// Message queue
    typedef TList< CMsg >                       t_MsgQueue;

public:

    /// Default constructor
    CMsgCom();

    /// Destructor
    virtual ~CMsgCom();

    /// Returns the object id
    oexGUID& msgId()
    {   return _m_guidId; }

    /// Registers a callback function
    oexBOOL msgRegister( oexGUID &x_rGuid, oexCONST CMsgTarget &x_adFunction );

    /// Registers a communication port
    oexBOOL msgUnregister( oexGUID &x_rGuid );

    /// Routes the message to a remote or local destination
    CMsg msgSend( CMsg &x_rMsg );

    /// Routes the message to a local destination
    oexBOOL msgRecv( CMsg &x_rMsg );
/*
    operator CMsgAddress&()
    {

    }
*/
protected:

    /// Processes the function queue
    /**
        \param [in] uMax    -   Maximum number of commands to execute,
                                zero for all.

        \return Number of commands executed.
    */
    oexUINT msgProcessQueue( oexINT nMax = 0 );

    CMsg msgExecute( CMsg &x_rMsg );

    /// Wait for message
    oexBOOL msgWaitMsg( oexUINT x_uTimeout )
    {   return _m_evMsgWaiting.Wait( x_uTimeout ); }

    /// Returns the message event
    CTlEvent& msgGetMsgEvent()
    {   return _m_evMsgWaiting; }

private:

    /// Thread lock
    CTlLock                     _m_lockMsgHandlers;

    /// Thread lock
    CTlLock                     _m_lockMsgQueue;

    /// Our address
    oexGUID                     _m_guidId;

    /// List of event handlers
    t_MsgHandlerList            _m_lstMsgHandlers;

    /// Message queue
    t_MsgQueue                  _m_lstMsgQueue;

    /// Message queue status event
    CTlEvent                    _m_evMsgWaiting;
};


