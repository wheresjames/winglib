
// sq_vmime.h

#pragma once

class CSqVMime
{
public:

	// Simple authentication handler
	class interactiveAuthenticator : public vmime::security::defaultAuthenticator
	{
	public:

		const vmime::string getUsername() const
		{	return m_username; }

		void setUsername( vmime::string s )
		{	m_username = s; }

		const vmime::string getPassword() const
		{	return m_password; }

		void setPassword( vmime::string s )
		{	m_password = s; }

	private:

		mutable vmime::string m_username;
		mutable vmime::string m_password;
	};

public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqVMime )
	_SQBIND_CLASS_CTOR_END( CSqVMime )

	/// Default constructor
	CSqVMime();

	/// Destructor
	virtual ~CSqVMime();

	/// Releases resources
	void Destroy();

	/// Opens the specified mail service
	int Open( const sqbind::stdString &sUrl );

	/// Deletes the specified message
	int Close( int expunge );

	/// Sets the username
	void setUsername( const sqbind::stdString &s )
	{	m_sUsername = s; }

	/// Returns the current username
	sqbind::stdString getUsername()
	{	return m_sUsername; }

	/// Sets the password
	void setPassword( const sqbind::stdString &s )
	{	m_sPassword = s; }

	/// Returns the current password
	sqbind::stdString getPassword()
	{	return m_sPassword; }

	/// Returns the count of messages in the inbox
	int getMessageCount();

	/// Returns a human readable description of the last error to occur
	sqbind::stdString getLastError()
	{	return m_sLastError; }

	/// Returns the specified message
	CVmMsg getMessage( int n );

	/// Deletes the specified message
	int deleteMessage( int n );

	/// Deletes the specified messages
	int deleteMessages( int from, int to );

	/// Adds a message to the current folder
	int addMessage( CVmMsg *pMsg );

	/// Returns a list of all the folders
	int getFolders( sqbind::CSqMulti *m );

	/// Sets the current folder
	int setFolder( const sqbind::stdString &sFolder );

	/// Sends the specified message
	int Send( const sqbind::stdString &sUrl, CVmMsg *pMsg );

private:
	
	/// Session object
	vmime::ref <vmime::net::session>	m_session;

	/// Message store
	vmime::ref <vmime::net::store>		m_st;

	/// Message store folder
	vmime::ref <vmime::net::folder>		m_f;

	/// Description of the last error that occured
	sqbind::stdString					m_sLastError;

	/// Username
	sqbind::stdString					m_sUsername;

	/// Password
	sqbind::stdString					m_sPassword;

};

SQBIND_DECLARE_INSTANCE( CSqVMime, CSqVMime );

