
// sq_ssl_port.cpp

#include "stdafx.h"

CSqSSLPortFactory::CSqSSLPortFactory()
{_STT();
}

CSqSSLPortFactory::~CSqSSLPortFactory()
{_STT();
}


CSqSSLPortFactory::CSqSSLPort::CSqSSLPort( SSL_CTX *ctx )
{_STT();

	m_ctx = ctx;
	m_ssl = oexNULL;
}

CSqSSLPortFactory::CSqSSLPort::~CSqSSLPort()
{_STT();

	if ( m_ssl )
	{	SSL_shutdown( m_ssl );
		SSL_free( m_ssl );
	} // end if

	m_ctx = oexNULL;
	m_ssl = oexNULL;
}

oex::oexBOOL CSqSSLPortFactory::CSqSSLPort::OnAttach()
{_STT();

	if ( !m_ctx )
		return oex::oexFALSE;

	// Create ssl object for this connection
	m_ssl = SSL_new( m_ctx );
	if ( !m_ssl )
	{	m_sLastError = ERR_error_string( ERR_get_error(), 0 );
		oexSHOW( m_sLastError.c_str() );
		return oex::oexFALSE;
	} // end if

	// Set socket handle
	if ( !SSL_set_fd( m_ssl, (int)GetSocketHandle() ) )
	{	m_sLastError = ERR_error_string( ERR_get_error(), 0 );
		oexSHOW( m_sLastError.c_str() );
		return oex::oexFALSE;
	} // end if

	// SSL accept
	if( -1 == SSL_accept( m_ssl ) )
	{	m_sLastError = ERR_error_string( ERR_get_error(), 0 );
		oexSHOW( m_sLastError.c_str() );
		return oex::oexFALSE;
	} // end if

	return oex::oexTRUE;
}

int CSqSSLPortFactory::CSqSSLPort::v_recv( int socket, void *buffer, int length, int flags )
{_STT();
	return SSL_read( m_ssl, buffer, length );
}

int CSqSSLPortFactory::CSqSSLPort::v_recvfrom( int socket, void *buffer, int length, int flags )
{_STT();
	return 0;
}

int CSqSSLPortFactory::CSqSSLPort::v_send( int socket, const void *buffer, int length, int flags )
{_STT();
	return SSL_write( m_ssl, buffer, length );
}

int CSqSSLPortFactory::CSqSSLPort::v_sendto(int socket, const void *message, int length, int flags )
{_STT();
	return 0;
}

CSqSSLPortFactory::CPortFactory::CPortFactory()
{_STT();

	m_ctx = oexNULL;

}

CSqSSLPortFactory::CPortFactory::~CPortFactory()
{_STT();
	Destroy();
}

void CSqSSLPortFactory::CPortFactory::Destroy()
{_STT();
	if ( m_ctx )
		SSL_CTX_free( m_ctx );

	m_ctx = oexNULL;
}

int CSqSSLPortFactory::CPortFactory::Initialize()
{_STT();

	if ( !CreateCtx() )
		return 0;

	return 1;
}


int CSqSSLPortFactory::CPortFactory::CreateCtx()
{_STT();

	// Initialize openssl
	OpenSSL_add_all_algorithms();
	SSL_load_error_strings();

	// Describes which SSL protocol will be used
	const SSL_METHOD *pMethod = SSLv23_server_method();
	if ( !pMethod )
	{	m_sLastError = ERR_error_string( ERR_get_error(), 0 );
		return 0;
	} // end if

	// Create context
	m_ctx = SSL_CTX_new( pMethod );
	if ( !m_ctx )
	{	m_sLastError = ERR_error_string( ERR_get_error(), 0 );
		return 0;
	} // end if

	return 1;
}

int CSqSSLPortFactory::CPortFactory::LoadCerts( const sqbind::stdString &sCertChain, const sqbind::stdString &sPrivateKey )
{_STT();

	if ( !m_ctx )
		return 0;

	// Load certificate chain
  	if ( 0 >= SSL_CTX_use_certificate_chain_file( m_ctx, sCertChain.c_str() ) )
	{	m_sLastError = ERR_error_string( ERR_get_error(), 0 );
		return 0;
	} // end if

	// Load private key
  	if ( 0 >= SSL_CTX_use_PrivateKey_file( m_ctx, sPrivateKey.c_str(), SSL_FILETYPE_PEM ) )
	{	m_sLastError = ERR_error_string( ERR_get_error(), 0 );
		return 0;
	} // end if

	// Verify key matches cert chain
	if ( !SSL_CTX_check_private_key( m_ctx ) )
	{	m_sLastError = oexT( "Certificate chain does not match private key" );
		return 0;
	} // end if

	return 1;
}
