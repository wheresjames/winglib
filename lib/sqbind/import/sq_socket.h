/*------------------------------------------------------------------
// sq_socket.h
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

// namespace
namespace sqbind
{
	class CSqSockAddress
	{
	public:
		SQBIND_CLASS_CTOR_BEGIN( CSqSockAddress )
			_SQBIND_CLASS_CTOR1( CSqSockAddress, OT_STRING ) ( sqbind::stdString( sa.GetString( 2 ), sq_getsize( x_v, 2 ) ) )
			_SQBIND_CLASS_CTOR2( CSqSockAddress, OT_STRING, OT_INTEGER ) ( sqbind::stdString( sa.GetString( 2 ), sq_getsize( x_v, 2 ) ), sa.GetInt( 3 ) )
		SQBIND_CLASS_CTOR_END( CSqSockAddress )

		/// Register class for squirrel
		static void Register( sqbind::VM vm );

		/// Default constructor
		CSqSockAddress() {}

		/// Construct from address
		CSqSockAddress( oexCONST oex::os::CIpAddress &r )
			: m_address( r )
		{}

		/// Contstruct from url
		CSqSockAddress( const sqbind::stdString &sUrl )
		{	LookupUrl( sUrl, 0 ); }

		/// Contstruct from url and port
		CSqSockAddress( const sqbind::stdString &sUrl, int nPort )
		{	LookupUrl( sUrl, nPort ); }

		/// Returns the address id string
		sqbind::stdString GetId();

		/// Returns the address id string
		void SetId( const sqbind::stdString &sId );

		/// Sets the dot address
		int SetDotAddress( const sqbind::stdString &sDot, int nPort );

		/// Sets the raw address values
		int SetRawAddress( int nAddr, int nPort );

		/// Returns the current host name
		sqbind::stdString getHostName();

		/// Returns the current host name
		sqbind::stdString getDomainName();

		/// Returns the current host name
		sqbind::stdString getFullHostName();

		/// Lookup host by name
		int getHostByName( const sqbind::stdString &sHost );

		/// Validates the address check sum
		int ValidateAddress();

		/// Look up specified url
		int LookupUrl( const sqbind::stdString &sUrl, int nPort );

		/// Look up specified host
		int LookupHost( const sqbind::stdString &sHost, int nPort );

		/// Returns a reference the the wrapped object
		oex::os::CIpAddress& Obj() { return m_address; }

		/// Returns the port
		int getPort();

		/// Returns a string identifying the dot address
		sqbind::stdString getDotAddress();

	private:

		/// Address object
		oex::os::CIpAddress		m_address;
	};


	class CSqSocket
	{
	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqSocket )
			_SQBIND_CLASS_CTOR1( CSqSocket, OT_STRING ) ( sqbind::stdString( sa.GetString( 2 ), sq_getsize( x_v, 2 ) ) )
			_SQBIND_CLASS_CTOR2( CSqSocket, OT_STRING, OT_INTEGER ) ( sqbind::stdString( sa.GetString( 2 ), sq_getsize( x_v, 2 ) ), sa.GetInt( 3 ) )
		SQBIND_CLASS_CTOR_END( CSqSocket )

		/// Register class for squirrel
		static void Register( sqbind::VM vm );

		/// Default constructor
		CSqSocket() { m_socket.OexConstruct(); }

		// Copy semantics
		CSqSocket( const CSqSocket &r ) { m_socket = r.m_socket; }
		CSqSocket& operator=( const CSqSocket &r ) { m_socket = r.m_socket; return *this; }

		/// Contstruct from url
		CSqSocket( const sqbind::stdString &sUrl )
		{	Connect( sUrl, 0 ); }

		/// Contstruct from url and port
		CSqSocket( const sqbind::stdString &sUrl, int nPort )
		{	Connect( sUrl, nPort ); }

		/// Close previous device
		void Destroy();

		/// Create a UDP socket
		int CreateUDP();

		/// Connects to the specified address
		int Connect( const sqbind::stdString &sUrl, int nPort );

		/// Detaches from the socket and returns the os socket handle
		SQInteger Detach();

		/// Binds the socket to a port
		int Bind( int nPort );

		/// Listens for incomming connections
		int Listen( int nMax );

		/// Accepts in incomming connection with the specified socket
		int Accept( CSqSocket *pSocket );

		/// Share the socket with another class
		int Share( CSqSocket *pSocket );

		/// Waits for the specified event(s)
		int WaitEvent( int nEvent, int nMax );

		/// Sets blocking mode
		int setBlockingMode( int nEnable );

		/// Reads from the port
		sqbind::stdString Read( int nMax );

		/// Writes to the port
		int Write( const sqbind::stdString &s, int nMax );

		/// Reads from the port
		sqbind::stdString ReadFrom( int nMax );

		/// Writes to the port
		int SendTo( const sqbind::stdString &s, int nMax );

		/// Reads from the port
		int ReadBin( sqbind::CSqBinary *pBin, int nMax );

		/// Reads from the port and appends it to the specified buffer
		int AppendBin( sqbind::CSqBinary *pBin, int nMax );

		/// Writes to the port
		int WriteBin( sqbind::CSqBinary *pBin, int nMax );

		/// Reads from the port
		int ReadFromBin( sqbind::CSqBinary *pBin, int nMax );

		/// Writes to the port
		int SendToBin( sqbind::CSqBinary *pBin, int nMax );

		/// Returns a string describing the last error
		sqbind::stdString getLastError();

		/// Returns last error value
		int getLastErrorValue();

		/// Returns a reference to the wrapped socket object
		oex::os::CIpSocket* Ptr() { return m_socket.Ptr(); }

		/// Returns the number of reads
		unsigned long getNumReads() { return m_socket->getNumReads(); }

		/// Returns the number of writes on the socket
		unsigned long getNumWrites() { return m_socket->getNumWrites(); }

		/// Returns the number of accepts on the socket
		unsigned long getNumAccepts() { return m_socket->getNumAccepts(); }

		/// Returns non-zero if the socket is running
		int isRunning( int nCheckActivity );

		/// Returns non-zero if the socket is connecting
		int isConnecting();

		/// Returns non-zero if the socket is connected
		int isConnected();

		/// Returns non-zero if there is recent socket activity
		int isActivity();

		/// Returns the socket connect state
		int getConnectState();

		/// Gets the peer address
		int getPeerAddress( CSqSockAddress *pAddr );

		/// Gets the local address
		int getLocalAddress( CSqSockAddress *pAddr );

		/// Gets the peer address
		int setPeerAddress( CSqSockAddress *pAddr );

		/// Gets the local address
		int setLocalAddress( CSqSockAddress *pAddr );

		/// Callback script
		int setScript( sqbind::CSqMsgQueue *pMsgQueue, const sqbind::stdString &sId, const sqbind::stdString &sScript, const sqbind::stdString &sFunction );

		oex::oexUINT32 htonl( oex::oexUINT32 v )
		{	return m_socket->hton_l( v ); }
		oex::oexUINT32 ntohl( oex::oexUINT32 v )
		{	return m_socket->ntoh_l( v ); }
		oex::oexUINT16 htons( oex::oexUINT16 v )
		{	return m_socket->hton_s( v ); }
		oex::oexUINT16 ntohs( oex::oexUINT16 v )
		{	return m_socket->ntoh_s( v ); }

	private:

		/// Socket class
		oex::TMem< oex::os::CIpSocket >	m_socket;

	};

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqSocket, CSqSocket )
SQBIND_DECLARE_INSTANCE( sqbind::CSqSockAddress, CSqSockAddress )
