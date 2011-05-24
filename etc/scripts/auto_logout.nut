
class CGlobal
{
	// Maximum time in seconds to allow the cursor idle before logging out
//	max_idle = 10;
	max_idle = 10 * 60;
	
	x = 0;
	y = 0;
	to = 0;
};

local _g = CGlobal();

function _init()
{
	return 0;
}

function _idle() : ( _g )
{
	local gmt = ::_self.gmt_time();

	// Reset timeout if mouse moves
	local mp = CSqGui().get_cursor_pos();
	if ( !_g.to || mp.getX() != _g.x || mp.getY() != _g.y )
		_g.x = mp.getX(), _g.y = mp.getY(), _g.to = gmt + _g.max_idle;
	
	// Logoff timeout?
	if ( gmt > _g.to )
	{	::_self.echo( " !!! LOGGING OFF !!!" );
		_g.to = gmt + _g.max_idle;
		::_self.logoff();		
	} // end if

	return 0;
}

