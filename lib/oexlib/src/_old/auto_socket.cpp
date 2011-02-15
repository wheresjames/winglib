/*------------------------------------------------------------------
// auto_socket.cpp
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

#include "oexlib.h"

OEX_USING_NAMESPACE

CAutoSocket::CAutoSocket()
{
    // Socket is not connected
    m_uStatus = eStatusDisconnected;
}

CAutoSocket::~CAutoSocket()
{
    Destroy();
}

void CAutoSocket::Destroy()
{
    // Kill off the socket
    CIpSocket::Destroy();
}

oexINT CAutoSocket::OnSocketEvent()
{
    // Must have a socket handle
    if ( !IsSocket() ) 
        return -1;

    // Dispatch events
    oexUINT uEventMask;
    while ( uEventMask = WaitEvent( ~0, 0 ) )
    {
        // !!! These messages are sent in a purposeful order

        // Ensure connect messages are first
        if ( 0 != ( uEventMask & os::CIpSocket::eConnectEvent ) )
        {   if ( GetLastError() ) m_uStatus = eStatusDisconnected;
            else m_uStatus = eStatusConnected; //, m_evConnected.Set();
            OnConnect( GetLastError() );
        } // end if

        if ( 0 != ( uEventMask & os::CIpSocket::eAcceptEvent ) )
        {	m_uStatus = eStatusConnected;
            OnAccept( GetLastError() );
        } // end if
        
        if ( 0 != ( uEventMask & os::CIpSocket::eOobEvent ) )
        {	m_uStatus = eStatusConnected;
            OnOOB( GetLastError() );
        } // end if

        if ( 0 != ( uEventMask & os::CIpSocket::eQosEvent ) )
        {	m_uStatus = eStatusConnected;
            OnQOS( GetLastError() );
        } // end if

        if ( 0 != ( uEventMask & os::CIpSocket::eGroupQosEvent ) )
        {	m_uStatus = eStatusConnected;
            OnGroupQOS( GetLastError() );
        } // end if

        if ( 0 != ( uEventMask & os::CIpSocket::eRoutingInterfaceChangeEvent ) )
        {	m_uStatus = eStatusConnected;
            OnRoutingInterfaceChange( GetLastError() );
        } // end if

        if ( 0 != ( uEventMask & os::CIpSocket::eWriteEvent ) )
        {	m_uStatus = eStatusConnected;
            OnWrite( GetLastError() );
        } // end if

        if ( 0 != ( uEventMask & os::CIpSocket::eReadEvent ) )
        {	m_uStatus = eStatusConnected;
            OnRead( GetLastError() );
        } // end if

        // Ensure close messages are last
        if ( 0 != ( uEventMask & os::CIpSocket::eCloseEvent ) )
        {	m_uStatus = eStatusDisconnected;
            if ( !GetLastError() ) OnRead( 0 );
            OnClose( GetLastError() );
        } // end if

    } // end while

    return oexRES_OK;
}

