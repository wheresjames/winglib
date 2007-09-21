// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

    class CTestMonitor
    {   public: 
        CTestMonitor() { m_uConstructed = m_uDestructed = 0; m_uValue = 0; }
        void Reset() { m_uConstructed = m_uDestructed = 0; m_uValue = 0; }
        oex::oexUINT m_uConstructed;
        oex::oexUINT m_uDestructed;
        oex::oexUINT m_uValue;
    };

    // Base test object
    class CBaseTestObject
    {   public:
        CBaseTestObject()
        {   m_bConstructed = oex::oexTRUE; 
            m_pMon = oexNULL; 
            m_uValue = 0; 
        }
        CBaseTestObject( CTestMonitor *pMon )
        {   m_bConstructed = oex::oexTRUE; 
            m_uValue = 0; 
            m_pMon = pMon; 
            if ( m_pMon ) 
                m_pMon->m_uConstructed++; 
        }
        ~CBaseTestObject()
        {   if ( m_pMon ) 
                m_pMon->m_uDestructed++; 
        }
        void SetMonitor( CTestMonitor *pMon )
        {   m_pMon = pMon; }
        void SetValue( oex::oexUINT uValue ) 
        {   m_uValue = uValue; 
            if ( m_pMon ) 
                m_pMon->m_uValue += uValue; 
        }        
        oex::oexUINT GetValue() { return m_uValue; }        
    private:
        CTestMonitor        *m_pMon;
        oex::oexBOOL         m_bConstructed;
        oex::oexUINT         m_uValue;
    };

    template < typename T > 
    	static T ReturnTest( T t ) { return t; }

oex::oexRESULT TestAllocator()
{
    // Veriry over-run / under-run protection
    char* pChar = OexAllocNew< char >( 100 );

    if ( !oexVERIFY( !oex::os::CSys::MemCmp( &pChar[ 100 ], 
                        oex::CAlloc::m_ucOverrunPadding, 
                        sizeof( oex::CAlloc::m_ucOverrunPadding ) ) ) )
        return -1;

    if ( !oexVERIFY( !oex::os::CSys::MemCmp( &pChar[ -(int)sizeof( oex::CAlloc::m_ucUnderrunPadding ) ], 
                        oex::CAlloc::m_ucUnderrunPadding, 
                        sizeof( oex::CAlloc::m_ucUnderrunPadding ) ) ) )
        return -2;

    OexAllocDelete( pChar );


    // Allocate generic array
    int * buf = OexAllocNew< int >( 100 );
    if ( !buf )
        return -1;

	int i;
    for ( i = 0; i < 100; i++ )
        buf[ 0 ] = i;

    if ( !oexVERIFY( oex::CAlloc::ArraySize( buf ) == 100 ) )
        return -2;

    if ( !oexVERIFY( oex::CAlloc::UsableSize( buf ) == sizeof( int ) * 100 ) )
        return -3;

    buf = OexAllocResize( buf, 50 );
    if ( !buf )
        return -4;

    if ( !oexVERIFY( oex::CAlloc::ArraySize( buf ) == 50 ) )
        return -5;

    if ( !oexVERIFY( oex::CAlloc::UsableSize( buf ) == sizeof( int ) * 50 ) )
        return -6;

    for ( i = 0; i < 50; i++ )
        buf[ 0 ] = i;

    OexAllocDelete( buf );


    // Test reference counting
    buf = OexAllocNew< int >( 100 );
    if ( !buf )
        return -1;

    // Add a reference
    oex::CAlloc::AddRef( buf );

    // Delete twice
    OexAllocDelete( buf );
    OexAllocDelete( buf );

    // Allocate single object
    CTestMonitor tm;
    CBaseTestObject *p = OexAllocConstruct< CBaseTestObject >( &tm );

    if ( !oexVERIFY_PTR( p ) )
        return -7;

    if ( !oexVERIFY( 1 == tm.m_uConstructed ) )
        return -8;

    OexAllocDestruct( p );

    if ( !oexVERIFY( 1 == tm.m_uDestructed ) )
        return -9;


    // Allocate array
    tm.Reset();
    p = OexAllocConstructArray< CBaseTestObject >( 3, &tm );

    if ( !oexVERIFY( p ) )
        return -9;

    if ( !oexVERIFY( 3 == tm.m_uConstructed ) )
        return -8;

    OexAllocDestruct( p );

    if ( !oexVERIFY( 3 == tm.m_uDestructed ) )
        return -9;

    { // Scope

        oex::TMem< char > mem;
        
        if ( !oexVERIFY_PTR( mem.OexNew( 13 ).c_Ptr() ) )
            return -10;

        // Check overrun protection
//        mem.OexResize( 12 );

        oex::zstr::Copy( mem.Ptr(), "Hello World!" );

    } // end scope

    { // Scope

        CTestMonitor tm;
        oex::TMem< CBaseTestObject > mem;

        if ( !oexVERIFY_PTR( mem.OexConstruct( &tm ).c_Ptr() ) )
            return -11;

        if ( !oexVERIFY( 1 == tm.m_uConstructed ) )
            return -12;

        tm.Reset();
        if ( !oexVERIFY_PTR( mem.OexConstruct( &tm ).c_Ptr() ) )
            return -13;

        if ( !oexVERIFY( 1 == tm.m_uConstructed ) )
            return -14;

        tm.Reset();
        if ( !oexVERIFY_PTR( mem.OexConstructArray( 2, &tm ).c_Ptr() ) )
            return -15;

        if ( !oexVERIFY( 2 == tm.m_uConstructed ) )
            return -16;

        // Strange allocation method
        CBaseTestObject *pBto = oex::TMem< CBaseTestObject >().OexConstruct().Detach();

        oex::TMem< CBaseTestObject >( pBto ).Delete();

        // Check overrun protection
//        mem.OexResize( 12 );

//        oex::zstr::Copy( mem.Ptr(), "Hello World!" );

    } // end scope

    { // Scope

        CTestMonitor tm;
        oex::TMem< CBaseTestObject > mem;

        if ( !oexVERIFY_PTR( mem.OexConstructArray( 4, &tm ).c_Ptr() ) )
            return -17;

        if ( !oexVERIFY( 2 == mem.Resize( 2 ).Size() ) )
            return -18;        

        if ( !oexVERIFY( 4 == tm.m_uConstructed ) )
            return -12;

        if ( !oexVERIFY( 2 == tm.m_uDestructed ) )
            return -12;

    } // end scope

    { // Scope

        oex::TMem< char > mem;

        // Work out the resize function
        const oex::oexUINT uTestSize = 1000;
        for ( oex::oexUINT i = 0; i < uTestSize; i++ )
        {
            if ( !oexVERIFY_PTR( mem.OexResize( i + 1 ).c_Ptr() ) )
                return -20;

            *mem.Ptr( i ) = (char)i;

        } // end for

        // Verify the memory data
        for ( oex::oexUINT i = 0; i < uTestSize; i++ )
            if ( !oexVERIFY( *mem.Ptr( i ) == (char)i ) )
                return -21;

    } // end scope

    return oex::oexRES_OK;
}

oex::oexRESULT TestFileMapping()
{
    oex::TFileMapping< char > fm;

    const oex::oexUINT uSize = 150;
    if ( !oexVERIFY( fm.Create( 0, 0, "Test", uSize ) ) )
        return -1;
    
    if ( !oexVERIFY( fm.Size() == uSize ) )
        return -2;

    // !!! This won't pass for every case, make sure there is
    //     enough room beyond uSize for the block overhead
    if ( !oexVERIFY( oex::cmn::NextPower2( uSize ) == fm.BlockSize() ) )
        return -3;

    oex::zstr::Copy( fm.Ptr(), "Hello World!" );

    if ( !oexVERIFY( !oex::str::Compare( fm.c_Ptr(), 12, "Hello World!", 12 ) ) )
        return -4;


    oex::TFileMapping< char > fm2;
    if ( !oexVERIFY( fm2.Create( 0, 0, "Test", uSize ) ) )
        return -5;

    if ( !oexVERIFY( fm.Size() == uSize ) )
        return -6;

    if ( !oexVERIFY( !oex::str::Compare( fm.c_Ptr(), 12, "Hello World!", 12 ) ) )
        return -7;


    CTestMonitor tm;
    oex::TMem< CBaseTestObject > mm, mm2;
    
    if ( !oexVERIFY(  mm.SetName( "Name" ).Construct( &tm ).Ptr() ) )
        return -8;

    mm.Ptr()->SetValue( 111 );

    if ( !oexVERIFY( mm2.SetName( "Name" ).Construct( &tm ).Ptr() ) )
        return -9;

    if ( !oexVERIFY( 1 == tm.m_uConstructed ) )
        return -12;

    if ( !oexVERIFY( 111 == mm2.Ptr()->GetValue() ) )
        return -9;

    mm.Delete();
    mm2.Delete();

    if ( !oexVERIFY( 1 == tm.m_uDestructed ) )
        return -12;

    return oex::oexRES_OK;
}

oex::oexRESULT TestGuids()
{
    // {6674C3D8-BB11-4a58-BCE0-A34DC74365AF}
    static const oex::oexGUID guidTest = 
    { 0x6674c3d8, 0xbb11, 0x4a58, { 0xbc, 0xe0, 0xa3, 0x4d, 0xc7, 0x43, 0x65, 0xaf } };

    oex::oexGUID    guid1, guid2;
    oex::oexTCHAR   szGuid1[ 1024 ] = oexT( "" );
    oex::oexTCHAR   szGuid2[ 1024 ] = oexT( "" );

    // Guid / String conversions
    oex::guid::GuidToString( szGuid1, sizeof( szGuid1 ), &guidTest );
    oex::guid::StringToGuid( &guid1, szGuid1, sizeof( szGuid1 ) );
    if ( !oexVERIFY( oex::guid::CmpGuid( &guidTest, &guid1 ) ) )
        return -1;

    oex::guid::CopyGuid( &guid2, &guid1 );
    if ( !oexVERIFY( oex::guid::CmpGuid( &guid1, &guid2 ) ) )
        return -3;

    oex::guid::SetGuid( &guid1, 0, 0, sizeof( guid1 ) );
    oex::guid::OrGuid( &guid1, &guid2 );
    if ( !oexVERIFY( oex::guid::CmpGuid( &guid1, &guid2 ) ) )
        return -4;

    oex::guid::SetGuid( &guid1, 0xffffffff, 0, sizeof( guid1 ) );
    oex::guid::AndGuid( &guid1, &guid2 );
    if ( !oexVERIFY( oex::guid::CmpGuid( &guid1, &guid2 ) ) )
        return -5;

    oex::guid::XorGuid( &guid1, &guid2 );
    oex::guid::SetGuid( &guid2, 0, 0, sizeof( guid1 ) );
    if ( !oexVERIFY( oex::guid::CmpGuid( &guid1, &guid2 ) ) )
        return -6;

    return oex::oexRES_OK;
}

oex::oexRESULT TestStrings()
{      
    oex::CStr str1, str2;
    oex::oexSTR pStr;

		// Buffer over-run protection test 
//        oex::oexSTR pOvPtr = str1.Allocate( 4 );
//        oex::zstr::Copy( pOvPtr, "12345" );
//        str1.Length();

		// Buffer under-run protection test
//        oex::oexSTR pOvPtr = str1.Allocate( 4 );
//        pOvPtr--; *pOvPtr = 1;

    if ( !oexVERIFY_PTR( pStr = str1.OexAllocate( 12 ) ) )
        return -1;

    oex::zstr::Copy( pStr, "Hello World!" );

    if ( !oexVERIFY( !oex::os::CSys::MemCmp( str1.Ptr(), "Hello World!", str1.Length() ) ) )
        return -2;

    str1.Destroy();

	if ( !oexVERIFY( 0 == str1.Length() ) )
		return -1;
	
	str1 = oexT( "Hello World!" );
	if ( !oexVERIFY( 12 == str1.Length() ) )
		return -2;

	str1 += oexT( " - Goodbye Bugs!" );
	if ( !oexVERIFY( 28 == str1.Length() ) )
		return -3;	

	if ( !oexVERIFY( str1 == "Hello World! - Goodbye Bugs!" ) )
		return -3;	

    pStr = str1.Allocate( 4 );
    oex::zstr::Copy( pStr, oexT( "wxyz" ) );
	if ( !oexVERIFY( str1.Length() == 4 ) )
		return -4;

    // Test replace and binary compare
	if (	!oexVERIFY_PTR( str1.Replace( 'w', '*' ).Ptr() ) || !oexVERIFY( str1 == oexT( "*xyz" ) )
		 || !oexVERIFY_PTR( str1.Replace( 'y', '\x0' ).Ptr() ) || !oexVERIFY( !str1.Compare( "*x\x0z", 4 ) )
		 || !oexVERIFY_PTR( str1.Replace( 'z', '*' ).Ptr() ) || !oexVERIFY( !str1.Compare( "*x\x0*", 4 ) ) )
		return -5;

    str1.Fmt( "lu = %lu, s = %s, f = %f", (oex::oexULONG)11, oexT( "String" ), (oex::oexDOUBLE)3.14f );
	if ( !oexVERIFY( str1 == "lu = 11, s = String, f = 3.140000" ) )
		return -6;

    oex::oexGUID guid;
	str1 = "850A51F0-2CF7-4412-BB83-1AEF68BAD88C";
	if ( !oexVERIFY( str2.GuidToString( str1.StringToGuid( &guid ) ) == str1 ) )
		return -23;

    if ( !oexVERIFY( oex::CStr( "TaBlE" ).ToLower() == "table" ) ||
		 !oexVERIFY( oex::CStr( "cHaIr" ).ToUpper() == "CHAIR" ) )
		return -18;

	str1 = "Test String";
	str2 = ReturnTest( str1 );
	if ( !oexVERIFY( str1.Ptr() == str2.Ptr() ) )
		return -9;

	str1 += " - Make copy";
	if ( !oexVERIFY( str1.Ptr() != str2.Ptr() ) ||
 		 !oexVERIFY( str1 == "Test String - Make copy" ) ||
 		 !oexVERIFY( str2 == "Test String" ) )
		return -10;
	
	str2 = str1.SubStr( 5, 6 );
	if ( !oexVERIFY( str2 == "String" ) )
		return -11;

	// Non-Shared version
	str2 = "Hello World!";
	str2.Sub( 6, 5 );
	if ( !oexVERIFY( str2 == "World" ) )
		return -12;

	// Shared version ( should break the share )
	str2 = str1;
	str2.Sub( 5, 6 );
	if ( !oexVERIFY( str2 == "String" ) )
		return -13;

	str1 = "123456789012345678901234567890";
    if ( !oexVERIFY( str1.Drop( "15", oex::oexTRUE ) == "234678902346789023467890" ) )
		return -19;

	str1 = "123456789012345678901234567890";
	if ( !oexVERIFY( str1.Drop( "15", oex::oexFALSE ) == "151515" ) )
		return -20;

	str1 = "123456789012345678901234567890";
	if ( !oexVERIFY( str1.DropRange( '4', '6', oex::oexTRUE ) == "123789012378901237890" ) )
		return -21;

	str1 = "123456789012345678901234567890";
	if ( !oexVERIFY( str1.DropRange( '4', '6', oex::oexFALSE ) == "456456456" ) )
		return -22;

    str1 = "-+-ABC-+-DEF-+-";

    str1.RTrim( "-+" );
    if ( !oexVERIFY( str1 == "-+-ABC-+-DEF" ) )
        return -24;

    str1.LTrim( "-+" );
    if ( !oexVERIFY( str1 == "ABC-+-DEF" ) )
        return -24;

    str1.RTrim( "DEF" );
    if ( !oexVERIFY( str1 == "ABC-+-" ) )
        return -24;

    if ( !oexVERIFY( 12345 == oex::CStr( "12345" ).ToNum() ) )
        return -5;

    if ( !oexVERIFY( -12345 == oex::CStr( "-12345" ).ToNum() ) )
        return -6;

    if ( !oexVERIFY( 0x1234abcd == oex::CStr( "1234abcd" ).ToNum( 0, 16 ) ) )
        return -7;

    if ( !oexVERIFY( 0x1234abcd == oex::CStr( "0x1234abcd").ToNum( 0, 16 ) ) )
        return -8;

    if ( !oexVERIFY( -0x1234abcd == oex::CStr( "-0x1234abcd" ).ToNum( 0, 16 ) ) )
        return -9;

    str1 = "1234abc";
    oex::oexINT nEnd = 0;
    if ( !oexVERIFY( 1234 == str1.ToNum( 0, 10, &nEnd, oex::oexTRUE ) ) )
        return -10;

    if ( !oexVERIFY( 4 == nEnd ) )
        return -11;

    if ( !oexVERIFY( str1 == "abc" ) )
        return -12;

    str1 = oexT( '1' );
	if ( !oexVERIFY( str1.Length() == 1 ) || !oexVERIFY( str1 == "1" ) )
		return -16;

	str1.Allocate( 0 );
	str1 = 1; str1 += ") PI = "; str1 += 3.14159;
	if ( !oexVERIFY( str1 == "1) PI = 3.14159" ) )
		return -7;
	
	str1.Allocate( 0 );
	str1 << 2 << ") E = " << 2.71f;
	if ( !oexVERIFY( str1 == "2) E = 2.71" ) )
		return -8;

    return oex::oexRES_OK;
}

oex::oexRESULT TestLists()
{      
	// List
	oex::TList< int > lst;

	// Iterator
	oex::TList< int >::iterator it = 
	    lst.Append( 4 );

	// Add nodes
	lst.Append( 8 );
	lst.Append( 16 );
	lst.Append( 32 );
	lst.Append( 64 );

	// *** Verify size
	if ( !oexVERIFY( lst.Size() == 5 ) )
		return -1;

	int a = *it;
	int b = *(it++);
	int c = *(it--);
	int d = *(it--);

	// *** Verify list values
	if (	!oexVERIFY( 4 == a ) || !oexVERIFY( 8 == b )
		 || !oexVERIFY( 4 == c ) || !oexVERIFY( 4 == d ) )
		return -2;

	// List values
	int vals[] = { 4, 8, 16, 32, 64, -1 };

	// *** iteration method #1
	int i = 0;
	do
	{	
		if ( !oexVERIFY( it.Obj() == vals[ i++ ] ) )
			return -3;

	} while ( it.Next() );

	// *** iteration method #2
	i = 0;
	for ( oex::TList< int >::iterator it2; lst.Next( it2 ); )
	{
		if ( !oexVERIFY( it2.Obj() == vals[ i++ ] ) )
			return -4;

	} // end if

	// *** reverse iteration method #1
	i = 4;
	it = lst.Last();
	do
	{	
		if ( !oexVERIFY( it.Obj() == vals[ i-- ] ) ||
             !oexVERIFY( -1 <= i ) )
			return -5;

	} while ( it.Prev() );

	// *** reverse iteration method #2
	i = 4;
	for ( oex::TList< int >::iterator it3; lst.Prev( it3 ); )
	{
		if ( !oexVERIFY( it3.Obj() == vals[ i-- ] ) ||
             !oexVERIFY( -1 <= i ) )
			return -6;

	} // end if

    // Destroy the list
	lst.Destroy();

    // Iterator should not be in the list now
	if ( !oexVERIFY( it.Begin() && it.End() ) )
		return -7;

    // Verify iterator still has valid memory
	if ( !oexVERIFY( it.Obj() == 4 ) )
		return -8;

    oex::TList< oex::CStr > strlst;
	oex::oexCSTR szStr[] = { oexT( "This" ), oexT( "is" ), oexT( "a" ), oexT( "list" ), oexT( "of" ), oexT( "strings" ) , oexNULL };

	// Create list
	strlst << oexT( "This" ) << oexT( "is" ) << oexT( "a" ) << oexT( "list" ) << oexT( "of" ) << oexT( "strings" );

	i = 0;
	for ( oex::TList< oex::CStr >::iterator itStr; szStr[ i ] && strlst.Next( itStr ); i++ )
		if ( !oexVERIFY( itStr->Cmp( szStr[ i ] ) ) )
			return -7;

	oex::TList< oex::CStr >::iterator itStr;
	if ( !oexVERIFY( ( itStr = strlst.SearchValue( "hi", oex::CStr::CmpSubStr ) ).IsValid() ) 
		 || !oexVERIFY( *itStr == szStr[ 0 ] ) )
		return -8;

    oex::TList< oex::CStr > strlst2( strlst );

	// First list should be empty now
	if ( !oexVERIFY( !strlst.Size() ) )
		return -8;

	// Verify assignment went ok
	i = 0; 
	for ( oex::TList< oex::CStr >::iterator itStr; szStr[ i ] && strlst2.Next( itStr ); i++ )
		if ( !oexVERIFY( itStr->Cmp( szStr[ i ] ) ) )
			return -9;

	strlst2.Destroy();

	// From string array
	strlst.Append( szStr, 6 );
	i = 0;
    for ( oex::TList< oex::CStr >::iterator itStr; szStr[ i ] && strlst.Next( itStr ); i++ )
		if ( !oexVERIFY( itStr->Cmp( szStr[ i ] ) ) )
			return -10;

	// Attempt to make a copy
	strlst2 = strlst.Copy();
    
	i = 0;
    for ( oex::TList< oex::CStr >::iterator itStr; szStr[ i ] && strlst.Next( itStr ); i++ )
		if ( !oexVERIFY( itStr->Cmp( szStr[ i ] ) ) )
			return -10;
	i = 0;
	for ( oex::TList< oex::CStr >::iterator itStr; szStr[ i ] && strlst2.Next( itStr ); i++ )
		if ( !oexVERIFY( itStr->Cmp( szStr[ i ] ) ) )
			return -10;

	strlst.Destroy();

	i = 0;
	strlst << 1 << 3.14f << "String";
    oex::oexCSTR szStr2[] = { "1", "3.14", "String", oexNULL };
    for ( oex::TList< oex::CStr >::iterator itStr; szStr2[ i ] && strlst.Next( itStr ); i++ )
		if ( !oexVERIFY( itStr->Cmp( szStr2[ i ] ) ) )
			return -10;

    return oex::oexRES_OK;
}

oex::oexRESULT TestAssoLists()
{      
    oex::TAssoList< oex::oexINT, oex::oexINT > alii;

	alii[ 4 ] = 11;
	alii[ 5000 ] = 22;
	alii[ 600000 ] = 33;

	if ( !oexVERIFY( 3 == alii.Size() ) )
		return -1;

	if (	!oexVERIFY( 11 == alii[4 ] ) 
			|| !oexVERIFY( 22 == alii[ 5000 ] )
			|| !oexVERIFY( 33 == alii[ 600000 ] ) )
		return -2;


	oex::TAssoList< oex::CStr, oex::oexINT > alsi;

	alsi[ "Idx 1" ] = 11;
	alsi[ "Idx 2" ] = 22;
	alsi[ "Idx 3" ] = 33;

	if ( !oexVERIFY( 3 == alsi.Size() ) )
		return -3;

	if (	!oexVERIFY( 11 == alsi[ "Idx 1" ] ) 
			|| !oexVERIFY( 22 == alsi[ "Idx 2" ] )
			|| !oexVERIFY( 33 == alsi[ "Idx 3" ] ) )
		return -4;

	oex::TAssoList< oex::CStr, oex::CStr > alss;

	alss[ "Idx 1" ] = "11";
	alss[ "Idx 2" ] = "22";
	alss[ "Idx 3" ] = 33;
	alss[ "Idx 4" ] = 3.141f;

//	if ( !oexVERIFY( CParser::Implode( alss.Copy(), "," ) == "11,22,33,3.141" ) )
//		return -5;

	oex::TAssoList< oex::CStr, oex::CStr >::iterator itSs;
    if ( !oexVERIFY( ( itSs = alss.SearchKey( "2", oex::CStr::CmpSubStr ) ).IsValid() ) 
		 || !oexVERIFY( *itSs == "22" ) )
		return -6;

	if ( !oexVERIFY( 4 == alss.Size() ) )
		return -7;

	if (	!oexVERIFY( alss[ "Idx 1" ] == "11" ) 
			|| !oexVERIFY( alss[ "Idx 2" ] == "22" )
			|| !oexVERIFY( alss[ "Idx 3" ].ToLong() == 33 )
			|| !oexVERIFY( alss[ "Idx 4" ].ToDouble() > 3 )
			|| !oexVERIFY( alss[ "Idx 4" ].ToDouble() < 4 ) )
		return -8;

	oex::TAssoList< oex::CStr, oex::CStr > alss2 = alss;

	if ( !oexVERIFY( !alss.Size() ) )
		return -9;

	if (	!oexVERIFY( alss2[ "Idx 1" ] == "11" ) 
			|| !oexVERIFY( alss2[ "Idx 2" ] == "22" )
			|| !oexVERIFY( alss2[ "Idx 3" ].ToLong() == 33 )
			|| !oexVERIFY( alss2[ "Idx 4" ].ToDouble() > 3 )
			|| !oexVERIFY( alss2[ "Idx 4" ].ToDouble() < 4 ) )
		return -10;

	oex::TAssoList< oex::CStr, oex::CStr > alss3 = alss2.Copy();

	if ( !oexVERIFY( 4 == alss2.Size() ) || !oexVERIFY( 4 == alss3.Size() ) )
		return -11;

	if (	!oexVERIFY( alss2[ "Idx 1" ] == "11" ) 
			|| !oexVERIFY( alss2[ "Idx 2" ] == "22" )
			|| !oexVERIFY( alss2[ "Idx 3" ].ToLong() == 33 )
			|| !oexVERIFY( alss2[ "Idx 4" ].ToDouble() > 3 )
			|| !oexVERIFY( alss2[ "Idx 4" ].ToDouble() < 4 ) )
		return -12;

	if (	!oexVERIFY( alss3[ "Idx 1" ] == "11" ) 
			|| !oexVERIFY( alss3[ "Idx 2" ] == "22" )
			|| !oexVERIFY( alss3[ "Idx 3" ].ToLong() == 33 )
			|| !oexVERIFY( alss3[ "Idx 4" ].ToDouble() > 3 )
			|| !oexVERIFY( alss3[ "Idx 4" ].ToDouble() < 4 ) )
		return -13;

	oex::TAssoList< oex::CStr, oex::TAssoList< oex::CStr, oex::CStr > > alsss;

	alsss[ "1" ][ "a" ] = "1a";
	alsss[ "1" ][ "b" ] = "1b";
	alsss[ "2" ][ "a" ] = "2a";
	alsss[ "3" ][ "a" ] = "3a";

	if ( !oexVERIFY( 2 == alsss[ "1" ].Size() ) 
		 || !oexVERIFY( 1 == alsss[ "2" ].Size() )
		 || !oexVERIFY( 1 == alsss[ "3" ].Size() )  )
		return -14;

	if (	!oexVERIFY( alsss[ "1" ][ "a" ] == "1a" ) 
		 || !oexVERIFY( alsss[ "1" ][ "b" ] == "1b" ) 
		 || !oexVERIFY( alsss[ "2" ][ "a" ] == "2a" ) 
		 || !oexVERIFY( alsss[ "3" ][ "a" ] == "3a" ) )
		 return -15;

    if ( !oexVERIFY( alsss.IsKey( "1" ) ) )
        return -16;

    alsss.Unset( "1" );
    if ( !oexVERIFY( !alsss.IsKey( "1" ) ) )
        return -18;

    return oex::oexRES_OK;
}

oex::oexRESULT TestPropertyBag()
{      
    oex::CPropertyBag pb;

	pb[ "1" ] = "1";
	pb[ "1" ][ "a" ] = "1a";
	pb[ "1" ][ "b" ] = "1b";
	pb[ "1" ][ "c" ] = "1c";
	pb[ "2" ][ "a" ] = "2a";
	pb[ "2" ][ "b" ] = "2b";

	pb[ "n" ][ 1 ] = 11;
	pb[ "n" ][ 2 ] = 22;

	pb[ "c" ][ "pi" ] = 3.14159f;
	pb[ "c" ][ "e" ] = 2.71828f;
	pb[ "c" ][ "phi" ] = 1.618f;
			
	if ( !oexVERIFY( pb[ "1" ] == "1" ) )
		return -1;

	if ( !oexVERIFY( pb[ "n" ][ 1 ].ToLong() == 11 ) )
		return -2;
	
    if ( !oexVERIFY( pb[ "n" ][ 2 ] == 22 ) )
        return -2;

	if ( !oexVERIFY( pb[ "c" ][ "pi" ] == 3.14159f ) )
		return -3;

    if ( !oexVERIFY( oex::CParser::Implode( pb[ "1" ].List(), "," ) == "1a,1b,1c" ) )
		return -4;

	oex::TPropertyBag< oex::oexINT, oex::oexINT > pbii;

	pbii[ 2 ] = 2;

	oex::TPropertyBag< oex::oexINT, oex::CStr > pbis;

	pbis[ 2 ] = "2";

    return oex::oexRES_OK;
}

oex::oexRESULT TestParser()
{      
	// Test explode function
    oex::oexCSTR szStr[] = { "This", "is", "a", "string", 0 };

    oex::TList< oex::CStr > lst = oex::CParser::Explode( "This---is---a---string", "---" );

	oex::oexUINT i = 0;
	for ( oex::TList< oex::CStr >::iterator it; szStr[ i ] && lst.Next( it ); i++ )
		if ( !oexVERIFY( it->Cmp( szStr[ i ] ) ) )
			return -1;

	// Test single char splitting
	lst = oex::CParser::Explode( "1234567890", "" );
	if ( !oexVERIFY( lst.Size() == 10 ) )
		return -2;

	// Implode check
	if ( !oexVERIFY( oex::CParser::Implode( lst.Copy(), "," ) == "1,2,3,4,5,6,7,8,9,0" ) )
		return -3;

	oex::oexINT tp = 0;
	oex::CStr str1 = oex::CStr::NextToken( "1234567890", "45", &tp );
	if ( !oexVERIFY( str1 == "45" ) || !oexVERIFY( 5 == tp ) )
		return -17;

	lst = oex::CParser::GetTokens( "we12 can 34 read56the789__numbers", "1234567890" );
	if ( !oexVERIFY( oex::CParser::Implode( lst.Copy(), "," ) == "12,34,56,789" ) )
		return -18;

	lst = oex::CParser::Split( "This&is#a??sentence", "&#?" );
	if ( !oexVERIFY( oex::CParser::Implode( lst, "," ) == "This,is,a,sentence" ) )
		return -19;

	oex::oexCSTR pStr = "Hello World !@#$%^&*()-=";
	if ( !oexVERIFY( oex::CParser::UrlDecode( oex::CParser::UrlEncode( pStr ) ) == pStr ) )
		return -20;

	pStr = "a=b&c=d&e=";
	if ( !oexVERIFY( oex::CParser::EncodeUrlParams( oex::CParser::DecodeUrlParams( pStr ) ) == pStr ) )
		return -21;

	pStr = "a=b&c=d&e=hello%20world";
	if ( !oexVERIFY( oex::CParser::EncodeUrlParams( oex::CParser::DecodeUrlParams( pStr ) ) == pStr ) )
		return -22;

    oex::CPropertyBag pb;

    pb[ "1" ] = "111";
    pb[ "2" ] = "222";
    pb[ "3" ] = "333";

    oex::CStr sStr = oex::CParser::Serialize( pb );
    oex::CPropertyBag pb2 = oex::CParser::Deserialize( sStr );
    if ( !oexVERIFY( oex::CParser::Serialize( pb2 ) == sStr ) )
        return -23;

    pb2 = ReturnTest( pb );

    if ( !oexVERIFY( oex::CParser::Serialize( pb2 ) == sStr ) )
        return -24;

    if ( !oexVERIFY( !pb.Size() ) )
        return -25;    
     
    pb2.Destroy();

	pb[ "1" ] = "1";
	pb[ "1" ][ "a" ] = "1a";
	pb[ "1" ][ "b" ] = "1b";
	pb[ "1" ][ "c" ] = "1c";
	pb[ "2" ][ "a" ] = "2a";
	pb[ "2" ][ "b" ] = "2b";

	pb[ "n" ][ 1 ] = 11;
	pb[ "n" ][ 2 ] = 22;

	pb[ "c" ][ "pi" ] = 3.14159f;
	pb[ "c" ][ "e" ] = 2.71828f;
	pb[ "c" ][ "phi" ] = 1.618f;

    sStr = oex::CParser::Serialize( pb );
    pb2 = oex::CParser::Deserialize( sStr );
    oex::CStr sStr2 = oex::CParser::Serialize( pb2 );
    if ( !oexVERIFY( oex::CParser::Serialize( pb2 ) == sStr ) )
       return -24;

    pb.Destroy();

    pb[ "1" ] = "111";
    pb[ "2" ] = "222";
    pb[ "3" ] = "333";

    pb2[ "x" ] = pb;

    if ( !oexVERIFY( pb2[ "x" ][ "1" ] == 111 ) )
        return -23;

    if ( !oexVERIFY( 5 == pb2.Size() ) || !oexVERIFY( 0 == pb.Size() ) )
        return -24;

    sStr = oex::CParser::Serialize( pb2 );

    pb = ReturnTest( pb2 );

    if ( !oexVERIFY( 5 == pb.Size() ) || !oexVERIFY( 0 == pb2.Size() ) )
        return -25;

    if ( !oexVERIFY( oex::CParser::Serialize( pb ) == sStr ) )
        return -26;

    pb2 = ReturnTest( pb.Copy() );

    if ( !oexVERIFY( oex::CParser::Serialize( pb ) == sStr ) )
        return -27;

    if ( !oexVERIFY( oex::CParser::Serialize( pb2 ) == sStr ) )
        return -28;

    pb = oex::CParser::Deserialize( "x{a=b}" );

    if ( !oexVERIFY( pb.IsKey( "x" ) ) )
        return -29;

    if ( !oexVERIFY( pb[ "x" ].IsKey( "a" ) ) )
        return -30;

    if ( !oexVERIFY( pb[ "x" ][ "a" ].ToString() == "b" ) )
        return -31;
        
/*
    oex::CPropertyBag url = oex::CParser::DecodeUrlParams( "a=b&c=d&e=&f" );

	oex::CPropertyBag url_enc = oex::CParser::CompileTemplate( "_pre_ : [url] = {url} / _sep_ . _end_ ;" );

	oexTRACE( "%s\n", oex::CParser::Encode( url, url_enc ).Ptr() );

	// "begin . []={} . sep . end"

	// "pre / []={} . sep / post"

	// "(open) []={} / sep (post)"

	// "pre : []={} / sep . post

	oex::CPropertyBag pb;

	pb[ "1" ] = "1";
	pb[ "1" ][ "a" ] = "1a";
	pb[ "1" ][ "b" ] = "1b";
	pb[ "1" ][ "c" ] = "1c";
	pb[ "2" ][ "a" ] = "2a";
	pb[ "2" ][ "b" ] = "2b";

	oex::CPropertyBag ini_enc = 
		oex::CParser::CompileTemplate(	"		<[> [url] <]> _%n_ {#sub} _%n_ .;"	oexNL
									"#sub +	[url] = {url} /_%n_ .;"				oexNL
									"#sub -	<*> = {url} /_%n_ .;"				oexNL
								);

	oexTRACE( "%s\n", ini_enc.PrintR().Ptr() );

//		oexTRACE( "%s\n", oex::CParser::Encode( pb, ini_enc ).Ptr() );
*/

    return oex::oexRES_OK;
}

oex::oexRESULT TestFile()
{      
    oex::CFile f;
    oex::CStr sFileName, sContents = "Safe to delete this file.";

    // Create file name
    sFileName << "C:/" << oex::CStr().GuidToString() << ".txt";

    if ( !oexVERIFY( f.CreateNew( sFileName.Ptr() ).IsOpen() ) )
        return -1;

    if ( !oexVERIFY( f.Write( sContents ) ) )
        return -2;

    f.SetPtrPosBegin( 0 );

    if ( !oexVERIFY( f.Read() == sContents ) )
        return -4;

    if ( !oexVERIFY( f.Delete() ) )
        return -5;

    // WARNING: Recursive delete test!
//        CFile::DeletePath( "C:/temp" );

    return oex::oexRES_OK;
}

oex::oexRESULT TestZip()
{      
    oex::CStr sStr = "This string will be compressed.  It has to be fairly long or the"
                "compression library won't really be able to compress it much.  "
                "I also had to add more text so I could get a zero in the compressed data.  "
                "Now is the time for all good men to come to the aid of their country";

    oex::CStr sCmp = oex::zip::CCompress::Compress( sStr );

    if ( !oexVERIFY( sCmp.Length() ) || !oexVERIFY( sCmp.Length() < sStr.Length() ) )
        return -1;

    // Verify raw compression
    if ( !oexVERIFY( sStr == oex::zip::CUncompress::Uncompress( sCmp ) ) )
        return -2;

    return oex::oexRES_OK;
}

oex::oexRESULT Test_CSysTime()
{      
    oex::CSysTime st;

    st.SetTime( 1997, 12, 25, 16, 15, 30, 500, 4 );

//        oexTRACE( "%s\n", st.FormatTime( "%W, %B %D, %Y - %h:%m:%s %A" ).Ptr() );
//        oexTRACE( "%s\n", st.FormatTime( "%Y/%c/%d - %g:%m:%s.%l" ).Ptr() );
//        oexTRACE( "%s\n", st.FormatTime( "%w, %d %b %Y %g:%m:%s GMT" ).Ptr() );

    if ( !oexVERIFY( st.FormatTime( "%W, %B %D, %Y - %h:%m:%s %A" ) == "Thursday, December 25, 1997 - 04:15:30 PM" ) )
        return -1;
    
    if ( !oexVERIFY( st.FormatTime( "%Y/%c/%d - %g:%m:%s.%l" ) == "1997/12/25 - 16:15:30.500" ) )
        return -2;

    if ( !oexVERIFY( st.FormatTime( "%w, %d %b %Y %g:%m:%s GMT" ) == "Thu, 25 Dec 1997 16:15:30 GMT" ) )
        return -3;

    oex::CSysTime st2;

    st2.SetMilliSecond( 500 );

    if ( !oexVERIFY( st2.ParseTime( "%W, %B %D, %Y - %h:%m:%s %A", "Thursday, December 25, 1997 - 04:15:30 PM" ) ) )
        return -4;

    if ( !oexVERIFY( st2.GetYear() == 1997 && st2.GetMonth() == 12 && st2.GetDay() == 25
                     && st2.Get12Hour() == 4 && st2.GetHour() == 16 && st2.GetMinute() == 15 && st2.GetSecond() == 30 ) )
        return -5;

    if ( !oexVERIFY( st2 == st ) )
        return -6;

    if ( !oexVERIFY( st2.ParseTime( "%Y/%c/%d - %g:%m:%s.%l", "1997/12/25 - 16:15:30.500" ) ) )
        return -7;

    if ( !oexVERIFY( st2 == st ) )
        return -8;

    if ( !oexVERIFY( st2.ParseTime( "%w, %d %b %Y %g:%m:%s GMT", "Thu, 25 Dec 1997 16:15:30 GMT" ) ) )
        return -9;

    if ( !oexVERIFY( st2 == st ) )
        return -10;

    return oex::oexRES_OK;
}


oex::oexRESULT Test_CIpAddress()
{
    if ( !oexVERIFY( oex::os::CIpSocket::InitSockets() ) )
        return -1;

    oex::oexCSTR pUrl = "http://user:password@server.com:1111/my/path/doc.php?a=b&c=d";
    oex::CPropertyBag pbUrl = oex::os::CIpAddress::ParseUrl( pUrl );        

    // Verify each component
    if ( !oexVERIFY( pbUrl[ "scheme" ] == "http" )
         || !oexVERIFY( pbUrl[ "username" ] == "user" )
         || !oexVERIFY( pbUrl[ "password" ] == "password" )
         || !oexVERIFY( pbUrl[ "host" ] == "server.com" )
         || !oexVERIFY( pbUrl[ "port" ] == "1111" )
         || !oexVERIFY( pbUrl[ "path" ] == "/my/path/doc.php" )
         || !oexVERIFY( pbUrl[ "extra" ] == "a=b&c=d" ) )
        return -2;

    if ( !oexVERIFY( oex::os::CIpAddress::BuildUrl( pbUrl ) == pUrl ) )
        return -3;

    oex::os::CIpAddress ia;

    if ( !oexVERIFY( ia.LookupUrl( "http://user:password@localhost:1111/my/path/doc.php?a=b&c=d" ) ) 
         || !oexVERIFY( ia.GetDotAddress() == "127.0.0.1" ) 
         || !oexVERIFY( ia.GetPort() == 1111 ) )
        return -4;
        
    if ( !oexVERIFY( ia.LookupHost( "localhost", 2222 ) ) 
         || !oexVERIFY( ia.GetDotAddress() == "127.0.0.1" )
         || !oexVERIFY( ia.GetPort() == 2222 ) )
        return -5;

    ia.LookupHost( "google.com", 80 );
    oexTRACE( "%s\n", ia.GetId().Ptr() );

    oex::os::CIpSocket::UninitSockets();

    return oex::oexRES_OK;
}

oex::oexRESULT Test_CIpSocket()
{
    if ( !oexVERIFY( oex::os::CIpSocket::InitSockets() ) )
        return -1;

    // *** TCP

    oex::os::CIpSocket server, session, client;

    if ( !oexVERIFY( server.Bind( 23456 ) ) )
        return -2;

    if ( !oexVERIFY( server.Listen() ) )
        return -3;

    if ( !oexVERIFY( client.Connect( "localhost", 23456 ) ) )
        return -4;

    if ( !oexVERIFY( server.WaitEvent( oex::os::CIpSocket::eAcceptEvent, oexDEFAULT_TIMEOUT ) ) )
        return -5;

    if ( !oexVERIFY( server.Accept( session ) ) )
        return -6;

    if ( !oexVERIFY( session.WaitEvent( oex::os::CIpSocket::eConnectEvent, oexDEFAULT_TIMEOUT ) ) 
         || !oexVERIFY( client.WaitEvent( oex::os::CIpSocket::eConnectEvent, oexDEFAULT_TIMEOUT ) ) )
        return -7;

    oex::oexCSTR pStr = "B6F5FF3D-E9A5-46ca-ADB8-D655427EB94D";

    if ( !oexVERIFY( session.Send( pStr ) ) )
        return -8;

    if ( !oexVERIFY( client.WaitEvent( oex::os::CIpSocket::eReadEvent, oexDEFAULT_TIMEOUT ) ) )
        return -9;

    if ( !oexVERIFY( client.Recv() == pStr ) )
        return -10;

    client.Destroy();
    if ( !oexVERIFY( session.WaitEvent( oex::os::CIpSocket::eCloseEvent, oexDEFAULT_TIMEOUT ) ) )
        return -11;
            
    session.Destroy();
    server.Destroy();


    // *** UDP

    if ( !oexVERIFY( server.Create( oex::os::CIpSocket::eAfInet, oex::os::CIpSocket::eTypeDgram, oex::os::CIpSocket::eProtoUdp ) )
         || !oexVERIFY( server.Bind( 12345 ) ) )
        return -12;

    if ( !oexVERIFY( client.Create( oex::os::CIpSocket::eAfInet, oex::os::CIpSocket::eTypeDgram, oex::os::CIpSocket::eProtoUdp ) ) )
        return -13;

    if ( !oexVERIFY( client.PeerAddress().LookupHost( "localhost", 12345 ) ) )
        return -14;

    if ( !oexVERIFY( client.SendTo( pStr ) ) )
        return -15;

    if ( !oexVERIFY( server.WaitEvent( oex::os::CIpSocket::eReadEvent, oexDEFAULT_TIMEOUT ) ) )
        return -16;

    if ( !oexVERIFY( server.RecvFrom() == pStr ) )
        return -17;

    client.Destroy();
    server.Destroy();

    oex::os::CIpSocket::UninitSockets();

    return oex::oexRES_OK;
}

oex::oexRESULT Test_CCircBuf()
{
    oex::CCircBuf cb;

    oex::oexCSTR pStr = oexT( "Hello World" );
    oex::oexUINT uStr = oex::zstr::Length( pStr );
    oex::oexUINT uBufferedData = 0;

    for ( oex::oexUINT i = 0; i < 1000; i++ )
    {
        for ( oex::oexUINT x = 0; x < 8; x++ )
            cb.Write( pStr ), uBufferedData += uStr;

        if ( !oexVERIFY( cb.Read( uStr ) == pStr ) )
            return -1;

        uBufferedData -= uStr;

    } // end for

    if ( !oexVERIFY( cb.GetMaxRead() == uBufferedData ) )
        return -2;

    // Shared memory buffer test
    oex::CCircBuf sb1, sb2;

    sb1.SetName( oex::CStr().GuidToString().Ptr() );
    sb2.SetName( sb1.GetName() );

    sb1.Allocate( 100000 );
    sb2.Allocate( 100000 );

    uBufferedData = 0;
    for ( oex::oexUINT i = 0; i < 1000; i++ )
    {
        for ( oex::oexUINT x = 0; x < 8; x++ )
            sb1.Write( pStr ), uBufferedData += uStr;

        if ( !oexVERIFY( sb2.Read( uStr ) == pStr ) )
            return -3;

        uBufferedData -= uStr;

    } // end for

    if ( !oexVERIFY( sb1.GetMaxRead() == uBufferedData ) )
        return -4;

    return oex::oexRES_OK;
}

oex::oexRESULT Test_CFifoSync()
{
    oex::CFifoSync fs;

    fs.SetMaxBuffers( 10000 );

    oex::oexCSTR pStr = oexT( "Hello World" );
    oex::oexUINT uStr = oex::zstr::Length( pStr );
    oex::oexUINT uBufferedData = 0;

    for ( oex::oexUINT i = 0; i < 10; i++ )
    {
        for ( oex::oexUINT x = 0; x < 8; x++ )
            fs.Write( pStr, uStr ), uBufferedData++;

        if ( !oexVERIFY( fs.Read() == pStr ) )
            return -1;

        uBufferedData--;

    } // end for

    oex::oexUINT uUsed = fs.GetUsedBuffers();
  	if ( !oexVERIFY( fs.GetUsedBuffers() == uBufferedData ) )
        return -2;


    // Shared memory buffer test
    oex::CFifoSync fs1, fs2;

    fs1.SetName( oex::CStr().GuidToString().Ptr() );
    fs2.SetName( fs1.GetName() );

    fs1.Allocate( 100000, 10000 );
    fs2.Allocate( 100000, 10000 );

    uBufferedData = 0;
    for ( oex::oexUINT i = 0; i < 1000; i++ )
    {
        for ( oex::oexUINT x = 0; x < 8; x++ )
            fs1.Write( pStr ), uBufferedData++;

        if ( !oexVERIFY( fs2.Read() == pStr ) )
            return -3;

        uBufferedData--;

    } // end for

  	if ( !oexVERIFY( fs1.GetUsedBuffers() == uBufferedData ) )
        return -2;

    return oex::oexRES_OK;
}

oex::oexRESULT Test_CDataPacket()
{
    oex::CDataPacket dp1, dp2, dp3;

    oex::CStr sStr1 = "This is the first data string";
    oex::CStr sStr2 = "This is the second data string";

    // Write a packet of data
    if ( !oexVERIFY( dp1.WritePacket( 1, 1, sStr1 ) )
         || !oexVERIFY( dp1.WritePacket( 1, 2, sStr2 ) ) )
        return -1;

    // 'Transmit' packet
    if ( !oexVERIFY( dp2.ReadPacket( dp1.Read() ) ) )
        return -2;

    // Verify the data
    if ( !oexVERIFY( dp2.ReadPacketString( 0, 1 ) == sStr1 ) 
         || !oexVERIFY( dp2.SkipPacket() )
         || !oexVERIFY( dp2.ReadPacketString( 0, 2 ) == sStr2 ) )
        return -3;

    dp1.Destroy();
    dp2.Destroy();
    
    // Write a packet of data
    if ( !oexVERIFY( dp1.InitPacket( 1, 2, sStr1.Length() + sStr2.Length() ) )
         || !oexVERIFY( dp1.AddPacketData( 1, sStr1 ) )
         || !oexVERIFY( dp1.AddPacketData( 2, sStr2 ) ) 
         || !oexVERIFY( dp1.EndPacket() ) )
        return -1;

    // 'Transmit' packet
    if ( !oexVERIFY( dp2.ReadPacket( dp1.Read() ) ) )
        return -2;

    // Verify the data
    if ( !oexVERIFY( dp2.ReadPacketString( 0, 1 ) == sStr1 )              
         || !oexVERIFY( dp2.ReadPacketString( 0, 2 ) == sStr2 ) )
        return -3;

    dp1.Destroy();
    dp2.Destroy();

    // Shared memory version
    dp1.SetName( oex::CStr().GuidToString().Ptr() );
    dp2.SetName( dp1.GetName() );

    dp1.Allocate( 10000 );
    dp2.Allocate( 10000 );

    if ( !oexVERIFY( dp1.WritePacket( 1, 1, sStr1 ) )
         || !oexVERIFY( dp1.WritePacket( 1, 2, sStr2 ) ) )
        return -1;

    if ( !oexVERIFY( dp3.ReadPacket( dp2.Read() ) ) )
        return -2;

    // Verify the data
    if ( !oexVERIFY( dp3.ReadPacketString( 0, 1 ) == sStr1 ) 
         || !oexVERIFY( dp3.SkipPacket() )
         || !oexVERIFY( dp3.ReadPacketString( 0, 2 ) == sStr2 ) )
        return -3;

    return oex::oexRES_OK;
}

oex::oexRESULT Test_CThreadPool()
{

    return oex::oexRES_OK;
}


int main(int argc, char* argv[])
{
	// Initialize the oex library
	oexINIT();

    TestAllocator();

    TestFileMapping();

    TestGuids();

    TestStrings();

    TestLists();

    TestAssoLists();

    TestPropertyBag();

    TestParser();

    TestFile();

    TestZip();

    Test_CSysTime();
    
    Test_CCircBuf();

    Test_CIpAddress();

    Test_CIpSocket();

    Test_CFifoSync();

    Test_CDataPacket();

    Test_CThreadPool();

	// Initialize the oex library
    oexUNINIT();	

	return 0;
}

