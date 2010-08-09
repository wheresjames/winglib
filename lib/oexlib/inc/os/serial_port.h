/*------------------------------------------------------------------
// serial_port.h
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

//==================================================================
// CSerialPort
//
/// Serial port communication
/**
*/
//==================================================================
class CSerialPort
{
public:

	// Parity settings
	enum Parity
	{
		eParity_None			= 0,
		eParity_Odd				= 1,
		eParity_Even			= 2,
		eParity_Mark			= 3,
		eParity_Space			= 4
	};

	// Stop bit settings
	enum StopBits
	{
		eStopBits_One			= 1,
		eStopBits_Two			= 2,
		eStopBits_One5			= 3
	};

	// Baud rates
	enum BaudRate
	{
		eBaud_110				= 110,
		eBaud_300				= 300,
		eBaud_600				= 600,
		eBaud_1200				= 1200,
		eBaud_2400				= 2400,
		eBaud_4800				= 4800,
		eBaud_9600				= 9600,
		eBaud_14400				= 14400,
		eBaud_19200				= 19200,
		eBaud_38400				= 38400,
		eBaud_56000				= 56000,
		eBaud_57600				= 57600,
		eBaud_115200			= 115200,
		eBaud_128000			= 128000,
		eBaud_230400			= 230400,
		eBaud_256000			= 256000,
		eBaud_460800			= 460800,
		eBaud_500000			= 500000,
		eBaud_576000			= 576000,
		eBaud_921600			= 921600,
		eBaud_1000000			= 1000000,
		eBaud_1152000			= 1152000,
		eBaud_1500000			= 1500000,
		eBaud_2000000			= 2000000,
		eBaud_2500000			= 2500000,
		eBaud_3000000			= 3000000,
		eBaud_3500000			= 3500000,
		eBaud_4000000			= 4000000
	};

	// Flow control values
	enum XCtrl
	{
		eXCtrlDisable			= 0,
		eXCtrlEnable			= 1,
		eXCtrlHandshake			= 2,
		eXCtrlToggle			= 3
	};

	enum 
	{
		eDefaultTxBufferSize	= 4096,
		eDefaultRxBufferSize	= 4096
	};

public:

	/// Default Constructor
	CSerialPort();

	/// Destructor
	virtual ~CSerialPort();

	/// Closes the port and releases related resources
	void Destroy();

	/// Opens the specified port
	/**
		\param [in] x_pPort	- Name of the port to open
	*/
	oexBOOL Open( oexCSTR x_pPort );

	/// Opens the specified port
	/**
		\param [in] x_pPort	- Port index
	*/
	oexBOOL Open( oexINT x_nPort );

	/// Purges the port
	oexBOOL Purge();

	/// Set the port status
	oexBOOL SetStatus();

	/// Sets the port timeout parameters
	/**
		\param [in] x_to	- Timeout value in seconds
	*/
	oexBOOL SetTimeout( oexINT x_to = 3 );

	/// Clears serial port errors
	oexBOOL ClearErrors();

	/// Returns the last error code
	oexRESULT GetLastError()
	{	return m_nError; }

	/// Returns non-zero if there is a valid port open
	oexBOOL IsPort();

	/// Writes data to the port
	oexSIZE_T Write( oexCPVOID x_pBuf, oexSIZE_T x_nSize );

	/// Writes data to the port
	oexSIZE_T Write( CStr8 data, oexSIZE_T x_nSize = 0 )
	{	if ( !x_nSize ) x_nSize = data.Length();
		else if ( data.Length() < x_nSize ) x_nSize = data.Length();
		return Write( data.Ptr(), x_nSize );
	}

	/// Writes data to the port
	oexSIZE_T WriteBin( CBin *bin )
	{	if ( !bin ) return 0;
		return Write( bin->Ptr(), bin->getUsed() );
	}

	/// Reads data from the port
	oexSIZE_T Read( oexPVOID x_pBuf, oexSIZE_T x_nSize );

	/// Reads data into a string object
	CStr8 Read( oexSIZE_T x_nSize );

	/// Reads data into a string object
	oexSIZE_T ReadBin( CBin *bin, oexSIZE_T x_nSize );

public:

	/// Sets the default parameters for the port
	oexBOOL SetDefaults();

	/// Returns the byte size
	oexINT getByteSize();

	/// Sets the byte size
	void setByteSize( oexINT v );

	/// Returns the baud rate
	oexINT getBaudRate();

	/// Sets the baud rate
	void setBaudRate( oexINT v );

	/// Returns the stop bits
	oexINT getStopBits();

	/// Sets the stop bits
	void setStopBits( oexINT v );

	/// Returns the parity
	oexINT getParity();

	/// Sets the parity
	void setParity( oexINT v );

	/// Gets RTS handshaking
	oexINT getRtsCtrl();

	/// Sets RTS handshaking
	void setRtsCtrl( oexINT v );

	/// Gets DTR handshaking
	oexINT getDtrCtrl();

	/// Sets Dtr handshaking
	void setDtrCtrl( oexINT v );

	/// Gets the tx buffer size
	oexINT getTxBufferSize()
	{	return m_nTxBufferSize; }

	/// Sets the tx buffer size
	void setTxBufferSize( oexINT v )
	{	m_nTxBufferSize = v; }

	/// Gets the tx buffer size
	oexINT getRxBufferSize()
	{	return m_nRxBufferSize; }

	/// Sets the tx buffer size
	void setRxBufferSize( oexINT v )
	{	m_nRxBufferSize = v; }

	/// Returns the name of the open port
	CStr& GetName() { return m_sPort; }

	/// Returns the port index if any
	oexINT GetIndex() { return m_nPort; }

private:

	/// The last error code
	oexRESULT				m_nError;

	/// Pointer to os specific serial port settings
	oexPVOID				m_pSettings;

	/// Port name
	CStr					m_sPort;

	/// Port index
	oexINT					m_nPort;

	/// Tx buffer size
	oexINT					m_nTxBufferSize;

	/// Rx buffer size
	oexINT					m_nRxBufferSize;

};


