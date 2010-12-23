/*------------------------------------------------------------------
// sq_socket.cpp
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

#include "../stdafx.h"

using namespace sqbind;

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqSockAddress, CSqSockAddress )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSockAddress, GetId )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSockAddress, SetId )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSockAddress, SetDotAddress )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSockAddress, SetRawAddress )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSockAddress, ValidateAddress )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSockAddress, LookupUrl )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSockAddress, LookupHost )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSockAddress, getPort )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSockAddress, getDotAddress )

SQBIND_REGISTER_CLASS_END()

void CSqSockAddress::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqSockAddress );
}

sqbind::stdString CSqSockAddress::GetId()
{_STT();
	return m_address.GetId().Ptr();
}

void CSqSockAddress::SetId( const sqbind::stdString &sId )
{_STT();
	m_address.SetId( sId.c_str() );
}

int CSqSockAddress::SetDotAddress( const sqbind::stdString &sDot, int nPort )
{_STT();
	return m_address.SetDotAddress( sDot.c_str(), nPort );
}

int CSqSockAddress::SetRawAddress( int nAddr, int nPort )
{_STT();
	return m_address.SetRawAddress( nAddr, nPort );
}

int CSqSockAddress::ValidateAddress()
{_STT();
	return m_address.ValidateAddress();
}

int CSqSockAddress::LookupUrl( const sqbind::stdString &sUrl, int nPort )
{_STT();
	return m_address.LookupUrl( sUrl.c_str(), nPort );
}

int CSqSockAddress::LookupHost( const sqbind::stdString &sHost, int nPort )
{_STT();
	return m_address.LookupHost( sHost.c_str(), nPort );
}

int CSqSockAddress::getPort()
{_STT();
	return m_address.GetPort();
}

sqbind::stdString CSqSockAddress::getDotAddress()
{_STT();
	return m_address.GetDotAddress().Ptr();
}

SQBIND_REGISTER_CLASS_BEGIN( sqbind::CSqSocket, CSqSocket )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, Destroy )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, Connect )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, CreateUDP )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, setBlockingMode )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, Bind )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, Listen )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, Accept )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, Share )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, WaitEvent )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, Read )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, Write )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, ReadFrom )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, SendTo )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, ReadBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, WriteBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, ReadFromBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, SendToBin )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, getPeerAddress )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, setPeerAddress )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, getLocalAddress )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, setLocalAddress )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, getLastError )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, getLastErrorValue )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, getNumReads )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, getNumWrites )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, getNumAccepts )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, getConnectState )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, isRunning )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, isConnected )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, isConnecting )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, isActivity )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, setScript )

	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, htonl )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, ntohl )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, htons )
	SQBIND_MEMBER_FUNCTION(  sqbind::CSqSocket, ntohs )

	SQBIND_ENUM( oex::os::CIpSocket::eReadEvent,		EVT_READ )
	SQBIND_ENUM( oex::os::CIpSocket::eWriteEvent,		EVT_WRITE )
	SQBIND_ENUM( oex::os::CIpSocket::eConnectEvent,		EVT_CONNECT )
	SQBIND_ENUM( oex::os::CIpSocket::eCloseEvent,		EVT_CLOSE )
	SQBIND_ENUM( oex::os::CIpSocket::eAcceptEvent,		EVT_ACCEPT )

SQBIND_REGISTER_CLASS_END()

void CSqSocket::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CSqSocket );
}

void CSqSocket::Destroy()
{_STT();
	m_socket->Destroy();
}

int CSqSocket::CreateUDP()
{_STT();
	return m_socket->Create( oex::os::CIpSocket::eAfInet,
							 oex::os::CIpSocket::eTypeDgram,
							 oex::os::CIpSocket::eProtoUdp );
}

int CSqSocket::setBlockingMode( int nEnable )
{_STT();

	if ( nEnable )
		m_socket->CloseEventHandle();
	else
		m_socket->EventSelect();

	return 1;
}

int CSqSocket::Connect( const sqbind::stdString &sUrl, int nPort )
{_STT();
	return m_socket->Connect( sUrl.c_str(), nPort ) ? 1 : 0;
}

int CSqSocket::Bind( int nPort )
{_STT();
	return m_socket->Bind( nPort );
}

int CSqSocket::Listen( int nMax )
{_STT();
	return m_socket->Listen( nMax );
}

int CSqSocket::Accept( CSqSocket *pSocket )
{_STT();
	if ( !pSocket )
		return 0;

	return m_socket->Accept( *pSocket->Ptr() );
}

int CSqSocket::Share( CSqSocket *pSocket )
{_STT();
	if ( !pSocket || !pSocket->m_socket.Ptr() )
		return 0;

	m_socket.Share( pSocket->m_socket );

	return 1;
}


int CSqSocket::setScript( sqbind::CSqMsgQueue *pMsgQueue, const sqbind::stdString &sId, const sqbind::stdString &sScript, const sqbind::stdString &sFunction )
{_STT();
	// Sanity checks
	if ( !pMsgQueue || !sScript.length() || !sFunction.length() )
		return 0;

	sqbind::stdString sReply;

	// Create the script object
	pMsgQueue->spawn( &sReply, oexT( "." ), sId.c_str(), sScript, 1 );

	// Get queue pointer
	sqbind::CSqMsgQueue *q = pMsgQueue->GetQueue( sId.c_str() );
	if ( !q )
	{	pMsgQueue->kill( oexNULL, sId.c_str() );
		oexERROR( 0, oexMks( oexT( "Failed to spawn " ), sId.c_str() ) );
		return 0;
	} // end if

	{ // Scope

		// Call into child
		oexAutoLock ll( q->GetLock() );
		if ( !ll.IsLocked() )
			return 0;

		// Get a pointer to the engine
		sqbind::CSqEngine *pEngine = q->GetEngine();
		if ( !pEngine )
			return 0;

		// Make the call
		if ( !pEngine->Execute( &sReply, sFunction.c_str(), this ) )
			return 0;

	} // end scope

	return 1;
}


int CSqSocket::getConnectState()
{_STT();
	return m_socket->GetConnectState();
}

int CSqSocket::isRunning( int nCheckActivity )
{_STT();
	return m_socket->IsRunning( nCheckActivity );
}

int CSqSocket::isConnecting()
{_STT();
	return m_socket->IsConnecting();
}

int CSqSocket::isConnected()
{_STT();
	return m_socket->IsConnected();
}

int CSqSocket::isActivity()
{_STT();
	return m_socket->IsActivity();
}

int CSqSocket::getPeerAddress( CSqSockAddress *pAddr )
{_STT();
	if ( !pAddr )
		return 0;

	pAddr->Obj() = m_socket->PeerAddress();

	return 1;
}

int CSqSocket::getLocalAddress( CSqSockAddress *pAddr )
{_STT();
	if ( !pAddr )
		return 0;

	pAddr->Obj() = m_socket->LocalAddress();

	return 1;
}

int CSqSocket::setPeerAddress( CSqSockAddress *pAddr )
{_STT();
	if ( !pAddr )
		return 0;

	m_socket->PeerAddress() = pAddr->Obj();

	return 1;
}

int CSqSocket::setLocalAddress( CSqSockAddress *pAddr )
{_STT();
	if ( !pAddr )
		return 0;

	m_socket->LocalAddress() = pAddr->Obj();

	return 1;
}

int CSqSocket::WaitEvent( int nEvent, int nMax )
{_STT();
	return m_socket->WaitEvent( nEvent, nMax );
}

sqbind::stdString CSqSocket::getLastError()
{_STT();
	return m_socket->GetLastErrorMsg().Ptr();
}

int CSqSocket::getLastErrorValue()
{_STT();
	return m_socket->GetLastError();
}

sqbind::stdString CSqSocket::Read( int nMax )
{_STT();
	oex::CStr s = oexMbToStr( m_socket->Recv( nMax ) );
	return sqbind::stdString( s.Ptr(), s.Length() );

}

int CSqSocket::Write( const sqbind::stdString &s, int nMax )
{_STT();
	return m_socket->Send( oexStrToMb( oex::CStr( s.c_str(), s.length() ) ) );
}

sqbind::stdString CSqSocket::ReadFrom( int nMax )
{_STT();
	oex::CStr s = oexMbToStr( m_socket->RecvFrom( nMax ) );
	return sqbind::stdString( s.Ptr(), s.Length() );
}

int CSqSocket::SendTo( const sqbind::stdString &s, int nMax )
{_STT();
	return m_socket->SendTo( oexStrToMb( oex::CStr( s.c_str(), nMax ) ) ); 
}

int CSqSocket::ReadBin( sqbind::CSqBinary *pBin, int nMax )
{_STT();
	if ( !pBin )
		return 0;
	*pBin = m_socket->ReadBin( nMax );
	return pBin->getUsed();
}

int CSqSocket::WriteBin( sqbind::CSqBinary *pBin, int nMax )
{_STT();
	if ( !pBin )
		return 0;
	return m_socket->WriteBin( pBin->Mem(), nMax );
}

int CSqSocket::ReadFromBin( sqbind::CSqBinary *pBin, int nMax )
{_STT();
	if ( !pBin )
		return 0;
	*pBin = m_socket->ReadFromBin( nMax );
	return pBin->getUsed();
}

int CSqSocket::SendToBin( sqbind::CSqBinary *pBin, int nMax )
{_STT();
	if ( !pBin )
		return 0;
	return m_socket->SendToBin( pBin->Mem(), nMax );
}

