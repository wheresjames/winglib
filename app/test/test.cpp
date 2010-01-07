// test.cpp : Defines the entry point for the console application.
//

/*

Im trying to tell you something about my life
Maybe give me insight between black and white
And the best thing youve ever done for me
Is to help me take my life less seriously
Its only life after all
Yeah

Well darkness has a hunger thats insatiable
And lightness has a call thats hard to hear
I wrap my fear around me like a blanket
I sailed my ship of safety till I sank it
Im crawling on your shores

I went to the doctor, I went to the mountains
I looked to the children, I drank from the fountains
Theres more than one answer to these questions
Pointing me in a crooked line
And the less I seek my source for some definitive
(the less I seek my source)
The closer I am to fine
The closer I am to fine

And I went to see the doctor of philosophy
With a poster of rasputin and a beard down to his knee
He never did marry or see a b-grade movie
He graded my performance, he said he could see through me
I spent four years prostrate to the higher mind
Got my paper and I was free

I went to the doctor, I went to the mountains
I looked to the children, I drank from the fountains
Theres more than one answer to these questions
Pointing me in a crooked line
The less I seek my source for some definitive
(the less I seek my source)
The closer I am to fine
The closer I am to fine

I stopped by the bar at 3 a.m.
To seek solace in a bottle or possibly a friend
And I woke up with a headache like my head against a board
Twice as cloudy as Id been the night before
And I went in seeking clarity.

I went to the doctor, I went to the mountains
I looked to the children, I drank from the fountains
Yeah we go to the doctor, we go to the mountains
We look to the children, we drink from the fountains
Yeah we go to the bible, we go through the workout
We read up on revival and we stand up for the lookout
Theres more than one answer to these questions
Pointing me in a crooked line
The less I seek my source for some definitive
(the less I seek my source)
The closer I am to fine

- Indigo Girls

*/

#define TEST_TCP_PORT	21216

#include "stdafx.h"
#include "stdio.h"

using namespace oex;

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

    public:
        oex::oexUINT         m_uValue;
    };

    template < typename T >
    	static T ReturnTest( const T t ) { return (T)t; }


oex::oexRESULT TestCommon()
{
	oexEcho( oexT( "======== Common..." ) );

	int a[ 100 ]; // = { 0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, ... };
	int sz = sizeof( a ) / sizeof( int );

	for ( int i = 0; i < sz; i++ )
		a[ i ] = i << 1;

	// For every possible sub set of a and value of t in limits of a
	for ( int x = 0; x < sz; x++ )
		for ( int y = x; y < sz; y++ )
			for ( int t = 0; t < ( sz << 1 ); t++ )
			{
				// Try to find t
				int res = oex::cmn::bsearch( &a[ x ], y - x, t );

				// actual index of t in &a[ x ]
				int i = ( t >> 1 ) - x;

				// Would the index be valid?
				if ( x >= y || t & 1 || i < 0 || i >= ( y - x ) )
					i = -1;

				// Did bsearch() get the right answer?
				if ( !oexVERIFY( res == i ) )
				{	oexSHOW( res ); oexSHOW( t ); oexSHOW( i ); oexSHOW( x ); oexSHOW( y );
					return -1;
				} // endi f

			} // end for

    return oex::oexRES_OK;
}

oex::oexRESULT TestAllocator()
{
	oexEcho( oexT( "======== Memory allocator..." ) );

    // Veriry over-run / under-run protection
#if defined( oexDEBUG ) || defined( OEX_ENABLE_RELEASE_MODE_MEM_CHECK )

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

#endif

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

	if ( !oexVERIFY( 1 == oex::CAlloc::GetRefCount( buf ) ) )
		return -2;

    // Add a reference
    if ( !oexVERIFY( 2 == oex::CAlloc::AddRef( buf ) ) )
    	return -3;

	if ( !oexVERIFY( 2 == oex::CAlloc::GetRefCount( buf ) ) )
		return -4;

    // Delete once
    if ( !oexVERIFY( 1 == OexAllocDelete( buf ) ) )
    	return -5;

	if ( !oexVERIFY( 1 == oex::CAlloc::GetRefCount( buf ) ) )
		return -6;

	// Delete twice
    if ( !oexVERIFY( 0 == OexAllocDelete( buf ) ) )
    	return -7;

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
        const oexSIZE_T uTestSize = 1000;
        for ( oexSIZE_T i = 0; i < uTestSize; i++ )
        {
            if ( !oexVERIFY_PTR( mem.OexResize( i + 1 ).c_Ptr() ) )
                return -20;

            *mem.Ptr( i ) = (char)i;

        } // end for

        // Verify the memory data
        for ( oexSIZE_T i = 0; i < uTestSize; i++ )
            if ( !oexVERIFY( *mem.Ptr( i ) == (char)i ) )
                return -21;

    } // end scope

	// Test alignment functions
	for ( oexSIZE_T i = 0; i < 100; i++ )
		if ( !oexVERIFY( 0 == ( oex::cmn::Align2( i ) & 1 ) )
			 || !oexVERIFY( 0 == ( oex::cmn::Align4( i ) & 3 ) )
			 || !oexVERIFY( 0 == ( oex::cmn::Align8( i ) & 7 ) )
			 || !oexVERIFY( 0 == ( oex::cmn::Align16( i ) & 15 ) ) )
			return -22;

	// Test alignment functions
	for ( oexSIZE_T i = 0; i < 100; i++ )
		if ( !oexVERIFY( oex::cmn::IsAligned2( oex::cmn::Align2( i ) ) )
			 || !oexVERIFY( oex::cmn::IsAligned4( oex::cmn::Align4( i ) ) )
			 || !oexVERIFY( oex::cmn::IsAligned8( oex::cmn::Align8( i ) ) )
			 || !oexVERIFY( oex::cmn::IsAligned16( oex::cmn::Align16( i ) ) ) )
			return -22;

// +++ Get this working in WinCE
#if !defined( OEX_WINCE )

	// Declare aligned buffers
	oexAligned( oexCHAR, bufp[ 2 ] ) = { 0 };
	oexAligned2( oexCHAR, buf2[ 2 ] ) = { 0 };
	oexAligned4( oexCHAR, buf4[ 2 ] ) = { 0 };
	oexAligned8( oexCHAR, buf8[ 2 ] ) = { 0 };
	oexAligned16( oexCHAR, buf16[ 2 ] ) = { 0 };

	// Verify buffer alignments
	if ( !oexVERIFY( oexIsAligned( (oexLONG)bufp ) )
		 || !oexVERIFY( oexIsAligned2( (oexLONG)buf2 ) )
		 || !oexVERIFY( oexIsAligned4( (oexLONG)buf4 ) )
		 || !oexVERIFY( oexIsAligned8( (oexLONG)buf8 ) )
		 || !oexVERIFY( oexIsAligned16( (oexLONG)buf16 ) )
	   )
		return -23;
#endif

    return oex::oexRES_OK;
}

oex::oexRESULT TestBinary()
{
	oexEcho( oexT( "======== Binary buffers..." ) );

	oex::CBin b1;

	if ( !oexVERIFY( b1.Allocate( 128 ) ) )
		return -1;

	if ( !oexVERIFY( 128 == b1.Size() ) )
		return -2;

	if ( !oexVERIFY( 0 == b1.getUsed() ) )
		return -3;

	if ( !oexVERIFY( b1.MemCpy( "Hello", 5 ) ) )
		return -4;

	if ( !oexVERIFY( 5 == b1.getUsed() ) )
		return -5;

	if ( !oexVERIFY( !oex::os::CSys::MemCmp( b1.Ptr(), "Hello", b1.getUsed() ) ) )
		return -6;

	// !!! Careful, this string may *not* null terminated
	//     It's ok here, operator == is binary compatible
	//     but if you use TStr::Ptr(), you may have issues
	if ( !oexVERIFY( b1.getString() == "Hello" ) )
		return -7;

	if ( !oexVERIFY( b1.getSafeString() == "Hello" ) )
		return -7;

	if ( !oexVERIFY( 5 == b1.setString( "olleH" ) ) )
		return -8;

	if ( !oexVERIFY( !oex::os::CSys::MemCmp( b1.Ptr(), "olleH", b1.getUsed() ) ) )
		return -9;

	if ( !oexVERIFY( 10 == b1.AppendBuffer( "Hello", 5 ) ) )
		return 10;

	if ( !oexVERIFY( !oex::os::CSys::MemCmp( b1.Ptr(), "olleHHello", b1.getUsed() ) ) )
		return -11;

	if ( !oexVERIFY( 1 == oexGetRefCount( b1.Ptr() ) ) )
		return -12;

	oex::CBin b2( b1 );

	if ( !oexVERIFY( b1.Ptr() == b2.Ptr() ) )
		return -13;

	if ( !oexVERIFY( 2 == oexGetRefCount( b1.Ptr() ) ) )
		return -14;

	b2.Destroy();

	if ( !oexVERIFY( 1 == oexGetRefCount( b1.Ptr() ) ) )
		return -15;

	b2 = b1;

	if ( !oexVERIFY( b1.Ptr() == b2.Ptr() ) )
		return -16;

	if ( !oexVERIFY( 2 == oexGetRefCount( b1.Ptr() ) ) )
		return -17;

	b2.Copy();

	if ( !oexVERIFY( 1 == oexGetRefCount( b1.Ptr() ) ) )
		return -18;

	b1.Destroy();

	oex::CBin::t_byte* pChar = OexAllocNew< oex::CBin::t_byte >( 100 );

	if ( !oexVERIFY( 1 == oexGetRefCount( pChar ) ) )
		return -19;

	b2.setBuffer( pChar, 100 );

	if ( !oexVERIFY( pChar == b2.Ptr() ) )
		return -20;

	if ( !oexVERIFY( 100 == b2.getUsed() ) )
		return -21;

	if ( !oexVERIFY( 2 == oexGetRefCount( pChar ) ) )
		return -22;

	OexAllocDelete< oex::CBin::t_byte >( pChar );

	if ( !oexVERIFY( 1 == oexGetRefCount( pChar ) ) )
		return -23;

	b2.Destroy();


	if ( !oexVERIFY( 5 == b1.setString( "Hello" ) ) )
		return -24;

	if ( !oexVERIFY( oexSetBin( oexT( "TestBuffer" ), &b1 ) ) )
		return -25;

	if ( !oexVERIFY( oexIsBin( oexT( "TestBuffer" ) ) ) )
		return -26;

	if ( !oexVERIFY( !oexIsBin( oexT( "WrongBuffer" ) ) ) )
		return -27;

	b2 = oexGetBin( oexT( "WrongBuffer" ) );

	if ( !oexVERIFY( !b2.Ptr() && !b2.getUsed() ) )
		return -28;

	b2 = oexGetBin( oexT( "TestBuffer" ) );

	if ( !oexVERIFY( b1.Ptr() == b2.Ptr() ) )
		return -29;

	if ( !oexVERIFY( 5 == b2.getUsed() ) )
		return -30;

	CStr8 s = b2.getString();

	if ( !oexVERIFY( s.Ptr() == b2.Ptr() ) )
		return -31;

	if ( !oexVERIFY( s == "Hello" ) )
		return -32;

	oexCPVOID ptr = b1.Ptr();

	if ( !oexVERIFY( 4 == oexGetRefCount( ptr ) ) )
		return -33;

	s.Destroy();

	if ( !oexVERIFY( 3 == oexGetRefCount( ptr ) ) )
		return -34;

	b2.Destroy();

	if ( !oexVERIFY( 2 == oexGetRefCount( ptr ) ) )
		return -35;

	oexSetBin( oexT( "TestBuffer" ), 0 );

	if ( !oexVERIFY( 1 == oexGetRefCount( ptr ) ) )
		return -36;

	b1.Destroy();

	return oex::oexRES_OK;
}

oex::oexRESULT TestStrings()
{
	oexEcho( oexT( "======== String functions..." ) );

    oex::CStr str1, str2;
    oex::oexSTR pStr;

		// Buffer over-run protection test
//        oex::oexSTR pOvPtr = str1.Allocate( 4 );
//        oex::zstr::Copy( pOvPtr, "12345" );
//        str1.Length();

		// Buffer under-run protection test
//        oex::oexSTR pUnPtr = str1.Allocate( 4 );
//        pUnPtr--; *pUnPtr = 1;

	pStr = str1.OexAllocate( 12 );
    if ( !oexVERIFY_PTR( pStr ) )
        return -1;

    oex::zstr::Copy( pStr, oexT( "Hello World!" ) );

    if ( !oexVERIFY( !oex::os::CSys::MemCmp( str1.Ptr(), oexT( "Hello World!" ), str1.Length() ) ) )
        return -2;

    str1.Destroy();

	if ( !oexVERIFY( 0 == str1.Length() ) )
		return -3;

	str1 = oexT( "Hello World!" );
	if ( !oexVERIFY( 12 == str1.Length() ) )
		return -4;

	str1 += oexT( " - Goodbye Bugs!" );
	if ( !oexVERIFY( 28 == str1.Length() ) )
		return -5;

	if ( !oexVERIFY( str1 == oexT( "Hello World! - Goodbye Bugs!" ) ) )
		return -6;

    pStr = str1.OexAllocate( 4 );
    oex::zstr::Copy( pStr, oexT( "wxyz" ) );
	if ( !oexVERIFY( str1.Length() == 4 ) )
		return -7;

    // Test replace and binary compare
	if (	!oexVERIFY_PTR( str1.Replace( 'w', '*' ).Ptr() ) || !oexVERIFY( str1 == oexT( "*xyz" ) )
		 || !oexVERIFY_PTR( str1.Replace( 'y', '\x0' ).Ptr() ) || !oexVERIFY( !str1.Compare( oexT( "*x\x0z" ), 4 ) )
		 || !oexVERIFY_PTR( str1.Replace( 'z', '*' ).Ptr() ) || !oexVERIFY( !str1.Compare( oexT( "*x\x0*" ), 4 ) ) )
		return -8;

    str1.Fmt( oexT( "d = %d" ), (oex::oexINT)9 );
	if ( !oexVERIFY( str1 == oexT( "d = 9" ) ) )
		return -9;

    str1.Fmt( oexT( "u = %u" ), (oex::oexULONG)11 );
	if ( !oexVERIFY( str1 == oexT( "u = 11" ) ) )
		return -9;

    str1.Fmt( oexT( "s = %s" ), oexT( "String" ) );
	if ( !oexVERIFY( str1 == oexT( "s = String" ) ) )
		return -9;

    str1.Fmt( oexT( "f = %f" ), (oex::oexDOUBLE)3.14f );
	if ( !oexVERIFY( str1 == oexT( "f = 3.140000" ) ) )
		return -9;

    str1.Fmt( oexT( "u = %u, s = %s, f = %f" ), (oex::oexULONG)11, oexT( "String" ), (oex::oexDOUBLE)3.14f );
	if ( !oexVERIFY( str1 == oexT( "u = 11, s = String, f = 3.140000" ) ) )
		return -9;

    oex::oexGUID guid;
    oexZeroMemory( &guid, sizeof( guid ) );
	oexCSTR pTest = oexT( "01234567-8901-2345-6789-012345678901" );

	static const oexGUID guidTest =
//		{ 0x01234567, 0x8901, 0x2345, { 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x89, 0x01 } };
		oexINITGUID( 0x01234567, 0x8901, 0x2345, 0x67, 0x89, 0x01, 0x23, 0x45, 0x67, 0x89, 0x01 );

	if ( !oexVERIFY( guid::CmpGuid( guid::StringToGuid( &guid, pTest, 36 ), &guidTest ) ) )
		return -10;

	if ( !oexVERIFY( oex::CStr( pTest ).GuidToString( &guidTest ) == pTest ) )
		return -10;

	str1 = oexT( "850A51F0-2CF7-4412-BB83-1AEF68BAD88C" );
	if ( !oexVERIFY( str2.GuidToString( str1.StringToGuid( &guid ) ) == str1 ) )
		return -10;

    if ( !oexVERIFY( oex::CStr( oexT( "TaBlE" ) ).ToLower() == oexT( "table" ) )
         || !oexVERIFY( oex::CStr( oexT( "cHaIr" ) ).ToUpper() == oexT( "CHAIR" ) ) )
		return -11;

    if ( !oexVERIFY( oex::CStr( oexT( "Hello" ) ).Reverse() == oexT( "olleH" ) ) )
        return -12;

	str1 = oexT( "Test String" );
	str2 = ReturnTest( str1 );
	if ( !oexVERIFY( str1.Ptr() == str2.Ptr() ) )
		return -13;

	str1 += oexT( " - Make copy" );
	if ( !oexVERIFY( str1.Ptr() != str2.Ptr() ) ||
 		 !oexVERIFY( str1 == oexT( "Test String - Make copy" ) ) ||
 		 !oexVERIFY( str2 == oexT( "Test String" ) ) )
		return -14;

	// Shared version ( should break the share )
	str2 = str1.SubStr( 5, 6 );
	if ( !oexVERIFY( str2 == oexT( "String" ) ) )
		return -15;

	// Non-Shared version
	str2 = oexT( "Hello World!" );
	str2.Sub( 6, 5 );
	if ( !oexVERIFY( str2 == oexT( "World" ) ) )
		return -16;

	str1 = oexT( "Hello World!" );
	str1++; str1++;
	if ( !oexVERIFY( str1 == oexT( "llo World!" ) ) )
		return -16;

	str1.Append( oexT( "123" ) );
	if ( !oexVERIFY( str1 == oexT( "llo World!123" ) ) )
		return -16;

	if ( !oexVERIFY( ( oex::CStr( oexT( "abcdef" ) ).LTrim( 2 ) << oexT( "ghi" ) ) == oexT( "cdefghi" ) ) )
		return -16;

    str2 = oexT( "  hello  " );
    str2.TrimWhiteSpace();
    if ( !oexVERIFY( str2 == oexT( "hello" ) ) )
        return -17;

    str2 = str2;
    if ( !oexVERIFY( str2 == oexT( "hello" ) ) )
        return -18;

	str1 = oexT( "123456789012345678901234567890" );
    if ( !oexVERIFY( str1.Drop( oexT( "15" ), oex::oexTRUE ) == oexT( "234678902346789023467890" ) ) )
		return -20;

	str1 = oexT( "123456789012345678901234567890" );
	if ( !oexVERIFY( str1.Drop( oexT( "15" ), oex::oexFALSE ) == oexT( "151515" ) ) )
		return -21;

	str1 = oexT( "123456789012345678901234567890" );
	if ( !oexVERIFY( str1.DropRange( '4', '6', oex::oexTRUE ) == oexT( "123789012378901237890" ) ) )
		return -22;

	str1 = oexT( "123456789012345678901234567890" );
	if ( !oexVERIFY( str1.DropRange( '4', '6', oex::oexFALSE ) == oexT( "456456456" ) ) )
		return -23;

    str1 = oexT( "-+-ABC-+-DEF-+-" );

    str1.RTrim( oexT( "-+" ) );
    if ( !oexVERIFY( str1 == oexT( "-+-ABC-+-DEF" ) ) )
        return -24;

    str1.LTrim( oexT( "-+" ) );
    if ( !oexVERIFY( str1 == oexT( "ABC-+-DEF" ) ) )
        return -25;

    str1.RTrim( oexT( "DEF" ) );
    if ( !oexVERIFY( str1 == oexT( "ABC-+-" ) ) )
        return -26;

    if ( !oexVERIFY( 12345 == oex::CStr( oexT( "12345" ) ).ToNum() ) )
        return -27;

    if ( !oexVERIFY( -12345 == oex::CStr( oexT( "-12345" ) ).ToNum() ) )
        return -28;

    if ( !oexVERIFY( 0x1234abcd == oex::CStr( oexT( "1234abcd" ) ).ToNum( 0, 16 ) ) )
        return -29;

    if ( !oexVERIFY( 0x1234abcd == oex::CStr( oexT( "0x1234abcd" ) ).ToNum( 0, 16 ) ) )
        return -30;

    if ( !oexVERIFY( -0x1234abcd == oex::CStr( oexT( "-0x1234abcd" ) ).ToNum( 0, 16 ) ) )
        return -31;

    str1 = oexT( "1234abc" );
    oexSIZE_T nEnd = 0;
    if ( !oexVERIFY( 1234 == str1.ToNum( 0, 10, &nEnd, oex::oexTRUE ) ) )
        return -32;

    if ( !oexVERIFY( 4 == nEnd ) )
        return -33;

    if ( !oexVERIFY( str1 == oexT( "abc" ) ) )
        return -34;

    str1 = oexT( '1' );
	if ( !oexVERIFY( str1.Length() == 1 ) || !oexVERIFY( str1 == oexT( "1" ) ) )
		return -35;

	str1.Allocate( 0 );
	str1 = 1; str1 += oexT( ") PI = " ); str1 += 3.14159;
	if ( !oexVERIFY( str1 == oexT( "1) PI = 3.14159" ) ) )
		return -36;

	str1.Allocate( 0 );
	str1 << 2 << oexT( ") E = " ) << 2.71f;
	if ( !oexVERIFY( str1 == oexT( "2) E = 2.71" ) ) )
		return -37;

    str1 = oexT( "c:/temp/myfile.txt" );

    if ( !oexVERIFY( str1.GetPath() == oexT( "c:/temp" ) ) )
		return -38;

    if ( !oexVERIFY( str1.GetFileName() == oexT( "myfile.txt" ) ) )
		return -39;

    str1 = oexT( "c:/temp//\\/myfile.txt" );

    if ( !oexVERIFY( str1.GetPath() == oexT( "c:/temp" ) ) )
		return -40;

    if ( !oexVERIFY( str1.GetFileName() == oexT( "myfile.txt" ) ) )
		return -41;

	// Test string conversions
	str2 = str1.GuidToString();
	if ( !oexVERIFY( str2 == oexMbToStr( oexStrToMb( str1 ) ) ) )
		return -42;

	if ( !oexVERIFY( !oex::zstr::Compare( str2.Ptr(), oexMbToStrPtr( oexStrToMbPtr( str1.Ptr() ) ) ) ) )
		return -43;

	str2 = str1.GuidToString();
	if ( !oexVERIFY( str2 == oexStr8ToStr( oexStrToStr8( str1 ) ) ) )
		return -44;

	if ( !oexVERIFY( !oex::zstr::Compare( str2.Ptr(), oexStr8ToStrPtr( oexStrToStr8Ptr( str1.Ptr() ) ) ) ) )
		return -45;

#if !defined( OEX_NOWCHAR )
	str2 = str1.GuidToString();
	if ( !oexVERIFY( str2 == oexStrWToStr( oexStrToStrW( str1 ) ) ) )
		return -46;

	if ( !oexVERIFY( !oex::zstr::Compare( str2.Ptr(), oexStrWToStrPtr( oexStrToStrWPtr( str1.Ptr() ) ) ) ) )
		return -47;
#endif

	str2 = str1.GuidToString();
	if ( !oexVERIFY( str2 == oexBinToStr( oexStrToBin( str1 ) ) ) )
		return -48;

	// +++ Check into why this fails, at least in unicode
//	if ( !oexVERIFY( !oex::zstr::Compare( str2.Ptr(), oexBinToStrPtr( oexStrToBinPtr( str1.Ptr() ) ) ) ) )
//		return -49;

	str1 = oexT( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
	if ( !oexVERIFY( str1.Replace( oexT( "ABC" ), oexT( "123" ) ) == oexT( "123DEFGHIJKLMNOPQRSTUVWXYZ" ) )
	     || !oexVERIFY( str1.Replace( oexT( "LMNO" ), oexT( "1234" ) ) == oexT( "ABCDEFGHIJK1234PQRSTUVWXYZ" ) )
	     || !oexVERIFY( str1.Replace( oexT( "XYZ" ), oexT( "123" ) ) == oexT( "ABCDEFGHIJKLMNOPQRSTUVW123" ) )
	     || !oexVERIFY( str1.Replace( oexT( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ), oexT( "123" ) ) == oexT( "123" ) )
	     || !oexVERIFY( oex::CStr( oexT( "my new script" ) ).Replace( oexT( " " ), oexT( "_" ) ) == oexT( "my_new_script" ) )
		 )
		return -49;

	if ( !oexVERIFY( !oex::CStr( oexT( "/" ) ).LTrim( oexT( "/" ) ).Length() )
	     || !oexVERIFY( !oex::CStr( oexT( "/\\\\" ) ).LTrim( oexT( "/\\" ) ).Length() )
	     || !oexVERIFY( !oex::CStr( oexT( "/" ) ).RTrim( oexT( "/" ) ).Length() )
	     || !oexVERIFY( !oex::CStr( oexT( "/\\\\" ) ).RTrim( oexT( "/\\" ) ).Length() ) )
		return -50;

	if ( !oexVERIFY( ( oex::CStr( oexT( "123" ) ) << oexT( "456" ) ) == oexT( "123456" ) ) )
		return -51;

	if ( !oexVERIFY( ( oex::CStr( oexT( "abc/def" ) ).GetFileName() << oexT( "ghi" ) ) == oexT( "defghi" ) ) )
		return -52;

	str1 = oexT( "ABCDEFGHIJKLMNOPQRSTUVWXYZ" );
	if ( !oexVERIFY( 0 <= str1.FindSubStr( oexT(  "DEFG" ) ) ) )
		return -53;

	if ( !oexVERIFY( 0 <= oexGetModuleFileName().GetFileName().FindSubStr( oexT( "test" ) ) ) )
		return -54;

	if ( !oexVERIFY( oex::CStr( oexT( "path" ) ).GetPath() == oexT( "" ) ) )
		return -55;

	if ( !oexVERIFY( oex::CStr( oexT( "test.png" ) ).GetFileName() == oexT( "test.png" ) ) )
		return -56;

	if ( !oexVERIFY( oex::CStr( oexT( "test.png" ) ).GetFileExtension() == oexT( "png" ) ) )
		return -57;

	if ( !oexVERIFY( oex::CStr( oexT( "somedirectory.hi/test.png" ) ).GetFileExtension() == oexT( "png" ) ) )
		return -58;

	if ( !oexVERIFY( oex::CStr( oexT( "somedirectory.hi/test.not.png" ) ).GetFileExtension() == oexT( "png" ) ) )
		return -59;

	if ( !oexVERIFY( oex::CBase16::Decode( oex::CBase16::Encode( "Hello World!" ) ) == "Hello World!" ) )
		return -60;

	if ( !oexVERIFY( oex::CBase64::Decode( oex::CBase64::Encode( "Hello World!" ) ) == "Hello World!" ) )
		return -60;

	str1 = oexT( "1.2.3" );

	if ( !oexVERIFY( str1.Parse( oexT( "." ) ) == oexT( "1" ) ) )
		return -61;

	if ( !oexVERIFY( str1 == oexT( ".2.3" ) ) )
		return -62;

	if ( !oexVERIFY( str1.Length() == 4 ) )
		return -63;

	str1.Skip( oexT( "." ) );

	if ( !oexVERIFY( str1 == oexT( "2.3" ) ) )
		return -64;

	if ( !oexVERIFY( str1.Length() == 3 ) )
		return -65;


	str1 = oexT( "HTTP/1.1" );

	if ( !oexVERIFY( CParser::ParseToken( str1.SkipWhiteSpace(), CStrList() << oexT( "HTTP" ), oexFALSE ) == oexT( "HTTP" ) ) )
		return -66;

	if ( !oexVERIFY( *str1 == oexT( '/' ) ) )
		return 67;

	str1++;

	if ( !oexVERIFY( str1.ParseNextToken() == oexT( "1.1" ) ) )
		return 68;


	// +++ This caused a crash somehow, note sSub passed as parameter
	// oex::CStr sSub = oexBuildPath( sRoot, oexBuildPath( sSub, sScript.c_str() ) );

    return oex::oexRES_OK;
}

oex::oexRESULT TestFileMapping()
{
	oexEcho( oexT( "======== File mapping..." ) );

    oex::TFileMapping< oex::oexTCHAR > fm;

    const oexFILESIZE_T uSize = 150;
    if ( !oexVERIFY( fm.Create( 0, 0, 0, oexT( "Test" ), uSize ) ) )
        return -1;

    if ( !oexVERIFY( fm.Size() == uSize ) )
        return -2;

    // !!! This won't pass for every case, make sure there is
    //     enough room beyond uSize for the block overhead
//    if ( !oexVERIFY( oex::cmn::NextPower2( uSize * sizeof( oex::oexTCHAR ) ) == fm.BlockSize() ) )
//        return -3;

    oex::zstr::Copy( fm.Ptr(), oexT( "Hello World!" ) );

    if ( !oexVERIFY( !oex::str::Compare( fm.c_Ptr(), 12, oexT( "Hello World!" ), 12 ) ) )
        return -4;


    oex::TFileMapping< oex::oexTCHAR > fm2;
    if ( !oexVERIFY( fm2.Create( 0, 0, 0, oexT( "Test" ), uSize ) ) )
        return -5;

    if ( !oexVERIFY( fm.Size() == uSize ) )
        return -6;

    if ( !oexVERIFY( !oex::str::Compare( fm.c_Ptr(), 12, oexT( "Hello World!" ), 12 ) ) )
        return -7;


    CTestMonitor tm;
    oex::TMem< CBaseTestObject > mm, mm2;

    if ( !oexVERIFY(  mm.SetName( oexT( "Name" ) ).OexConstruct( &tm ).Ptr() ) )
        return -8;

//    mm->SetValue( 111 );
    mm->m_uValue = 111;

    if ( !oexVERIFY( mm2.SetName( oexT( "Name" ) ).OexConstruct( &tm ).Ptr() ) )
        return -9;

    if ( !oexVERIFY( 1 == tm.m_uConstructed ) )
        return -12;

    if ( !oexVERIFY( 111 == mm2->GetValue() ) )
        return -9;

    mm.Delete();

    if ( !oexVERIFY( 0 == tm.m_uDestructed ) )
        return -11;

    mm2.Delete();

    if ( !oexVERIFY( 1 == tm.m_uDestructed ) )
        return -12;

    return oex::oexRES_OK;
}


oex::oexRESULT TestGuids()
{
	oexEcho( oexT( "======== GUIDS..." ) );

    // {6674C3D8-BB11-4a58-BCE0-A34DC74365AF}
    static const oex::oexGUID guidTest =
    	oexINITGUID( 0x6674c3d8, 0xbb11, 0x4a58, 0xbc, 0xe0, 0xa3, 0x4d, 0xc7, 0x43, 0x65, 0xaf );

    oex::oexGUID    guid1, guid2;
    oex::oexTCHAR   szGuid1[ 1024 ] = oexT( "" );

//	oexSHOW( guidTest );

    // Guid / String conversions
    oex::guid::GuidToString( szGuid1, oexSizeOfArray( szGuid1 ), &guidTest );
    oex::guid::StringToGuid( &guid1, szGuid1, oexSizeOfArray( szGuid1 ) );
    if ( !oexVERIFY( oex::guid::CmpGuid( &guidTest, &guid1 ) ) )
        return -1;

    oex::guid::CopyGuid( &guid2, &guid1 );
    if ( !oexVERIFY( oex::guid::CmpGuid( &guid1, &guid2 ) ) )
        return -2;

    oex::guid::SetGuid( &guid1, 0, 0, sizeof( guid1 ) );
    oex::guid::OrGuid( &guid1, &guid2 );
    if ( !oexVERIFY( oex::guid::CmpGuid( &guid1, &guid2 ) ) )
        return -3;

    oex::guid::SetGuid( &guid1, 0xffffffff, 0, sizeof( guid1 ) );
    oex::guid::AndGuid( &guid1, &guid2 );
    if ( !oexVERIFY( oex::guid::CmpGuid( &guid1, &guid2 ) ) )
        return -4;

    oex::guid::XorGuid( &guid1, &guid2 );
    oex::guid::SetGuid( &guid2, 0, 0, sizeof( guid2 ) );
    if ( !oexVERIFY( oex::guid::CmpGuid( &guid1, &guid2 ) ) )
        return -5;

    return oex::oexRES_OK;
}


/*
struct A
{
	A() { oexEcho( "A()" ); }
	A( const A &a ) { oexEcho( "A( copy )" ); }
	~A() { oexEcho( "~A()" ); }
	A& operator = ( const A &a ) { oexEcho( "A assignment" ); return *this; }
};

struct B
{
	B() { oexEcho( "B()" ); }
//	B( const B &b ) { oexEcho( "B( copy )" ); }
	~B() { oexEcho( "~B()" ); }
//	B& operator = ( const B &b ) { oexEcho( "B assignment" ); return *this; }

	A a;
};
*/

oex::oexRESULT TestLists()
{
	oexEcho( oexT( "======== Linked lists..." ) );

/*
	ECHO( "----------- TMem -----------" );

	TList< B > l;

	l << B();

	ECHO( "---" );
	l << B();

	ECHO( "---" );
	l << B();

	ECHO( "---" );
	l.Erase( l[ 1 ] );
*/

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
		if ( !oexVERIFY( 0 <= i )
			 || !oexVERIFY( it.Obj() == vals[ i ] ) )
			return -5;

		i--;

	} while ( it.Prev() );

	// *** reverse iteration method #2
	i = 4;
	for ( oex::TList< int >::iterator it3; lst.Prev( it3 ); )
	{
		if ( !oexVERIFY( 0 <= i )
			 || !oexVERIFY( it3.Obj() == vals[ i ] ) )
			return -6;

		i--;

	} // end if

    lst.Insert( 11 );

    if( !oexVERIFY( lst.First().Obj() == 11 ) )
        return -7;

	oex::TList< int > lst_2;

    oex::TList< int >::iterator itMove = lst.First();

    lst.Remove( itMove );
    lst_2.Insert( itMove );

    if( !oexVERIFY( lst.First().Obj() == 4 ) )
        return -8;

    if( !oexVERIFY( lst_2.First().Obj() == 11 ) )
        return -9;

    // Append second list onto the first
    lst.Append( lst_2 );

    if( !oexVERIFY( lst.Last().Obj() == 11 ) )
        return -10;

    if( !oexVERIFY( !lst_2.Size() ) )
        return -11;

    // Destroy the list
	lst.Destroy();

    // Iterator should not be in the list now
	if ( !oexVERIFY( it.Begin() && it.End() ) )
		return -7;

    // Verify iterator still has valid memory
	if ( !oexVERIFY( it.Obj() == 4 ) )
		return -8;

    // String list test
    oex::TList< oex::CStr > strlst;
	oex::oexCSTR szStr[] = { oexT( "This" ), oexT( "is" ), oexT( "a" ), oexT( "list" ), oexT( "of" ), oexT( "strings" ) , oexNULL };

    // Add two items to the list
    strlst.Append( vals, 2 );

	if ( !oexVERIFY( oex::CParser::Implode( strlst, oexT( "," ) ) == oexT( "4,8" ) ) )
		return -9;

    // Swap
    strlst.MoveBefore( strlst.Last(), strlst.First() );
	if ( !oexVERIFY( oex::CParser::Implode( strlst, oexT( "," ) ) == oexT( "8,4" ) ) )
		return -10;

    // Swap
    strlst.MoveAfter( strlst.First(), strlst.Last() );
	if ( !oexVERIFY( oex::CParser::Implode( strlst, oexT( "," ) ) == oexT( "4,8" ) ) )
		return -11;

    // Swap
    strlst.MoveDown( strlst.First() );
	if ( !oexVERIFY( oex::CParser::Implode( strlst, oexT( "," ) ) == oexT( "8,4" ) ) )
		return -12;

    // Swap
    strlst.MoveUp( strlst.Last() );
	if ( !oexVERIFY( oex::CParser::Implode( strlst, oexT( "," ) ) == oexT( "4,8" ) ) )
		return -13;

    // Lose the list
    strlst.Destroy();

	// Create list
	strlst << oexT( "This" ) << oexT( "is" ) << oexT( "a" ) << oexT( "list" ) << oexT( "of" ) << oexT( "strings" );

	i = 0;
	for ( oex::TList< oex::CStr >::iterator itStr; szStr[ i ] && strlst.Next( itStr ); i++ )
		if ( !oexVERIFY( itStr->Cmp( szStr[ i ] ) ) )
			return -7;

	oex::TList< oex::CStr >::iterator itStr;
	if ( !oexVERIFY( ( itStr = strlst.SearchValue( oexT( "hi" ), oex::CStr::CmpSubStr ) ).IsValid() )
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
			return -11;
	i = 0;
	for ( oex::TList< oex::CStr >::iterator itStr; szStr[ i ] && strlst2.Next( itStr ); i++ )
		if ( !oexVERIFY( itStr->Cmp( szStr[ i ] ) ) )
			return -12;

	strlst.Destroy();

	i = 0;
	strlst << 1 << 3.14f << oexT( "String" );
    oex::oexCSTR szStr2[] = { oexT( "1" ), oexT( "3.14" ), oexT( "String" ), oexNULL };
    for ( oex::TList< oex::CStr >::iterator itStr; szStr2[ i ] && strlst.Next( itStr ); i++ )
		if ( !oexVERIFY( itStr->Cmp( szStr2[ i ] ) ) )
			return -13;

	oex::oexCSTR pIni = oexT( "; This is a test INI file"			"\r\n"
							"# [comment]"							"\r\n"
							"val1=Hello"							"\r\n"
							"val2=World"							"\r\n"
							""										"\r\n"
							"[group1]"								"\r\n"
							"val1=yup"								"\r\n"
							"val2=noreturn"
						);

	oex::CPropertyBag pb = oex::CParser::DecodeIni( pIni );
	if ( !oexVERIFY(  pb[ oexT( "val1" ) ].ToString() == oexT( "Hello" ) ) )
		return -20;

	if ( !oexVERIFY(  pb[ oexT( "group1" ) ][ oexT( "val1" ) ].ToString() == oexT( "yup" ) ) )
		return -21;

	if ( !oexVERIFY(  pb[ oexT( "group1" ) ][ oexT( "val2" ) ].ToString() == oexT( "noreturn" ) ) )
		return -22;

	pb = oex::CParser::DecodeIni( oex::CParser::EncodeIni( pb ) );

	if ( !oexVERIFY( pb[ oexT( "val1" ) ].ToString() == oexT( "Hello" ) ) )
		return -23;

	if ( !oexVERIFY( pb[ oexT( "group1" ) ][ oexT( "val1" ) ].ToString() == oexT( "yup" ) ) )
		return -24;

    return oex::oexRES_OK;
}

oex::oexRESULT TestAssoLists()
{
	oexEcho( oexT( "======== Mapped lists..." ) );

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

	alsi[ oexT( "Idx 1" ) ] = 11;
	alsi[ oexT( "Idx 2" ) ] = 22;
	alsi[ oexT( "Idx 3" ) ] = 33;

	if ( !oexVERIFY( 3 == alsi.Size() ) )
		return -3;

	if (	!oexVERIFY( 11 == alsi[ oexT( "Idx 1" ) ] )
			|| !oexVERIFY( 22 == alsi[ oexT( "Idx 2" ) ] )
			|| !oexVERIFY( 33 == alsi[ oexT( "Idx 3" ) ] ) )
		return -4;

	oex::TAssoList< oex::CStr, oex::CStr > alss;

	alss[ oexT( "Idx 1" ) ] = oexT( "11" );
	alss[ oexT( "Idx 2" ) ] = oexT( "22" );
	alss[ oexT( "Idx 3" ) ] = 33;
	alss[ oexT( "Idx 4" ) ] = 3.141f;

//	if ( !oexVERIFY( CParser::Implode( alss.Copy(), "," ) == "11,22,33,3.141" ) )
//		return -5;

	oex::TAssoList< oex::CStr, oex::CStr >::iterator itSs;
    if ( !oexVERIFY( ( itSs = alss.SearchKey( oexT( "2" ), oex::CStr::CmpSubStr ) ).IsValid() )
		 || !oexVERIFY( *itSs == oexT( "22" ) ) )
		return -6;

	if ( !oexVERIFY( 4 == alss.Size() ) )
		return -7;

	if (	!oexVERIFY( alss[ oexT( "Idx 1" ) ] == oexT( "11" ) )
			|| !oexVERIFY( alss[ oexT( "Idx 2" ) ] == oexT( "22" ) )
			|| !oexVERIFY( alss[ oexT( "Idx 3" ) ].ToLong() == 33 )
			|| !oexVERIFY( alss[ oexT( "Idx 4" ) ].ToDouble() > 3 )
			|| !oexVERIFY( alss[ oexT( "Idx 4" ) ].ToDouble() < 4 ) )
		return -8;

	oex::TAssoList< oex::CStr, oex::CStr > alss2 = alss;

	if ( !oexVERIFY( !alss.Size() ) )
		return -9;

	if (	!oexVERIFY( alss2[ oexT( "Idx 1" ) ] == oexT( "11" ) )
			|| !oexVERIFY( alss2[ oexT( "Idx 2" ) ] == oexT( "22" ) )
			|| !oexVERIFY( alss2[ oexT( "Idx 3" ) ].ToLong() == 33 )
			|| !oexVERIFY( alss2[ oexT( "Idx 4" ) ].ToDouble() > 3 )
			|| !oexVERIFY( alss2[ oexT( "Idx 4" ) ].ToDouble() < 4 ) )
		return -10;

	oex::TAssoList< oex::CStr, oex::CStr > alss3 = alss2.Copy();

	if ( !oexVERIFY( 4 == alss2.Size() ) || !oexVERIFY( 4 == alss3.Size() ) )
		return -11;

	if (	!oexVERIFY( alss2[ oexT( "Idx 1" ) ] == oexT( "11" ) )
			|| !oexVERIFY( alss2[ oexT( "Idx 2" ) ] == oexT( "22" ) )
			|| !oexVERIFY( alss2[ oexT( "Idx 3" ) ].ToLong() == 33 )
			|| !oexVERIFY( alss2[ oexT( "Idx 4" ) ].ToDouble() > 3 )
			|| !oexVERIFY( alss2[ oexT( "Idx 4" ) ].ToDouble() < 4 ) )
		return -12;

	if (	!oexVERIFY( alss3[ oexT( "Idx 1" ) ] == oexT( "11" ) )
			|| !oexVERIFY( alss3[ oexT( "Idx 2" ) ] == oexT( "22" ) )
			|| !oexVERIFY( alss3[ oexT( "Idx 3" ) ].ToLong() == 33 )
			|| !oexVERIFY( alss3[ oexT( "Idx 4" ) ].ToDouble() > 3 )
			|| !oexVERIFY( alss3[ oexT( "Idx 4" ) ].ToDouble() < 4 ) )
		return -13;

	oex::TAssoList< oex::CStr, oex::TAssoList< oex::CStr, oex::CStr > > alsss;

	alsss[ oexT( "1" ) ][ oexT( "a" ) ] = oexT( "1a" );
	alsss[ oexT( "1" ) ][ oexT( "b" ) ] = oexT( "1b" );
	alsss[ oexT( "2" ) ][ oexT( "a" ) ] = oexT( "2a" );
	alsss[ oexT( "3" ) ][ oexT( "a" ) ] = oexT( "3a" );

	if ( !oexVERIFY( 2 == alsss[ oexT( "1" ) ].Size() )
		 || !oexVERIFY( 1 == alsss[ oexT( "2" ) ].Size() )
		 || !oexVERIFY( 1 == alsss[ oexT( "3" ) ].Size() )  )
		return -14;

	if (	!oexVERIFY( alsss[ oexT( "1" ) ][ oexT( "a" ) ] == oexT( "1a" ) )
		 || !oexVERIFY( alsss[ oexT( "1" ) ][ oexT( "b" ) ] == oexT( "1b" ) )
		 || !oexVERIFY( alsss[ oexT( "2" ) ][ oexT( "a" ) ] == oexT( "2a" ) )
		 || !oexVERIFY( alsss[ oexT( "3" ) ][ oexT( "a" ) ] == oexT( "3a" ) ) )
		 return -15;

    if ( !oexVERIFY( alsss.IsKey( oexT( "1" ) ) ) )
        return -16;

    alsss.Unset( oexT( "1" ) );
    if ( !oexVERIFY( !alsss.IsKey( oexT( "1" ) ) ) )
        return -18;

    return oex::oexRES_OK;
}

oex::oexRESULT TestPropertyBag()
{
	oexEcho( oexT( "======== Property bags..." ) );

    oex::CPropertyBag pb;

	pb[ oexT( "1" ) ] = oexT( "1" );
	pb[ oexT( "1" ) ][ oexT( "a" ) ] = oexT( "1a" );
	pb[ oexT( "1" ) ][ oexT( "b" ) ] = oexT( "1b" );
	pb[ oexT( "1" ) ][ oexT( "c" ) ] = oexT( "1c" );
	pb[ oexT( "2" ) ][ oexT( "a" ) ] = oexT( "2a" );
	pb[ oexT( "2" ) ][ oexT( "b" ) ] = oexT( "2b" );
	pb[ oexT( "3" ) ][ oexT( "a" ) ][ oexT( "1" ) ] = oexT( "3a1" );

	pb[ oexT( "n" ) ][ 1 ] = 11;
	pb[ oexT( "n" ) ][ 2 ] = 22;

	pb[ oexT( "c" ) ][ oexT( "pi" ) ] = 3.14159f;
	pb[ oexT( "c" ) ][ oexT( "e" ) ] = 2.71828f;
	pb[ oexT( "c" ) ][ oexT( "phi" ) ] = 1.618f;

	if ( !oexVERIFY( pb[ oexT( "1" ) ] == oexT( "1" ) ) )
		return -1;

	if ( !oexVERIFY( pb[ oexT( "n" ) ][ 1 ].ToLong() == 11 ) )
		return -2;

    if ( !oexVERIFY( pb[ oexT( "n" ) ][ 2 ] == 22 ) )
        return -2;

	if ( !oexVERIFY( pb[ oexT( "c" ) ][ oexT( "pi" ) ] == 3.14159f ) )
		return -3;

    if ( !oexVERIFY( oex::CParser::Implode( pb[ oexT( "1" ) ].List(), oexT( "," )) == oexT( "1a,1b,1c" ) ) )
		return -4;

	CStr s = pb.at( oexT( "1.a" ) ).ToString();
	if ( !oexVERIFY( pb.at( oexT( "1.a" ) ).ToString() == oexT( "1a" ) ) )
		return -5;

	if ( !oexVERIFY( pb.at( oex::CStr( oexT( "1/b" ) ), oex::CStr( oexT( "/" ) ) ) == oexT( "1b" ) ) )
		return -6;

	if ( !oexVERIFY( pb.at( oexT( "3.a.1" ) ) == oexT( "3a1" ) ) )
		return -7;

	if ( !oexVERIFY( pb.at( oex::CStr( oexT( "3/a/1" ) ), oex::CStr( oexT( "/" ) ) ) == oexT( "3a1" ) ) )
		return -8;

	oex::TPropertyBag< oex::oexINT, oex::oexINT > pbii;

	pbii[ 2 ] = 2;
    pbii[ 2 ] = 3;

    if ( !oexVERIFY( pbii[ 2 ] == 3 ) )
        return -5;

	oex::TPropertyBag< oex::oexINT, oex::CStr > pbis;

	pbis[ 2 ] = oexT( "2" );

    return oex::oexRES_OK;
}

oex::oexRESULT TestParser()
{
	oexEcho( oexT( "======== Text parsers..." ) );

	// Test explode function
    oex::oexCSTR szStr[] = { oexT( "This" ), oexT( "is" ), oexT( "a" ), oexT( "string" ), 0 };

    oex::TList< oex::CStr > lst = oex::CParser::Explode( oexT( "This---is---a---string" ), oexT( "---" ) );

	oexSIZE_T i = 0;
	for ( oex::TList< oex::CStr >::iterator it; szStr[ i ] && lst.Next( it ); i++ )
		if ( !oexVERIFY( it->Cmp( szStr[ i ] ) ) )
			return -1;

	// Test single char splitting
	lst = oex::CParser::Explode( oexT( "1234567890" ), oexT( "" ) );
	if ( !oexVERIFY( lst.Size() == 10 ) )
		return -2;

	// Implode check
	if ( !oexVERIFY( oex::CParser::Implode( lst.Copy(), oexT( "," ) ) == oexT( "1,2,3,4,5,6,7,8,9,0" ) ) )
		return -3;

	oex::oexINT tp = 0;
	oex::CStr str1 = oex::CStr::NextToken( oexT( "1234567890" ), oexT( "45" ), &tp );
	if ( !oexVERIFY( str1 == oexT( "45" ) ) || !oexVERIFY( 5 == tp ) )
		return -17;

	lst = oex::CParser::GetTokens( oexT( "we12 can 34 read56the789__numbers" ), oexT( "1234567890" ) );
	if ( !oexVERIFY( oex::CParser::Implode( lst.Copy(), oexT( "," ) ) == oexT( "12,34,56,789" ) ) )
		return -18;

	lst = oex::CParser::Split( oexT( "This&is#a??sentence" ), oexT( "&#?" ) );
	if ( !oexVERIFY( oex::CParser::Implode( lst, oexT( "," ) ) == oexT( "This,is,a,sentence" ) ) )
		return -19;

	oex::oexCSTR pStr = oexT( "Hello World !@#$%^&*()-=" );
	if ( !oexVERIFY( oex::CParser::UrlDecode( oex::CParser::UrlEncode( pStr ) ) == pStr ) )
		return -20;

	pStr = oexT( "a=b&c=d&e=" );
	if ( !oexVERIFY( oex::CParser::EncodeUrlParams( oex::CParser::DecodeUrlParams( pStr ) ) == pStr ) )
		return -21;

	pStr = oexT( "a=b&c=d&e=hello%20world" );
	if ( !oexVERIFY( oex::CParser::EncodeUrlParams( oex::CParser::DecodeUrlParams( pStr ) ) == pStr ) )
		return -22;

    oex::CStrList sl1 = oex::CParser::StringPermutations( oex::CStr( oexT( "abc" ) ) );
    if ( !oexVERIFY( 6 == sl1.Size() ) )
        return -23;

    if ( !oexVERIFY( oex::CParser::Implode( sl1, oexT( "," ) ) == oexT( "abc,acb,bac,bca,cba,cab" ) ) )
        return -24;

    oex::CPropertyBag pb;

    pb[ oexT( "1" ) ] = oexT( "111" );
    pb[ oexT( "2" ) ] = oexT( "222" );
    pb[ oexT( "3" ) ] = oexT( "333" );

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

	pb[ oexT( "1" ) ] = oexT( "1" );
	pb[ oexT( "1" ) ][ oexT( "a" ) ] = oexT( "1a" );
	pb[ oexT( "1" ) ][ oexT( "b" ) ] = oexT( "1b" );
	pb[ oexT( "1" ) ][ oexT( "c" ) ] = oexT( "1c" );
	pb[ oexT( "2" ) ][ oexT( "a" ) ] = oexT( "2a" );
	pb[ oexT( "2" ) ][ oexT( "b" ) ] = oexT( "2b" );

	pb[ oexT( "n" ) ][ 1 ] = 11;
	pb[ oexT( "n" ) ][ 2 ] = 22;

	pb[ oexT( "c" ) ][ oexT( "pi" ) ] = 3.14159f;
	pb[ oexT( "c" ) ][ oexT( "e" ) ] = 2.71828f;
	pb[ oexT( "c" ) ][ oexT( "phi" ) ] = 1.618f;

    sStr = oex::CParser::Serialize( pb );
    pb2 = oex::CParser::Deserialize( sStr );
    if ( !oexVERIFY( oex::CParser::Serialize( pb2 ) == sStr ) )
       return -24;

    pb.Destroy();

    pb[ oexT( "1" ) ] = oexT( "111" );
    pb[ oexT( "2" ) ] = oexT( "222" );
    pb[ oexT( "3" ) ] = oexT( "333" );

    pb2[ oexT( "x" ) ] = pb;

    if ( !oexVERIFY( pb2[ oexT( "x" ) ][ oexT( "1" ) ] == 111 ) )
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

    pb = oex::CParser::Deserialize( oexT( "x{a=b,c}" ) );

    if ( !oexVERIFY( pb.IsKey( oexT( "x" ) ) ) )
        return -29;

    if ( !oexVERIFY( pb[ oexT( "x" ) ].IsKey( oexT( "a" ) ) ) )
        return -30;

    if ( !oexVERIFY( pb[ oexT( "x" ) ][ oexT( "a" ) ].ToString() == oexT( "b" ) ) )
        return -31;

    if ( !oexVERIFY( pb[ oexT( "x" ) ].IsKey( oexT( "c" ) ) ) )
        return -32;

    pb = oex::CParser::Deserialize( oexT( "z" ) );

    if ( !oexVERIFY( pb.IsKey( oexT( "z" ) ) ) )
        return -33;

	pb = oex::CParser::Deserialize( oexT( "a = b,\r\n d =\r\n\t{ \r\nx = e, y,\n z, h\re\nl\tl\t o, a%20b }" ) );

    if ( !oexVERIFY( pb.IsKey( oexT( "a" ) ) ) )
        return -34;

    if ( !oexVERIFY( pb[ oexT( "a" ) ].ToString() == oexT( "b" ) ) )
        return -35;

    if ( !oexVERIFY( pb.IsKey( oexT( "d" ) ) ) )
        return -36;

    if ( !oexVERIFY( pb[ oexT( "d" ) ][ oexT( "x" ) ].ToString() == oexT( "e" ) ) )
        return -37;

    if ( !oexVERIFY( pb[ oexT( "d" ) ].IsKey( oexT( "y" ) ) ) )
        return -38;

    if ( !oexVERIFY( pb[ oexT( "d" ) ].IsKey( oexT( "z" ) ) ) )
        return -39;

    if ( !oexVERIFY( pb[ oexT( "d" ) ].IsKey( oexT( "hello" ) ) ) )
        return -40;

    if ( !oexVERIFY( pb[ oexT( "d" ) ].IsKey( oexT( "a b" ) ) ) )
        return -41;

	pb = oex::CParser::Deserialize( oexT( "{a,b,c}" ) );

    if ( !oexVERIFY( pb.IsKey( oexT( "" ) ) ) )
        return -42;

    if ( !oexVERIFY( pb[ oexT( "" ) ].IsKey( oexT( "a" ) ) ) )
        return -43;

    if ( !oexVERIFY( pb[ oexT( "" ) ].IsKey( oexT( "b" ) ) ) )
        return -44;

	if ( !oexVERIFY( pb[ oexT( "" ) ].IsKey( oexT( "c" ) ) ) )
        return -45;

	oex::CStr sCmdLine = oexT( "-a -simple:param -file:'C:/Program Files/myfile.txt' -two:'hi and\\'hi' -empty: hello world" );
	oexINT nParams = CParser::ParseCommandLine( sCmdLine, pb );

    if ( !oexVERIFY( 7 == nParams ) )
        return -32;

    if ( !oexVERIFY( pb[ oexT( "a" ) ].ToString() == oexT( "" ) ) )
        return -33;

    if ( !oexVERIFY( pb[ oexT( "simple" ) ].ToString() == oexT( "param" ) ) )
        return -34;

    if ( !oexVERIFY( pb[ oexT( "file" ) ].ToString() == oexT( "C:/Program Files/myfile.txt" ) ) )
        return -35;

    if ( !oexVERIFY( pb[ oexT( "two" ) ].ToString() == oexT( "hi and\\'hi" ) ) )
        return -36;

    if ( !oexVERIFY( pb[ oexT( "empty" ) ].ToString() == oexT( "" ) ) )
        return -37;

    if ( !oexVERIFY( pb[ oexT( "0" ) ].ToString() == oexT( "hello" ) ) )
        return -38;

    if ( !oexVERIFY( pb[ oexT( "1" ) ].ToString() == oexT( "world" ) ) )
        return -39;

	oexCSTR pCmdLine[] = { oexT( "test.exe" ), oexT( "-d:res res2" ),  oexT( "-o:mydir" ) };
	pb = CParser::ParseCommandLine( 3, pCmdLine );

    if ( !oexVERIFY( pb.Size() == 2 ) )
        return -32;

    if ( !oexVERIFY( pb[ oexT( "d" ) ].ToString() == oexT( "res res2" ) ) )
        return -33;

    if ( !oexVERIFY( pb[ oexT( "o" ) ].ToString() == oexT( "mydir" ) ) )
        return -34;

	oex::CStr sMime = oex::CStr() << oexT( "a: b\r\n" )
								  << oexT( "c: d\r\n" )
								  << oexT( "e: f\r\n" )
								  << oexT( "\r\n" );

	pb = oex::CParser::DecodeMIME( sMime );

    if ( !oexVERIFY( pb[ oexT( "a" ) ].ToString() == oexT( "b" ) ) )
        return -40;

    if ( !oexVERIFY( pb[ oexT( "c" ) ].ToString() == oexT( "d" ) ) )
        return -41;

    if ( !oexVERIFY( pb[ oexT( "e" ) ].ToString() == oexT( "f" ) ) )
        return -42;

	sMime.Destroy();
	sMime << oexT( "a: b\r\n" )
		  << oexT( "c: d\r\n" )
		  << oexT( "e: f" );

	pb = oex::CParser::DecodeMIME( sMime );

    if ( !oexVERIFY( pb[ oexT( "a" ) ].ToString() == oexT( "b" ) ) )
        return -43;

    if ( !oexVERIFY( pb[ oexT( "c" ) ].ToString() == oexT( "d" ) ) )
        return -44;

    if ( !oexVERIFY( pb[ oexT( "e" ) ].ToString() == oexT( "f" ) ) )
        return -45;

	sMime.Destroy();
	sMime << oexT( "a: b\r\n" )
		  << oexT( "c:\r\n" )
		  << oexT( "e: f" );

	pb = oex::CParser::DecodeMIME( sMime );

    if ( !oexVERIFY( pb[ oexT( "a" ) ].ToString() == oexT( "b" ) ) )
        return -46;

    if ( !oexVERIFY( pb[ oexT( "c" ) ].ToString() == oexT( "" ) ) )
        return -47;

    if ( !oexVERIFY( pb[ oexT( "e" ) ].ToString() == oexT( "f" ) ) )
        return -48;
/*
    oex::CPropertyBag url = oex::CParser::DecodeUrlParams( oexT( "a=b&c=d&e=&f" );

	oex::CPropertyBag url_enc = oex::CParser::CompileTemplate( oexT( "_pre_ : [url] = {url} / _sep_ . _end_ ;" );

	oexTRACE( oexT( "%s\n" ), oex::CParser::Encode( url, url_enc ).Ptr() );

	// "begin . []={} . sep . end"

	// "pre / []={} . sep / post"

	// "(open) []={} / sep (post)"

	// "pre : []={} / sep . post

	oex::CPropertyBag pb;

	pb[ oexT( "1" ) ] = oexT( "1" );
	pb[ oexT( "1" ) ][ oexT( "a" ) ] = oexT( "1a" );
	pb[ oexT( "1" ) ][ oexT( "b" ) ] = oexT( "1b" );
	pb[ oexT( "1" ) ][ oexT( "c" ) ] = oexT( "1c" );
	pb[ oexT( "2" ) ][ oexT( "a" ) ] = oexT( "2a" );
	pb[ oexT( "2" ) ][ oexT( "b" ) ] = oexT( "2b" );

	oex::CPropertyBag ini_enc =
		oex::CParser::CompileTemplate(	"		<[> [url] <]> _%d_ {#sub} _%d_ .;"	oexNL
									"#sub +	[url] = {url} /_%d_ .;"				oexNL
									"#sub -	<*> = {url} /_%d_ .;"				oexNL
								);

	oexTRACE( oexT( "%s\n" ), ini_enc.PrintR().Ptr() );

//		oexTRACE( oexT( "%s\n" ), oex::CParser::Encode( pb, ini_enc ).Ptr() );
*/

    return oex::oexRES_OK;
}

oex::oexRESULT TestFile()
{
	oexEcho( oexT( "======== Files..." ) );

    oex::CFile f;
    oex::CStr sFileName, sContents = oexT( "Safe to delete this file." );

    // Create file name
    sFileName << oex::CStr().GuidToString() << oexT( ".txt" );

    if ( !oexVERIFY( f.CreateNew( sFileName.Ptr() ).IsOpen() ) )
        return -1;

    if ( !oexVERIFY( f.Write( oexStrToBin( sContents ) ) ) )
        return -2;

    f.SetPtrPosBegin( 0 );

    if ( !oexVERIFY( oexBinToStr( f.Read() ) == sContents ) )
        return -4;

    if ( !oexVERIFY( f.Delete() ) )
        return -5;

    // WARNING: Recursive delete test!
//        CFile::DeletePath( oexT( "C:/temp" );

    return oex::oexRES_OK;
}


oex::oexRESULT TestZip()
{
#ifdef OEX_ENABLE_ZIP
	oexEcho( oexT( "======== Zip file support..." ) );

    oex::CStr sStr = oexT( "This string will be compressed.  It has to be fairly long or the 			\
                            compression library won't really be able to compress it much.   			\
                            I also had to add more text so I could get a zero in the compressed data.   \
                            Now is the time for all good men to come to the aid of their country.		\
                            Ha, that was kinda random." );

    oex::CStr8 sCmp = oex::zip::CCompress::Compress( oexStrToBin( sStr ) );

    if ( !oexVERIFY( sCmp.Length() ) || !oexVERIFY( sCmp.LengthInBytes() < sStr.LengthInBytes() ) )
        return -1;

    // Verify raw compression
    if ( !oexVERIFY( sStr == oexBinToStr( oex::zip::CUncompress::Uncompress( sCmp ) ) ) )
        return -2;

#else

	oexEcho( oexT( "!!! ZIP file support disabled" ) );

#endif

	// Check high compression text that is larger than internal buffer
	// defined at oss/unzip.h(230)
	oexTCHAR buf[ 1024 * 64 ];
	for ( unsigned int i = 0; i < sizeof( buf ); i++ )
		buf[ i ] = (oexTCHAR)( i % 64 ) + oexT( ' ' );

	sStr.Set( buf, sizeof( buf ) );

	sCmp = oex::zip::CCompress::Compress( oexStrToBin( sStr ) );

    if ( !oexVERIFY( sCmp.Length() ) || !oexVERIFY( sCmp.LengthInBytes() < sStr.LengthInBytes() ) )
        return -3;

	oex::CStr sUn = oexBinToStr( oex::zip::CUncompress::Uncompress( sCmp ) );

	if ( !oexVERIFY( sUn.Length() == sStr.Length() ) )
		return -4;

    // Verify raw compression
    if ( !oexVERIFY( sStr == sUn ) )
        return -5;

    return oex::oexRES_OK;
}

oex::oexRESULT TestResources()
{
	oexEcho( oexT( "======== Resources..." ) );

	// oex::CStr8 res = oexGetResource( "hello.txt" );
	// oexCHAR8 *_p = oexGetResourcePtr( "hello.txt" );
	// oexINT _l = oexGetResourceLen( "hello.txt" );

	if ( !oexVERIFY( oexGetResourceCount() == 4 ) )
		return -1;

	int i = 0;
	while ( oexGetResourcePtr( i ) && oexGetResourceName( i ) != oexT( "res/hello.txt" ) )
		i++;

	if ( !oexVERIFY( oexGetResourceName( i ) == oexT( "res/hello.txt" ) ) )
		return -2;

	if ( !oexVERIFY( oexGetResourceSize( i ) == 14 ) )
		return -3;

	if ( !oexVERIFY_PTR( oexGetResourcePtr( i ) ) )
		return -4;

	if ( !oexVERIFY( !oexGetResourcePtr( 4 ) ) )
		return -5;

	const void *_p = 0; long _l = 0;
	if ( !oexVERIFY( !oexGetResource( i, &_p, &_l ) ) )
		return -6;

	if ( !oexVERIFY( oexMbToStr( oex::zip::CUncompress::Uncompress( (oexCHAR8*)_p, _l ) ) == oexT( "Hello World!" ) ) )
		return -7;

	if ( !oexVERIFY( !oexGetResource( oexT( "res2/hello2.txt" ), &_p, &_l ) ) )
		return -8;

	if ( !oexVERIFY( oexMbToStr( oex::zip::CUncompress::Uncompress( (oexCHAR8*)_p, _l ) ) == oexT( "Hello World!" ) ) )
		return -9;

	// Get image resource
	oex::CStr8 sImg = oexGetResource( oexT( "res/imgs/saturn.jpg" ) );

	if ( !oexVERIFY( 4437 == sImg.Length() ) )
	{	oexSHOW( sImg.Length() );
		return -10;
	} // end if

    return oex::oexRES_OK;
}


oex::oexRESULT Test_CSysTime()
{
	oexEcho( oexT( "======== Time functions..." ) );

	oexEcho( oex::CSysTime().GetLocalTime().FormatTime( oexT( "Current Time: %W, %B %D, %Y - %h:%m:%s %A" ) ).Ptr() );

    oex::CSysTime st;

    st.SetTime( 1997, 12, 25, 16, 15, 30, 500, 0, 0, 4 );

//        oexTRACE( oexT( "%s\n" ), st.FormatTime( oexT( "%W, %B %D, %Y - %h:%m:%s %A" ) ).Ptr() );
//        oexTRACE( oexT( "%s\n" ), st.FormatTime( oexT( "%Y/%c/%d - %g:%m:%s.%l" ).Ptr() );
//        oexTRACE( oexT( "%s\n" ), st.FormatTime( oexT( "%w, %d %b %Y %g:%m:%s GMT" ).Ptr() );

    if ( !oexVERIFY( st.FormatTime( oexT( "%W, %B %D, %Y - %h:%m:%s %A" ) ) == oexT( "Thursday, December 25, 1997 - 04:15:30 PM" ) ) )
        return -1;

    if ( !oexVERIFY( st.FormatTime( oexT( "%Y/%c/%d - %g:%m:%s.%l" ) ) == oexT( "1997/12/25 - 16:15:30.500" ) ) )
        return -2;

    if ( !oexVERIFY( st.FormatTime( oexT( "%w, %d %b %Y %g:%m:%s GMT" ) ) == oexT( "Thu, 25 Dec 1997 16:15:30 GMT" ) ) )
        return -3;

    oex::CSysTime st2;

    st2.SetMilliSecond( 500 );

    if ( !oexVERIFY( st2.ParseTime( oexT( "%W, %B %D, %Y - %h:%m:%s %A" ), oexT( "Thursday, December 25, 1997 - 04:15:30 PM" ) ) ) )
        return -4;

    if ( !oexVERIFY( st2.GetYear() == 1997 && st2.GetMonth() == 12 && st2.GetDay() == 25
                     && st2.Get12Hour() == 4 && st2.GetHour() == 16 && st2.GetMinute() == 15 && st2.GetSecond() == 30 ) )
        return -5;

    if ( !oexVERIFY( st2 == st ) )
        return -6;

    if ( !oexVERIFY( st2.ParseTime( oexT( "%Y/%c/%d - %g:%m:%s" ), oexT( "1997/12/25 - 16:15:30" ) ) ) )
        return -7;

    if ( !oexVERIFY( st2 == st ) )
        return -8;

    if ( !oexVERIFY( st2.ParseTime( oexT( "%w, %d %b %Y %g:%m:%s GMT" ), oexT( "Thu, 25 Dec 1997 16:15:30 GMT" ) ) ) )
        return -9;

    if ( !oexVERIFY( st2 == st ) )
        return -10;

    st2.SetUnixTime( st2.GetUnixTime() );
    if ( !oexVERIFY( st2.FormatTime( oexT( "%Y/%c/%d - %g:%m:%s" ) ) == oexT( "1997/12/25 - 16:15:30" ) ) )
        return -11;

    st2.SetDosTime( st2.GetDosTime() );
    if ( !oexVERIFY( st2.FormatTime( oexT( "%Y/%c/%d - %g:%m:%s" ) ) == oexT( "1997/12/25 - 16:15:30" ) ) )
        return -12;

    st2.SetNetTime( st2.GetNetTime() );
    if ( !oexVERIFY( st2.FormatTime( oexT( "%Y/%c/%d - %g:%m:%s" ) ) == oexT( "1997/12/25 - 16:15:30" ) ) )
        return -13;

    return oex::oexRES_OK;
}

oex::oexRESULT Test_CIpAddress()
{
	oexEcho( oexT( "======== IP Address..." ) );

    oex::oexCSTR pUrl = oexT( "http://user:password@server.com:1111/my/path/doc.php?a=b&c=d" );
    oex::CPropertyBag pbUrl = oex::os::CIpAddress::ParseUrl( pUrl );

//    oexTRACE( pbUrl[ oexT( "scheme" ) ].ToString().Ptr() );

    // Verify each component
    if ( !oexVERIFY( pbUrl[ oexT( "scheme" ) ] == oexT( "http" ) )
         || !oexVERIFY( pbUrl[ oexT( "username" ) ] == oexT( "user" ) )
         || !oexVERIFY( pbUrl[ oexT( "password" ) ] == oexT( "password" ) )
         || !oexVERIFY( pbUrl[ oexT( "host" ) ] == oexT( "server.com" ) )
         || !oexVERIFY( pbUrl[ oexT( "port" ) ] == oexT( "1111" ) )
         || !oexVERIFY( pbUrl[ oexT( "path" ) ] == oexT( "/my/path/doc.php" ) )
         || !oexVERIFY( pbUrl[ oexT( "extra" ) ] == oexT( "a=b&c=d" ) ) )
        return -2;

    if ( !oexVERIFY( oex::os::CIpAddress::BuildUrl( pbUrl ) == pUrl ) )
        return -3;

    oex::os::CIpAddress ia;

    if ( !oexVERIFY( ia.LookupHost( oexT( "localhost" ), 2222 ) )
         || !oexVERIFY( ia.GetDotAddress() == oexT( "127.0.0.1" ) )
         || !oexVERIFY( ia.GetPort() == 2222 ) )
        return -5;

    if ( !oexVERIFY( ia.LookupUrl( oexT( "http://user:password@localhost:1111/my/path/doc.php?a=b&c=d" ) ) )
         || !oexVERIFY( ia.GetDotAddress() == oexT( "127.0.0.1" ) )
         || !oexVERIFY( ia.GetPort() == 1111 ) )
        return -4;

    ia.LookupHost( oexT( "google.com" ), 80 );
    oexPrintf( oexT( "google.com = %s = %s\n" ), oexStrToMbPtr( ia.GetDotAddress().Ptr() ), oexStrToMbPtr( ia.GetId().Ptr() ) );

    return oex::oexRES_OK;
}

// Three seconds should be more than enough for our test
#define SOCKET_TIMEOUT	( 3 * 1000 )

oex::oexRESULT Test_CIpSocket()
{
	oexEcho( oexT( "======== Socket support..." ) );

    // *** TCP

    oex::os::CIpSocket server, session, client;

    if ( !oexVERIFY( server.Bind( TEST_TCP_PORT ) ) )
        return -2;

    if ( !oexVERIFY( server.Listen() ) )
        return -3;

    if ( !oexVERIFY( client.Connect( oexT( "127.0.0.1" ), TEST_TCP_PORT ) ) )
        return -4;

    if ( !oexVERIFY( server.WaitEvent( oex::os::CIpSocket::eAcceptEvent, SOCKET_TIMEOUT ) ) )
        return -5;

    if ( !oexVERIFY( server.Accept( session ) ) )
        return -6;

    if ( !oexVERIFY( session.WaitEvent( oex::os::CIpSocket::eConnectEvent, SOCKET_TIMEOUT ) ) )
    	return -7;

    if ( !oexVERIFY( client.WaitEvent( oex::os::CIpSocket::eConnectEvent, SOCKET_TIMEOUT ) ) )
        return -8;

    oex::oexCSTR pStr = oexT( "B6F5FF3D-E9A5-46ca-ADB8-D655427EB94D" );

    if ( !oexVERIFY( session.Send( oexStrToBin( pStr ) ) ) )
        return -9;

    if ( !oexVERIFY( client.WaitEvent( oex::os::CIpSocket::eReadEvent, SOCKET_TIMEOUT ) ) )
        return -10;

    if ( !oexVERIFY( oexBinToStr( client.Recv() ) == pStr ) )
        return -11;

    client.Destroy();
    if ( !oexVERIFY( session.WaitEvent( oex::os::CIpSocket::eCloseEvent, SOCKET_TIMEOUT ) ) )
        return -12;

    session.Destroy();
    server.Destroy();


    // *** UDP

    if ( !oexVERIFY( server.Create( oex::os::CIpSocket::eAfInet, oex::os::CIpSocket::eTypeDgram, oex::os::CIpSocket::eProtoUdp ) )
         || !oexVERIFY( server.Bind( TEST_TCP_PORT ) ) )
        return -13;

    if ( !oexVERIFY( client.Create( oex::os::CIpSocket::eAfInet, oex::os::CIpSocket::eTypeDgram, oex::os::CIpSocket::eProtoUdp ) ) )
        return -14;

    if ( !oexVERIFY( client.PeerAddress().LookupHost( oexT( "127.0.0.1" ), TEST_TCP_PORT ) ) )
        return -15;

    if ( !oexVERIFY( client.SendTo( oexStrToBin( pStr ) ) ) )
        return -16;

    if ( !oexVERIFY( server.WaitEvent( oex::os::CIpSocket::eReadEvent, SOCKET_TIMEOUT ) ) )
        return -17;

    if ( !oexVERIFY( oexBinToStr( server.RecvFrom() ) == pStr ) )
        return -18;

    client.Destroy();
    server.Destroy();

    return oex::oexRES_OK;
}

oex::oexRESULT Test_CCircBuf()
{
	oexEcho( oexT( "======== Circular buffers..." ) );

    oex::CCircBuf cb;

    oex::oexCSTR pStr = oexT( "Hello World" );
    oexSIZE_T uStr = oex::zstr::Length( pStr );
    oexSIZE_T uBufferedData = 0;

    for ( oexSIZE_T i = 0; i < 1000; i++ )
    {
        for ( oexSIZE_T x = 0; x < 8; x++ )
            cb.Write( oexStrToStr8Ptr( pStr ) ),
			uBufferedData += uStr;

        if ( !oexVERIFY( oexStr8ToStr( cb.Read( uStr ) ) == pStr ) )
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
    for ( oexSIZE_T i = 0; i < 1000; i++ )
    {
        for ( oexSIZE_T x = 0; x < 8; x++ )
            sb1.Write( oexStrToStr8( pStr ) ),
			uBufferedData += uStr;

        if ( !oexVERIFY( oexStr8ToStr( sb2.Read( uStr ) ) == pStr ) )
            return -3;

        uBufferedData -= uStr;

    } // end for

    if ( !oexVERIFY( sb1.GetMaxRead() == uBufferedData ) )
        return -4;

    return oex::oexRES_OK;
}

oex::oexRESULT Test_CFifoSync()
{
	oexEcho( oexT( "======== FIFO buffers..." ) );

    oex::CFifoSync fs;

    fs.SetMaxBuffers( 10000 );

    oex::oexCSTR pStr = oexT( "Hello World" );
    oexSIZE_T uBufferedData = 0;

    for ( oexSIZE_T i = 0; i < 10; i++ )
    {
        for ( oexSIZE_T x = 0; x < 8; x++ )
        {
            uBufferedData++;
            fs.Write( oexStrToBin( pStr ) );

        } // end for

        if ( !oexVERIFY( oexBinToStr( fs.Read() ) == pStr ) )
            return -1;

        uBufferedData--;

    } // end for

    oexSIZE_T uUsed = fs.GetUsedBuffers();
  	if ( !oexVERIFY( fs.GetUsedBuffers() == uBufferedData ) )
        return -2;

    // Shared memory buffer test
    oex::CFifoSync fs1, fs2;

    fs1.SetName( oex::CStr().GuidToString().Ptr() );
    fs2.SetName( fs1.GetName() );

    fs1.Allocate( 400000, 20000 );
    fs2.Allocate( 400000, 20000 );

    uBufferedData = 0;
    for ( oexSIZE_T i = 0; i < 1000; i++ )
    {
        for ( oexSIZE_T x = 0; x < 8; x++ )
            fs1.Write( oexStrToBin( pStr ) ),
			uBufferedData++;

        if ( !oexVERIFY( oexBinToStr( fs2.Read() ) == pStr ) )
            return -3;

        uBufferedData--;

    } // end for

    uUsed = fs1.GetUsedBuffers();
  	if ( !oexVERIFY( fs1.GetUsedBuffers() == uBufferedData ) )
        return -2;

    return oex::oexRES_OK;
}

oex::oexRESULT Test_CDataPacket()
{
	oexEcho( oexT( "======== Packets..." ) );

    oex::CDataPacket dp1, dp2, dp3;

    oex::CStr sStr1 = oexT( "This is the first data string" );
    oex::CStr sStr2 = oexT( "This is the second data string" );

    // Write a packet of data
    if ( !oexVERIFY( dp1.WritePacket( 1, 1, oexStrToBin( sStr1 ) ) )
         || !oexVERIFY( dp1.WritePacket( 1, 2, oexStrToBin( sStr2 ) ) ) )
        return -1;

    // 'Transmit' packet
    if ( !oexVERIFY( dp2.ReadPacket( dp1.Read() ) ) )
        return -2;

    // Verify the data
    if ( !oexVERIFY( oexBinToStr( dp2.ReadPacketString( 0, 1 ) ) == sStr1 )
         || !oexVERIFY( dp2.SkipPacket() )
         || !oexVERIFY( oexBinToStr( dp2.ReadPacketString( 0, 2 ) ) == sStr2 ) )
        return -3;

    dp1.Destroy();
    dp2.Destroy();

    // Write a packet of data
    if ( !oexVERIFY( dp1.InitPacket( 1, 2, sStr1.LengthInBytes() + sStr2.LengthInBytes() ) )
         || !oexVERIFY( dp1.AddPacketData( 1, oexStrToBin( sStr1 ) ) )
         || !oexVERIFY( dp1.AddPacketData( 2, oexStrToBin( sStr2 ) ) )
         || !oexVERIFY( dp1.EndPacket() ) )
        return -1;

    // 'Transmit' packet
    if ( !oexVERIFY( dp2.ReadPacket( dp1.Read() ) ) )
        return -2;

    // Verify the data
    if ( !oexVERIFY( oexBinToStr( dp2.ReadPacketString( 0, 1 ) ) == sStr1 )
         || !oexVERIFY( oexBinToStr( dp2.ReadPacketString( 0, 2 ) ) == sStr2 ) )
        return -3;

    dp1.Destroy();
    dp2.Destroy();

    // Shared memory version
    dp1.SetName( oex::CStr().GuidToString().Ptr() );
    dp2.SetName( dp1.GetName() );

    dp1.Allocate( 10000 );
    dp2.Allocate( 10000 );

    if ( !oexVERIFY( dp1.WritePacket( 1, 1, oexStrToBin( sStr1 ) ) )
         || !oexVERIFY( dp1.WritePacket( 1, 2, oexStrToBin( sStr2 ) ) ) )
        return -1;

    if ( !oexVERIFY( dp3.ReadPacket( dp2.Read() ) ) )
        return -2;

    // Verify the data
    if ( !oexVERIFY( oexBinToStr( dp3.ReadPacketString( 0, 1 ) ) == sStr1 )
         || !oexVERIFY( dp3.SkipPacket() )
         || !oexVERIFY( oexBinToStr( dp3.ReadPacketString( 0, 2 ) ) == sStr2 ) )
        return -3;

    return oex::oexRES_OK;
}

class CMyThread : public oex::CThread
{
public:

	CMyThread()
	{
		m_val = 0;
	}

	virtual oex::oexBOOL InitThread( oex::oexPVOID x_pData )
	{	m_val++; // 1
		return oex::oexTRUE;
	}

	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
	{	m_val++; // 2
		return oex::oexFALSE;
	}

	virtual oex::oexINT EndThread( oex::oexPVOID x_pData )
	{
		m_val++; // 3

		m_evDone.Signal();
		GetStopEvent().Wait();

		m_val++; // 4

		return oex::oexFALSE;
	}

	oexEvent			m_evDone;
	oex::oexINT 		m_val;
};

class CMyThread2 : public oex::CThread
{
public:

	CMyThread2()
	{
		m_val = 0;
	}

	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
	{
		if ( 0 >= GetStopEvent().Wait( m_sigInc ) )
			return oex::oexFALSE;

		m_val++;

		if ( m_sigInc.Reset() )
			return oex::oexFALSE;

		return oex::oexTRUE;
	}

	oex::oexINT 		m_val;

	oexSignal			m_sigInc;
};

class CMyThread3 : public oex::CThread
{
public:

	CMyThread3( oexLock *x_lock, oex::oexINT *x_val, oex::oexINT *x_count, oex::oexINT x_id )
	{	m_lock = x_lock; m_val = x_val; m_count = x_count; m_id = x_id; m_bThrottle = 0; }

	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
	{
		oexAutoLock al( m_lock );

		m_count[ m_id ]++;
		(*m_val)++;

		if ( !al.IsLocked() )
			return oex::oexFALSE;

		if ( m_bThrottle )
			oexSleep( 15 );

		if ( 1 != *m_val )
			return oex::oexFALSE;

		(*m_val)--;
		return oex::oexTRUE;
	}

	oexLock  		*m_lock;
	oex::oexINT 	*m_val;
	oex::oexINT		*m_count;
	oex::oexINT		m_id;
	oex::oexBOOL	m_bThrottle;
};

class CMyThread4 : public oex::CThread
{
public:

	CMyThread4()
	{
		m_uElapsed = 0;
	}

	virtual oex::oexBOOL DoThread( oex::oexPVOID x_pData )
	{
		// Save the time stamp
		oex::oexINT uTime = oexGetUnixTime();

		// Should time out
		oexAutoLock al( m_lock, 3000 );

		// We should not get the lock
		if ( !al.IsLocked() )

			// How much time went by?
			m_uElapsed = oexGetUnixTime() - uTime;

		return oex::oexFALSE;
	}

	oexLock				m_lock;
	oex::oexUINT		m_uElapsed;
};

oex::oexRESULT Test_Threads()
{
	oexEcho( oexT( "======== Threads..." ) );

	{ // Test resource reference counting
		oexEvent e1;
		oexEvent e2( e1 );
		oexEvent e3;
		e3 = e2;
	} // end scope

	CMyThread t;

	if ( !oexVERIFY( 0 == t.Start() ) )
		return -1;

	if ( !oexVERIFY( t.IsRunning() ) )
		return -2;

	if ( !oexVERIFY( 0 == t.m_evDone.Wait() ) )
		return -3;

	if ( !oexVERIFY( 3 == t.m_val ) )
		return -4;

	if ( !oexVERIFY( 0 == t.Stop() ) )
		return -5;

	if ( !oexVERIFY( 4 == t.m_val ) )
		return -6;

	// *** Event / Mutex test

	CMyThread2 t2;

	if ( !oexVERIFY( 0 == t2.Start( 0, 0 ) ) )
		return -10;

	oex::CStr str;
	while ( 10 > t2.m_val )
	{
		if ( !oexVERIFY( t2.IsRunning() ) )
			return -11;

		if ( !oexVERIFY( 0 == t2.m_sigInc.Signal() ) )
			return -12;

		if ( !oexVERIFY( 0 == t2.m_sigInc.GetResetEvent().Wait() ) )
			return -13;

		str << t2.m_val << oexT( "," );

		oexSleep( 100 );

	} // end while

	if ( !oexVERIFY( 0 == t2.Stop( 10000 ) ) )
		return -15;

	if ( !oexVERIFY( str == oexT( "1,2,3,4,5,6,7,8,9,10," ) ) )
		return -16;

	// *** Timeout test

	CMyThread4 t4;

	if ( !oexVERIFY( !t4.m_lock.Wait( 0 ) ) )
		return 23;

	if ( !oexVERIFY( 0 == t4.Start() ) )
		return -24;

	if ( !oexVERIFY( t4.WaitThreadExit() ) )
		return -25;

	t4.m_lock.Reset();

	if ( 3 != t4.m_uElapsed )
		oexEcho( oexMks( oexT( "Lock timeout set to 3 seconds, actual delay =  " ), t4.m_uElapsed ).Ptr() );

	// *** Lock test

	oexLock lock;
	oex::oexINT val = 0;
	oex::oexINT count[ 4 ] = { 0, 0, 0, 0 };

	CMyThread3 tl1( &lock, &val, count, 0 ),
			   tl2( &lock, &val, count, 1 ),
			   tl3( &lock, &val, count, 2 ),
			   tl4( &lock, &val, count, 3 );

	if ( !oexVERIFY( 0 == tl1.Start( 0, 0 )
	                 && 0 == tl2.Start( 0, 0 )
	                 && 0 == tl3.Start( 0, 0 )
	                 && 0 == tl4.Start( 0, 0 ) ) )
		return -20;

	oexSleep( 1000 );

	if ( !oexVERIFY( 0 == tl1.Stop()
	                 && 0 == tl2.Stop()
	                 && 0 == tl3.Stop()
	                 && 0 == tl4.Stop() ) )
	    return -21;

	oexPrintf( oexT( "Unthrottled Thread counts are : %d, %d, %d, %d\n" ),
			   count[ 0 ], count[ 1 ], count[ 2 ], count[ 3 ] );

	if ( !oexVERIFY( 0 == val ) )
	{	oexSHOW( val );
		return -22;
	} // end if

	oex::oexINT min = oex::cmn::Min( count[ 0 ], count[ 1 ], count[ 2 ], count[ 3 ] );
	oex::oexINT max = oex::cmn::Max( count[ 0 ], count[ 1 ], count[ 2 ], count[ 3 ] );
	if ( max > ( min * 4 ) )
		oexEcho( oexT( " Hmmmm.... Unthrottled thread counts are not fair" ) );

	if ( !oexVERIFY( 0 == val ) )
	{	oexSHOW( val );
		return -22;
	} // end if

	oexZero( count );
	tl1.m_bThrottle = 1;
	tl2.m_bThrottle = 1;
	tl3.m_bThrottle = 1;
	tl4.m_bThrottle = 1;

	if ( !oexVERIFY( 0 == tl1.Start( 0, 0 )
	                 && 0 == tl2.Start( 0, 0 )
	                 && 0 == tl3.Start( 0, 0 )
	                 && 0 == tl4.Start( 0, 0 ) ) )
		return -20;

	oexSleep( 1000 );

	if ( !oexVERIFY( 0 == tl1.Stop()
	                 && 0 == tl2.Stop()
	                 && 0 == tl3.Stop()
	                 && 0 == tl4.Stop() ) )
	    return -21;

	oexPrintf( oexT( "Throttled Thread counts are : %d, %d, %d, %d\n" ),
			   count[ 0 ], count[ 1 ], count[ 2 ], count[ 3 ] );

	if ( !oexVERIFY( 0 == val ) )
	{	oexSHOW( val );
		return -22;
	} // end if

	min = oex::cmn::Min( count[ 0 ], count[ 1 ], count[ 2 ], count[ 3 ] );
	max = oex::cmn::Max( count[ 0 ], count[ 1 ], count[ 2 ], count[ 3 ] );
	if ( max > ( min * 4 ) )
		oexEcho( oexT( " Hmmmm.... Throttled thread counts are not fair" ) );

	oex::oexINT nTotal = count[ 0 ] + count[ 1 ] + count[ 2 ] + count[ 3 ];
	if ( nTotal < 65 || nTotal > 75 )
		oexEcho( oexMks( oexT( " There should be about 70 counts total, but there are " ), nTotal ).Ptr() );

	return oex::oexRES_OK;
}

class CCallbackClass
{
public:

    int Add( int p1, int p2 )
    {
        return p1 + p2;
    }

    oex::CStr Return( oex::CStr str )
    {
        return str;
    }

};

oex::oexINT OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
						   oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer )
{
	if ( oexPtrToInt( x_pData ) != 6789 )
		return 0;

	return 0;
}

oex::oexINT OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession )
{
	if ( oexPtrToInt( x_pData ) != 9876 )
		return 0;

	x_pSession->Content() << "Hello World!";

	return 0;
}

oex::oexRESULT Test_CHttpSession()
{
	oexEcho( oexT( "======== HTTP..." ) );

	oex::os::CIpSocket client;
	oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > server;

	// *** Multi-threaded
	server.EnableMultiThreading( oexTRUE );

	server.SetSessionCallback( (oex::oexPVOID)OnSessionCallback, (void*)9876 );

	if ( !oexVERIFY( server.StartServer( TEST_TCP_PORT, OnServerEvent, (void*)6789 ) ) )
		return -2;

	if ( !oexVERIFY( !server.GetInitEvent().Wait() ) )
		return -3;

	if ( !oexVERIFY( server.GetInitStatus() ) )
		return -4;

    if ( !oexVERIFY( client.Connect( oexT( "127.0.0.1" ), TEST_TCP_PORT ) ) )
        return -5;

    if ( !oexVERIFY( client.WaitEvent( oex::os::CIpSocket::eConnectEvent, SOCKET_TIMEOUT ) ) )
        return -6;

	if ( !oexVERIFY( client.Send( "GET / HTTP/1.0\r\n\r\n" ) ) )
        return -7;

	while ( !server.GetNumTransactions() )
		oexSleep( 15 );

	while ( server.GetNumActiveClients() )
		oexSleep( 15 );

    if ( !oexVERIFY( client.WaitEvent( oex::os::CIpSocket::eReadEvent, SOCKET_TIMEOUT ) ) )
        return -8;

	oex::CStr8 sData = client.Read();

	if ( !oexVERIFY( 0 <= sData.Match( "HTTP/1.0" ) ) )
		return -9;

	int max = 10;
	while ( 0 > sData.Match( "Hello World!" ) && max )
	{
		max--;

		// We're not a proper client, so just wait for data
		oexSleep( 100 );

		sData += client.Read();

	} // end if

	if ( !oexVERIFY( max ) )
	{	oexSHOW( oexMbToStr( sData ) );
		return -11;
	} // end if	

	client.Destroy();

	server.Stop();


	// *** Single-threaded
	server.EnableMultiThreading( oexFALSE );

	server.SetSessionCallback( (oex::oexPVOID)OnSessionCallback, (void*)9876 );

	if ( !oexVERIFY( server.StartServer( TEST_TCP_PORT, OnServerEvent, (void*)6789 ) ) )
		return -9;

	if ( !oexVERIFY( !server.GetInitEvent().Wait() ) )
		return -10;

	if ( !oexVERIFY( server.GetInitStatus() ) )
		return -11;

    if ( !oexVERIFY( client.Connect( oexT( "127.0.0.1" ), TEST_TCP_PORT ) ) )
        return -12;

    if ( !oexVERIFY( client.WaitEvent( oex::os::CIpSocket::eConnectEvent, SOCKET_TIMEOUT ) ) )
        return -13;

	if ( !oexVERIFY( client.Send( "GET / HTTP/1.0\r\n\r\n" ) ) )
        return -14;

	while ( !server.GetNumTransactions() )
		oexSleep( 15 );

	while ( server.GetNumActiveClients() )
		oexSleep( 15 );

    if ( !oexVERIFY( client.WaitEvent( oex::os::CIpSocket::eReadEvent, SOCKET_TIMEOUT ) ) )
        return -15;

	sData = client.Read();

	if ( !oexVERIFY( 0 <= sData.Match( "HTTP/1.0" ) ) )
		return -16;

	if ( 0 > sData.Match( "Hello World!" ) )
	{
		// We're not a proper client, so just wait for data
		oexSleep( 100 );

		sData = client.Read();

		if ( !oexVERIFY( 0 <= sData.Match( "Hello World!" ) ) )
			return -18;

	} // end if

	client.Destroy();

	server.Stop();

	return oex::oexRES_OK;
}

oex::oexRESULT Test_MsgParams()
{
//	oex::CMsgParams mp( 1, 2 );


	return oex::oexRES_OK;
}

/*
oex::oexRESULT Test_TArbDelegate()
{
    CCallbackClass cc;
    oex::TArbDelegate< oex::CAutoStr > ad;

    ad.Register( &cc, &CCallbackClass::Add );
    if ( !oexVERIFY( ad( 1, 2 ) == 3 ) )
        return -1;

    ad.Register( &cc, &CCallbackClass::Return );
    if ( !oexVERIFY( ad( oexT( "Hello" ) ) == oexT( "Hello" ) ) )
        return -1;

    return oex::oexRES_OK;
}
*/
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------
//------------------------------------------------------------------


oex::oexRESULT Test_CDispatch()
{
/*    CCallbackClass cc;

    // +++ Change "f=Add,p{0=1,1=2}" to "Add{0=1,1=2}"
    if ( !oexVERIFY( msgCall( oexT( "Add" ), 1, 2 ) == oexT( "f=Add,p{0=1,1=2}" ) ) )
        return -1;

    // Set delegates
    oex::CDispatch dsp;
    dsp.OexRpcRegisterPtr( &cc, CCallbackClass, Add );
    dsp.OexRpcRegisterPtr( &cc, CCallbackClass, Return );

    oex::CStr sRet;
    if ( !oexVERIFY( dsp.Execute( msgCall( oexT( "Add" ), 1, 2 ), &sRet ) && sRet == 3 ) )
        return -2;

    if ( !oexVERIFY( dsp.Execute( msgCall( oexT( "Return" ), oexT( "Hello" ) ), &sRet ) && sRet == oexT( "Hello" ) ) )
        return -3;

    CBaseTestObject bto;

    bto.SetValue( 11 );

    dsp.OexRpcRegisterPtr( &bto, CBaseTestObject, SetValue );

    dsp.Queue( oexNULL, dsp.Call( oexT( "SetValue" ), 22 ) );

    dsp.ProcessQueue();

    if ( !oexVERIFY( 22 == bto.GetValue() ) )
        return -4;

    // Test single execute
    dsp.Queue( oexNULL, dsp.Call( oexT( "SetValue" ), 33 ) );
    dsp.Queue( oexNULL, dsp.Call( oexT( "SetValue" ), 44 ) );

    dsp.ProcessQueue( 1 );
    if ( !oexVERIFY( 33 == bto.GetValue() ) )
        return -5;

    dsp.ProcessQueue();
    if ( !oexVERIFY( 44 == bto.GetValue() ) )
        return -6;

    // Test priority
    dsp.Queue( oexNULL, dsp.Call( oexT( "SetValue" ), 33 ) );
    dsp.Queue( oexNULL, dsp.Call( oexT( "SetValue" ), 44 ), 1 );
    dsp.ProcessQueue();
    if ( !oexVERIFY( 33 == bto.GetValue() ) )
        return -7;
*/
    return oex::oexRES_OK;
}
/*
class CMsgTest : public oex::CMsgObject
{
public:

    CMsgTest()
    {
//        msgRegisterThisFunction( CMsgTest, Add );
//        msgRegisterThisFunction( CMsgTest, Return );
//        msgRegisterThisFunction( CMsgTest, ReturnPtr );
    }

    virtual int Add( int p1, int p2 )
    {   return p1 + p2;
    }

    oex::CStr Return( oex::CStr str )
    {   return str;
    }

    oex::CStr ReturnPtr( oex::oexCSTR ptr, oexSIZE_T uSize )
    {   return ptr;
    }
};
*/

oex::oexRESULT Test_CMsgParam()
{


    return oex::oexRES_OK;
}

/*
oex::oexRESULT Test_CMsg()
{
    oexSIZE_T len = oex::obj::Size( oexT( "This is a test string." ) );

    CMsgTest mt;
    oex::CMsgTarget target;
    target.Register( &mt, &CMsgTest::Add );

    oex::CMsg msg = oexMsg( 0, oexTo( "Add" ), 1, 2 );

    // Routed version
//    oex::CMsg msg = oexMsg( "neb://192.168.2.69/device_1",
//                                oexT( "Add" ), 1, 2 );

    if ( !oexVERIFY( msg[ 0 ] == 1 ) )
        return -1;

    if ( !oexVERIFY( msg[ 1 ] == 2 ) )
        return -2;

    // Call the function
    if ( !oexVERIFY( target( msg )[ 0 ] == 3 ) )
        return -3;

    // Test string
    target.Register( &mt, &CMsgTest::ReturnPtr );

    msg = oexMsg( 0, oexTo( "ReturnPtr" ), oexT( "Hello World!" ), 12 );

    if ( !oexVERIFY( target( msg )[ 0 ] == oexT( "Hello World!" ) ) )
        return -4;

    // Send a message to the object
    oex::CMsg reply = msgOrb.Send( oexMsg( 0, oexTo( "Add", &mt.msgId() ), 1, 2 ) );

    if ( !oexVERIFY( !reply.Wait( 0 ).IsReplyReady() ) )
        return -4;

    if ( !oexVERIFY( 1 == mt.msgProcessQueue() ) )
        return -5;

    if ( !oexVERIFY( reply.Wait( 0 ).IsReplyReady() ) )
        return -6;

    if ( !oexVERIFY( reply[ 0 ] == 3 ) )
        return -7;

/ *
    CMsgTest mt;

    oex::CMsg reply = mt.msgSend( msgCreate( 0, msgTo( "Add" ), 1, 2 ) );

    mt.msgProcessQueue();

    if ( !oexVERIFY( reply.Wait( 3000 ).IsReplyReady() ) )
        return -3;

    if ( !oexVERIFY( reply.GetReply() == 3 ) )
        return -4;

//    CMsgProxy mp = msgCreateObject( "CMsgTest" );

//  CMsgProxy mp = msgCreateObject( "npp://server/CMsgTest" );
/ *
    if ( !oexVERIFY( mp.IsConnected() ) )
        return -1

    if ( !oexVERIFY( mp.Wait( 3000 ) ) )
        return -2;

    CMsg reply = mp.Send( msgCreate( "Add", 1, 2 ) );

    if ( !oexVERIFY( reply.Wait( 3000 ) ) )
        return -3;

    if ( !oexVERIFY( reply.GetReply() == 3 ) )
        return -4;
* /

    return oex::oexRES_OK;
}


/ *
oex::oexRESULT Test_CMsg()
{
    oexSIZE_T len = oex::obj::Size( oexT( "This is a test string." ) );

    CMsgTest mt;
    oex::CMsgTarget target;
    target.Register( &mt, &CMsgTest::Add );

    oex::CMsg msg = oexMsg( 0, oexTo( "Add" ), 1, 2 );

    // Routed version
//    oex::CMsg msg = oexMsg( "neb://192.168.2.69/device_1",
//                                oexT( "Add" ), 1, 2 );

    if ( !oexVERIFY( msg[ 0 ] == 1 ) )
        return -1;

    if ( !oexVERIFY( msg[ 1 ] == 2 ) )
        return -2;

    // Call the function
    if ( !oexVERIFY( target( msg )[ 0 ] == 3 ) )
        return -3;

    // Test string
    target.Register( &mt, &CMsgTest::ReturnPtr );

    msg = oexMsg( 0, oexTo( "ReturnPtr" ), oexT( "Hello World!" ), 12 );

    if ( !oexVERIFY( target( msg )[ 0 ] == oexT( "Hello World!" ) ) )
        return -4;


    // Send a message to the object
    oex::CMsg reply = oexNet.Send( oexMsg( 0, oexTo( "Add", &mt.oexMsgId() ), 1, 2 ) );

    if ( !oexVERIFY( !reply.Wait( 0 ) ) )
        return -4;

    if ( !oexVERIFY( 1 == mt.ProcessQueue() ) )
        return -5;

    if ( !oexVERIFY( reply.Wait( 0 ) ) )
        return -6;

    if ( !oexVERIFY( reply[ 0 ] == 3 ) )
        return -7;

    return oex::oexRES_OK;
}
* /

class CThreadTest : public oex::CMsgThread
{
public:

    CThreadTest()
    {
        msgRegisterThisFunction( CThreadTest, Add );
        msgRegisterThisFunction( CThreadTest, SetCallback );

    }

    int Add( int a, int b )
    {
        return a + b;
    }

    void SetCallback( oex::CMsgAddress ma )
    {   m_maCallback = ma;
    }

private:

    oex::CMsgAddress         m_maCallback;
};

oex::oexRESULT Test_CThread()
{
    CThreadTest tt;

    if ( !oexVERIFY( tt.Start() ) )
        return -1;

    oex::CMsg reply = tt.msgSend( msgCreate( 0, msgTo( "Add" ), 1, 2 ) );

    if ( !oexVERIFY( reply.Wait( 3000 ).IsReplyReady() ) )
        return -2;

    if ( !oexVERIFY( reply[ 0 ] == 3 ) )
        return -3;

//    oex::CMsg callback;
//    msgOrb.RegisterReply( callback );

//    tt.msgSend( msgCreate( 0, msgTo( "SetCallback" ), callback.Src() ) );

//    if ( !oexVERIFY( callback.Wait( 3000 ).IsReplyReady() ) )
//        return -2;

    return oex::oexRES_OK;
}

/ *
oex::oexRESULT Test_CAutoSocket()
{
    // Create echo server
    oex::TNetServer< oex::CAutoSocket, oex::TEchoProtocol< oex::CAutoSocket > > server;

    // Start the server thread
    server.Start();

    // Start the server
    server.Queue( oexNULL, oexCall( oexT( "Bind" ), 23456 ) );
    oex::CDispatch::CMessage reply( server.Queue( oexNULL, oexCall( oexT( "Listen" ), 0 ) ) );
    if ( !reply.Wait( oexDEFAULT_WAIT_TIMEOUT ).IsDone() )
        return -1;

    oex::os::CIpSocket client;

    if ( !oexVERIFY( client.Connect( oexT( "localhost" ), 23456 ) ) )
        return -2;

    if ( !oexVERIFY( client.WaitEvent( oex::os::CIpSocket::eConnectEvent, oexDEFAULT_WAIT_TIMEOUT ) ) )
        return -3;

    oex::oexCSTR pStr = oexT( "779684C3-94CB-4b66-8C74-7ADC70BA1AC9" );

    if ( !oexVERIFY( client.Send( oexStrToBin( pStr ) ) ) )
        return -4;

    if ( !oexVERIFY( client.WaitEvent( oex::os::CIpSocket::eReadEvent, oexDEFAULT_WAIT_TIMEOUT ) ) )
        return -5;

    if ( !oexVERIFY( oexBinToStr( client.Recv() ) == pStr ) )
        return -6;

    client.Destroy();
    server.Destroy();

    return oex::oexRES_OK;
}


oex::oexRESULT Test_CFtpSession()
{
    // FTP server
    oex::TNetServer< oex::CAutoSocket, oex::CFtpDiskSession > ftp_server;

    // Start the server
    ftp_server.Start();
    ftp_server.Queue( oexNULL, oexCall( oexT( "Bind" ), 2111 ) );
    ftp_server.Queue( oexNULL, oexCall( oexT( "Listen" ), 0 ) );

    // Wait for connection
    while ( !ftp_server.GetTotalConnections() )
        oex::os::CSys::Sleep( 100 );

    // Wait for disconnect
    while ( ftp_server.GetSessionList().Size() )
        oex::os::CSys::Sleep( 100 );

    // Just wait forever
//    oex::os::CSys::Sleep( 60 * 60 * 1000 );

    return oex::oexRES_OK;
}

oex::oexRESULT Test_CHttpSession()
{
    // FTP server
    oex::TNetServer< oex::CAutoSocket, oex::CHttpSession > http_server;

    // Start the server
    http_server.Start();
    http_server.Queue( oexNULL, oexCall( oexT( "Bind" ), 8111 ) );
    http_server.Queue( oexNULL, oexCall( oexT( "Listen" ), 0 ) );

    // Wait for connection
    while ( !http_server.GetTotalConnections() )
        oex::os::CSys::Sleep( 100 );

    // Wait for disconnect
    while ( http_server.GetSessionList().Size() )
        oex::os::CSys::Sleep( 100 );

    // Just wait forever
//    oex::os::CSys::Sleep( 60 * 60 * 1000 );

    return oex::oexRES_OK;
}

oex::oexRESULT Test_CVfsSession()
{
    // FTP server
    oex::TNetServer< oex::CAutoSocket, oex::CVfsFtpSession > vfs_server;

    // Start the server
    vfs_server.Start();
    vfs_server.Queue( oexNULL, oexCall( oexT( "Bind" ), 2111 ) );
    vfs_server.Queue( oexNULL, oexCall( oexT( "Listen" ), 0 ) );

    // Wait for connection
    while ( !vfs_server.GetTotalConnections() )
        oex::os::CSys::Sleep( 100 );

    // Wait for disconnect
    while ( vfs_server.GetSessionList().Size() )
        oex::os::CSys::Sleep( 100 );

    // Just wait forever
//    oex::os::CSys::Sleep( 60 * 60 * 1000 );

    return oex::oexRES_OK;
}
*/

#if defined( OEX_ENABLE_VIDEO )
oex::oexRESULT Test_CaptureApi( oex::oexUINT uApi, oex::CStr sFile )
{
	oex::vid::CCapture cCapture;

	// Don't flag an error here, maybe there is no capture device
	if ( !cCapture.Open( uApi, 0, 0, 320, 240, 0, 15, 0 ) )
		return -1;

	// Show supported formats
//	oexEcho( cCapture.GetSupportedFormats().Ptr() );

	if ( !cCapture.Init() )
		return -2;

	if ( !oexVERIFY( cCapture.IsOpen() ) )
		return -3;

//	oexEcho( oexT( "Starting capture...\n" ) );

	if ( !oexVERIFY( cCapture.StartCapture() ) )
		return -4;

	// Wait a second for the image to stabilize
	oexSleep( 1000 );

	for ( int i = 0; i < 1; i++ )
	{
//		oexEcho( oexT( "Waiting for frame...\n" ) );

		if ( !oexVERIFY( cCapture.WaitForFrame( 3000 ) ) )
			return -5;

		if ( !oexVERIFY( cCapture.GetBuffer() && cCapture.GetBufferSize() ) )
			return -6;

		oex::CDib img;
		if ( !oexVERIFY( img.Create( oexNULL, oexNULL, cCapture.GetWidth(), cCapture.GetHeight(), 24, 1 ) ) )
			return -7;

		// Convert to RGB
		img.Copy( cCapture.GetBuffer(), cCapture.GetBufferSize() );
//		img.CopySBGGR8( cCapture.GetBuffer() );
//		img.CopyGrey( cCapture.GetBuffer() );

		int nImageSize = oex::CImage::GetScanWidth( cCapture.GetWidth(), 24 )
		       				* oex::cmn::Abs( cCapture.GetHeight() );

//		oexPrintf( oexT( "Writing image to file : %s\n" ), oexStrToMb( sFile ).Ptr() );
//		oexPrintf( oexT( "w=%d, h=%d, Buffer Size = %d, Image Size = %d\n" ), cCapture.GetWidth(), cCapture.GetHeight(), cCapture.GetBufferSize(), nImageSize );

		if ( !oexVERIFY( oex::CDib::SaveDibFile( sFile.Ptr(), img.GetImageHeader(), img.GetBuffer(), nImageSize ) ) )
			return -8;

		if ( !oexVERIFY( cCapture.ReleaseFrame() ) )
			return -9;

	} // end for

	if ( !oexVERIFY( cCapture.StopCapture() ) )
		return -10;

	if ( !oexVERIFY( cCapture.Destroy() ) )
		return -11;

	return 0;
}

oex::oexRESULT Test_CCapture()
{
	oexEcho( oexT( "======== Video capture API..." ) );

	oexINT idx = -1;
	while ( oexCONST vid::SVideoSystemInfo* pVsi = vid::CCapture::GetNextAvailableSystem( idx ) )
	{
		oexPrintf( oexT( "--- %s - %s ---\r\n" ), oexStrToMbPtr( pVsi->pTag ), oexStrToMbPtr( pVsi->pName ) );

		oex::CStr sName = oex::CStr().Fmt( oexT( "./_captest_%s_%d.bmp" ), oexStrToMbPtr( pVsi->pTag ), 0 );
		oex::CStr sFile = oexGetModulePath( sName.Ptr() );

		oexINT nRet = 0;
		if ( !pVsi->bSupported )
			oexPrintf( oexT( "Unsupported\r\n" ) );

		else if ( !( nRet = Test_CaptureApi( pVsi->uType, sFile ) ) )
			oexPrintf( oexT( "Succeded - %s\r\n" ), oexStrToMb( sName ).Ptr() );

		else if ( -1 == nRet )
			oexPrintf( oexT( "No Compatible Device Found\r\n" ) );

		else
			oexPrintf( oexT( "Failed : %d\r\n" ), nRet );

		// Next item
		idx = pVsi->nIndex;

	} // end while

    return oex::oexRES_OK;
}
#endif

#if defined( OEX_ENABLE_SQLITE )
oex::oexRESULT Test_CSQLite()
{
	oexEcho( oexT( "======== CSQLite..." ) );

	oex::CSQLite sq;

	if ( !oexVERIFY( sq.Open( oexT( ":memory:" ) ) ) )
		return -1;

	if ( !oexVERIFY( !sq.IsTable( oexT( "test" ) ) ) )
		return -2;

	if ( !oexVERIFY( sq.Exec( oexT( "CREATE TABLE test ( id INTEGER NOT NULL, name CHAR(64), title CHAR(64), PRIMARY KEY(id) )" ) ) ) )
		return -2;

	if ( !oexVERIFY( sq.IsTable( oexT( "test" ) ) ) )
		return -2;

	if ( !oexVERIFY( sq.Exec( oexT( "INSERT INTO test VALUES(0,'Bob','Engineer')" ) ) ) )
		return -3;

	if ( !oexVERIFY( sq.Exec( oexT( "INSERT INTO test (name,title) VALUES('Kim','Artist')" ) ) ) )
		return -4;

	CPropertyBag pb;
	pb[ oexT( "name" ) ] = oexT( "Amy" );
	pb[ oexT( "title" ) ] = oexT( "\"Director\" of 'Sales' and `Marketing`" );
	if ( !oexVERIFY( sq.Insert( oexT( "test" ), pb ) ) )
		return -5;

	if ( !oexVERIFY( sq.Exec( oexT( "SELECT * FROM test" ) ) ) )
		return -6;

	if ( !oexVERIFY( sq.NumRows() == 3 ) )
		return -7;

	if ( !oexVERIFY( sq.Row( 0 )[ oexT( "name" ) ].ToString() == oexT( "Bob" )
					 && sq.Row( 0 )[ oexT( "title" ) ].ToString() == oexT( "Engineer" ) ) )
		return -8;

	if ( !oexVERIFY( sq.Row( 1 )[ oexT( "name" ) ].ToString() == oexT( "Kim" )
					 && sq.Row( 1 )[ oexT( "title" ) ].ToString() == oexT( "Artist" ) ) )
		return -9;

	if ( !oexVERIFY( sq.Row( 2 )[ oexT( "name" ) ].ToString() == oexT( "Amy" )
					 && sq.Row( 2 )[ oexT( "title" ) ].ToString() == oexT( "\"Director\" of 'Sales' and `Marketing`" ) ) )
		return -10;

	pb[ oexT( "title" ) ] = oexT( "Sales" );
	if ( !oexVERIFY( sq.Update( oexT( "test" ), oexT( "`name`='Amy'" ), pb ) ) )
		return -11;

	if ( !oexVERIFY( sq.Exec( oexT( "SELECT * FROM test WHERE `name`='Amy'" ) ) ) )
		return -12;

	if ( !oexVERIFY( sq.NumRows() == 1 ) )
		return -13;

	if ( !oexVERIFY( sq.Row( 0 )[ oexT( "name" ) ].ToString() == oexT( "Amy" )
					 && sq.Row( 0 )[ oexT( "title" ) ].ToString() == oexT( "Sales" ) ) )
		return -14;

	if ( !oexVERIFY( sq.QueryColumnInfo() ) )
		return -15;

	if ( !oexVERIFY( sq.NumRows() == 3 ) )
		return -16;

	if ( !oexVERIFY( sq.Row( 0 )[ oexT( "name" ) ].ToString() == oexT( "id" ) ) )
		return -17;

	if ( !oexVERIFY( sq.Row( 1 )[ oexT( "name" ) ].ToString() == oexT( "name" ) ) )
		return -18;

	if ( !oexVERIFY( sq.Row( 2 )[ oexT( "name" ) ].ToString() == oexT( "title" ) ) )
		return -19;

	return oex::oexRES_OK;
}
#endif

//int wmain( int argc, wchar_t *argv[] )
//int wmain( int argc, wchar_t *argv[], wchar_t *envp[] )

int main(int argc, char* argv[])
{
    // Initialize the oex library
	oexINIT();

	// Initialize resources
	oexInitResources();

    if ( !oexVERIFY( oex::os::CIpSocket::InitSockets() ) )
        return -1;

	oexEcho( oexT( "-------- Starting tests --------" ) );
	oexNOTICE( 0, oexT( "Tests started" ) );

    TestCommon();

    TestAllocator();

    TestStrings();

    TestFileMapping();

    TestGuids();

    TestLists();

    TestAssoLists();

    TestPropertyBag();

    TestParser();

    TestFile();

    TestZip();

	TestResources();

    Test_CSysTime();

	Test_Threads();

	TestBinary();

#ifndef OEX_LOWRAM

    Test_CCircBuf();

    Test_CFifoSync();

    Test_CDataPacket();

#endif

    Test_CIpAddress();

    Test_CIpSocket();

	Test_CHttpSession();

//	Test_MsgParams();

//    Test_TArbDelegate();

//    Test_CDispatch();

//    Test_CMsg();

//    Test_CThread();

//    Test_CFtpSession();

//    Test_CHttpSession();

//    Test_CVfsSession();

#if defined( OEX_ENABLE_SQLITE )
	Test_CSQLite();
#endif

#if defined( OEX_ENABLE_VIDEO )
    Test_CCapture();
#endif

	// Release sockets
    oex::os::CIpSocket::UninitSockets();

	// Initialize the oex library
    oexUNINIT();

	return 0;
}

