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

#include "../../../oexlib.h"
#include "std_os.h"

OEX_USING_NAMESPACE
using namespace OEX_NAMESPACE::os;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

oexINT64 CHqTimer::m_llFreq = CHqTimer::osGetTimerFreq();

oexINT64 CHqTimer::osGetTimerFreq()
{
	oexINT64 llFreq = 0;

	if ( !QueryPerformanceFrequency( (LARGE_INTEGER*)&llFreq ) )
		llFreq = 1000;

	return llFreq;
}

oexINT64 CHqTimer::osGetTimerValue()
{
	if ( m_llFreq != 1000 )
	{
		oexINT64 ll;

		QueryPerformanceCounter( (LARGE_INTEGER*)&ll );	 

		return ll;

	} // end if

	return (oexINT64)GetTickCount();
}

oexINT64 CHqTimer::Elapsed( oexINT64 llStart, oexINT64 llStop )
{
	if ( m_llFreq != 1000 )
        return llStop - llStart;

    // Check for 32 bit roll over in GetTickCount() 
    if ( llStart > llStop ) llStart += 0xffffffff;
    return llStop - llStart;
}

oexUINT CHqTimer::GetBootCount()
{   return ::GetTickCount();
}

