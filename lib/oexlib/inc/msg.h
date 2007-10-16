/*------------------------------------------------------------------
// msg.h
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

/// Message container
class CMsg
{
public:

    /// Default constructor
    CMsg()
    {
        m_cMi.OexConstruct();
    }

    /// Copy constructor
    CMsg( oexCONST CMsg &x_rMsg )
    {
        m_cMi.Share( x_rMsg.m_cMi );
    }

    CMsg( oexSTR8 x_pBin, oexUINT x_uSize )
    {
        m_cMi.OexConstruct();
        Mi().Deserialize( x_pBin, x_uSize );
    }

    CMsg( CStr8 &x_sStr )
    {
        m_cMi.OexConstruct();
        Mi().Deserialize( x_sStr );
    }

    /// Destructor
    virtual ~CMsg()
    {}

    /// Copy operator
    CMsg& operator = ( oexCONST CMsg &x_rMsg )
    {   m_cMi.Share( x_rMsg.m_cMi );
        return *this;
    }

    /// Returns non-zero if there is valid memory
    oexBOOL IsValid()
    {   return m_cMi.Ptr() ? oexTRUE : oexFALSE; }

    /// Returns a serialized version of the data
    CStr8& Serialize()
    {
        // Return the serialized packet
        return m_cMi.Ptr()->Serialize();
    }

    /// Returns a reference to the message info structure
    CMsgInfo& Mi()
    {   return *m_cMi.Ptr(); }

public:

    /// Creates a message object
    static CMsg Msg( oexUINT x_uFlags, CMsgAddress *x_pDst )
    {
        CMsg msg;

        // Start packet
        if ( !msg.Mi().Open( x_uFlags, 0, 0 ) )
            return msg;

        // Set destination address
        if ( x_pDst )
            *msg.Mi().Dst() = *x_pDst;

        msg.Mi().End();

        // Return the message object
        return msg;
    }

    /// Creates a message object
    template< typename T_P1 >
        static CMsg Msg( oexUINT x_uFlags, CMsgAddress *x_pDst, T_P1 p1 )
    {
        CMsg msg;

        // Start packet
        if ( !msg.Mi().Open( x_uFlags, 1, obj::Size( &p1 ) ) )
            return msg;

        // Set destination address
        if ( x_pDst )
            *msg.Mi().Dst() = *x_pDst;

        // Add params
        if ( !oexCHECK( msg.Mi().AddParam( CMsgTypeAdapter::GetType< T_P1 >(), obj::Ptr( &p1 ), obj::Size( &p1 ) ) ) )
            return CMsg();

        msg.Mi().End();

        // Return the message object
        return msg;
    }

    /// Creates a message object
    template< typename T_P1, typename T_P2 >
        static CMsg Msg( oexUINT x_uFlags, CMsgAddress *x_pDst, T_P1 p1, T_P2 p2 )
    {
        CMsg msg;

        // Start packet
        if ( !msg.Mi().Open( x_uFlags, 2,   obj::Size( &p1 )
                                             + obj::Size( &p2 ) ) )
            return msg;

        // Set destination address
        if ( x_pDst )
            *msg.Mi().Dst() = *x_pDst;

        // Add params
        if ( !oexCHECK( msg.Mi().AddParam( CMsgTypeAdapter::GetType< T_P1 >(), obj::Ptr( &p1 ), obj::Size( &p1 ) ) ) )
            return CMsg();

        if ( !oexCHECK( msg.Mi().AddParam( CMsgTypeAdapter::GetType< T_P2 >(), obj::Ptr( &p2 ), obj::Size( &p2 ) ) ) )
            return CMsg();

        msg.Mi().End();

        // Return the message object
        return msg;
    }

public:

    template< typename T >
        oexBOOL VerifyParam( oexUINT x_uIndex )
    {
        if ( !IsValid() )
            return oexFALSE;

        oexUINT uData = 0;
        oexUINT uType = 0;
        if ( !Mi().GetParamInfo( x_uIndex, oexNULL, &uData, &uType ) )
            return oexFALSE;

        // Verify the param type  iii commented out because we're converting on the fly
//        oexASSERT( GetType< T >() == uType && obj::Size( (T*)0 ) == uData );

        return oexTRUE;
    }

public:

    /// Parameter selector
    CMsgTypeAdapter operator [] ( oexUINT x_uIndex )
    {
        if ( !IsValid() )
            return CMsgTypeAdapter();

        oexUINT uData = 0;
        oexUINT uType = 0;
        oexSTR8 pData = oexNULL;
        if ( !Mi().GetParamInfo( x_uIndex, &pData, &uData, &uType ) )
            return CMsgTypeAdapter();

        // Return data wrapper
        return CMsgTypeAdapter( this, pData, uData, uType );
    }

    /// Return first parameter, this will be the reply in a reply packet
    CMsgTypeAdapter GetReply()
    {   return ( *this )[ 0 ]; }

    /// Returns the message priority level
    oexUINT GetPriority()
    {
        if ( !IsValid() )
            return 0;

        return m_cMi.Ptr()->GetPriority();
    }

    /// Returns the message execution time
    oexUINT GetTime()
    {
        if ( !IsValid() )
            return 0;

        return m_cMi.Ptr()->GetTime();
    }

    /// Enable reply event
    oexBOOL EnableReplyEvent( oexBOOL x_bEnable )
    {
        if ( !IsValid() )
            return oexFALSE;

        return m_cMi.Ptr()->EnableReplyEvent( x_bEnable );
    }

    /// Returns non-zero if this message requires a reply
    oexBOOL WantReply()
    {
        if ( !IsValid() )
            return oexFALSE;

        return m_cMi.Ptr()->WantReply();
    }

    /// Sets the ready event state
    oexBOOL SetReadyEvent( oexBOOL x_bSet )
    {
        if ( !IsValid() )
            return oexFALSE;

        return m_cMi.Ptr()->SetReadyEvent( x_bSet );
    }

    /// Waits for ready event to enter the signaled state
    CMsg& Wait( oexUINT x_uTimeout )
    {
        if ( IsValid() )           
            m_cMi.Ptr()->WaitReady( x_uTimeout );

        return *this;
    }

    /// Returns non-zero if the reply is ready
    oexBOOL IsReplyReady()
    {   return m_cMi.Ptr()->WaitReady( 0 ); }

    /// Returns non-zero if the reply buffer should be skipped.
    /// This occurs when a class sends a message to itself.
    oexBOOL IsDirectReply()
    {
        if ( !IsValid() )
            return oexFALSE;

        return 0 != ( m_cMi.Ptr()->GetFlags() & oexMsgDirectReply );
    }

    /// Sets the direct reply flag.  This flag signals that the 
    /// reply buffer should be skipped.
    oexBOOL SetDirectReply( oexBOOL x_bSet )
    {
        if ( !IsValid() )
            return oexFALSE;

        if ( x_bSet )
            m_cMi.Ptr()->SetFlags( m_cMi.Ptr()->GetFlags() | oexMsgDirectReply );
        else
            m_cMi.Ptr()->SetFlags( m_cMi.Ptr()->GetFlags() & ~oexMsgDirectReply );

        return oexTRUE;
    }

    /// Returns non-zero if at least one other reference on the memory
    oexBOOL IsConnected()
    {   return 1 < m_cMi.GetRefCount(); }


private:

    /// Message payload
    TMem< CMsgInfo >        m_cMi;
};

