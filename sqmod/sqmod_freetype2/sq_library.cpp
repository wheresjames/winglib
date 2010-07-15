// sq_library.cpp

#include "stdafx.h"

CFtLibrary::CFtLibrary()
{_STT();

	// Initialize freetype library
	m_last_error = FT_Init_FreeType( &m_library );

}

CFtLibrary::~CFtLibrary() 
{
	Destroy(); 
}


void CFtLibrary::Destroy()
{_STT();

	FT_Done_FreeType( m_library );
}

int CFtLibrary::LoadFile( CFtFace *pFace, const sqbind::stdString &sFile, int nIndex )
{_STT();

	if ( !pFace )
		return 0;

	// oexSysDir( file );

	// Attempt to load font
	m_last_error = FT_New_Face( m_library, sFile.c_str(), nIndex, &pFace->Obj() );

	return m_last_error ? 0 : 1;
}

