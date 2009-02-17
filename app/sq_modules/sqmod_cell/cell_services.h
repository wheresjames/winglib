
// cell_services.h

#pragma once

class CCellServices
{
public:

	/// Default constructor
	CCellServices()
	{
		oexZeroMemory( &m_services, sizeof( m_services ) );
	}

	/// Destructor
	~CCellServices()
	{
	}

	/// Returns _services reference
	operator _services&() { return m_services; }

private:

	/// Structure describing available services
	_services	m_services;

};
