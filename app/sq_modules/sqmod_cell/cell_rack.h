
// cell_rack.h

#pragma once

class CCellRack
{
public:

	/// Default constructor
	CCellRack()
	{
		oexZeroMemory( &m_rack, sizeof( m_rack ) );
	}

	/// Destructor
	~CCellRack()
	{
	}

	/// Returns _rack reference
	operator _rack&() { return m_rack; }

	/// Returns _rack reference
	_rack& Obj() { return m_rack; }

private:

	/// Structure describing device rack
	_rack	m_rack;

};
