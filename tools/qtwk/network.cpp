
#include "stdafx.h"
#include "network.h"

template < typename T_Q, typename T_PB  >
	void tf_MapToPropertyBag( T_Q &q, T_PB &pb )
	{	for ( typename T_Q::iterator it = q.begin(); it != q.end(); it++ )
			pb[ T_PB::t_key( it->first.c_str(), it->first.length() ) ]
				= T_PB::t_val( it->second.str().c_str(), it->second.str().length() );
	}

CNetworkReply::CNetworkReply( QObject *parent, const QNetworkRequest &req, const QNetworkAccessManager::Operation op )
	: QNetworkReply( parent )
{
	// Setup the request
	setRequest( req );
	setUrl( req.url() );
	setOperation( op );
	QNetworkReply::open( QIODevice::ReadOnly | QIODevice::Unbuffered );

	// Bogus content
	m_lOffset = 0;
	m_content.clear();

	// Get the path to the file
	QByteArray path = req.url().path().toUtf8();
	oex::CStr full = oexBuildPath( "qtwk/", oex::CStr( path.data(), path.length() ) );

#if defined( _DEBUG )
	oexSHOW( full );
#endif

	// See if there's an active page with this name
	oexResourceFn( testfn ) = oexGetResourceFn( full );
	if ( testfn )
	{
		// Really sux that QUrl doesn't seem to be able to just return the GET params
		oex::CPropertyBag pb, url = oex::os::CIpAddress::ParseUrl( req.url().toString().toUtf8().data() );

		// GET params are in 'extra'
		if ( url[ "extra" ].ToString().Length() )
			pb[ "GET" ] = oex::CParser::DecodeUrlParams( url[ "extra" ].ToString() );

		if ( url[ "fragment" ].ToString().Length() )
			pb[ "FRAGMENT" ] = url[ "fragment" ].ToString();

		// Run the page
		oex::CStr out;
		testfn( pb, out );

		// Set the output
		m_content.append( out.Ptr(), out.Length() );

	} // end if

	else
	{
		// Look for raw data
		oex::CStr8 sData = oexGetResource( full );
		if ( sData.Length() )
			m_content.append( sData.Ptr(), sData.Length() );
		else
			m_content.append( "404 - Not Found" );

	} // end else

	oex::CStr sMime = full.GetMimeType();

	// Set headers
	setHeader( QNetworkRequest::ContentTypeHeader, QVariant( sMime.Ptr() ) );
	setHeader( QNetworkRequest::ContentLengthHeader, QVariant( m_content.size() ) );

	// Call notify functions
	QMetaObject::invokeMethod( this, "metaDataChanged", Qt::QueuedConnection );
	QMetaObject::invokeMethod( this, "readyRead", Qt::QueuedConnection );
	QMetaObject::invokeMethod( this, "downloadProgress", Qt::QueuedConnection,
							   Q_ARG( qint64, m_content.size() ), Q_ARG( qint64, m_content.size() ) );
	QMetaObject::invokeMethod( this, "finished", Qt::QueuedConnection );
}

qint64 CNetworkReply::readData( char* pData, qint64 lMaxSize )
{
	// Have we copied all the data?
	if ( m_lOffset >= m_content.size() )
		return -1;

	// Copy a block of data
	qint64 lCount = qMin( lMaxSize, m_content.size() - m_lOffset );
	memcpy( pData, m_content.constData() + m_lOffset, lCount );
	m_lOffset += lCount;

	// Return the number of bytes copied
	return lCount;
}


CNetworkMgr::CNetworkMgr( QObject *pParent, QNetworkAccessManager *pPrev )
	: QNetworkAccessManager( pParent )
{

	if ( pPrev )
	{
		setCache( pPrev->cache() );
		setCookieJar( pPrev->cookieJar() );
		setProxy( pPrev->proxy() );
		setProxyFactory( pPrev->proxyFactory() );

	} // end if

}

QNetworkReply* CNetworkMgr::createRequest( QNetworkAccessManager::Operation op, const QNetworkRequest &req, QIODevice *device )
{
	// oexSHOW( req.url().toString().toUtf8().data() );

	if ( req.url().host() == "embedded" )
		return new CNetworkReply( this, req, op );

	return QNetworkAccessManager::createRequest( op, req, device );
}
