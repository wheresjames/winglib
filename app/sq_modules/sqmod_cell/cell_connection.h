
// cell_connection.h

#pragma once

class CCellConnection
{
public:

	/// Default constructor
	CCellConnection()
	{	oexZeroMemory( &m_comm, sizeof( m_comm ) );
	}

	/// Destructor
	~CCellConnection()
	{	Destroy(); }

	int Destroy();

	/// Connect to device
	int Connect( const sqbind::stdString &sIp );

	/// Read the specified tag
	sqbind::stdString ReadTag( const sqbind::stdString &sProgram, const sqbind::stdString &sTag );

	/// Returns a string describing backplane data
	sqbind::stdString GetBackplaneData();

	/// Sets information about the last error
	sqbind::stdString SetLastError( const sqbind::stdString sErr )
	{	return m_sLastError = sErr; }

	/// Returns information about the last error
	sqbind::stdString GetLastError()
	{	return m_sLastError; }

private:

	/// Holds last error information
	sqbind::stdString	m_sLastError;

	/// Connection address
	sqbind::stdString	m_sIp;

	/// Connection informatino
	_comm_header		m_comm;

	/// Path
	_path 				m_path;

	/// Configuration tags
	_tag_data			m_tagsConfig;
	
	/// Detail tags
	_tag_data			m_tagsDetails;

	/// Program list
	_prog_list			m_prog_list;

	/// Device rack
	CCellRack			m_cRack;

	/// Device backplane
	CCellBackplane		m_cBackplane;

	/// Device services
	CCellServices		m_cServices;

	/// Tag map
	sqbind::CSqMap		m_mapTags;
};
