
// cell_connection.h

#pragma once

class CCellConnection
{
public:

	/// Default constructor
	CCellConnection()
	{	m_comm.file_handle = 0;
	}

	/// Destructor
	~CCellConnection()
	{	Destroy(); }

	int Destroy();

	/// Connect to device
	int Connect( const std::tstring &sIp );

	/// Returns a string describing backplane data
	std::tstring GetBackplaneData();

	/// Sets information about the last error
	std::tstring SetLastError( const std::tstring sErr )
	{	return m_sLastError = sErr; }

	/// Returns information about the last error
	std::tstring GetLastError()
	{	return m_sLastError; }

private:

	/// Holds last error information
	std::string			m_sLastError;

	/// Connection address
	std::string			m_sIp;

	/// Connection informatino
	_comm_header		m_comm;

	/// Path
	_path 				m_path;

	/// Device rack
	CCellRack			m_cRack;

	/// Device backplane
	CCellBackplane		m_cBackplane;

	/// Device services
	CCellServices		m_cServices;
};
