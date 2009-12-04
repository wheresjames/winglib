/*------------------------------------------------------------------
// hq_timer.h
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
// CHqTimer
//
/// Timer class using high accuracy hardware timer if available
/**
	This class attempts to use the high accuracy hardware timer
	via the Windows API QueryPerformanceCounter() function if
	available.  If this counter is not available, it falls back
	on the less reliable GetTickCount() value.

  \code

	CHqTimer htTimer1;

	htTimer1.Start();

	DoSomethingSlow();

	oexINT64 llElapsedTicks = htTimer.Elapsed();

	oexDOUBLE dElapsedSeconds = htTimer.ElapsedSeconds();

  \endcode

*/
//==================================================================
class CHqTimer
{
public:

	/// Default Constructor
	CHqTimer( oexBOOL x_bStart = oexFALSE )
	{
		// Reset timer values
		m_llStart = m_llStop = 0;

        // Do we want to start the timer?
        if ( x_bStart )
            Start();
	}

	/// Returns the timer frequency
	static oexINT64 osGetTimerFreq();

	/// Returns the timer value
	static oexINT64 osGetTimerValue();

	/// Destructor
	virtual ~CHqTimer() {}

	//==============================================================
	// GetTimerFrequency()
	//==============================================================
	/// Returns the frequency of the active timer
	static oexINT64 GetTimerFrequency()
	{	return m_llFreq; }

	//==============================================================
	// GetTimer()
	//==============================================================
	/// Return the current timer value
	static oexINT64 GetTimer()
	{	return osGetTimerValue(); }

	//==============================================================
	// GetTimer()
	//==============================================================
	/// Return the current timer value
	static oexDOUBLE GetTimerSeconds()
	{	oexINT64 freq = osGetTimerFreq();
		if ( !freq ) return 0;
		oexINT64 val = osGetTimerValue();
		return oexDOUBLE( val ) / oexDOUBLE( freq );
	}

	//==============================================================
	// IsReset()
	//==============================================================
	/// Returns non-zero if timer is reset
	oexBOOL IsReset()
    {   return ( !m_llStart && !m_llStop ); }

	//==============================================================
	// IsRunning()
	//==============================================================
	/// Returns non-zero if timer is running
	oexBOOL IsRunning()
    {   return ( m_llStart && !m_llStop ); }

	//==============================================================
	// IsStopped()
	//==============================================================
	/// Returns non-zero if timer is stopped
	oexBOOL IsStopped()
    {   return ( m_llStart && m_llStop ); }

	//==============================================================
	// Reset()
	//==============================================================
	/// Resets internal values
	void Reset()
    {   m_llStart = m_llStop = 0; }

	//==============================================================
	// Start()
	//==============================================================
	/// Saves the current time as start time
	oexINT64 Start()
    {   return m_llStart = GetTimer(); }

	//==============================================================
	// Start()
	//==============================================================
	/// Saves the specified time as start time
	/**
		\param [in] x_ll	-	Start time
	*/
	oexINT64 Start( oexINT64 x_ll )
    {   return m_llStart = x_ll; }

	//==============================================================
	// Stop()
	//==============================================================
	/// Saves the current time as stop time
	oexINT64 Stop()
    {   return m_llStop = GetTimer(); }

	//==============================================================
	// Stop()
	//==============================================================
	/// Saves the specified time as stop time
	/**
		\param [in] ll	-	Stop time
	*/
	oexINT64 Stop( oexINT64 x_ll )
    {   return m_llStop = x_ll; }

	//==============================================================
	// Ellapsed()
	//==============================================================
	/// Returns amount of time ellapsed between two values
    static oexINT64 Elapsed( oexINT64 llStart, oexINT64 llStop );

	//==============================================================
	// Ellapsed()
	//==============================================================
	/// Returns amount of time captured or ellapsed since
	oexINT64 Elapsed()
    {   if ( m_llStop ) return Elapsed( m_llStart, m_llStop );
        return Elapsed( m_llStart, GetTimer() );
    }

	//==============================================================
	// EllapsedSeconds()
	//==============================================================
	/// Returns ellapsed time in seconds
	oexDOUBLE ElapsedSeconds()
    {   return ( (oexDOUBLE)Elapsed() / (oexDOUBLE)GetTimerFrequency() ); }

	//==============================================================
	// GetBootCount()
	//==============================================================
	/// Returns number of milliseconds since the computer booted
    static oexUINT GetBootCount();


private:

	/// Clock frequency
	static oexINT64			m_llFreq;

	/// Start time marker
	oexINT64				m_llStart;

	/// Stop time marker
	oexINT64				m_llStop;

};

//==================================================================
// CTimeout
//
/// Tracks timeout
/**

*/
//==================================================================
class CTimeout
{
public:

	/// Default constructor
	CTimeout()
	{	m_timeout = 0; }

	/// Sets the timeout value in seconds
	CTimeout( double x_dTimeout )
	{	Set( x_dTimeout ); }

	/// Destructor
	~CTimeout() { }

	/// Clears the timer value
	void Clear()
	{	m_timeout = 0; }

	/// Sets the timeout value in seconds
	void Set( double x_dTimeout )
	{	m_timeout = CHqTimer::GetTimerSeconds() + x_dTimeout; }

	/// Sets the timeout value in milli-seconds
	void SetMs( oexUINT x_uTimeout )
	{	Set( (double)x_uTimeout / (double)1000 ); }

	/// Retuns non-zero if timer set and not expired
	oexBOOL IsValid()
	{	return ( 1 < m_timeout && CHqTimer::GetTimerSeconds() < m_timeout ); }

	/// Returns non-zero if the timer has been set
	oexBOOL IsSet()
	{	return 1 < m_timeout ; }

	/// Returns non-zero if the timer is set and has expired
	oexBOOL IsExpired()
	{	return ( 1 < m_timeout && CHqTimer::GetTimerSeconds() > m_timeout ); }

	/// Returns the time in seconds remaining
	oexDOUBLE Remaining()
	{	double t = CHqTimer::GetTimerSeconds();
		if ( 1 < m_timeout && t < m_timeout )
			return m_timeout - t;
		return 0;
	}

private:

	/// Timeout value
	double				m_timeout;
};

