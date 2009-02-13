
// cell_backplane.h

#pragma once

class CCellBackplane
{
public:

	/// Default constructor
	CCellBackplane()
	{
	}

	/// Destructor
	~CCellBackplane()
	{
	}

	/// Returns _backplane_data reference
	operator _backplane_data&() { return m_backplane; }

private:

	/// Structure describing device backplane
	_backplane_data	m_backplane;

};
