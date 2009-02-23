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
		str = CStr::NextToken( x_pStr, l, x_pValid, &p );
		
		if ( str.Length() ) 
			lst << str;

	} while ( str.Length() );

	return lst;
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

