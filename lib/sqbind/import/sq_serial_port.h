/*------------------------------------------------------------------
// sq_serial_port.h
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
    class CSqSerialPort
    {
	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqSerialPort )
		SQBIND_CLASS_CTOR_END( CSqSerialPort )

		/// Constructor
		CSqSerialPort() {}

		/// Destructor
		virtual ~CSqSerialPort() {}

		/// Registers the class
		static void Register( sqbind::VM vm );

		/// Closes the serial port
		void Destroy() { m_port.Destroy(); }

		/// Opens the specified serial port
		int Open( int nPort )
		{	return m_port.Open( nPort ); }

		/// Returns non-zero if a valid port is open
		int isPort()
		{	return m_port.IsPort(); }

		/// Returns non-zero if a valid port is open
		int isOpen()
		{	return m_port.IsPort(); }

		/// Opens the named serial port
		int OpenNamed( const stdString &sName )
		{	return m_port.Open( sName.c_str() ); }

		/// Returns the port system name
		sqbind::stdString getName() { return oex2std( m_port.GetName() ); }

		/// Returns the port index
		int getIndex() { return m_port.GetIndex(); }

		/// Returns the number of bytes in the rx queue
		int getReadBytes() { return m_port.GetReadBytes(); }

		/// Returns the number of bytes in the tx queueS
		int getWriteBytes() { return m_port.GetWriteBytes(); }

		/// Writes data to the serial port
		int Write( const stdString &sData )
		{	return m_port.Write( oexStrToBinPtr( sData.c_str() ), sData.length() ); }

		/// Writes binary data to the serial port
		int WriteBin( CSqBinary *pBin, int nBytes )
		{	if ( !pBin ) return 0;
			if ( nBytes > (int)pBin->getUsed() ) nBytes = pBin->getUsed();
			return m_port.Write( pBin->Ptr(), nBytes );
		}

		/// Reads a string from the serial port
		stdString Read( int nBytes )
		{	return oexBinToStr( m_port.Read( nBytes ) ).Ptr(); }

		/// Reads binary data from the serial port
		int ReadBin( CSqBinary *pBin, int nBytes )
		{
			if ( !pBin )
				return 0;

			// Allocate buffer
			if ( (int)pBin->Size() < nBytes )
				if ( !pBin->Allocate( nBytes ) )
					return 0;

			// Attempt read
			int nRead = (int)m_port.Read( pBin->_Ptr(), nBytes );
			if ( 0 > nRead ) nRead = 0;

			// How'd it go?
			pBin->setUsed( nRead );

			return nRead;
		}

		/// Appends binary data from the serial port
		int AppendBin( CSqBinary *pBin, int nBytes )
		{
			if ( !pBin )
				return 0;

			// Allocate buffer
			if ( (int)pBin->Size() < nBytes )
				if ( !pBin->Allocate( nBytes ) )
					return 0;

			// Is the buffer full?
			if ( pBin->getUsed() >= pBin->Size() )
				return pBin->getUsed();

			// Attempt read
			int nRead = (int)m_port.Read( pBin->_Ptr( pBin->getUsed() ), nBytes );
			if ( 0 > nRead ) nRead = 0;

			// How'd it go?
			pBin->setUsed( pBin->getUsed() + nRead );

			return pBin->getUsed();
		}

		/// Sets the baud rate
		void setBaudRate( int v ) { m_port.setBaudRate( v ); }

		/// Returns the baud rate
		int getBaudRate() { return m_port.getBaudRate(); }

		/// Sets the stop bits
		void setStopBits( int v ) { m_port.setStopBits( v ); }

		/// Returns the stop bits
		int getStopBits() { return m_port.getStopBits(); }

		/// Sets the parity, 0 = none, 1 = odd, 2 = even
		void setParity( int v ) { m_port.setParity( v ); }

		/// Returns the parity
		int getParity() { return m_port.getParity(); }

		/// Sets the RTS control
		void setRtsCtrl( int v ) { m_port.setRtsCtrl( v ); }

		/// Returns the RTS control
		int getRtsCtrl() { return m_port.getRtsCtrl(); }

		/// Sets the RTS control
		void setDtrCtrl( int v ) { m_port.setDtrCtrl( v ); }

		/// Returns the RTS control
		int getDtrCtrl() { return m_port.getDtrCtrl(); }

		/// Returns a reference to the wrapped object
		oex::os::CSerialPort& Obj() { return m_port; }

	private:

		/// Serial port object
		oex::os::CSerialPort	m_port;

    };

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqSerialPort, CSqSerialPort )

