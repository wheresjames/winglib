#ifndef _CPUUSAGE_H
#define _CPUUSAGE_H

//==================================================================
// CCpuUsage
//
/// Monitors the CPU usage
/**
	
*/
//==================================================================
class CCpuUsage
{
public:

	/// Default constructor
	CCpuUsage();

	/// Destructor
	virtual ~CCpuUsage();

	//==============================================================
	// GetCpuUsage()
	//==============================================================
	/// Returns the current CPU usage
	int GetCpuUsage();

	//==============================================================
	// GetCpuUsage()
	//==============================================================
	/// Returns the current CPU usage by the named process
	/**
		\param [in] pProcessName	-	Process name
		
		\return CPU usage
	
		\see 
	*/
	int GetCpuUsage(LPCTSTR pProcessName);

	//==============================================================
	// EnablePerformaceCounters()
	//==============================================================
	/// Enable performance counters
	/**
		\param [in] bEnable		-	Non-zero to enable
		
		\return Returns the value in bEnable.
	
		\see 
	*/
	BOOL EnablePerformaceCounters(BOOL bEnable = TRUE);

// Attributes
private:

	/// Flag used to initiailze cpu monitor
	bool			m_bFirstTime;

	/// Old CPU value
	LONGLONG		m_lnOldValue ;

	/// Timer value
	LARGE_INTEGER	m_OldPerfTime100nSec;

	/// Average cpu time
	LONG			m_lAvg;
};


#endif