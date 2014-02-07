// ff_audio_resample.cpp

#include "stdafx.h"

SQBIND_REGISTER_CLASS_BEGIN( CFfAudioResample, CFfAudioResample )

//	SQBIND_STATIC_FUNCTION( CFfAudioResample,  )
//	SQBIND_STATIC_FUNCTION( CFfAudioResample,  )
//	SQBIND_STATIC_FUNCTION( CFfAudioResample,  )
//	SQBIND_STATIC_FUNCTION( CFfAudioResample,  )
//	SQBIND_STATIC_FUNCTION( CFfAudioResample,  )

//	SQBIND_GLOBALCONST(  )

SQBIND_REGISTER_CLASS_END()

void CFfAudioResample::Register( sqbind::VM vm )
{_STT();
	SQBIND_EXPORT( vm, CFfAudioResample );
}

CFfAudioResample::CFfAudioResample()
{_STT();

	m_ctx = 0;
	m_nInRate = 0;
	m_nOutRate = 0;
}

void CFfAudioResample::Destroy()
{_STT();

	if ( m_ctx )
		av_resample_close( m_ctx ), m_ctx = 0;

	m_nInRate = 0;
	m_nOutRate = 0;
}

int CFfAudioResample::Init( int nInRate, int nOutRate, int nFilterLength, int nLog2PhaseCount, int nLinear, double dCutoff )
{_STT();

	// Out with the old
	Destroy();

	// Validate sample rates
	if ( 0 >= nInRate || 0 >= nOutRate )
		return 0;

	// Build resampling context
	m_ctx = av_resample_init( nOutRate, nInRate, nFilterLength, nLog2PhaseCount, nLinear, dCutoff );
	if ( !m_ctx )
		return 0;

	// Save rates
	m_nInRate = nInRate;
	m_nOutRate = nOutRate;

	return 1;
}

int CFfAudioResample::Resample( sqbind::CSqBinary *pSrc, sqbind::CSqBinary *pDst )
{_STT();

	// Validate resampler
	if ( !m_ctx || 0 >= m_nInRate || 0 >= m_nOutRate )
		return 0;

	// Validate params
	if ( !pDst || !pSrc )
		return 0;

	// Do we have data?
	if ( 0 >= pSrc->getUsed() )
		return 0;

	// Calculate required output buffer size
	oex::oexINT64 nOutSize = pSrc->getUsed() * m_nOutRate / m_nInRate + FF_INPUT_BUFFER_PADDING_SIZE + 1024;

	// Allocate space for output data if needed
	if ( ( pDst->Size() - pDst->getUsed() ) < nOutSize )
		if ( !pDst->Resize( pDst->getUsed() + nOutSize ) )
			return 0;
	
	// Build resampling context
	int consumed = 0;
	int out = av_resample( m_ctx, (short*)pDst->Ptr( pDst->getUsed() ), (short*)pSrc->Ptr(), 
						   &consumed, pSrc->getUsed() / 2, pDst->Size() / 2, 0 );
	if ( 0 > out )
	{	pDst->setUsed( 0 );
		return 0;
	} // end if

	// Number of bytes used
	pDst->setUsed( out );

	return out;
}


