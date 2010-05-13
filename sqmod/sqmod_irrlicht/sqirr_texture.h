// sqirr_texture.h

#pragma once

class CSqirrTexture
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqirrTexture )
	_SQBIND_CLASS_CTOR_END( CSqirrTexture )

	/// Default constructor
	CSqirrTexture() { m_p = 0; }
	CSqirrTexture( irr::video::ITexture *p ) { if ( p ) { m_p = p; p->grab(); } }

	~CSqirrTexture() { Destroy(); }
	void Destroy() { if ( m_p ) { m_p->drop(); m_p = 0; } }

	// Copy semantics
	CSqirrTexture( const CSqirrTexture &r ) { m_p = r.m_p; if ( m_p ) m_p->grab(); }
	CSqirrTexture& operator = ( const CSqirrTexture &r ) { Destroy(); m_p = r.m_p; if ( m_p ) m_p->grab(); return *this; }
	irr::video::ITexture* Ptr() { return m_p; }

	long GetWidth()
	{   if ( !m_p )
			return 0;
		return m_p->getSize().Width;
	}

	long GetHeight()
	{   if ( !m_p )
			return 0;
		return m_p->getSize().Height;
	}

	sqbind::CSqBinary Lock()
	{
		if ( !m_p )
			return sqbind::CSqBinary();

		oex::oexBYTE *pBuf = (oex::oexBYTE*)m_p->lock();
		if ( !pBuf )
			return sqbind::CSqBinary();

		int nSize = m_p->getSize().Width * m_p->getSize().Height * 4;
		if ( !nSize )
			return sqbind::CSqBinary();

		sqbind::CSqBinary buf;
		buf.setBuffer( (sqbind::CSqBinary::t_byte*)pBuf, nSize, 0, 0 );
		return buf;

	} // end if

	int Unlock()
	{
		if ( !m_p )
			return 0;

		m_p->unlock();

		return 1;
	}

protected:

	irr::video::ITexture    	*m_p;
};

_SQBIND_DECLARE_INSTANCE( CSqirrTexture, CSqirrTexture );

