
class C3dEditCursor
{
	m_size = 10.;

	m_x = 0;
	m_y = 0;
	m_z = 0;

	m_p = 0;

	m_irr = 0;

	m_btn = 0;

	m_sel = 0;

	// Create a direction arrow
	function AddArrowMesh( p, size, r, g, b, rot, pos )
	{
		local arrow = m_irr.AddArrowMesh( size, size, size, 8 );    
		m_irr.SetVertexColors( arrow, CSqirrColor( r, g, b, 127 ) );
		arrow.SetRotation( rot );
		arrow.SetPosition( pos );
		arrow.SetParent( p );
		arrow.SetID( 0x80000000 );
		return arrow;
	}

	function Create( irr, size )
	{
		m_irr = irr;
		m_size = size;

		// Add center point
		m_p = m_irr.AddSphereMesh( size / 2, size / 2, 8 );
		m_irr.SetVertexColors( m_p, CSqirrColor( 255, 255, 255, 0 ) );
//		m_p.SetID( 0x80000000 );

		m_x = AddArrowMesh( m_p, size * 1.2, 200, 50, 50, 
							CSqirrVector3d( 0., 0., 90. ), 
							CSqirrVector3d( -( size * 2 ), 0., 0. ) );

		m_y = AddArrowMesh( m_p, size * 1.2, 50, 200, 50, 
							CSqirrVector3d( 0., 0., 0. ), 
							CSqirrVector3d( 0., size * 2, 0. ) );

		m_z = AddArrowMesh( m_p, size * 1.2, 50, 150, 200, 
							CSqirrVector3d( 90., 0., 0. ), 
							CSqirrVector3d( 0., 0., size * 2 ) );

		SetVisible( 0 );
	}

	function SetVisible( b )
	{	if ( m_p ) 
			m_p.SetVisible( b ); 
	}

	function OnEvent( e, x, y, w )
	{
		// Are we active?
		if ( !m_p ) // || !m_p.IsVisible() )
			return 0;

		// Left click?
		if ( e.tointeger() == m_irr.EMIE_LMOUSE_PRESSED_DOWN )
			OnButtonDown( 1, x, y );

		else if ( e.tointeger() == m_irr.EMIE_LMOUSE_LEFT_UP )
			OnButtonUp( 1, x, y );

		else if ( e.tointeger() == m_irr.EMIE_MOUSE_MOVED )
			OnMouseMove( x, y );
	}

	function OnButtonDown( btn, x, y )
	{
		switch( btn )
		{
			case 1 :
				m_btn = SelectAt( x, y ) ? btn : 0;
				break;

			default :
				m_btn = 0;
				break;

		} // end switch

	}

	function OnButtonUp( btn, x, y )
	{
		m_btn = 0;
	}

	function OnMouseMove( x, y )
	{
		if ( 1 == m_btn && m_sel && m_sel.IsValid() )
		{
			local pos = CSqirrVector3d();
			if ( m_irr.screenToWorld( CSqirrVector2d( x.tofloat(), y.tofloat() ), 
									  pos, m_irr.getNodeDist( m_sel ) ) )
				m_sel.SetCenter( pos );

			UpdateCursor();

		} // end if
	}

	function SelectAt( x, y )
	{
		if ( m_sel && m_sel.IsValid() )
			m_sel.SetDebugDataVisible( CSqirrNode().EDS_OFF ),
			m_sel = 0;

		m_sel = m_irr.NodeAtScreen( CSqirrVector2d( x.tofloat(), y.tofloat() ), 1 );

		UpdateCursor();
		if ( !m_sel || !m_sel.IsValid() )
			return 0;

		m_sel.SetDebugDataVisible( CSqirrNode().EDS_FULL );

		return 1;
	}

	function UpdateCursor()
	{
		if ( !m_p ) return;

		if ( !m_sel || !m_sel.IsValid() )
		{    m_p.SetVisible( 0 ); return; }

		local center = m_sel.GetAbsoluteCenter();

		local screen_pos = m_irr.getScreenPos( center );

		if ( 0 > screen_pos.x() || 0 > screen_pos.y() )
		{   m_p.setVisible( 0 ); return; }

		local abspos = CSqirrVector3d( 0., 0., 0. );
		if ( !m_irr.screenToWorld( screen_pos, abspos, m_size * 32 ) )
		{   m_p.setVisible( 0 ); return; }

		m_p.SetVisible( 1 ); 
		m_p.SetAbsolutePosition( abspos );

//		::_self.echo( abspos.x() + "," + abspos.y() + "," + abspos.z() + " - "
//					  + "x=" + screen_pos.x() + ", y=" + screen_pos.y() );

	}

};
