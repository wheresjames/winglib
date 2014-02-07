// ff_audio_resample.h

class CFfAudioResample
{
public:

	// Declare constructors
	_SQBIND_CLASS_CTOR_BEGIN( CFfAudioResample )
	_SQBIND_CLASS_CTOR_END( CFfAudioResample )

	/// Default constructor
	CFfAudioResample();

	/// Destructor
	~CFfAudioResample() { Destroy(); }

	/// Registers the class
	static void Register( sqbind::VM vm );

	/** \addtogroup CFfAudioResample
		@{
	*/
	
	/// Release resources
	void Destroy();

	/// Initialize resample structure
	/**
		@param [in] nOutRate		Input rate in samples per second
		@param [in] nInRate			Input rate in samples per second
		@param [in] nFilterLength 	Length of each FIR filter in the filterbank 
									relative to the cutoff freq
		@param [in] nLog2PhaseCount	log2 of the number of entries in the 
									polyphase filterbank
		@param [in] nLinear 		If 1 then the used FIR filter will be 
									linearly interpolated between the 2 closest, 
									if 0 the closest will be used
		@param [in] dCutoff 		cutoff frequency, 1.0 corresponds to half 
									the output sampling rate 
	*/
	int Init( int nOutRate, int nInRate, int nFilterLength, int nLog2PhaseCount, int nLinear, double dCutoff );

	/// Returns the input sample rate
	int getInRate() { return m_nInRate; }
	
	/// Returns the output sample rate
	int getOutRate() { return m_nOutRate; }

	/// Resamples the specified input buffer
	/**
		@param [in] pSrc	Input buffer
		@param [out] pDst	Output buffer
		
		@return Returns number of bytes resampled or zero if error.
	*/
	int Resample( sqbind::CSqBinary *pSrc, sqbind::CSqBinary *pDst );

	/** @} */

public:


private:

	/// Input sample rate
	int						m_nInRate;

	/// Input sample rate
	int						m_nOutRate;

	/// Resample context
	AVResampleContext		*m_ctx;

};

DECLARE_INSTANCE_TYPE( CFfAudioResample );

