/*------------------------------------------------------------------
// msg_info.h
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

/// Message data
class CMsgInfo
{
public:

#pragma pack( push, 1 )

    /// Memory block information
    struct SBlockHeader
    {
        /// CRC of the data block
        oexUCHAR        ucCrc[ CCrcHash::eHashSize ];

        /// Number of parameters
        oexUINT         uParams;

        /// Priority
        oexUINT         uFlags;

        /// Desired execute time, GMT Unix timestamp, 0 for immediate
        oexUINT         uTime;
    };  

    /// Param descriptor
    struct SParamHeader
    {
        /// Param size
        oexUINT         uSize;

        /// Param type
        oexUINT         uType;
    };  

#pragma pack( pop )

public:

    /// Default constructor
    CMsgInfo() : m_evReady( oexTRUE )
    {
        m_uUsed = 0;
        m_pDst = oexNULL;
        m_pSrc = oexNULL;
        m_pBh = oexNULL;
        m_pPh = oexNULL;
        m_pData = oexNULL;
    }

    /// Destructor
    virtual ~CMsgInfo()
    {}

    /// Destroys the contained message
    void Destroy();

    /// Returns message overhead
    oexUINT OverheadSize( oexUINT x_uParams )
    {
        return   
                 sizeof( CMsgAddress )
               + sizeof( CMsgAddress )
               + sizeof( SBlockHeader )
               + sizeof( SParamHeader ) * x_uParams;
    }

    oexUINT OverheadSize()
    {
        if ( !m_pBh )
            return 0;
        return OverheadSize( m_pBh->uParams );
    }

    /// Starts the data packet
    oexBOOL Open( oexUINT x_uFlags, oexUINT x_uParams, oexUINT x_uTotalSize );

    /// Add a parameter to the call
    oexBOOL AddParam( oexUINT x_uType, oexCPVOID x_pData, oexUINT x_uSize );

    /// Closes the data packet
    oexBOOL End();

    /// Returns parameter information
    oexBOOL GetParamInfo( oexUINT x_uIndex, oexSTR8 *x_pData, oexUINT *x_puSize, oexUINT *x_puType );

    /// Sets the pointers
    oexBOOL SetPointers( oexINT x_nParams );

    oexBOOL VerifyPacket();

    /// Deserializes buffer
    /**
        Whole separate version because this one shares the memory
    */
    oexBOOL Deserialize( CStr8 &x_sBin );

    /// Deserialize
    oexBOOL Deserialize( oexCSTR8 x_pBin, oexUINT x_uSize );

    /// Returns a reference to the buffer
    CStr8& Serialize()
    {   return m_sBin; }

    /// Return destination address structure
    CMsgAddress* Dst()
    {   return m_pDst; }

    /// Return source address structure
    CMsgAddress* Src()
    {   return m_pSrc; }

    /// Returns the message priority level
    oexUINT GetPriority()
    {   if ( !m_pBh )
            return 0;
        return m_pBh->uFlags & oexMsgPriorityMask;
    }

    /// Returns the message priority level
    oexUINT GetTime()
    {   if ( !m_pBh )
            return 0;
        return m_pBh->uTime;
    }

    /// Returns the number of parameters
    oexUINT GetNumParams()
    {   if ( !m_pBh )
            return 0;
        return m_pBh->uParams;
    }

    /// Set to enable reply
    oexBOOL EnableReplyEvent( oexBOOL x_bEnable )
    {
        if ( x_bEnable )
            m_evReady.Create();
        else
            m_evReady.Destroy();

        return oexTRUE;
    }

    /// Signal reply is ready
    oexBOOL WantReply()
    {
        if ( !m_pBh )
            return oexFALSE;

        return !( m_pBh->uFlags & ( oexMsgReply | oexMsgNoReply ) );
    }

    oexBOOL SetReadyEvent( oexBOOL x_bSet )
    {
        if ( x_bSet )
            m_evReady.Set();
        else
            m_evReady.Reset();

        return oexTRUE;
    }

    oexBOOL WaitReady( oexUINT x_uTimeout )
    {
        return m_evReady.Wait( x_uTimeout );
    }

    /// Returns flags
    oexUINT GetFlags()
    {
        if ( !m_pBh )
            return 0;

        return m_pBh->uFlags;
    }

    /// Sets the flags
    void SetFlags( oexUINT uFlags )
    {
        if ( m_pBh )
            m_pBh->uFlags = uFlags;
    }


private:

    /// Holds *binary* message data
    CStr8           m_sBin;

    /// Destination address
    CMsgAddress     *m_pDst;

    /// Source address
    CMsgAddress     *m_pSrc;

    /// Block header info
    SBlockHeader    *m_pBh;

    /// Param header info
    SParamHeader    *m_pPh;

    /// Param header info
    oexSTR8         m_pData;

    /// Bytes used
    oexUINT         m_uUsed;

    /// Event to be signaled when the message is ready
    CTlEvent        m_evReady;
};

