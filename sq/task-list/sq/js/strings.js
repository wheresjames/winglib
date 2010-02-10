	 /*------------------------------------------------------------------
// strings.js
// Robert Umbehant
// rumbehant@wheresjames.com
// WheresJames Software (www.wheresjames.com)
//
// This file is covered by a 'BSD' style license as follows...
//
// Copyright (c) 2000, 2003, 2004
// Robert Umbehant
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or
// without modification, are permitted for commercial and
// non-commercial purposes, provided that the following
// conditions are met:
//
// * Redistributions of source code must retain the above copyright
//   notice, this list of conditions and the following disclaimer.
// * Redistributions in binary form must reproduce the above
//   copyright notice, this list of conditions and the following
//   disclaimer in the documentation and/or other materials
//   provided with the distribution.
// * Neither the name of WheresJames, Robert Umbehant, or the names
//   of its contributors may be used to endorse or promote products
//   derived from this software without specific prior written
//   permission.
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

	function MakeParams( x_params )
	{
		var ret = '';
		for ( var key in x_params )
		{
			if ( key && x_params[ key ] && x_params[ key ].constructor != Function )
			{
				// Continue link
				if ( ret ) ret += '&';

				if( x_params[ key ].constructor == Array ||
					x_params[ key ].constructor == Object )
				{
					ret += key + '=?' + escape( MakeParams( x_params[ key ] ) );
				}
				else ret += key + '=' + escape( x_params[ key ] );

			} // end if
		}

		return ret;
	}

	function ParseParams( x_params )
	{
		var ret = {};
		var parr = x_params.split( '&' );

		for ( var i = 0; i < parr.length; i++ )
		{
			kv = parr[ i ].split( '=' );
			if ( kv && kv[ 1 ] != null )
			{	if ( kv[ 1 ].charAt( 0 ) != '?' )
					ret[ kv[ 0 ] ] = unescape( kv[ 1 ] );
				else ret[ kv[ 0 ] ] = ParseParams( unescape( kv[ 1 ].substr( 1 ) ) );
			} // end if
		}
		return ret;
	}

	function SetDivTxt( x_div, x_txt )
	{	var t = document.getElementById( x_div );
		if ( t ) { t.innerHTML = x_txt; }
	}

	function SetDivImg( x_div, x_img )
	{	SetDivTxt( x_div, '<img src="' + x_img + '">' );
	}

	function print_r( theObj )
	{
		var str = "";

		if( theObj.constructor != Function )
		{
			str = "<ul>";
			for( var p in theObj )
			{
				if ( theObj[ p ].constructor != Function )
				{
					if( theObj[ p ].constructor == Array ||
						theObj[ p ].constructor == Object )
					{
						str += "<li>" + p + " = Array(...)</li>";

						str += "<ul>";
						str += print_r( theObj[ p ] );
						str += "</ul>";
					}
					else
					{
						str += "<li>" + p + " = " + theObj[ p ] + " </li>";
					}
				}
			}
			str += "</ul>";
		}
		return str;
	}

	function PadNum( n, pre, post, c )
	{	n = n.toString();
		if ( !c ) c = '0';
		while( n.length < pre ) n = c + n;
		while( n.length < post ) n = n + c;
		return n;
	}

	function sprintf( tmpl )
	{
		var i = 0;
		var str = '';
		var argn = 1;

		// Process the template
		while ( i < tmpl.length )
		{
			// If not the escape character
			if ( tmpl.charAt( i ) != '%' ) str += tmpl.charAt( i ).toString();

			// Replace escape sequence
			else
			{
				// Skip %
				i++;

				var pre = 0, post = 0;

				// Get prefix
				if ( tmpl.charAt( i ) >= '0' && tmpl.charAt( i ) <= '9' )
					pre = tmpl.charAt( i++ );

				// Get postfix
				if ( tmpl.charAt( i ) == '.' &&
					 tmpl.charAt( ++i ) >= '0' &&
					 tmpl.charAt( i ) <= '9' )
						post = tmpl.charAt( i++ );

				switch( tmpl.charAt( i ) )
				{
					case 'u' :
						str += parseInt( arguments[ argn++ ] );
						break;

					case 'f' :

						var n = arguments[ argn++ ];
						n = Math.round( parseFloat( n ) * 100 ) / 100;

						fstr = new String( n );
						d = fstr.indexOf( '.' );

						if ( d >= 0 )
						{	var n1 = parseInt( fstr.substr( 0, d ) );
							var n2 = parseInt( fstr.substr( d + 1 ) );
							if ( isNaN( n1 ) ) n1 = 0;
							if ( isNaN( n2 ) ) n2 = 0;
							str = PadNum( n1.toString(), pre, 0 );
							str += '.';
							str += PadNum( n2.toString(), 0, post );
						} // end if

						else
						{
							if ( isNaN( n ) ) str = PadNum( 0, 0, pre );
							else str = PadNum( n.toString(), pre, 0 );
							str += '.';
							str += PadNum( 0, 0, post );
						} // end else

						break;

					default :
						str += tmpl.charAt( i ).toString();
						break;

				}; // end switch

			} // end else

			// Next character
			i++;

		} // end while

		return str;
	}
	
	function ScsSerialize( x_params )
	{
		var ret = '';
		for ( var key in x_params )
		{
		    if ( key && x_params[ key ] )
		    {
		        // Continue link
		        if ( ret ) ret += ',';

		        // Save the key
		        ret += escape( key );

		        if( x_params[ key ].constructor == Array ||
		            x_params[ key ].constructor == Object )
		        {
		            ret += '{' + ScsSerialize( x_params[ key ] ) + '}'; 
		        }
		        else ret += '=' + escape( x_params[ key ] );

		    } // end if

		} // end for

		return ret;
	}

	function ScsDeserialize( x_params, x_arr )
	{
//		if ( x_arr == "undefined" )
//			x_arr = {};
		x_params += ',';
		var l = x_params.length, s = 0, e = 0;

		while ( e < l )
		{
		    switch( x_params[ e ] )
		    {
		        case ',' : case '}' :
		        {
		            var a = x_params.substr( s, e - s ).split( '=' );

		            if ( 1 < e - s )
		            {
		                // Valid?
		                if ( null == a[ 0 ] ) a[ 0 ] = 0;

		                // Decode
		                else a[ 0 ] = unescape( a[ 0 ] );

		                // Single value?
		                if ( null == a[ 1 ] ) x_arr[ 0 ] = '';

		                // Key / value pair
		                else x_arr[ a[ 0 ] ] = unescape( a[ 1 ] );

		            } // end if

		            // Next data
		            s = e + 1;

		            // Punt if end of array
		            if ( '}' == x_params[ e ] ) return e + 1;

		        } break;

		        case '{' :
		        {
		            // Get the key
		            var k = x_params.substr( s, e - s );

		            if ( k.length )
		            {
		                // Decode the key
		                k = unescape( k );

		                // Decode array
		                x_arr[ k ] = Array();
		                e += ScsDeserialize( x_params.substr( e ), x_arr[ k ] );

		            } // end if

		            // Next data
		            s = e + 1;

		        } break;

		    } // end switch

		    // Next e
		    e++;

		} // end while

		return e;
	}	
	
