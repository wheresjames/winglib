// sqirr_animator.h

#pragma once

class CSqirrAnimator
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrAnimator )
	_SQBIND_CLASS_CTOR_END( CSqirrAnimator )

	/// Default constructor
	CSqirrAnimator() { m_p = 0; }
	CSqirrAnimator( irr::scene::ISceneNodeAnimator *p ) { if ( p ) { m_p = p; p->grab(); } }

	~CSqirrAnimator() { Destroy(); }

	/** \addtogroup CSqirrAnimator
		@{
	*/

	void Destroy() { if ( m_p ) { m_p->drop(); m_p = 0; } }

	/** @} */

	// Copy semantics
	CSqirrAnimator( const CSqirrAnimator &r ) { m_p = r.m_p; if ( m_p ) m_p->grab(); }
	CSqirrAnimator& operator = ( const CSqirrAnimator &r ) { Destroy(); m_p = r.m_p; if ( m_p ) m_p->grab(); return *this; }
	irr::scene::ISceneNodeAnimator* Ptr() { return m_p; }

protected:

	irr::scene::ISceneNodeAnimator    	*m_p;

};

_SQBIND_DECLARE_INSTANCE( CSqirrAnimator, CSqirrAnimator );

