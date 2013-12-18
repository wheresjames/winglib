/*------------------------------------------------------------------
// serial_port.cpp
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
#include "std_os.h"

#include <termios.h>

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

/// OS specific serial data
struct SSerialPortSettings
{
	/// File descriptor
	int fd;

	/// Terminal settings
	struct termios dios;

	/// Terminal settings
	struct termios tios;
};


CSerialPort::CSerialPort()
{_STT();

	m_nError = 0;
	m_nPort = -1;
	m_nTxBufferSize = eDefaultTxBufferSize;
	m_nRxBufferSize = eDefaultRxBufferSize;

	// Allocate settings object
	m_pSettings = OexAllocNew< SSerialPortSettings >( 1 ) ;
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS )
		return;

	pS->fd = -1;
	oexZero( pS->dios );
	oexZero( pS->tios );

	SetDefaults();
}

CSerialPort::~CSerialPort()
{_STT();
	Destroy();

	// Ditch the settings object
	if ( m_pSettings )
	{	OexAllocDelete< SSerialPortSettings >( (SSerialPortSettings*)m_pSettings );
		m_pSettings = oexNULL;
	} // end if
}

void CSerialPort::Destroy()
{_STT();

	m_sPort.Destroy();
	m_nPort = -1;

	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS )
		return;

	// Do we have a valid port handle?
	if ( 0 <= pS->fd )
	{
		// Restore default port settings
		tcsetattr( pS->fd, TCSANOW, &pS->dios );

		// Close the port
		close( pS->fd );

		pS->fd = -1;
		
	} // end if
}

oexBOOL CSerialPort::Open( oexINT x_nPort )
{_STT();
	
	// Build port name
	oexBOOL bRes = Open( ( CStr() << oexT( "/dev/ttyS" ) << x_nPort ).Ptr() );

	// Save port index
	if ( bRes )
		m_nPort = x_nPort;

	return bRes;
}

oexBOOL CSerialPort::Open( oexCSTR x_pPort )
{_STT();

	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS || !x_pPort )
		return oexFALSE;

	// Lose previous port
	Destroy();

	// Ensure valid port name
	if ( !x_pPort || !*x_pPort )
		return oexFALSE; 

	// Attempt to open the specified port
	pS->fd = open( x_pPort, O_RDWR | O_NOCTTY | O_NDELAY );
	if ( 0 > pS->fd )
	{	m_nError = errno;
		Destroy();
		return oexFALSE;
	} // end if

	// Get current port settings
	if ( -1 == tcgetattr( pS->fd, &pS->dios ) )
	{	m_nError = errno;
		Destroy();
		return oexFALSE;
	} // end if

	// Set port settings
	if ( !SetStatus() )
	{	Destroy();
		return oexFALSE;
	} // end if

	// Save the port name
	m_sPort = x_pPort;

	// Purge the port
	Purge();

	return oexTRUE;
}

oexBOOL CSerialPort::SetStatus()
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS || 0 > pS->fd )
		return oexFALSE;

	// Restore default port settings
	tcsetattr( pS->fd, TCSANOW, &pS->tios );

	return oexTRUE;

}

oexBOOL CSerialPort::SetTimeout( oexINT x_to )
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS || 0 > pS->fd )
		return oexFALSE;

	return oexTRUE;
}


oexBOOL CSerialPort::IsPort()
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS || 0 > pS->fd )
		return oexFALSE;

	return oexTRUE;
}

oexBOOL CSerialPort::Purge()
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS || 0 > pS->fd )
		return oexFALSE;

	// Flush the port
	if ( -1 == tcflush( pS->fd, TCIOFLUSH ) )
		return oexFALSE;

	return oexTRUE;
}

oexBOOL CSerialPort::SetDefaults()
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS )
		return oexFALSE;
						   
	// Initialize structure to zero
	oexZero( pS->tios );

	// Default port settings
	pS->tios.c_cflag = B9600 | CS8 | CREAD | CLOCAL; // | HUPCL;
	cfsetispeed( &pS->tios, B9600 );
	cfsetospeed( &pS->tios, B9600 );

	return oexTRUE;
}

oexBOOL CSerialPort::ClearErrors()
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS || 0 > pS->fd )
		return oexFALSE;

	return oexTRUE;
}

oexSIZE_T CSerialPort::GetReadBytes()
{
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS || 0 > pS->fd )
		return 0;

	return 0;
}

oexSIZE_T CSerialPort::GetWriteBytes()
{
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS || 0 > pS->fd)
		return 0;

	return 0;
}

oexSIZE_T CSerialPort::Write( oexCPVOID x_pBuf, oexSIZE_T x_nSize )
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS || 0 > pS->fd )
		return 0;

	// Attempt to write data
	int nWritten = write( pS->fd, x_pBuf, x_nSize );
	if ( 0 > nWritten )
	{	m_nError = errno;
		return 0;
	} // end if

	return (oexSIZE_T)nWritten;
}

/// Reads data from the port
oexSIZE_T CSerialPort::Read( oexPVOID x_pBuf, oexSIZE_T x_nSize )
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS || 0 > pS->fd )
		return 0;

	int nRead = read( pS->fd, x_pBuf, x_nSize );
	if ( 0 > nRead )
	{	m_nError = errno;
		return 0;
	} // end if

	return (oexSIZE_T)nRead;
}

CStr8 CSerialPort::Read( oexSIZE_T x_nSize )
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS || 0 > pS->fd )
		return CStr8();
	
	CStr8 s;
	oexSTR8 p = s.Allocate( x_nSize );
	if ( !p )
		return CStr8();

	// Attempt to read data from port
	int nRead = read( pS->fd, p, x_nSize );
	if ( 0 > nRead )
	{	m_nError = errno;
		return CStr8();
	} // end if

	// How many bytes did we read?
	s.SetLength( nRead );

	return s;
}

/// Reads data into a string object
oexSIZE_T CSerialPort::ReadBin( CBin *bin, oexSIZE_T x_nSize )
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !bin || !x_nSize || !pS || 0 > pS->fd )
		return 0;

	// Ensure space
	if ( bin->Size() < x_nSize )
		if ( !bin->Allocate( x_nSize ) )
			return 0;

	// Attempt to read data from port
	int nRead = read( pS->fd, bin->_Ptr(), x_nSize );
	if ( 0 > nRead )
	{	m_nError = errno;
		return 0;
	} // end if

	// How many bytes did we read?
	bin->setUsed( nRead );

	// Did we read anything?
	return (oexSIZE_T)nRead;
}

oexINT CSerialPort::getByteSize()
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS ) return -1;

	switch( pS->tios.c_cflag & CSIZE )
	{	case CS5 : return (oexINT)5;
		case CS6 : return (oexINT)6;
		case CS7 : return (oexINT)7;
		case CS8 : return (oexINT)8;
		default : break;
	}
	return -1;
}

void CSerialPort::setByteSize( oexINT v )
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS ) return;

	// Turn off all the bits
	pS->tios.c_cflag &= ~CSIZE;

	switch( v )
	{	case 5 : pS->tios.c_cflag |= CS5; break;		
		case 6 : pS->tios.c_cflag |= CS6; break;
		case 7 : pS->tios.c_cflag |= CS7; break;
		case 8 : pS->tios.c_cflag |= CS8; break;
		default : break;
	} // end switch
}

oexINT CSerialPort::getBaudRate()
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS ) return -1;

	int speed = 0;

#if defined( CBAUDEX )

	if ( pS->tios.c_cflag & CBAUDEX )
		speed = cfgetospeed( &pS->tios );
	else

#endif
		speed = pS->tios.c_cflag & CBAUD;

	switch( speed )
	{	case B0 : return 0;
		case B50 : return 50;
		case B75 : return 75;
		case B110 : return 110;
		case B134 : return 134;
		case B150 : return 150;
		case B200 : return 200;
		case B300 : return 300;
		case B600 : return 600;
		case B1200 : return 1200;
		case B1800 : return 1800;
		case B2400 : return 2400;
		case B4800 : return 4800;
		case B9600 : return 9600;
		case B19200 : return 19200;
		case B38400 : return 38400;
#if defined( CBAUDEX )
		case B57600 : return 57600;
		case B115200 : return 115200;
		case B230400 : return 230400;
		case B460800 : return 460800;
		case B500000 : return 500000;
		case B576000 : return 576000;
		case B921600 : return 921600;
		case B1000000 : return 1000000;
		case B1152000 : return 1152000;
		case B2000000 : return 2000000;
		case B2500000 : return 2500000;
		case B3000000 : return 3000000;
		case B3500000 : return 3500000;
		case B4000000 : return 4000000;
#endif
		default : break;
	}
	return -1;
}

void CSerialPort::setBaudRate( oexINT v )
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS ) return;

	speed_t speed = 0;

	switch( v )
	{
		case 0 : speed = B0; break;
		case 50 : speed = B50; break;
		case 75 : speed = B75; break;
		case eBaud_110 : speed = B110; break;
		case 134 : speed = B134; break;
		case 150 : speed = B150; break;
		case 200 : speed = B200; break;
		case eBaud_300 : speed = B300; break;
		case eBaud_600 : speed = B600; break;
		case eBaud_1200 : speed = B1200; break;
		case 1800 : speed = B1800; break;
		case eBaud_2400 : speed = B2400; break;
		case eBaud_4800 : speed = B4800; break;
		case eBaud_9600 : speed = B9600; break;
		case eBaud_19200 : speed = B19200; break;
		case eBaud_38400 : speed = B38400; break;
#if defined( CBAUDEX )
		case eBaud_57600 : speed = B57600; break;
		case eBaud_115200 : speed = B115200; break;
		case eBaud_230400 : speed = B230400; break;
		case eBaud_460800 : speed = B460800; break;
		case eBaud_500000 : speed = B500000; break;
		case eBaud_576000 : speed = B576000; break;
		case eBaud_921600 : speed = B921600; break;
		case eBaud_1000000 : speed = B1000000; break;
		case eBaud_1152000 : speed = B1152000; break;
		case eBaud_2000000 : speed = B2000000; break;
		case eBaud_2500000 : speed = B2500000; break;
		case eBaud_3000000 : speed = B3000000; break;
		case eBaud_3500000 : speed = B3500000; break;
		case eBaud_4000000 : speed = B4000000; break;
#endif
		default : return;
	} // end switch

#if defined( CBAUDEX )

	// Clear old baud rate
	pS->tios.c_cflag &= ~( CBAUD | CBAUDEX );
	pS->tios.c_cflag |= speed;

	// Set into extended variables
	cfsetispeed( &pS->tios, speed );
	cfsetospeed( &pS->tios, speed );

#else

	pS->tios.c_cflag &= ~CBAUD;
	pS->tios.c_cflag |= speed;

#endif
}

oexINT CSerialPort::getStopBits()
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS ) return -1;
	return ( pS->tios.c_cflag & CSTOPB ) ? eStopBits_Two : eStopBits_One;
}

void CSerialPort::setStopBits( oexINT v )
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS ) return;
	switch( v )
	{	case eStopBits_One : pS->tios.c_cflag &= ~CSTOPB; break;
		case eStopBits_Two : pS->tios.c_cflag |= CSTOPB; break;
		default : break;
	} // end switch	
}

oexINT CSerialPort::getParity()
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS ) return -1;

	if ( !( pS->tios.c_cflag & PARENB ) )
		return eParity_None;

	if ( pS->tios.c_cflag & PARODD )
		return eParity_Odd;

	return eParity_Even;
}

void CSerialPort::setParity( oexINT v )
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS ) return;

	switch( v )
	{	case eParity_None : pS->tios.c_cflag &= ~PARENB; break;
		case eParity_Even : pS->tios.c_cflag |= PARENB; pS->tios.c_cflag &= ~PARODD; break;
		case eParity_Odd : pS->tios.c_cflag |= PARENB; pS->tios.c_cflag |= PARODD; break;
	} // end switch

}

oexINT CSerialPort::getRtsCtrl()
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS ) return -1;
	return ( pS->tios.c_iflag & IXON ) ? eXCtrlEnable : eXCtrlDisable;
}

void CSerialPort::setRtsCtrl( oexINT v )
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS ) return;
	switch( v )
	{	case eXCtrlEnable : pS->tios.c_cflag |= IXON; break;
		case eXCtrlDisable : pS->tios.c_cflag &= ~IXON; break;
		default : break;
	} // end switch	
}

oexINT CSerialPort::getDtrCtrl()
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS ) return -1;
	return ( pS->tios.c_iflag & IXOFF ) ? eXCtrlEnable : eXCtrlDisable;
}

void CSerialPort::setDtrCtrl( oexINT v )
{_STT();
	SSerialPortSettings *pS = (SSerialPortSettings*)m_pSettings;
	if ( !pS ) return;
	switch( v )
	{	case eXCtrlEnable : pS->tios.c_cflag |= IXOFF; break;
		case eXCtrlDisable : pS->tios.c_cflag &= ~IXOFF; break;
		default : break;
	} // end switch	
}

