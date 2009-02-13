
// cell_services.h

#pragma once

class CCellServices
{
public:

	/// Default constructor
	CCellServices()
	{
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
