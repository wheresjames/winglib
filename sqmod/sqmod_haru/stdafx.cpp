// stdafx.cpp

#include "stdafx.h"

// Export Functions
SQBIND_REGISTER_CLASS_BEGIN( CHaruPdf, CHaruPdf )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, Destroy )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, getLastError )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, New )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, Save )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, getBin )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, AddPage )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, getPageWidth )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, getPageHeight )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, setLineWidth )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, Rectangle )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, Stroke )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, BeginText )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, EndText )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, SetFont )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, SetTtfFont )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, getTextWidth )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, getTextHeight )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, TextOut )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, MoveTextPos )
	SQBIND_MEMBER_FUNCTION( CHaruPdf, ShowText )
//	SQBIND_MEMBER_FUNCTION( CHaruPdf,  )
//	SQBIND_MEMBER_FUNCTION( CHaruPdf,  )
//	SQBIND_MEMBER_FUNCTION( CHaruPdf,  )
SQBIND_REGISTER_CLASS_END()

// Export classes
static void SQBIND_Export_haru( sqbind::VM x_vm )
{_STT();
	if ( !oexCHECK_PTR( x_vm ) )
		return;

    SQBIND_EXPORT( x_vm, CHaruPdf );
}

#if defined( SQBIND_STATIC )
	#include "sq_haru_pdf.cpp"
#else

	static void SQBIND_Export( sqbind::VM x_vm )
	{_STT(); SQBIND_Export_haru( x_vm ); }

	// Include squirrel module exported symbols
	#include <sqmod_extern.hpp>

#endif
