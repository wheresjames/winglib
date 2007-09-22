/*------------------------------------------------------------------
// parser.cpp
//
// Copyright (c) 1997
// Robert Umbehant
// winglib@wheresjames.com
// http://www.wheresjames.com
//
// Redistribution and use in source and binary forms, with or 
// without modification, are permitted for commercial and 
// non-commercial purposes, provided that the following 
// conditions are met:
//
// * Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// * The names of the developers or contributors may not be used to 
//   endorse or promote products derived from this software without 
//   specific prior written permission.
//
//   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND 
//   CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
//   INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF 
//   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
//   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
//   CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
//   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT 
//   NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
//   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
//   HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
//   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR 
//   OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, 
//   EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//----------------------------------------------------------------*/

#include "../oexlib.h"

OEX_USING_NAMESPACE

CStrList CParser::Explode( oexCSTR x_pStr, oexUINT x_uSize, oexCSTR x_pSep, oexUINT x_uSep )
{
	CStrList lst;
	if ( !oexVERIFY_PTR( x_pStr ) )
		return lst;

	if ( !x_pSep ) x_uSep = 0;
	else if ( !oexVERIFY_PTR( x_pSep ) )
		return lst;

	// Breaking down to chars?
	if ( !x_uSep )
	{	while ( *x_pStr )
			lst << *x_pStr, x_pStr++;
		return lst;
	} // end if
	
	oexUINT i = 0;		
	while ( x_uSize )
	{
		// Separator?
		if ( !str::CompareLen( &x_pStr[ i ], x_uSize, x_pSep, x_uSep, x_uSep ) )
		{	lst << CStr( x_pStr, 0, i );
			x_pStr = &x_pStr[ i + x_uSep ];
			x_uSize -= x_uSep; i = 0;
		} // end if

		else i++, x_uSize--;

	} // end while

	// Add last item
	if ( i ) lst << CStr( x_pStr, 0, i );

	return lst;
}

CStr CParser::GetToken( CStr &x_sStr, CStrList x_lst, oexBOOL x_bCaseSensitive )
{
    if ( !x_lst.Size() )
        return CStr();

    // Case sensitive?
    if ( x_bCaseSensitive )
    {   for ( CStrList::iterator it; x_lst.Next( it ); )
            if ( x_sStr.Cmp( it->Ptr(), it->Length() ) )
                return it.Obj();
    } // end if

    else
	    for ( CStrList::iterator it; x_lst.Next( it ); )
            if ( x_sStr.ICmp( it->Ptr(), it->Length() ) )
                return it.Obj();

    return CStr();
}

CStr CParser::ParseToken( CStr &x_sStr, CStrList x_lst, oexBOOL x_bCaseSensitive )
{
    if ( !x_lst.Size() )
        return CStr();

    // Case sensitive?
    if ( x_bCaseSensitive )
    {   for ( CStrList::iterator it; x_lst.Next( it ); )
            if ( x_sStr.CmpLen( it->Ptr(), it->Length() ) )
            {   x_sStr.LTrim( it->Length() ); return it.Obj(); }
    } // end if

    else
	    for ( CStrList::iterator it; x_lst.Next( it ); )
            if ( x_sStr.ICmpLen( it->Ptr(), it->Length() ) )
            {   x_sStr.LTrim( it->Length() ); return it.Obj(); }

    return CStr();
}

CStrList CParser::GetTokens( oexCSTR x_pStr, oexCSTR x_pValid )
{
	CStrList lst;
	if ( !oexVERIFY_PTR( x_pStr ) || !oexVERIFY_PTR( x_pValid ) )
		return lst;

	CStr str;
	oexINT p = 0;
	oexUINT l = zstr::Length( x_pStr );

	do
	{	
		str = CStr::NextToken( x_pStr, x_pValid, &p );
		
		if ( str.Length() ) 
			lst << str;

	} while ( str.Length() );

	return lst;
}

CStrList CParser::Split( oexCSTR x_pStr, oexUINT x_uSize, oexCSTR x_pSep )
{
	CStrList lst;
	if ( !oexVERIFY_PTR( x_pStr ) )
		return lst;

	oexUINT x_uSep = 0;
	if ( !x_pSep ) x_uSep = 0;
	else if ( !oexVERIFY_PTR( x_pSep ) )
		return lst;
	else x_uSep = zstr::Length( x_pSep );

	// Breaking down to chars?
	if ( !x_uSep )
	{	while ( *x_pStr )
			lst << *x_pStr, x_pStr++;
		return lst;
	} // end if
	
	while ( x_uSize )
	{
		// Find a separator
		oexINT nSep = str::FindCharacters( x_pStr, x_uSize, x_pSep, x_uSep );
		
		if ( 0 > nSep ) 
		{	lst << x_pStr; 
			return lst; 
		} // end if
		
		else
		{	x_uSize -= nSep;
            lst << CStr( x_pStr, 0, nSep );
            oexINT nEnd = str::SkipCharacters( &x_pStr[ nSep ], x_uSize, x_pSep, x_uSep );
			if ( 0 <= nEnd ) x_pStr += nSep + nEnd, x_uSize -= nEnd;
			else return lst;
		} // end else

	} // end while

	// Add last item
	if ( *x_pStr ) lst << x_pStr;

	return lst;
}

CPropertyBag CParser::DecodeUrlParams( CStr x_str )
{
	CPropertyBag pb;
	CStr key, val;

	CStrList lst = CParser::Split( x_str, oexT( "&" ) );

	for ( CStrList::iterator it; lst.Next( it ); )
	{
		key = UrlDecode( it->Parse( oexT( "=" ) ) );
		if ( key.Length() ) (*it)++;
		val = UrlDecode( it.Obj() );

		// Key value pair
		if ( key.Length() && val.Length() )
			pb[ key ] = val;

		// NULL key assignment
		else if ( key.Length() )
			pb[ key ] = oexT( "" );

		// Assume NULL key assignment
		else if ( val.Length() )
			pb[ val ] = oexT( "" );

	} // end while

	return pb;
}

CStr CParser::EncodeUrlParams( CPropertyBag x_pb )
{
	CStr str;

	for( CPropertyBag::iterator it; x_pb.List().Next( it ); )
	{
		if ( str.Length() ) str << '&';

		str << UrlEncode( it.Node()->key ) << oexT( "=" ) << UrlEncode( it->ToString() );

	} // end for

	return str;
}

oexBOOL CParser::IsUrlChar( oexTCHAR x_ch ) 
{   return  ( oexT( 'a' ) <= x_ch && oexT( 'z' ) >= x_ch ) ||
            ( oexT( 'A' ) <= x_ch && oexT( 'Z' ) >= x_ch ) ||
            ( oexT( '0' ) <= x_ch && oexT( '9' ) >= x_ch ) ||
            oexT( '_' ) == x_ch || oexT( '-' ) == x_ch || 
            oexT( '.' ) == x_ch;
}


CStr CParser::UrlEncode( CStr x_str )
{
	CStr ret, num;
	oexINT nLen = x_str.Length();

	while ( 0 < nLen-- )
	{
		if ( IsUrlChar( *x_str ) )
			ret << *x_str;
		else
			ret << num.Fmt( oexT( "%%%02lX" ), *x_str );

		x_str++;

	} // end while

	return ret;
}

CStr CParser::UrlDecode( CStr x_str )
{
	CStr ret, num;
	oexINT nLen = x_str.Length();

	while ( 0 < nLen-- )
	{
		if ( oexT( '%' ) != *x_str )
			ret << *x_str;

		else
		{
			x_str++;

			if ( x_str.IsMatchAt( 0, oexT( "0123456789abcdefABCDEF" ) ) )
				ret << ( (oexTCHAR)num.Sub( x_str, 0, 2 ).ToLong( 16 ) ), x_str++, nLen -= 2;

			else
				ret << *x_str;

		} // end else

		x_str++;

	} // end while

	return ret;
}

CPropertyBag CParser::DecodeMIME( CStr &x_sStr )
{
    CPropertyBag pb;

    while ( x_sStr.Length() )
    {
        CStr sKey = x_sStr.Parse( oexT(  ":" ) ), sVal;
        if ( *x_sStr == ':' ) x_sStr++;

        // Read value string
        do
        {   sVal << x_sStr.SkipWhiteSpace().Parse( oexT( "\r\n" ) );
            x_sStr.NextLine();
        } while ( x_sStr.Length() && x_sStr.IsWhiteSpaceAt( 0 ) );
       
        // Ensure valid strings
        if ( sKey.Length() && sVal.Length() )
            pb[ UrlDecode( sKey ) ] = UrlDecode( sVal );

    } // end while

    return pb;
}

CStr CParser::EncodeMime( CPropertyBag &x_pb )
{
	CStr str;

    // Write out each value
	for( CPropertyBag::iterator it; x_pb.List().Next( it ); )

        if ( it.Node()->key.Length() && it->ToString().Length() )

            str << UrlEncode( it.Node()->key ) << oexT( ": " ) << UrlEncode( it->ToString() ) << oexT( "\r\n" );

	return str;
}

oexLONG CParser::Deserialize( CStr &x_sStr, CPropertyBag &x_pb, oexBOOL x_bMerge, oexLONG *x_pLast )
{
    // Lose previous contents
    if ( !x_bMerge ) 
        x_pb.Destroy();

    // Punt if null string
    if ( !x_sStr.Length() )
        return 0;

    oexLONG lItems = 0;
    oexLONG lLen = x_sStr.Length(), s = 0, e = 0;

    while ( e <= lLen )
    {
        switch( x_sStr[ e ] )
        {
            case oexT( ',' ) : case oexT( '}' ) : case 0 :
            {
                if ( 1 < e - s )
                {
                    // Find '='
                    oexLONG a = s; 
                    while ( a < e && oexT( '=' ) != x_sStr[ a ] ) a++;

                    CStr sKey, sVal;

                    // First character is separator
                    if ( a == s ) 
                        sKey = UrlDecode( CStr( x_sStr.Ptr( s + 1 ), e - s - 1 ) );

                    else sKey = UrlDecode( CStr( x_sStr.Ptr( s ), a - s ) );
                    
                    // Single token
                    if ( 1 >= e - a ) 
                        x_pb[ sKey ] = oexT( "" );

                    // Both tokens present
                    else x_pb[ sKey ] = UrlDecode( CStr( x_sStr.Ptr( a + 1 ), e - a - 1 ) );

                    // Count one item
                    lItems++;

                } // end if

                // Next element
                s = e + 1;

                // Time to exit?
                if ( oexT( '}' ) == x_sStr[ e ] )
                {   if ( x_pLast ) *x_pLast = e + 1; return lItems; }

            } break;

            case oexT( '{' ) :
            {              
                // Get key
                CStr sKey;
                
                // Find '='
                oexLONG a = s; 
                while ( a < e && oexT( '=' ) != x_sStr[ a ] ) a++;

                // First character is separator
                if ( a == s ) 
                    sKey = UrlDecode( CStr( x_sStr.Ptr( s + 1 ), e - s - 1 ) );

                // No default value
                else if ( a >= e )
                    sKey = UrlDecode( CStr( x_sStr.Ptr( s ), e - s ) );

                else if ( a < e ) 
                {   sKey = UrlDecode( CStr( x_sStr.Ptr( s ), a - s ) );
                    x_pb[ sKey ] = UrlDecode( CStr( x_sStr.Ptr( a + 1 ), e - a - 1 ) );
                    lItems++;
                } // end if

                // Do we have a key?
                if ( sKey.Length() )
                {
                    // This will point to the end of the array we're about to decode
                    oexLONG lEnd = 0;

                    // Get the sub array
                    lItems += Deserialize(  CStr( x_sStr.Ptr( e + 1 ) ), 
                                            x_pb[ sKey ], oexTRUE, &lEnd );

                    // Skip the array we just decoded
                    e += lEnd;

                } // end if

                // Skip this token
                s = e + 1; 

            } break;

        } // end switch

        // Next i
        e++;

    } // end while

    return lItems;
}

CStr CParser::Serialize( CPropertyBag &x_pb )
{
    // Just return default value if not an array
    if ( !x_pb.IsArray() )
        return CStr();

    CStr sStr;
    for ( CPropertyBag::iterator it; x_pb.List().Next( it ); )
    {
        // Add separator if needed
        if ( sStr.Length() )
            sStr << oexT( ',' );

        // Set the key
        sStr << CParser::UrlEncode( it.Node()->key );

        // Encode default value
        if ( it->IsDefaultValue() )
            sStr << oexT( '=' ) << CParser::UrlEncode( it->ToString() );

        // Recurse for array
        if ( it->IsArray() )
            sStr << oexT( '{' ) << Serialize( *it ) << oexT( '}' );

    } // end for
        
    return sStr;
}


/*
oexBOOL CParser::CompileStatement( CStr &str, CPropertyBag &fmt )
{
	oexUINT i = 0;
	while ( str.SkipWhiteSpace().Length() )
	{
		// What to do with this character?
		switch( *str )
		{
			// Check for end of statement
			case oexT( ';' ) :
				str++; return oexTRUE;
				break;

			// Key define
			case oexT( '[' ) :
				fmt[ i ][ "type" ] = "key";
				fmt[ i++ ][ "data" ] = str.ParseQuoted( "[", "]" );
				break;

			// Val define
			case oexT( '{' ) :
				fmt[ i ][ "type" ] = "val";
				fmt[ i++ ][ "data" ] = str.ParseQuoted( "{", "}" );
				break;

			// Statement terminator
			case oexT( '.' ) :
				str++, fmt[ i++ ][ "type" ] = "end";
				break;

			// Statement terminator
			case oexT( '/' ) :
				str++, fmt[ i++ ][ "type" ] = "opt";
				break;

			// Pre statement break
			case oexT( ':' ) :
			{	str++;
				CPropertyBag pre = fmt;
				i = 0;
				fmt[ i ][ "type" ] = "pre";
				fmt[ i++ ][ "data" ] = pre;
			} break;

			// Literal string
			case oexT( '\"' ) : case oexT( '\'' ) : case oexT( '<' ) :
				fmt[ i ][ "type" ] = "lit";
				fmt[ i++ ][ "data" ] = str.ParseQuoted( "\"\'<", "\"\'>", "%" ).DropWhiteSpace();
				break;

			// Just accept any other characters as literal
			default :
				fmt[ i ][ "type" ] = "lit";
				fmt[ i++ ][ "data" ] = str.Parse( ":/.[{\"\'<;" ).DropWhiteSpace();
				break;

		} // end switch

	} // end while

	return oexTRUE;
}


CPropertyBag CParser::CompileTemplate( CStr str )
{
	CPropertyBag	fmt;
	oexUINT			uLine = 1;
	CStr			name, slot, body;
	oexCSTR			pTokenChars = oexT( "_123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ*" );

	// Process string
	while ( str.SkipWhiteSpace().Length() )
	{
		// Check for name
		if ( oexT( '#' ) == *str )
			str++, name = str.ParseToken( pTokenChars ), str.SkipWhiteSpace();
		else 
			name = "*";

		// Check for slot
		slot = "";
		while ( str.IsMatchAt( 0, oexT( "-+*" ) ) )
			slot = *str, str++;

		// Ensure we have a slot
		if ( !slot.Length() )
			slot = "-+*";

		// Compile this statement
		if ( !CompileStatement( str, fmt[ name ][ slot ][ "stmt" ] ) )
		{	fmt.Destroy(); return fmt; }

		uLine++;

	} // end while

	return fmt;
}

oexBOOL CParser::EncodeParam( CStr& str, CStr &key, CPropertyBag &val, CPropertyBag &fmt, oexBOOL bFirst, oexBOOL bLast )
{
	oexUINT i = 0;
	oexBOOL bSkip = oexFALSE;
	while( fmt[ i ].IsSet() )
	{
		CStr &type = fmt[ i ][ "type" ];
		CStr &data = fmt[ i ][ "data" ];

		if ( bFirst && type == "pre" )
			EncodeParam( str, key, val, fmt[ i ][ "data" ], bFirst, bLast );

		else if ( type == "end" )
		{	if ( !bLast )
				return oexTRUE;
			else
				bSkip = oexFALSE;
		} // end else if

		else if ( !bSkip )
		{
			if ( type == "lit" )
				str << data;

			else if ( type == "key" )
				str << key;

			else if ( type == "val" )
				str << val;

			else if ( type == "opt" )
			{	if ( bLast )
					bSkip = oexTRUE;
			} // end else if

		} // end else if

		i++;

	} // end while

	return oexTRUE;
}

CStr CParser::Encode( CPropertyBag &enc, CPropertyBag &fmt, CStr root )
{
	CStr str;

	CPropertyBag *pArrFmt = oexNULL, *pKvFmt = oexNULL, *pDefFmt = oexNULL;

	// Find appropriate templates
	CPropertyBag::iterator it;
	if ( ( it = fmt[ root ].List().SearchKey( "+", CStr::CmpMatch ) ).IsValid() )
		pArrFmt = &(*it)[ "stmt" ];
	if ( ( it = fmt[ root ].List().SearchKey( "-", CStr::CmpMatch ) ).IsValid() )
		pKvFmt = &(*it)[ "stmt" ];
	if ( ( it = fmt[ root ].List().SearchKey( "*", CStr::CmpMatch ) ).IsValid() )
		pDefFmt = &(*it)[ "stmt" ];

	// For each item
	for ( CPropertyBag::iterator it; enc.List().Next( it ); )
	{
		if ( it->IsArray() )
		{
			// Array param format
			if ( pArrFmt )
				EncodeParam( str, it.Node()->key, *it, *pArrFmt, oexNULL == it.IsPrev(), oexNULL == it.IsNext() );

			// Encode default parameter first if any
			if ( (*it)->Length() && fmt[ "*" ][ "-" ].IsSet() )
				EncodeParam( str, it.Node()->key, *it, fmt[ "*" ][ "-" ], 
								oexNULL == it.IsPrev(), oexNULL == it.IsNext() );

			if ( pKvFmt )
			{
				str << Encode( it, fmt, "sub" );

			} // end if

		} // end if

		// Key / Value pair format
		else if ( pValFmt )
			EncodeParam( str, it.Node()->key, *it, *pValFmt, oexNULL == it.IsPrev(), oexNULL == it.IsNext() );

	} // end for

	return str;
}


//CPropertyBag CParser::DecodeUrlParams( CStr str )
//{	Parse( str, "[url] = {url} /& .;" );
*/

