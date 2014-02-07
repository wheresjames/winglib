// sq_library.cpp

#include "stdafx.h"

// CFtLibrary
SQBIND_REGISTER_CLASS_BEGIN( CFtLibrary, CFtLibrary )

	SQBIND_MEMBER_FUNCTION( CFtLibrary, Destroy )
	SQBIND_MEMBER_FUNCTION( CFtLibrary, getLastError )
	SQBIND_MEMBER_FUNCTION( CFtLibrary, LoadFile )
	SQBIND_MEMBER_FUNCTION( CFtLibrary, LoadFont )
//	SQBIND_MEMBER_FUNCTION( CFtLibrary,  )
//	SQBIND_MEMBER_FUNCTION( CFtLibrary,  )

SQBIND_REGISTER_CLASS_END()

void CFtLibrary::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CFtLibrary );
}

CFtLibrary::CFtLibrary()
{_STT();

	m_library = 0;

	// Initialize freetype library
	m_last_error = FT_Init_FreeType( &m_library );

}

CFtLibrary::~CFtLibrary() 
{
	Destroy(); 
}


void CFtLibrary::Destroy()
{_STT();

	if ( m_library )
	{	FT_Done_FreeType( m_library );
		m_library = 0;
	} // end if
}

int CFtLibrary::LoadFile( CFtFace *pFace, const sqbind::stdString &sFile, int nIndex )
{_STT();

	if ( !pFace )
		return 0;

	if ( !oexExists( sFile.c_str() ) )
		return 0;

	// Attempt to load font
	m_last_error = FT_New_Face( m_library, oexStrToMbPtr( sFile.c_str() ), nIndex, &pFace->Obj() );

	return m_last_error ? 0 : 1;
}

int CFtLibrary::LoadFont( CFtFace *pFace, const sqbind::stdString &sFont, int nIndex )
{_STT();

	if ( !pFace )
		return 0;

	return LoadFile( pFace, sqbind::oex2std( oexBuildSysPath( 0, oex::os::CBaseFile::eFidFonts, sFont.c_str() ) ), nIndex );
}

