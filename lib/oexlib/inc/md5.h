/*------------------------------------------------------------------
// md5.h
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


namespace oss
{

class CMd5
{
public:

    /// Context for CMd5Rsa class
    struct SContext
    {   oexUINT32       uState[ 4 ];
        oexUINT32       uCount[ 2 ];
        oexUCHAR        ucBuf[ 64 ];
    };

    struct SDigest
    {
        union
        {   oexGUID     guid;
            oexUCHAR    ucBuf[ 16 ];
        };
    };

public:

    /// Default constructor
    CMd5() { Init(); }

    /// Default destructor
    virtual ~CMd5() {}

    /// Initialize the md5
    void Init();

    /// Update the md5
    void Update( oexCPVOID x_pBuf, oexUINT x_uSize );

    /// Finalize the md5
    void Final( oexGUID *x_pGuid  = oexNULL );

    /// Calculates an md5 in one step
    static oexGUID* Transform( oexGUID *x_pGuid, oexCPVOID x_pBuf, oexUINT x_uSize );

    /// Returns hash as a guid
    oexGUID& GetGuid() { return m_md5.guid; }

    /// Returns hash as a guid
    operator oexGUID&() { return m_md5.guid; }

    /// Returns hash as a guid
    operator oexGUID*() { return &m_md5.guid; }

    /// Returns the md5 hash buffer
    oexCPVOID GetBuffer() { return m_md5.ucBuf; }

private:

    /// MD5 context
    SContext            m_md5Context;

    /// The actual md5 hash
    SDigest             m_md5;
};

};
