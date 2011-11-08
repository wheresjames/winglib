
#pragma once

//#ifdef Q_MOC_RUN
#	include "stdafx.h"
//#endif


class CNetworkReply : public QNetworkReply
{
	Q_OBJECT

public:

	/// Constructor
	CNetworkReply( QObject *parent, const QNetworkRequest &req, const QNetworkAccessManager::Operation op );

	/// Aborts the transfer
	void abort() { QNetworkReply::close(); }

	/// Return the number of bytes available
	qint64 bytesAvailable() const { return m_content.size(); }

	/// Return non zero for sequential data
	bool isSequential() const { return true; }

protected:

	/// Used to retrieve a block of content data
	qint64 readData( char* pData, qint64 lMaxSize );

private:

	/// Offset progress
	qint64 m_lOffset;

	/// Data buffer
	QByteArray m_content;

};

class CNetworkMgr : public QNetworkAccessManager
{
	Q_OBJECT

public:

	CNetworkMgr( QObject *pParent, QNetworkAccessManager *pPrev );

protected:

	QNetworkReply* createRequest( QNetworkAccessManager::Operation op, const QNetworkRequest &req, QIODevice *device );


};
