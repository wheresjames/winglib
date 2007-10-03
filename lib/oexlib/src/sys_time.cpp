/*------------------------------------------------------------------
// sys_time.cpp
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

// Some helpful delta's
// 1970 = 1900 + 2208988800
// 1980 = 1970 + 315536400
// 1980 = 1900 + 2524525200
// 1900 = 1601 + 9435484800
// 1970 = 1601 + 11644473600
// 1980 = 1601 + 11960010000

static oexCSTR s_days[] = { oexT( "Sun" ), oexT( "Mon" ), oexT( "Tue" ), oexT( "Wed" ), oexT( "Thu" ), oexT( "Fri" ), oexT( "Sat" ), oexT( "Inv" ) };
static oexCSTR s_months[] = { oexT( "Inv" ), oexT( "Jan" ), oexT( "Feb" ), oexT( "Mar" ), oexT( "Apr" ), oexT( "May" ), oexT( "Jun" ), 
							  oexT( "Jul" ), oexT( "Aug" ), oexT( "Sep" ), oexT( "Oct" ), oexT( "Nov" ), oexT( "Dec" ) };
static oexCSTR s_fdays[] = { oexT( "Sunday" ), oexT( "Monday" ), oexT( "Tuesday" ), oexT( "Wednesday" ), oexT( "Thursday" ), oexT( "Friday" ), oexT( "Saturday" ), oexT( "Invalid" ) };
static oexCSTR s_fmonths[] = { oexT( "Invalid" ), oexT( "January" ), oexT( "February" ), oexT( "March" ), oexT( "April" ), oexT( "May" ), oexT( "June" ), 
							   oexT( "July" ), oexT( "August" ), oexT( "September" ), oexT( "October" ), oexT( "November" ), oexT( "December" ) };
// static oexUINT g_mdays[] = { 0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

#define FTOFF_1900		9435484800
#define FTOFF_1970		11644473600
#define FTOFF_1980		11960010000

oexUINT CSysTime::DivideSeconds(oexUINT totalseconds, oexUINT *days, oexUINT *hours, oexUINT *mins, oexUINT *secs)
{
	oexUINT s = 0, m = 0, h = 0, d = 0;

	// Get number of seconds
	s = totalseconds % 60; totalseconds -= s;

	// Get minutes
	m = ( totalseconds / 60 ) % 60; totalseconds -= m;

	// Get hours
	h = ( totalseconds / ( 60 * 60 ) ) % 24; totalseconds -= h;

	// Calc days
	d = totalseconds / ( 60 * 60 * 24 );

	// Write variables
	if ( days != oexNULL ) *days = d; else h += d * 60 * 60 * 24;
	if ( hours != oexNULL ) *hours = h; else m += h * 60 * 60;
	if ( mins != oexNULL ) *mins = m; else s = m * 60;
	if ( secs != oexNULL ) *secs = s;

	return totalseconds;
}

oexUINT CSysTime::IntegrateSeconds(oexUINT days, oexUINT hours, oexUINT mins, oexUINT secs)
{
	// Tally total seconds
	return (	( days * ( 60 * 60 * 24 ) ) + 
				( hours * ( 60 * 60 ) ) +
				( mins * 60 ) +
				  secs );
}

oexUINT CSysTime::GetUnixTime()
{
    oexINT64 ll = os::CSys::SystemTimeToFileTime( m_time );

	ll /= (oexINT64)10000000;
	ll -= (oexINT64)FTOFF_1970;

	return (oexUINT)ll;
}

CSysTime& CSysTime::SetUnixTime(oexUINT x_uTime, oexINT x_lTzBias )
{
	oexINT64 ll = x_uTime;

	ll += (oexINT64)FTOFF_1970;
	ll *= (oexINT64)10000000;

    os::CSys::FileTimeToSystemTime( m_time, ll );

    if ( eInvalid != x_lTzBias ) m_time.nTzBias = x_lTzBias;

    return *this;
}

oexUINT CSysTime::GetDosTime()
{
    oexINT64 ll = os::CSys::SystemTimeToFileTime( m_time );

	ll /= (oexINT64)10000000;
	ll -= (oexINT64)FTOFF_1980;

	return (oexUINT)ll;
}

CSysTime& CSysTime::SetDosTime(oexUINT x_uTime, oexINT x_lTzBias )
{
	oexINT64 ll = x_uTime;

	ll += (oexINT64)FTOFF_1980;
	ll *= (oexINT64)10000000;

    os::CSys::FileTimeToSystemTime( m_time, ll );

    if ( eInvalid != x_lTzBias ) m_time.nTzBias = x_lTzBias;

    return *this;
}

oexUINT CSysTime::GetNetTime()
{
    oexINT64 ll = os::CSys::SystemTimeToFileTime( m_time );

	ll /= (oexINT64)10000000;
	ll -= (oexINT64)FTOFF_1900;

	return (oexUINT)ll;
}

CSysTime& CSysTime::SetNetTime(oexUINT x_uTime, oexINT x_lTzBias )
{
	oexINT64 ll = x_uTime;

	ll += (oexINT64)FTOFF_1900;
	ll *= (oexINT64)10000000;

    os::CSys::FileTimeToSystemTime( m_time, ll );

    if ( eInvalid != x_lTzBias ) m_time.nTzBias = x_lTzBias;

    return *this;
}


oexCSTR CSysTime::GetAbrMonthName( oexLONG x_m )
{   if ( x_m < 0 || x_m > 12 ) return s_months[ 0 ];
	return s_months[ x_m ];
}

oexCSTR CSysTime::GetMonthName( oexLONG x_m )
{   if ( x_m < 0 || x_m > 12 ) return s_fmonths[ 0 ];
	return s_fmonths[ x_m ];
}

oexCSTR CSysTime::GetAbrDayName( oexLONG x_d )
{   if ( x_d < 0 || x_d > 6 ) return s_days[ 7 ];
	return s_days[ x_d ];
}

oexCSTR CSysTime::GetDayName( oexLONG x_d )
{   if ( x_d < 0 || x_d > 6 ) return s_fdays[ 7 ];
	return s_fdays[ x_d ];
}

// %h = hour 12 hour fixed 2 digits
// %H = hour 12 hour
// %g = hour 24 hour fixed 2 digits
// %G = hour 24 hour
// %m = minute fixed 2 digits
// %M = minute
// %s = second fixed 2 digits
// %S = second
// %l = milli seconds fixed 3 digits
// %L = milli seconds
// %a = am/pm
// %A = AM/PM
// %c = Month [01-12] fixed 2 digits
// %C = Month [1-12]
// %d = Day [01-31] fixed 2 digits
// %D = Day [1-31]
// %i = Day of the week [0-6]
// %I = Day of the week [1-7]
// %y = 2 digit year
// %Y = 4 digit year
// %a = am/pm
// %A = AM/PM
// %w = Abbreviated day of week [Sun,Mon,Tue,Wed,Thu,Fri,Sat]
// %W = Day of week [Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday]
// %b = Abbreviated Month [Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec]
// %B = Month [January,February,March,April,May,June,July,August,September,October,November,December]
CStr CSysTime::FormatTime( oexCSTR x_sTmpl, oexBOOL *x_bErrors )
{
    CStr str;
	oexUINT x = 0;

	// Process the template
	while ( x_sTmpl[ x ] != 0 )
	{
		// If not the escape character
		if ( x_sTmpl[ x ] != '%' ) str << x_sTmpl[ x ];

		// Replace escape sequence
		else switch( x_sTmpl[ ++x ] )
		{
			case 'h' : str.AppendNum( oexT( "%0.2lu" ), (oexUINT)Get12Hour() ); break;

			case 'H' : str.AppendNum( oexT( "%lu" ), (oexUINT)Get12Hour() ); break;

			case 'g' : str.AppendNum( oexT( "%0.2lu" ), (oexUINT)m_time.uHour ); break;

			case 'G' : str.AppendNum( oexT( "%lu" ), (oexUINT)m_time.uHour ); break;

			case 'm' : str.AppendNum( oexT( "%0.2lu" ), (oexUINT)m_time.uMinute ); break;

			case 'M' : str.AppendNum( oexT( "%lu" ), (oexUINT)m_time.uMinute ); break;

			case 's' : str.AppendNum( oexT( "%0.2lu" ), (oexUINT)m_time.uSecond ); break;

			case 'S' : str.AppendNum( oexT( "%lu" ), (oexUINT)m_time.uSecond ); break;

			case 'l' : str.AppendNum( oexT( "%0.3lu" ), (oexUINT)m_time.uMillisecond ); break;

			case 'L' : str.AppendNum( oexT( "%lu" ), (oexUINT)m_time.uMillisecond ); break;

			case 'c' : str.AppendNum( oexT( "%0.2lu" ), (oexUINT)m_time.uMonth ); break;

			case 'C' : str.AppendNum( oexT( "%lu" ), (oexUINT)m_time.uMonth ); break;

			case 'd' : str.AppendNum( oexT( "%0.2lu" ), (oexUINT)m_time.uDay ); break;

			case 'D' : str.AppendNum( oexT( "%lu" ), (oexUINT)m_time.uDay ); break;

			case 'i' : str.AppendNum( oexT( "%lu" ), (oexUINT)m_time.uDayOfWeek ); break;

			case 'I' : str.AppendNum( oexT( "%lu" ), (oexUINT)m_time.uDayOfWeek + 1 ); break;

			case 'y' : str.AppendNum( oexT( "%0.2lu" ), (oexUINT)m_time.uYear ); break;

			case 'Y' : str.AppendNum( oexT( "%lu" ), (oexUINT)m_time.uYear ); break;

            case 'a' : str << ( IsPM() ? oexT( "pm" ) : oexT( "am" ) ); break;

			case 'A' : str << ( IsPM() ? oexT( "PM" ) : oexT( "AM" ) ); break;

			case 'w' : str << GetAbrDayName( m_time.uDayOfWeek ); break;

			case 'W' : str << GetDayName( m_time.uDayOfWeek ); break;

			case 'b' : str << GetAbrMonthName( m_time.uMonth ); break;

			case 'B' : str << GetMonthName( m_time.uMonth ); break;

			case 'Z' : case 'z' :
			{
				switch( x_sTmpl[ ++x ] )
				{
					case 's' : case 'S' : str << ( ( 0 > m_time.nTzBias ) ? '-' : '+' ); break;

                    case 'h' : str.AppendNum( oexT( "%0.2lu" ), (oexUINT) ( 0 > m_time.nTzBias ? -m_time.nTzBias : m_time.nTzBias ) / 60 ); break;

					case 'H' : str.AppendNum( oexT( "%lu" ), (oexUINT) ( 0 > m_time.nTzBias ? -m_time.nTzBias : m_time.nTzBias ) / 60 ); break;

					case 'm' : str.AppendNum( oexT( "%0.2lu" ), (oexUINT) ( 0 > m_time.nTzBias ? -m_time.nTzBias : m_time.nTzBias ) % 60 ); break;

					case 'M' : str.AppendNum( oexT( "%lu" ), (oexUINT) ( 0 > m_time.nTzBias ? -m_time.nTzBias : m_time.nTzBias ) % 60 ); break;

					// Unknown escape sequence
					default : 
                        oexASSERT( 0 );
                        if ( x_bErrors ) *x_bErrors = oexTRUE; 
                        break;

				} // end switch

			} break;

			default :

                // Ensure valid template string
                oexASSERT( '%' == x_sTmpl[ x ] );

                if ( x_bErrors && '%' != x_sTmpl[ x ] )
                    *x_bErrors = oexTRUE;

                str << x_sTmpl[ x ];

				break;
		
		}; // end switch

		// Next character
		x++;

	} // end while

	return str;
}

oexBOOL CSysTime::ParseTime( oexCSTR x_sTmpl, CStr x_sStr )
{
    oexUINT x = 0;
	oexUINT uYear = eInvalid;
	oexUINT uMonth = eInvalid;
	oexUINT uDay = eInvalid;
	oexUINT uHour = eInvalid;
	oexUINT uMinute = eInvalid;
	oexUINT uSecond = eInvalid;
	oexUINT uMillisecond = eInvalid;
	oexUINT uDayOfWeek = eInvalid;

	oexBOOL bTzNeg = oexFALSE;
	oexINT  nHBias = eInvalid;
	oexINT  nMBias = eInvalid;

    oexBOOL bErrors = oexFALSE;
    oexINT nEnd = 0;

	// Process the template
	while ( x_sTmpl[ x ] != 0 )
    {
		// If not the escape character
		if ( x_sTmpl[ x ] != '%' ) 
		{	
            // Check character
			if ( x_sTmpl[ x ] != '*' && x_sTmpl[ x ] != '?' && *x_sStr != x_sTmpl[ x ] ) 
                bErrors = oexTRUE;

            x_sStr++;
		}

        else switch ( x_sTmpl[ ++x ] )
        {
            case 'h' : 
                uHour = (oexUINT)x_sStr.ToNum( 2, 10, &nEnd, oexTRUE ); 
                if ( 2 != nEnd || 1 > uHour || 12 < uHour ) bErrors = oexTRUE; 
                break;

            case 'H' : 
                uHour = (oexUINT)x_sStr.ToNum( 0, 10, &nEnd, oexTRUE );
                if ( !nEnd || 1 > uHour || 12 < uHour ) bErrors = oexTRUE; 
                break;

            case 'g' : 
                uHour = (oexUINT)x_sStr.ToNum( 2, 10, &nEnd, oexTRUE ); 
                if ( 2 != nEnd || 24 < uHour ) bErrors = oexTRUE; 
                break;

            case 'G' : 
                uHour = (oexUINT)x_sStr.ToNum( 0, 10, &nEnd, oexTRUE );
                if ( !nEnd || 24 < uHour ) bErrors = oexTRUE; 
                break;

            case 'm' : 
                uMinute = (oexUINT)x_sStr.ToNum( 2, 10, &nEnd, oexTRUE ); 
                if ( 2 != nEnd || 60 < uMinute ) bErrors = oexTRUE; 
                break;

            case 'M' : 
                uMinute = (oexUINT)x_sStr.ToNum( 0, 10, &nEnd, oexTRUE );
                if ( !nEnd || 60 < uMinute ) bErrors = oexTRUE; 
                break;

            case 's' : 
                uSecond = (oexUINT)x_sStr.ToNum( 2, 10, &nEnd, oexTRUE ); 
                if ( 2 != nEnd || 60 < uSecond ) bErrors = oexTRUE; 
                break;

            case 'S' : 
                uSecond = (oexUINT)x_sStr.ToNum( 0, 10, &nEnd, oexTRUE );
                if ( !nEnd || 60 < uSecond ) bErrors = oexTRUE; 
                break;

            case 'l' : 
                uMillisecond = (oexUINT)x_sStr.ToNum( 3, 10, &nEnd, oexTRUE ); 
                if ( 3 != nEnd || 1000 < uMillisecond ) bErrors = oexTRUE; 
                break;

            case 'L' : 
                uMillisecond = (oexUINT)x_sStr.ToNum( 0, 10, &nEnd, oexTRUE );
                if ( !nEnd || 1000 < uMillisecond ) bErrors = oexTRUE; 
                break;

            case 'c' : 
                uMonth = (oexUINT)x_sStr.ToNum( 2, 10, &nEnd, oexTRUE ); 
                if ( 2 != nEnd || 12 < uMonth ) bErrors = oexTRUE; 
                break;

            case 'C' : 
                uMonth = (oexUINT)x_sStr.ToNum( 0, 10, &nEnd, oexTRUE );
                if ( !nEnd || 12 < uMonth ) bErrors = oexTRUE; 
                break;

            case 'd' : 
                uDay = (oexUINT)x_sStr.ToNum( 2, 10, &nEnd, oexTRUE ); 
                if ( 2 != nEnd || 31 < uDay ) bErrors = oexTRUE; 
                break;

            case 'D' : 
                uDay = (oexUINT)x_sStr.ToNum( 0, 10, &nEnd, oexTRUE );
                if ( !nEnd || 31 < uDay ) bErrors = oexTRUE; 
                break;

            case 'i' : 
                uDayOfWeek = (oexUINT)x_sStr.ToNum( 1, 10, &nEnd, oexTRUE ); 
                if ( 1 != nEnd || 6 < uDayOfWeek ) bErrors = oexTRUE; 
                break;

            case 'I' : 
                uDayOfWeek = (oexUINT)x_sStr.ToNum( 1, 10, &nEnd, oexTRUE );
                if ( 1 != nEnd || 7 < uDayOfWeek ) bErrors = oexTRUE; 
                break;

            case 'y' : 
                uYear = (oexUINT)x_sStr.ToNum( 2, 10, &nEnd, oexTRUE ); 
                if ( 2 != nEnd ) bErrors = oexTRUE; 
                break;

            case 'Y' : 
                uYear = (oexUINT)x_sStr.ToNum( 4, 10, &nEnd, oexTRUE );
                if ( 4 != nEnd ) bErrors = oexTRUE; 
                break;

			case 'a' : 
			case 'A' : 
            {   oexBOOL bInvalid = oexFALSE;
                if ( x_sStr.ICmpLen( oexT( "pm" ), 2 ) )
                    uHour = ( uHour + 12 ) % 24;
                else if ( !x_sStr.ICmpLen( oexT( "am" ), 2 ) )
                    bInvalid = bErrors = oexTRUE;
                if ( !bInvalid ) x_sStr.LTrim( 2 );
            } break;

			case 'w' : 
            {   oexUINT k;
                for ( k = 1; k <= 7; k++ )
                    if ( x_sStr.ICmpLen( GetAbrDayName( k ) ) )
                    {   uDayOfWeek = k; x_sStr.LTrim( zstr::Length( GetAbrDayName( k ) ) ); k = 99; }
                if ( k < 99 ) bErrors = oexTRUE;
            } break;

			case 'W' : 
            {   oexUINT k;
                for ( k = 1; k <= 7; k++ )
                    if ( x_sStr.ICmpLen( GetDayName( k ) ) )
                    {   uDayOfWeek = k; x_sStr.LTrim( zstr::Length( GetDayName( k ) ) ); k = 99; }
                if ( k < 99 ) bErrors = oexTRUE;
            } break;
            
			case 'b' : 
            {   oexUINT k;
                for ( k = 1; k <= 12; k++ )
                    if ( x_sStr.ICmpLen( GetAbrMonthName( k ) ) )
                    {   uMonth = k; x_sStr.LTrim( zstr::Length( GetAbrMonthName( k ) ) ); k = 99; }
                if ( k < 99 ) bErrors = oexTRUE;
            } break;

			case 'B' : 
            {   oexUINT k;
                for ( k = 1; k <= 12; k++ )
                    if ( x_sStr.ICmpLen( GetMonthName( k ) ) )
                    {   uMonth = k; x_sStr.LTrim( zstr::Length( GetMonthName( k ) ) ); k = 99; }
                if ( k < 99 ) bErrors = oexTRUE;
            } break;

			case 'Z' : case 'z' :
			{
				switch( x_sTmpl[ ++x ] )
				{
					case 's' : case 'S' :
						if ( '+' == *x_sStr ) x_sStr++, bTzNeg = oexFALSE;
						else if ( '-' == *x_sStr ) x_sStr++, bTzNeg = oexTRUE;
                        else bErrors = oexTRUE;
						break;

                    case 'h' : 
                        nHBias = (oexINT)x_sStr.ToNum( 2, 10, &nEnd, oexTRUE ); 
                        if ( 2 != nEnd ) bErrors = oexTRUE; 
                        break;

                    case 'H' : 
                        nHBias = (oexINT)x_sStr.ToNum( 0, 10, &nEnd, oexTRUE ); 
                        if ( !nEnd ) bErrors = oexTRUE; 
                        break;

                    case 'm' : 
                        nMBias = (oexINT)x_sStr.ToNum( 2, 10, &nEnd, oexTRUE ); 
                        if ( 2 != nEnd ) bErrors = oexTRUE; 
                        break;

                    case 'M' : 
                        nMBias = (oexINT)x_sStr.ToNum( 0, 10, &nEnd, oexTRUE ); 
                        if ( !nEnd ) bErrors = oexTRUE; 
                        break;

					default :
                        bErrors = oexTRUE;
						break;

				} // end switch

			} break;

			default :
                bErrors = oexTRUE;
				break;

        } // end else switch

        // Next character
        x++;

    } // end while

    // If this asserts you're time stamp did not match 
    // the template passed to CSysTime::ParseTime()
    oexASSERT( !bErrors );

	// Calculate bias
	oexINT nTzBias = eInvalid;
	if ( nHBias != eInvalid ) nTzBias = nHBias * 60;
	if ( nMBias != eInvalid ) nTzBias = ( nTzBias == eInvalid ) ? nMBias : nTzBias + nMBias;
	if ( eInvalid != nTzBias && bTzNeg ) nTzBias = -nTzBias;

	// Set valid parts of the time
	SetTime( uYear, uMonth, uDay, uHour, uMinute, uSecond, uMillisecond, uDayOfWeek, nTzBias );
    
    return oexTRUE;
}

