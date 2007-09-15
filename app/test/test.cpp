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

    for ( int i = 0; i < 100; i++ )
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

    for ( int i = 0; i < 50; i++ )
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
//	oex::TList< int >::iterator it = 
		lst.Append( 4 );

	// Add nodes
	lst.Append( 8 );
	lst.Append( 16 );
	lst.Append( 32 );
	lst.Append( 64 );

	// *** Verify size
	if ( !oexVERIFY( lst.Size() == 5 ) )
		return -1;



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

	// Initialize the oex library
    oexUNINIT();	

	return 0;
}

