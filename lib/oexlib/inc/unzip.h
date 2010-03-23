/*------------------------------------------------------------------
// unzip.h
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

#ifdef OEX_ENABLE_ZIP

namespace zip
{

class CUnzip
{
public:
    CUnzip();
    ~CUnzip();

private:

};

class CUncompress
{
public:

    CUncompress();
    ~CUncompress();

    void Destroy()
    {   m_fInput.Destroy();
        m_sInput.Destroy();
        m_fOutput.Destroy();
        m_sOutput.Destroy();
    }

    oexINT64 OnRead( oexSTR8 buf, unsigned size );
    oexINT64 OnWrite( oexCSTR8 buf, unsigned *size );

    CStr8 Uncompress();

    // Set input method
    oexBOOL SetPtrInput( const oexCHAR8 *p, oexLONG l ) { m_pInput = p; m_lInput = l, m_lInputPos = 0; return 0 < m_sInput.Length(); }
    oexBOOL SetStrInput( const CStr8 &x_sStr ) { m_sInput = x_sStr; return 0 < m_sInput.Length(); }
    oexBOOL SetFileInput( oexCSTR x_pFile ) { return m_fInput.OpenExisting( x_pFile ).IsOpen(); }

    // Set output method
    oexBOOL SetOutputFile( oexCSTR x_pFile ) { return m_fOutput.OpenExisting( x_pFile ).IsOpen(); }

    /// Returns a reference to the output string object
    CStr8& GetOutputStr() { return m_sOutput; }

    /// Returns a reference to the output file object
    CFile& GetOutputFile() { return m_fOutput; }

    /// One step string compress
    static CStr8 Uncompress( const CStr8 &x_sStr )
    {   CUncompress cmp; cmp.SetStrInput( x_sStr ); cmp.Uncompress(); return cmp.m_sOutput; }

    static CStr8 Uncompress( const oexCHAR8 *p, oexLONG l )
    {   CUncompress cmp; cmp.SetPtrInput( p, l ); cmp.Uncompress(); return cmp.m_sOutput; }

private:

    oexPVOID    		m_pCompress;

    CStr8        		m_sInput;
    CFile       		m_fInput;
	const oexCHAR8		*m_pInput;
	oexLONG				m_lInput;
	oexLONG				m_lInputPos;

    CStr8        		m_sOutput;
    CFile       		m_fOutput;
};

};

#endif // OEX_ENABLE_ZIP
