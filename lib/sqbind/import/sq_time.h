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
    class CSqTime
    {
	public:

		SQBIND_CLASS_CTOR_BEGIN( CSqTime )
		SQBIND_CLASS_CTOR_END( CSqTime )

		CSqTime() {}

		// Copy semantics
		CSqTime( const CSqTime &r ) { m_t = r.m_t; }
		CSqTime& operator=( const CSqTime &r ) { m_t = r.m_t; return *this; }

		void GetLocalTime()
		{	m_t.GetLocalTime(); }

		void SetLocalTime()
		{	m_t.SetLocalTime(); }

		void GetSystemTime()
		{	m_t.GetSystemTime(); }

		void SetSystemTime()
		{	m_t.SetSystemTime(); }

		void SetTime( int year, int month, int day, int hour, int minute, int second )
		{	m_t.SetTime( year, month, day, hour, minute, second ); }

		int GetYear() { return m_t.GetYear(); }
		int GetMonth() { return m_t.GetMonth(); }
		int GetDay() { return m_t.GetDay(); }
		int GetHour() { return m_t.GetHour(); }
		int Get12Hour() { return m_t.Get12Hour(); }
		int GetMinute() { return m_t.GetMinute(); }
		int GetSecond() { return m_t.GetSecond(); }
		int GetMilliSecond() { return m_t.GetMilliSecond(); }
		int GetMicroSecond() { return m_t.GetMicroSecond(); }
		int GetNanoSecond() { return m_t.GetNanoSecond(); }

		void SetYear( int n ) { m_t.SetYear( n ); }
		void SetMonth( int n ) { m_t.SetMonth( n ); }
		void SetDay( int n ) { m_t.SetDay( n ); }
		void SetHour( int n ) { m_t.SetHour( n ); }
		void Set12Hour( int n ) { m_t.Set12Hour( n ); }
		void SetMinute( int n ) { m_t.SetMinute( n ); }
		void SetSecond( int n ) { m_t.SetSecond( n ); }
		void SetMilliSecond( int n ) { m_t.SetMilliSecond( n ); }
		void SetMicroSecond( int n ) { m_t.SetMicroSecond( n ); }
		void SetNanoSecond( int n ) { m_t.SetNanoSecond( n ); }

		int IsPM() { return m_t.IsPM(); }

		int GetUnixTime() { return m_t.GetUnixTime(); }
		void SetUnixTime( int n ) { m_t.SetUnixTime( n ); }
		int GetDosTime() { return m_t.GetDosTime(); }
		void SetDosTime( int n ) { m_t.SetDosTime( n ); }
		int GetNetTime() { return m_t.GetNetTime(); }
		void SetNetTime( int n ) { m_t.SetNetTime( n ); }

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

		int ParseTime( const stdString &sTmpl, const stdString &sTime )
		{	return m_t.ParseTime( sTmpl.c_str(), sTime.c_str() ); }

		static void Register( sqbind::VM vm );

	private:

		oex::CSysTime		m_t;

    };

}; // end namespace

// Declare type for use as squirrel parameters
SQBIND_DECLARE_INSTANCE( sqbind::CSqTime, CSqTime )
