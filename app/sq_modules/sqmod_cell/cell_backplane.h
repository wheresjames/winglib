
// cell_backplane.h

#pragma once

class CCellBackplane
{
public:

	/// Default constructor
	CCellBackplane()
	{
		oexZeroMemory( &m_backplane, sizeof( m_backplane ) );
	}

	/// Destructor
	~CCellBackplane()
	{
	}

	/// Returns _backplane_data reference
	operator _backplane_data&() { return m_backplane; }

	/// Returns _backplane_data reference
	_backplane_data& Obj() { return m_backplane; }

private:

	/// Structure describing device backplane
	_backplane_data	m_backplane;

};
