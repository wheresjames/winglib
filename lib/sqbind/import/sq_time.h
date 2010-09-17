/*------------------------------------------------------------------
// sq_time.h
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

#pragma once

// namespace
namespace sqbind
{
    class CSqTimeRange
    {
	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqTimeRange )
			_SQBIND_CLASS_CTOR( CSqTimeRange, 1 ) ( sa.GetInt( 1 ) )
			_SQBIND_CLASS_CTOR( CSqTimeRange, 2 ) ( sa.GetInt( 1 ), sa.GetInt( 2 ) )
		SQBIND_CLASS_CTOR_END( CSqTimeRange )

		static void Register( sqbind::VM vm );

		CSqTimeRange() { m_nStart = m_nStartMs = m_nEnd = m_nEndMs = 0; }
		CSqTimeRange( SQINT t ) { m_nStart = m_nEnd = t; m_nStartMs = m_nEndMs = 0; }
		CSqTimeRange( SQINT tStart, SQINT tEnd ) { m_nStart = tStart; m_nEnd = tEnd;  m_nStartMs = m_nEndMs = 0; }
		CSqTimeRange( SQINT tStart, SQINT tStartMs, SQINT tEnd, SQINT tEndMs ) 
		{ m_nStart = tStart; m_nStartMs = tStartMs; m_nEnd = tEnd; m_nEndMs = tEndMs; }

		// Copy semantics
		CSqTimeRange( const CSqTimeRange &r ) 
		{ m_nStart = r.m_nStart; m_nStartMs = r.m_nStartMs; m_nEnd = r.m_nEnd; m_nEndMs = r.m_nEndMs; }
		CSqTimeRange& operator=( const CSqTimeRange &r ) 
		{ m_nStart = r.m_nStart; m_nStartMs = r.m_nStartMs; m_nEnd = r.m_nEnd; m_nEndMs = r.m_nEndMs; return *this; }

		/// Returns the start time
		SQINT getStart() { return m_nStart; }

		/// Sets the start time
		void setStart( SQINT n ) { m_nStart = n; }

		/// Returns the start time
		SQINT getStartMs() { return m_nStartMs; }

		/// Sets the start time
		void setStartMs( SQINT n ) { m_nStartMs = n; }

		/// Returns the end time
		SQINT getEnd() { return m_nEnd; }

		/// Sets the end time
		void setEnd( SQINT n ) { m_nEnd = n; }

		/// Returns the end time
		SQINT getEndMs() { return m_nEndMs; }

		/// Sets the end time
		void setEndMs( SQINT n ) { m_nEndMs = n; }

		/// Start time "t.ms"
		void setStartStr( const stdString &s )
		{	m_nStart = oexStrToLong( s.c_str() );
			stdString::size_type pos = s.find_first_of( oexT( '.' ) );
			m_nStartMs = ( stdString::npos != pos ) ? oexStrToLong( s.substr( pos + 1, 3 ).c_str() ) : 0;
		}

		/// Returns start time as "t.ms"
		stdString getStartStr()
		{	return oex2std( oexFmt( oexT( "%u.%.3u" ), m_nStart, m_nStartMs ) ); }

		/// End time "t.ms"
		void setEndStr( const stdString &s )
		{	m_nEnd = oexStrToLong( s.c_str() );
			stdString::size_type pos = s.find_first_of( oexT( '.' ) );
			m_nEndMs = ( stdString::npos != pos ) ? oexStrToLong( s.substr( pos + 1, 3 ).c_str() ) : 0;
		}

		/// Returns end time as "t.ms"
		stdString getEndStr()
		{	return oex2std( oexFmt( oexT( "%u.%.3u" ), m_nEnd, m_nEndMs ) ); }

		/// Calculate times
		void CalculateTime( SQINT nStart, SQINT nEnd, SQINT nDefaultRange )
		{	oex::CSysTime::t_time tMin = (oex::CDataLog::t_time)nStart;
			oex::CSysTime::t_time tMax = (oex::CDataLog::t_time)nEnd;
			oex::CSysTime::CalculateTimes( &tMin, &tMax, nDefaultRange );
			m_nStart = (SQINT)tMin; m_nStartMs = 0;
			m_nEnd = (SQINT)tMax; m_nEndMs = 0;
		}

		/// Compares time passed in to start time
		int CompareStart( int t, int ms )
		{	if ( t < m_nStart || t == m_nStart && ms < m_nStartMs )
				return -1;
			if ( t > m_nStart || t == m_nStart && ms > m_nStartMs )
				return 1; 
			return 0;
		}

		/// Compares time passed in to end time
		int CompareEnd( int t, int ms )
		{	if ( t < m_nStart || t == m_nStart && ms < m_nStartMs )
				return -1;
			if ( t > m_nStart || t == m_nStart && ms > m_nStartMs )
				return 1; 
			return 0;
		}

	private:

		/// Start time
		SQINT		m_nStart;

		/// Start time
		SQINT		m_nStartMs;

		/// End time
		SQINT		m_nEnd;

		/// End time
		SQINT		m_nEndMs;
	};

    class CSqTime
    {
	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqTime )
		SQBIND_CLASS_CTOR_END( CSqTime )

		CSqTime() {}

		// Copy semantics
		CSqTime( const CSqTime &r ) { m_t = r.m_t; }
		CSqTime& operator=( const CSqTime &r ) { m_t = r.m_t; return *this; }

		void setEscapeChar( const stdString &s ) { if ( s.length() ) m_t.setEscapeChar( s[ 0 ] ); }

		stdString getEscapeChar() { oex::oexTCHAR tc[ 2 ] = { m_t.getEscapeChar(), 0 }; return tc; }

		void GetLocalTime()
		{	m_t.GetLocalTime(); }

		void SetLocalTime()
		{	m_t.SetLocalTime(); }

		void GetSystemTime()
		{	m_t.GetSystemTime(); }

		void SetSystemTime()
		{	m_t.SetSystemTime(); }

		void SetTime( SQINT year, SQINT month, SQINT day, SQINT hour, SQINT minute, SQINT second )
		{	m_t.SetTime( year, month, day, hour, minute, second ); }

		SQINT GetYear() { return m_t.GetYear(); }
		SQINT GetMonth() { return m_t.GetMonth(); }
		SQINT GetDay() { return m_t.GetDay(); }
		SQINT GetDayOfWeek() { return m_t.GetDayOfWeek(); }		
		SQINT GetHour() { return m_t.GetHour(); }
		SQINT Get12Hour() { return m_t.Get12Hour(); }
		SQINT GetMinute() { return m_t.GetMinute(); }
		SQINT GetSecond() { return m_t.GetSecond(); }
		SQINT GetMilliSecond() { return m_t.GetMilliSecond(); }
		SQINT GetMicroSecond() { return m_t.GetMicroSecond(); }
		SQINT GetNanoSecond() { return m_t.GetNanoSecond(); }

		void SetYear( SQINT n ) { m_t.SetYear( n ); }
		void SetMonth( SQINT n ) { m_t.SetMonth( n ); }
		void SetDay( SQINT n ) { m_t.SetDay( n ); }
		void SetDayOfWeek( SQINT n ) { m_t.SetDayOfWeek( n ); }
		void SetHour( SQINT n ) { m_t.SetHour( n ); }
		void Set12Hour( SQINT n ) { m_t.Set12Hour( n ); }
		void SetMinute( SQINT n ) { m_t.SetMinute( n ); }
		void SetSecond( SQINT n ) { m_t.SetSecond( n ); }
		void SetMilliSecond( SQINT n ) { m_t.SetMilliSecond( n ); }
		void SetMicroSecond( SQINT n ) { m_t.SetMicroSecond( n ); }
		void SetNanoSecond( SQINT n ) { m_t.SetNanoSecond( n ); }

		void SetTzBias( SQINT n ) { m_t.SetTzBias( n ); }
		SQINT GetTzBias() { return m_t.GetTzBias(); }

		SQINT IsPM() { return m_t.IsPM(); }

		SQINT GetUnixTime() { return m_t.GetUnixTime(); }
		void SetUnixTime( SQINT n ) { m_t.SetUnixTime( n ); }
		SQINT GetDosTime() { return m_t.GetDosTime(); }
		void SetDosTime( SQINT n ) { m_t.SetDosTime( n ); }
		SQINT GetNetTime() { return m_t.GetNetTime(); }
		void SetNetTime( SQINT n ) { m_t.SetNetTime( n ); }
		SQINT GetTzUnixTime() { return m_t.GetUnixTime(); }

		/**
			\param [in] x_sTmpl	    -	Template string

			Formats a time string based on the specified template.

			-	\%h = hour 12 hour fixed 2 digits
			-	\%H = hour 12 hour
			-	\%g = hour 24 hour fixed 2 digits
			-	\%G = hour 24 hour
			-	\%m = minute fixed 2 digits
			-	\%M = minute
			-	\%s = second fixed 2 digits
			-	\%S = second
			-	\%l = milli seconds fixed 3 digits
			-	\%L = milli seconds
			-	\%u = micro seconds fixed 3 digits
			-	\%U = micro seconds
			-	\%n = nano seconds fixed 3 digits
			-	\%N = nano seconds
			-	\%a = am/pm
			-	\%A = AM/PM
			-	\%c = Month [01-12] fixed 2 digits
			-	\%C = Month [1-12]
			-	\%d = Day [01-31] fixed 2 digits
			-	\%D = Day [1-31]
			-	\%i = Day of the week [0-6]
			-	\%I = Day of the week [1-7]
			-	\%y = 2 digit year
			-	\%Y = 4 digit year
			-	\%a = am/pm
			-	\%A = AM/PM
			-	\%w = Abbreviated day of week [Sun,Mon,Tue,Wed,Thu,Fri,Sat]
			-	\%W = Day of week [Sunday,Monday,Tuesday,Wednesday,Thursday,Friday,Saturday]
			-	\%b = Abbreviated Month [Jan,Feb,Mar,Apr,May,Jun,Jul,Aug,Sep,Oct,Nov,Dec]
			-	\%B = Month [January,February,March,April,May,June,July,August,September,October,November,December]
			-	\%Za = Lower case time zone abbreviation [est,gmt,pst...]
			-	\%ZA = Upper case time zone abbreviation [EST,GMT,PST...]
			-	\%Zs = Time zone sign +/-
			-	\%Zh = Hours of offset in time zone fixed two digits
			-	\%ZH = Hours of offset in time zone
			-	\%Zh = Minutes of offset in time zone fixed two digits
			-	\%ZH = Minutes of offset in time zone
			-	\%Zz = Time zone offset in minutes with leading +/-
			-	\%ZZ = Time zone offset in seconds with leading +/-

			Some examples:

			-   "%W, %B %D, %Y - %h:%m:%s %A"  = Thursday, December 25, 1997 - 04:15:30 PM
			-   "%Y/%c/%d - %g:%m:%s.%l"    = 1997/12/25 - 16:15:30.500
			-   "%w, %d %b %Y %g:%m:%s GMT" = Thu, 25 Dec 1997 16:15:30 GMT
			-   "%w, %d-%b-%Y %g:%m:%s GMT" = Thu, 25-Dec-1997 16:15:30 GMT
			-	"%Y-%c-%d %g:%m:%s" = 2009-05-06 00:00:00

			\return Formated string

			\see ParseString()
		*/
		stdString FormatTime( const stdString &sTmpl )
		{	return m_t.FormatTime( sTmpl.c_str() ).Ptr(); }

		SQINT ParseTime( const stdString &sTmpl, const stdString &sTime )
		{	return m_t.ParseTime( sTmpl.c_str(), sTime.c_str() ); }

		/// Returns the abbreviated month name
		stdString getAbrMonthName( SQINT x_m )
		{	return m_t.GetAbrMonthName( x_m ); }

		/// Returns the month name
		stdString getMonthName( SQINT x_m )
		{	return m_t.GetMonthName( x_m ); }

		/// Returns the abbreviated day name
		stdString getAbrDayName( SQINT x_d )
		{	return m_t.GetAbrDayName( x_d ); }

		/// Returns the day name
		stdString getDayName( SQINT x_d )
		{	return m_t.GetDayName( x_d ); }

		static void Register( sqbind::VM vm );

	private:

		oex::CSysTime		m_t;

    };

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqTime, CSqTime )
SQBIND_DECLARE_INSTANCE( sqbind::CSqTimeRange, CSqTimeRange )
