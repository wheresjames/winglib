
// cell_rack.h

#pragma once

class CCellRack
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CCellRack )
	_SQBIND_CLASS_CTOR_END( CCellRack )

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
