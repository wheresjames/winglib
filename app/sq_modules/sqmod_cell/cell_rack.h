
// cell_rack.h

#pragma once

class CCellRack
{
public:

	/// Default constructor
	CCellRack()
	{
	}

	/// Destructor
	~CCellRack()
	{
	}

	/// Returns _rack reference
	operator _rack&() { return m_rack; }

private:

	/// Structure describing device rack
	_rack	m_rack;

};
