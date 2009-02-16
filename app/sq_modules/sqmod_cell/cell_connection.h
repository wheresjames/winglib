
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
	int Connect( const std::string &sIp );

	/// Sets information about the last error
	void SetLastError( std::string sErr )
	{	m_sLastError = sErr; }

	/// Returns information about the last error
	std::string GetLastError()
	{	return m_sLastError; }

private:

	/// Holds last error information
	std::string			m_sLastError;

	/// Connection address
	std::string			m_sIp;

	/// Connection informatino
	_comm_header		m_comm;

	/// Device rack
	CCellRack			m_rack;

	/// Device backplane
	CCellBackplane		m_backplane;

	/// Device services
	CCellServices		m_services;
};
