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

#include "../oexlib.h"

OEX_USING_NAMESPACE

CAutoSocket::CAutoSocket()
{
    // Register user callable functions
    OexRpcRegister( CAutoSocket, Connect );
    OexRpcRegister( CAutoSocket, Shutdown );
    OexRpcRegister( CAutoSocket, Bind );
    OexRpcRegister( CAutoSocket, Listen );

    // Socket is not connected
    m_uStatus = eStatusDisconnected;
}

CAutoSocket::~CAutoSocket()
{
    // Kill off the socket
    m_is.Destroy();
}

oexBOOL CAutoSocket::Connect( oexCSTR x_pAddress, oexUINT x_uPort )
{
    // Open the socket
    if ( !m_is.Connect( x_pAddress, x_uPort ) )
        return oexFALSE;

    // Register socket event callback function
//    oexON_EVENT( m_is.GetEventHandle(), CAutoSocket, OnSocketEvent );

    return oexTRUE;
}

oexBOOL CAutoSocket::Shutdown()
{    return m_is.Shutdown();
}

oexBOOL CAutoSocket::Bind( oexUINT x_uPort )
{   return m_is.Bind( x_uPort );
}

oexBOOL CAutoSocket::Listen( oexUINT x_uMax )
{   return m_is.Listen( x_uMax );
}



/*

        m_is.Connect( pb[ "addr" ].ToString().Ptr(), pb[ "port" ].ToLong() );

    else if ( pb[ "cmd" ].ToLong() == eCmdClose )
    {
        oexUINT uDelay = DelayClose();
        if ( uDelay )
            m_uCloseDelay = uDelay + os::CHqTimer::GetBootCount();
        else if ( pb[ "delay" ].ToLong() )
            m_uCloseDelay = pb[ "delay" ].ToLong() + os::CHqTimer::GetBootCount();
        else m_is.Shutdown();
    } // end else if

    else if ( pb[ "cmd" ].ToLong() == eCmdBind )
        m_is.Bind( pb[ "port" ].ToLong() );

    else if ( pb[ "cmd" ].ToLong() == eCmdListen )
    {   m_is.Listen( pb[ "max" ].ToLong() );
    os::CSys::Sleep( 250 );
        m_evListen.Set();
    } // end else if

    else if ( pb[ "cmd" ].ToLong() == eCmdMsg )
        OnSocketMessage( pb[ "msg" ].ToULong() );


oexBOOL CAutoSocket::InitThread( oexPVOID x_pData )
{
    m_uCloseDelay = 0;

    return oexTRUE;
}

oexBOOL CAutoSocket::DoThread( oexPVOID x_pData )
{
    os::CSys::t_WAITABLE phEvents[ 4 ];

    // Get events
    phEvents[ 0 ] = m_evQuit.GetHandle();
	phEvents[ 1 ] = m_fifo.GetDataReadyHandle();
    phEvents[ 2 ] = m_is.GetEventHandle();
    phEvents[ 3 ] = IsConnected() ? GetDataReadyHandle() : oexNULL;

    // How many valid handles
    oexUINT uHandles = !phEvents[ 2 ] ? 2 : !phEvents[ 3 ] ? 3 : 4;

    oexUINT uWait = os::CSys::vInfinite();;
    if ( m_uCloseDelay )
    {   oexUINT uTick = os::CHqTimer::GetBootCount();
        if ( uTick >= m_uCloseDelay )
        {   oexUINT uDelay = DelayClose();
            if ( !uDelay )
                m_is.Shutdown();
            else m_uCloseDelay = uDelay + os::CHqTimer::GetBootCount();
        } // end if
        else uWait = m_uCloseDelay - uTick;
    } // end if

    // Wait for something to happen
    oexINT nRet = os::CSys::WaitForMultipleObjects( uHandles, phEvents, oexFALSE, uWait );

    // Stop command?
    if ( 0 == nRet ) 
    {   NotifyShutdown();
        return oexFALSE;
    } // end if

    // Command?
    else if ( 1 == nRet )

        // Process the command
        OnCommand();

    // Write data waiting to go out?
    else if ( 2 == nRet )

        // Process socket event
        OnSocketEvent();

    // Write data waiting to go out?
    else if ( 3 == nRet )

        // Process socket event
        if ( IsConnected() ) 
            OnDataReady();

    return oexTRUE;
}

oexINT CAutoSocket::EndThread( oexPVOID x_pData )
{
    m_is.Destroy();

    m_evListen.Reset();
    m_evConnected.Reset();

    return oexTRUE;
}
*/

oexINT CAutoSocket::OnSocketEvent()
{
    // Must have a socket handle
    if ( !m_is.IsSocket() ) 
        return -1;

    // Dispatch events
    oexUINT uEventMask;
    while ( uEventMask = m_is.WaitEvent( ~0, 0 ) )
    {
        // !!! These messages are sent in a purposeful order

        // Ensure connect messages are first
        if ( 0 != ( uEventMask & os::CIpSocket::eConnectEvent ) )
        {   if ( m_is.GetLastError() ) m_uStatus = eStatusDisconnected;
            else m_uStatus = eStatusConnected; //, m_evConnected.Set();
            OnConnect( m_is.GetLastError() );
        } // end if

        if ( 0 != ( uEventMask & os::CIpSocket::eAcceptEvent ) )
        {	m_uStatus = eStatusConnected;
            OnAccept( m_is.GetLastError() );
        } // end if
        
        if ( 0 != ( uEventMask & os::CIpSocket::eOobEvent ) )
        {	m_uStatus = eStatusConnected;
            OnOOB( m_is.GetLastError() );
        } // end if

        if ( 0 != ( uEventMask & os::CIpSocket::eQosEvent ) )
        {	m_uStatus = eStatusConnected;
            OnQOS( m_is.GetLastError() );
        } // end if

        if ( 0 != ( uEventMask & os::CIpSocket::eGroupQosEvent ) )
        {	m_uStatus = eStatusConnected;
            OnGroupQOS( m_is.GetLastError() );
        } // end if

        if ( 0 != ( uEventMask & os::CIpSocket::eRoutingInterfaceChangeEvent ) )
        {	m_uStatus = eStatusConnected;
            OnRoutingInterfaceChange( m_is.GetLastError() );
        } // end if

        if ( 0 != ( uEventMask & os::CIpSocket::eWriteEvent ) )
        {	m_uStatus = eStatusConnected;
            OnWrite( m_is.GetLastError() );
        } // end if

        if ( 0 != ( uEventMask & os::CIpSocket::eReadEvent ) )
        {	m_uStatus = eStatusConnected;
            OnRead( m_is.GetLastError() );
        } // end if

        // Ensure close messages are last
        if ( 0 != ( uEventMask & os::CIpSocket::eCloseEvent ) )
        {	m_uStatus = eStatusDisconnected;
            if ( !m_is.GetLastError() ) OnRead( 0 );
            OnClose( m_is.GetLastError() );
//            DestroyObject();
//            NotifyShutdown();
        } // end if

    } // end while

    return oexRES_OK;
}

/*
oexBOOL CAutoSocket::OnCommand()
{	
    CPropertyBag pb = CParser::Deserialize( m_fifo.Read() );
    oexASSERT( pb.Size() );

    if ( pb[ "cmd" ].ToLong() == eCmdOpen )
        m_is.Connect( pb[ "addr" ].ToString().Ptr(), pb[ "port" ].ToLong() );

    else if ( pb[ "cmd" ].ToLong() == eCmdClose )
    {
        oexUINT uDelay = DelayClose();
        if ( uDelay )
            m_uCloseDelay = uDelay + os::CHqTimer::GetBootCount();
        else if ( pb[ "delay" ].ToLong() )
            m_uCloseDelay = pb[ "delay" ].ToLong() + os::CHqTimer::GetBootCount();
        else m_is.Shutdown();
    } // end else if

    else if ( pb[ "cmd" ].ToLong() == eCmdBind )
        m_is.Bind( pb[ "port" ].ToLong() );

    else if ( pb[ "cmd" ].ToLong() == eCmdListen )
    {   m_is.Listen( pb[ "max" ].ToLong() );
    os::CSys::Sleep( 250 );
        m_evListen.Set();
    } // end else if

    else if ( pb[ "cmd" ].ToLong() == eCmdMsg )
        OnSocketMessage( pb[ "msg" ].ToULong() );

    return oexTRUE;
}

oexBOOL CAutoSocket::Connect( oexCSTR pIpAddress, oexUINT uPort )
{
    if ( !pIpAddress || !*pIpAddress ) 
        return oexFALSE;

    // Lock the fifo buffer
    CTlLocalLock ll( m_fifo );
    if ( !ll.IsLocked() ) 
        return oexFALSE;

    CPropertyBag pb;
    pb[ "cmd" ] = eCmdOpen;
    pb[ "port" ] = uPort;
    pb[ "addr" ] = pIpAddress;

    if ( !m_fifo.Write( CParser::Serialize( pb ) ) )
        return oexFALSE;

    // Start the thread if needed
    if ( !IsRunning() ) Start();

    return oexTRUE;
}

oexBOOL CAutoSocket::Close( oexUINT x_uDelay )
{
    if ( !IsRunning() ) 
        return oexFALSE;

    // Lock the fifo buffer
    CTlLocalLock ll( m_fifo );
    if ( !ll.IsLocked() ) 
        return oexFALSE;

    CPropertyBag pb;
    pb[ "cmd" ] = eCmdClose;
    pb[ "delay" ] = x_uDelay;

    return m_fifo.Write( CParser::Serialize( pb ) );
}

oexBOOL CAutoSocket::Bind( oexUINT x_uPort )
{
    // Lock the fifo buffer
    CTlLocalLock ll( m_fifo );
    if ( !ll.IsLocked() ) 
        return oexFALSE;

    CPropertyBag pb;
    pb[ "cmd" ] = eCmdBind;
    pb[ "port" ] = x_uPort;

    if ( !m_fifo.Write( CParser::Serialize( pb ) ) )
        return oexFALSE;

    // Start the thread if needed
    if ( !IsRunning() ) Start();

    return oexTRUE;
}

oexBOOL CAutoSocket::Listen( oexUINT x_uMax )
{
    // Lock the fifo buffer
    CTlLocalLock ll( m_fifo );
    if ( !ll.IsLocked() ) 
        return oexFALSE;

    CPropertyBag pb;
    pb[ "cmd" ] = eCmdListen;
    pb[ "max" ] = x_uMax;

    if ( !m_fifo.Write( CParser::Serialize( pb ) ) )
        return oexFALSE;

    // Start the thread if needed
    if ( !IsRunning() ) Start();

    return oexTRUE;
}

oexBOOL CAutoSocket::SocketMessage( oexUINT uMsg )
{
    // Lock the fifo buffer
    CTlLocalLock ll( m_fifo );
    if ( !ll.IsLocked() ) 
        return oexFALSE;

    CPropertyBag pb;
    pb[ "cmd" ] = eCmdMsg;
    pb[ "msg" ] = uMsg;

    if ( !m_fifo.Write( CParser::Serialize( pb ) ) )
        return oexFALSE;

    return oexTRUE;
}

*/
