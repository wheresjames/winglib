/*------------------------------------------------------------------
// hq_timer.h
// Copyright (c) 2006 
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
*/
//==================================================================
class CHqTimer  
{
public:
	
	/// Default Constructor
	CHqTimer()
	{
		// Reset timer values
		m_llStart = m_llStop = 0;
	}
	
	/// Returns the timer frequency
	static stINT64 osGetTimerFreq();

	/// Returns the timer value
	static stINT64 osGetTimerValue();

	/// Destructor
	virtual ~CHqTimer() {}

	//==============================================================
	// GetTimerFrequency()
	//==============================================================
	/// Returns the frequency of the active timer
	static stINT64 GetTimerFrequency() 
	{	return m_llFreq; }

	//==============================================================
	// GetTimer()
	//==============================================================
	/// Return the current timer value
	static stINT64 GetTimer() 
	{	return osGetTimerValue(); }

	//==============================================================
	// IsReset()
	//==============================================================
	/// Returns non-zero if timer is reset
	stBOOL IsReset() { return ( !m_llStart && !m_llStop ); }

	//==============================================================
	// IsRunning()
	//==============================================================
	/// Returns non-zero if timer is running
	stBOOL IsRunning() { return ( m_llStart && !m_llStop ); }

	//==============================================================
	// IsStopped()
	//==============================================================
	/// Returns non-zero if timer is stopped
	stBOOL IsStopped() { return ( m_llStart && m_llStop ); }

	//==============================================================
	// Reset()
	//==============================================================
	/// Resets internal values
	void Reset() { m_llStart = m_llStop = 0; }

	//==============================================================
	// Start()
	//==============================================================
	/// Saves the current time as start time
	stINT64 Start() { return m_llStart = GetTimer(); }

	//==============================================================
	// Start()
	//==============================================================
	/// Saves the specified time as start time
	/**
		\param [in] ll	-	Start time
	*/
	stINT64 Start( stINT64 ll ) { return m_llStart = ll; }

	//==============================================================
	// Stop()
	//==============================================================
	/// Saves the current time as stop time
	stINT64 Stop() { return m_llStop = GetTimer(); }

	//==============================================================
	// Stop()
	//==============================================================
	/// Saves the specified time as stop time
	/**
		\param [in] ll	-	Stop time
	*/
	stINT64 Stop( stINT64 ll ) { return m_llStop = ll; }
	
	//==============================================================
	// Ellapsed()
	//==============================================================
	/// Returns amount of time captured or ellapsed since
	stINT64 Ellapsed()	{ if ( m_llStop ) return m_llStop - m_llStart; return GetTimer() - m_llStart; }

	//==============================================================
	// EllapsedSeconds()
	//==============================================================
	/// Returns ellapsed time in seconds
	stDOUBLE EllapsedSeconds() { return ( (stDOUBLE)Ellapsed() / (stDOUBLE)GetTimerFrequency() ); }

private:

	/// Clock frequency
	static stINT64		m_llFreq;

	/// Start time marker
	stINT64				m_llStart;

	/// Stop time marker
	stINT64				m_llStop;

};

