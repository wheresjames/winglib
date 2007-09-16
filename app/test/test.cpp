// test.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

class CMyClass
{
public:

    int x;

    CMyClass()
    {
        x = 1234;
    }

    CMyClass( int x_x )
    {
        x = x_x;
    }

    ~CMyClass()
    {
        x = 5678;
    }

};

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
    CMyClass *p = OexAllocConstruct< CMyClass >( 11 );

    if ( !oexVERIFY_PTR( p ) )
        return -7;

    if ( !oexVERIFY( 11 == p->x ) )
        return -8;

    OexAllocDestruct( p );


    // Allocate array
    p = OexAllocConstructArray< CMyClass >( 3, 12 );
    if ( !oexVERIFY( p ) )
        return -9;

    OexAllocDestruct( p );

    { // Scope

        oex::TMem< char > mem;
        
        if ( !oexVERIFY_PTR( mem.New( 13 ).c_Ptr() ) )
            return -10;

        // Check overrun protection
//        mem.OexResize( 12 );

        oex::zstr::Copy( mem.Ptr(), "Hello World!" );

    } // end scope

    { // Scope

        oex::TMem< CMyClass > mem;

        if ( !oexVERIFY_PTR( mem.Construct().c_Ptr() ) )
            return -11;

        if ( !oexVERIFY( 1234 == mem.Ptr()->x ) )
            return -12;

        if ( !oexVERIFY_PTR( mem.Construct( 10 ).c_Ptr() ) )
            return -13;

        if ( !oexVERIFY( 10 == mem.Ptr()->x ) )
            return -14;

        if ( !oexVERIFY_PTR( mem.ConstructArray( 2, 12 ).c_Ptr() ) )
            return -15;

        if ( !oexVERIFY( 12 == mem.Ptr( 0 )->x && 12 == mem.Ptr( 1 )->x ) )
            return -16;

        // Strange allocation method
        CMyClass *pMc = oex::TMem< CMyClass >().Construct().Detach();

        oex::TMem< CMyClass >( pMc ).Delete();

        // Check overrun protection
//        mem.OexResize( 12 );

//        oex::zstr::Copy( mem.Ptr(), "Hello World!" );

    } // end scope

    { // Scope

        oex::TMem< CMyClass > mem;

        if ( !oexVERIFY_PTR( mem.ConstructArray( 4 ).c_Ptr() ) )
            return -17;

        if ( !oexVERIFY( 2 == mem.Resize( 2 ).Size() ) )
            return -18;        

    } // end scope

    return oex::oexRES_OK;
}

oex::oexRESULT TestFileMapping()
{
    oex::TFileMapping< char > fm;

    const oex::oexUINT uSize = 150;
    if ( !oexVERIFY( fm.Create( 0, "Test", uSize ) ) )
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
    if ( !oexVERIFY( fm2.Create( 0, "Test", uSize ) ) )
        return -5;

    if ( !oexVERIFY( fm.Size() == uSize ) )
        return -6;

    if ( !oexVERIFY( !oex::str::Compare( fm.c_Ptr(), 12, "Hello World!", 12 ) ) )
        return -7;


    oex::TMem< CMyClass > mm, mm2;
    
    if ( !oexVERIFY( 10 == mm.SetName( "Name" ).Construct( 10 ).Ptr()->x ) )
        return -8;

    if ( !oexVERIFY( 10 == mm2.SetName( "Name" ).Construct( 11 ).Ptr()->x ) )
        return -9;

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

template < typename T > 
	static T ReturnTest( T t ) { return t; }

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

    if ( !oexVERIFY_PTR( pStr = str1.Allocate( 12 ) ) )
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
		if ( !oexVERIFY( it == vals[ i++ ] ) )
			return -3;

	} while ( it.Next() );

	// *** iteration method #2
	i = 0;
	for ( oex::TList< int >::iterator it2; lst.Next( it2 ); )
	{
		if ( !oexVERIFY( it2 == vals[ i++ ] ) )
			return -4;

	} // end if

	// *** reverse iteration method #1
	i = 4;
	it = lst.Last();
	do
	{	
		if ( !oexVERIFY( it == vals[ i-- ] ) ||
             !oexVERIFY( -1 <= i ) )
			return -5;

	} while ( it.Prev() );

	// *** reverse iteration method #2
	i = 4;
	for ( oex::TList< int >::iterator it3; lst.Prev( it3 ); )
	{
		if ( !oexVERIFY( it3 == vals[ i-- ] ) ||
             !oexVERIFY( -1 <= i ) )
			return -6;

	} // end if

    // Destroy the list
	lst.Destroy();

    // Iterator should not be in the list now
	if ( !oexVERIFY( it.Begin() && it.End() ) )
		return -7;

    // Verify iterator still has valid memory
	if ( !oexVERIFY( it == 4 ) )
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

/*	oex::TAssoList< oex::CStr, oex::CStr >::iterator itSs;
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

//    if ( !oexVERIFY( alsss.Unset( "1" ) ) )
//        return -17;

    if ( !oexVERIFY( !alsss.IsKey( "1" ) ) )
        return -18;
*/
    return oex::oexRES_OK;
}

oex::oexRESULT TestPropertyBag()
{      
/*    oex::CPropertyBag pb;

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

//	if ( !oexVERIFY( CParser::Implode( pb[ "1" ].List(), "," ) == "1a,1b,1c" ) )
//		return -4;


	oex::TPropertyBag< oex::oexINT, oex::oexINT > pbii;

	pbii[ 2 ] = 2;


	oex::TPropertyBag< oex::oexINT, oex::CStr > pbis;

	pbis[ 2 ] = "2";
*/
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

	// Initialize the oex library
    oexUNINIT();	

	return 0;
}

