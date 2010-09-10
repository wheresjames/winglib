/*------------------------------------------------------------------
// sys_time.h
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

//==================================================================
// CSysTime
//
/// Provides time manipulation functions
/**
	Provides time manipulation functions.  I'm not especially proud
	of the time zone handling, though it works properly.  However,
	the functions to convert to and from time strings have been
	priceless.  Not to mention conversion between UNIX, DOS, Net,
	and File times.

*/
//==================================================================
class CSysTime
{
public:

	// Time type
	typedef oexINT64		t_time;

public:

	enum
	{
		/// Invalid time component
		eInvalid = (oexINT)0x7fffffff
	};

	enum
	{
		/// Value indicating uninitialized time
		eNullTime  = 0,

		/// Value indicating local time
		eLocalTime  = 1,

		/// Value indicating GMT time
		eGmtTime = 2
	};

	enum
	{
		/// File time format
		eFmtFile = 1,

		/// Unix timestamp format
		eFmtUnix = 2,

		/// Dos timestamp format
		eFmtDos = 3,

		/// Net timestamp format
		eFmtNet = 4
	};

public:

	/// Constructor
	/**
		\param [in] x_uInitTime -   0   =   Uninitialized
									1   =   Local time
									2   =   GMT time
	*/
	CSysTime( oexUINT x_uInitTime = eNullTime )
	{
		m_tEscape = oexT( '%' );

		if ( eLocalTime == x_uInitTime )
			GetLocalTime();

		else if ( eGmtTime == x_uInitTime )
			GetSystemTime();

		else
			oexZeroMemory( &m_time, sizeof( m_time ) );
	}

	CSysTime( oexCONST CSysTime &x_st )
	{
		m_tEscape = oexT( '%' );
		os::CSys::MemCpy( &m_time, &x_st.m_time, sizeof( m_time ) ); 
	}

	CSysTime( oexINT x_nTimeFormat, oexINT64 x_llTime, oexINT x_lTzBias = eInvalid )
	{
		m_tEscape = oexT( '%' );

		if ( eFmtFile == x_nTimeFormat )
			SetFileTime( x_llTime, x_lTzBias );

		else if ( eFmtUnix == x_nTimeFormat )
			SetUnixTime( (oexUINT)x_llTime, x_lTzBias );

		else if ( eFmtDos == x_nTimeFormat )
			SetDosTime( (oexUINT)x_llTime, x_lTzBias );

		else if ( eFmtNet == x_nTimeFormat )
			SetNetTime( (oexUINT)x_llTime, x_lTzBias );

		else
			oexZeroMemory( &m_time, sizeof( m_time ) );
	}

	// Initializes object with string
	CSysTime( oexCSTR x_sTmpl, oexCONST CStr x_sStr )
	{
		m_tEscape = oexT( '%' );

		ParseTime( x_sTmpl, CStr( x_sStr ) );
	} 

	/// Destructor
	virtual ~CSysTime()
	{
	}
	
	CSysTime& GetLocalTime()
	{   os::CSys::GetLocalTime( m_time ); return *this; }

	CSysTime& SetLocalTime()
	{   os::CSys::SetLocalTime( m_time ); return *this; }

	CSysTime& GetSystemTime()
	{   os::CSys::GetSystemTime( m_time ); return *this; }

	CSysTime& SetSystemTime()
	{   os::CSys::SetSystemTime( m_time ); return *this; }

	//==============================================================
	// setEscapeChar()
	//==============================================================
	/// Sets the format escape charachter
	void setEscapeChar( oexTCHAR c ) {	m_tEscape = c; };

	//==============================================================
	// getEscapeChar()
	//==============================================================
	/// Gets the format escape charachter
	oexTCHAR getEscapeChar() {	return m_tEscape; };

	//==============================================================
	// SetTime()
	//==============================================================
	/// Sets the specified time
	/**
		\param [in] uYear			-	Four digit year
		\param [in] uMonth			-	Month ( 1 - 12 )
		\param [in] uDay			-	Day of the month ( 1 - 31 )
		\param [in] uHour			-	Hour of the day ( 0 - 23 )
		\param [in] uMinute			-	Minute of the hour ( 0 - 59 )
		\param [in] uSecond			-	Second of the minute ( 0 - 59 )
		\param [in] uMilliseconds	-	Millisecond value ( 0 - 999 )
		\param [in] uMicroseconds	-	Microsecond value ( 0 - 999 )
		\param [in] uNanoseconds	-	Nanosecond value ( 0 - 999 )
		\param [in] uDayOfWeek		-	Day of the week ( 1 - 7 )
		\param [in] lTzBias			-	Time zone bias, MAXLONG for
										none.
	*/
	CSysTime& SetTime(	oexUINT uYear = eInvalid, oexUINT uMonth = eInvalid, oexUINT uDay = eInvalid,
					oexUINT uHour = eInvalid, oexUINT uMinute = eInvalid, oexUINT uSecond = eInvalid,
					oexUINT uMilliseconds = eInvalid, oexUINT uMicrosecond = eInvalid, oexUINT uNanoseconds = eInvalid,
					oexUINT uDayOfWeek = eInvalid, oexINT nTzBias = eInvalid );

	//==============================================================
	// GetYear()
	//==============================================================
	/// Returns the four digit year
	oexUINT GetYear() { return m_time.uYear; }
	oexUINT SetYear( oexUINT y )
	{   if ( y > 9999 ) y = 9999; return m_time.uYear = y; }

	//==============================================================
	// GetMonth()
	//==============================================================
	/// Returns the month ( 1 - 12 )
	oexUINT GetMonth() { return m_time.uMonth; }
	oexUINT SetMonth( oexUINT m )
	{   if ( m > 12 ) m = 1; return m_time.uMonth = m; }

	//==============================================================
	// GetDay()
	//==============================================================
	/// Returns the day ( 1 - 31 )
	oexUINT GetDay() { return m_time.uDay; }
	oexUINT SetDay( oexUINT d )
	{   if ( d > 31 ) d = 1; return m_time.uDay = d; }

	//==============================================================
	// GetDayOfWeek()
	//==============================================================
	/// Returns the day of the week ( 1 - 7 )
	oexUINT GetDayOfWeek() { return m_time.uDayOfWeek; }
	oexUINT SetDayOfWeek( oexUINT d )
	{   if ( d > 7 ) d = 1; return m_time.uDayOfWeek = d; }

	//==============================================================
	// GetHour()
	//==============================================================
	/// Returns the hour ( 0 - 23 ).  For 12-hour value, see Get12Hour()
	oexUINT GetHour() { return m_time.uHour; }
	oexUINT SetHour( oexUINT h )
	{   if ( h >= 24 ) h %= 24; return m_time.uHour = h; }

	//==============================================================
	// Get12Hour()
	//==============================================================
	/// Returns the 12-hour value for the hour. ( 1 - 12 )
	oexUINT Get12Hour()
	{	oexUINT hour = m_time.uHour;
		if ( hour > 12 ) hour -= 12;
		else if ( hour == 0 ) hour = 12;
		return hour;
	}
	oexUINT Set12Hour( oexUINT h )
	{   if ( h < 1 || h > 12 ) h %= 12, h++; return m_time.uHour = h; }

	//==============================================================
	// GetMinute()
	//==============================================================
	/// Returns the minute ( 0 - 59 )
	oexUINT GetMinute() { return m_time.uMinute; }
	oexUINT SetMinute( oexUINT m )
	{   if ( m >= 60 ) m %= 60; return m_time.uMinute = m; }

	//==============================================================
	// GetSecond()
	//==============================================================
	/// Returns the second ( 0 - 59 )
	oexUINT GetSecond() { return m_time.uSecond; }
	oexUINT SetSecond( oexUINT s )
	{   if ( s >= 60 ) s %= 60; return m_time.uSecond = s; }

	//==============================================================
	// GetMilliSecond()
	//==============================================================
	/// Returns the milli-second value ( 0 - 999 )
	oexUINT GetMilliSecond() { return m_time.uMillisecond; }
	oexUINT SetMilliSecond( oexUINT m )
	{   if ( m >= 1000 ) m %= 1000; return m_time.uMillisecond = m; }

	//==============================================================
	// GetMicroSecond()
	//==============================================================
	/// Returns the micro-second value ( 0 - 999 )
	oexUINT GetMicroSecond() { return m_time.uMicrosecond; }
	oexUINT SetMicroSecond( oexUINT m )
	{   if ( m >= 1000 ) m %= 1000; return m_time.uMicrosecond = m; }

	//==============================================================
	// GetNanoSecond()
	//==============================================================
	/// Returns the nano-second value ( 0 - 999 )
	oexUINT GetNanoSecond() { return m_time.uNanosecond; }
	oexUINT SetNanoSecond( oexUINT m )
	{   if ( m >= 1000 ) m %= 1000; return m_time.uNanosecond = m; }

	//==============================================================
	// IsPM()
	//==============================================================
	/// Returns non-zero if the time is 'Post Meridiem' or 'after noon'.
	/**

		This function incorrectly returns non-zero for the time 12:00:00.
		Technically it is not PM until 12:00:01.  12:00:00 is not AM or
		PM.  Same for 00:00:00.  But this is the common use.

		\return Non-zero if the time is 'Post Meridiem' or 'after noon'.

		\see
	*/
	oexBOOL IsPM() { return ( m_time.uHour >= 12 ); }

	//==============================================================
	// SetTzBias()
	//==============================================================
	/// Sets the current timezone bias.
	/**
		\param [in] lTzBias

		\return The value in lTzBias
	*/
	oexINT SetTzBias( oexINT nTzBias )
	{	return ( m_time.nTzBias = nTzBias ); }

	//==============================================================
	// GetTzBias()
	//==============================================================
	/// Returns the current timezone bias
	oexINT GetTzBias() { return m_time.nTzBias; }

	/// Divide seconds
	oexUINT DivideSeconds(oexUINT totalseconds, oexUINT *days = oexNULL, oexUINT *hours = oexNULL, oexUINT *mins = oexNULL, oexUINT *secs = oexNULL);

	/// Integrate seconds
	oexUINT IntegrateSeconds(oexUINT days, oexUINT hours = 0, oexUINT mins = 0, oexUINT secs = 0);

	/// Returns the unix time stamp
	oexUINT GetUnixTime();

	/// Returns the unix time stamp with time zone bias
	oexUINT GetTzUnixTime();

	/// Sets the unix timestamp
	CSysTime& SetUnixTime( oexUINT x_uTime, oexINT x_lTzBias = eInvalid );

	/// Returns the dos timestamp
	oexUINT GetDosTime();

	/// Sets the dos timestamp
	CSysTime& SetDosTime( oexUINT x_uTime, oexINT x_lTzBias = eInvalid );

	/// Returns the net timestamp
	oexUINT GetNetTime();

	/// Sets the net timestamp
	CSysTime& SetNetTime( oexUINT x_uTime, oexINT x_lTzBias = eInvalid );

	/// Returns the abbreviated month name
	static oexCSTR GetAbrMonthName( oexLONG x_m );

	/// Returns the month name
	static oexCSTR GetMonthName( oexLONG x_m );

	/// Returns the abbreviated day name
	static oexCSTR GetAbrDayName( oexLONG x_d );

	/// Returns the day name
	static oexCSTR GetDayName( oexLONG x_d );

	/// Returns the file time format
	oexINT64 GetFileTime()
	{   return os::CSys::SystemTimeToFileTime( m_time ); }

	/// Sets the file time format
	void SetFileTime( oexINT64 x_llFt, oexINT x_lTzBias )
	{   if ( eInvalid != x_lTzBias ) m_time.nTzBias = x_lTzBias;
		os::CSys::FileTimeToSystemTime( m_time, x_llFt );
	}


	//==============================================================
	// FormatTime()
	//==============================================================
	/// Returns a formated time string
	/**
		\param [in] x_sTmpl	    -	Template string
		\param [out] x_bErrors  -   Optional parameter that is set
									to non-zero if any errors are
									detected while decoding.

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
		-	\%Zm = Minutes of offset in time zone fixed two digits
		-	\%ZM = Minutes of offset in time zone
		-	\%Zz = Time zone offset in minutes with leading +/-
		-	\%ZZ = Time zone offset in seconds with leading +/-

		Some examples:

		-   "%W, %B %D, %Y - %h:%m:%s %A"	= Thursday, December 25, 1997 - 04:15:30 PM
		-   "%Y/%c/%d - %g:%m:%s.%l"		= 1997/12/25 - 16:15:30.500
		-   "%w, %d %b %Y %g:%m:%s GMT"		= Thu, 25 Dec 1997 16:15:30 GMT
		-	"%c/%b/%Y:%g:%m:%s %Zs%Zh%Zm"	= 25/Dec/1997:16:15:30 -0500

		\return Formated string

		\see ParseString()
	*/
	CStr FormatTime( oexCSTR x_sTmpl, oexBOOL *x_bErrors = oexNULL );

	//==============================================================
	// ParseString()
	//==============================================================
	///
	/**
		\param [in] x_sTmpl	-	Template string
		\param [in] x_sStr  -   String to parse

		This function decomposes an existing time string based
		on the template string.

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
		-	\%Zm = Minutes of offset in time zone fixed two digits
		-	\%ZM = Minutes of offset in time zone
		-	\%Zz = Time zone offset in minutes with leading +/-
		-	\%ZZ = Time zone offset in seconds with leading +/-

		For timezones, leading +/- is optional. + is the default.

		\return This function returns Zero if the format does not match the template

		\see FormatTime()
	*/
	oexBOOL ParseTime( oexCSTR x_sTmpl, CStr x_sStr );

	/// Copy operator
	CSysTime& operator = ( oexCONST CSysTime &x_st )
	{   os::CSys::MemCpy( &m_time, &x_st.m_time, sizeof( m_time ) ); return *this; }

	/// Compare
	oexBOOL operator == ( oexCONST CSysTime &x_st )
	{   return !os::CSys::MemCmp( &m_time, &x_st.m_time, sizeof( m_time ) ) ? oexTRUE : oexFALSE; }

	oexBOOL operator != ( oexCONST CSysTime &x_st )
	{   return os::CSys::MemCmp( &m_time, &x_st.m_time, sizeof( m_time ) ) ? oexTRUE : oexFALSE; }

	/// Calculate time range
	static oexBOOL CalculateTimes( t_time *tMin, t_time *tMax, t_time tDefaultRange );

private:

	oexTCHAR								m_tEscape;

	/// Time structure
	os::CSys::STime							m_time;

};

