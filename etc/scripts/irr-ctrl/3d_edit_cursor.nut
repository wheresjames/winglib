
class CCameraMover
{
	m_click = CSqirrVector2d();

	m_vRotation = CSqirrVector3d();
	m_vPosition = CSqirrVector3d();
	m_vTarget = CSqirrVector3d();

	m_vPivotPoint = CSqirrVector3d();
	m_vPivotAngle = CSqirrVector3d();
	m_fPivotDist = 0;

	m_camera = 0;

	function SaveCameraInfo( irr, pivot, x, y )
	{
		m_click.set( x, y );
		m_camera = irr.getCamera();
		if ( !m_camera || !m_camera.node().IsValid() ) return;

		// Camera position
		m_vRotation = m_camera.node().GetRotation();
		m_vPosition = m_camera.node().GetPosition();
		m_vTarget = m_camera.GetTarget();

		m_vPivotPoint = pivot;
		m_camera.node().GetPivotAngle( m_vPivotPoint, m_vPivotAngle );
		m_fPivotDist = m_camera.node().GetPivotDist( m_vPivotPoint );
	}

	function MoveCamera( type, pivot, x, y )
	{
		if ( !m_camera || !m_camera.node().IsValid() )
			return 0;

		switch( type )
		{
			case 1 :

				local a = CSqirrVector3d( m_vPivotAngle.x() - ( m_click.y() - y ) / 2,
										  m_vPivotAngle.y() + ( m_click.x() - x ) / 2,
										  0. );

				m_camera.node().SetPivot( m_vPivotPoint, a, m_fPivotDist );
				m_camera.SetTarget( m_vPivotPoint );
				m_camera.node().FaceTarget( m_vTarget );

			break;

			case 2 :
				m_camera.node().SetRotation( m_vRotation );
				m_camera.node().SetPosition( m_vPosition );
				m_camera.SetTarget( m_vTarget );
				local dy = m_click.y() - y;
				local dx = m_click.x() - x;
				m_camera.Move( CSqirrVector3d( 0., 0., dx + dy ), 1 ); 
				break;

			default :
				return 0;
		} // end switch

		m_camera.UpdateCamera();

		return 1;
	}

};

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

	m_dir = 0;

	m_cm = CCameraMover();

	// Create a direction arrow
	function AddArrowMesh( p, size, r, g, b, rot, pos )
	{
		local arrow = m_irr.AddArrowMesh( size, size, size, 8 );    
		m_irr.SetVertexColors( arrow, CSqirrColor( r, g, b, 127 ) );
		arrow.SetRotation( rot );
		arrow.SetPosition( pos );
		arrow.SetParent( p );
		arrow.SetID( 0x10000000 );
		return arrow;
	}

	function Create( irr, size )
	{
		m_irr = irr;
		m_size = size;

		// Add center point
//		m_p = m_irr.AddSphereMesh( size, size, 8 );
		m_p = m_irr.AddSphere( size / 2, 8 );
		m_irr.SetVertexColors( m_p, CSqirrColor( 255, 255, 255, 0 ) );
		m_p.SetID( 0x10000000 );

		m_x = AddArrowMesh( m_p, size * 1.5, 200, 50, 50,
							CSqirrVector3d( 0., 0., 90. ),
							CSqirrVector3d( -( size * 3 ), 0., 0. ) );

		m_y = AddArrowMesh( m_p, size * 1.5, 50, 200, 50,
							CSqirrVector3d( 0., 0., 0. ),
							CSqirrVector3d( 0., size * 3, 0. ) );

		m_z = AddArrowMesh( m_p, size * 1.5, 50, 150, 200,
							CSqirrVector3d( 90., 0., 0. ),
							CSqirrVector3d( 0., 0., size * 3 ) );

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

		if ( e.tointeger() == m_irr.EMIE_MOUSE_MOVED )
			OnMouseMove( x.tofloat(), y.tofloat() );

		// Left click?
		else if ( e.tointeger() == m_irr.EMIE_LMOUSE_PRESSED_DOWN )
			OnButtonDown( 1, x.tofloat(), y.tofloat() );

		else if ( e.tointeger() == m_irr.EMIE_LMOUSE_LEFT_UP )
			OnButtonUp( 1, x.tofloat(), y.tofloat() );

		else if ( e.tointeger() == m_irr.EMIE_RMOUSE_PRESSED_DOWN )
			OnButtonDown( 2, x.tofloat(), y.tofloat() );

		else if ( e.tointeger() == m_irr.EMIE_RMOUSE_LEFT_UP )
			OnButtonUp( 2, x.tofloat(), y.tofloat() );

		else if ( e.tointeger() == m_irr.EMIE_MMOUSE_PRESSED_DOWN )
			OnButtonDown( 3, x.tofloat(), y.tofloat() );

		else if ( e.tointeger() == m_irr.EMIE_MMOUSE_LEFT_UP )
			OnButtonUp( 3, x.tofloat(), y.tofloat() );

	}

	function OnButtonDown( btn, x, y )
	{
		m_dir = 0;

		switch( btn )
		{
			case 1 :

				local select_new_node = 1;

				// Is an arrow being clicked?
				if ( m_sel && m_sel.IsValid() )
				{
					local arrow = m_irr.NodeAtScreen( CSqirrVector2d( x, y ), 0x10000000 );
					if ( arrow && arrow.IsValid() )
					{
						select_new_node = 0;

						if ( arrow.IsEqual( m_x ) )
							m_dir = CSqirrVector3d( 1., 0., 0. );
						else if ( arrow.IsEqual( m_y ) )
							m_dir = CSqirrVector3d( 0., 1., 0. );
						else if ( arrow.IsEqual( m_z ) )
							m_dir = CSqirrVector3d( 0., 0., 1. );

						m_btn = btn;

					} // end if

				} // end if

				// Do we want a new selection?
				if ( select_new_node )
					m_btn = SelectAt( x, y ) ? btn : 0;

				break;

			case 2 :
			case 3 :

				local pivot = CSqirrVector3d();
				if ( m_sel && m_sel.IsValid() )
					pivot = m_sel.GetAbsolutePosition();

				m_cm.SaveCameraInfo( m_irr, pivot, x, y );

				m_btn = btn;
				
				break;

			default :
				m_btn = 0;
				break;

		} // end switch


	}

	function OnButtonUp( btn, x, y )
	{
		m_btn = 0;
		m_dir = 0;
	}

	function OnMouseMove( x, y )
	{
		if ( 1 == m_btn && m_sel && m_sel.IsValid() )
		{
			if ( m_dir )
			{
				local intersect = CSqirrVector3d();
				local pos = m_sel.GetAbsolutePosition();
				local screen_pos = m_irr.getScreenPos( pos );

				if ( m_dir.y() ) 
					x = screen_pos.x();

				if ( m_irr.getMouseIntersect( CSqirrVector2d( x, y ),
											  pos, m_dir, intersect ) )
					m_sel.SetAbsoluteCenter( intersect );

			} // end if

			else
			{	local pos = CSqirrVector3d();
				if ( m_irr.screenToWorld( CSqirrVector2d( x, y ), 
										  pos, m_irr.getNodeDist( m_sel ) ) )
					m_sel.SetAbsoluteCenter( pos );
			} // end else

			UpdateCursor();

		} // end if

		else if ( 2 == m_btn )
		{
			if ( m_sel && m_sel.IsValid() )
				m_cm.MoveCamera( 1, m_sel.GetAbsolutePosition(), x, y );
			else
				m_cm.MoveCamera( 1, CSqirrVector3d(), x, y );
			
			UpdateCursor();

		} // end else if

		else if ( 3 == m_btn )
		{
			if ( m_sel && m_sel.IsValid() )
				m_cm.MoveCamera( 2, m_sel.GetAbsolutePosition(), x, y );
			else
				m_cm.MoveCamera( 2, CSqirrVector3d(), x, y );

			UpdateCursor();

		} // end else if

	}

	function SelectAt( x, y )
	{
		if ( m_sel && m_sel.IsValid() )
			m_sel.SetDebugDataVisible( CSqirrNode().EDS_OFF ),
			m_sel = 0;

		m_sel = m_irr.NodeAtScreen( CSqirrVector2d( x, y ), 1 );

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
		{   m_p.SetVisible( 0 ); return; }

		local abspos = CSqirrVector3d( 0., 0., 0. );
		if ( !m_irr.screenToWorld( screen_pos, abspos, m_size * 32 ) )
		{   m_p.SetVisible( 0 ); return; }

		m_p.SetVisible( 1 ); 
		m_p.SetAbsolutePosition( abspos );

//		::_self.echo( abspos.x() + "," + abspos.y() + "," + abspos.z() + " - "
//					  + "x=" + screen_pos.x() + ", y=" + screen_pos.y() );

	}

};
