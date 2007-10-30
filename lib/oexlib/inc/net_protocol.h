/*------------------------------------------------------------------
// net_protocol.h
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


/// Inherit from this class to implement a network protocol
class CProtocol
{
public:

    /// Default constructor
    CProtocol()
    {   m_pMc = oexNULL; 
        guid::ZeroGuid( &m_guidSessionId );
    }

    /// Returns a pointer to the server dispatch object
    CMsgCom* Server()
    {   return m_pMc; }

    /// Set dispatch object
    void SetServerMsgCom( CMsgCom *x_pMc )
    {   m_pMc = x_pMc; }

    /// Sets the session id
    void SetSessionId( oexGUID &x_guid )
    {   guid::CopyGuid( &m_guidSessionId, &x_guid ); }

    /// Returns the session id
    oexGUID& GetSessionId()
    {   return m_guidSessionId; }

    /// Closes the session
    void CloseSession()
    {
//        if ( m_pMc )
//            m_pMc->msgSend( oexMsg( 0, oexTo( oexT( "CloseSession" ) ), m_guidSessionId ) ); //->Queue( 0, oexCall( oexT( "CloseSession" ), m_guidSessionId ) );
    }

    /// Over-ride to register interface functions
    virtual void OnRegisterFunctions( CMsgCom *x_pMc )
    {
        if ( !x_pMc )
            return;

        x_pMc->oexMsgRegisterThisFunction( CProtocol, CloseSession );
    }

private:

    /// Session id
    oexGUID             m_guidSessionId;

    /// Pointer to server dispatch object
    CMsgCom             *m_pMc;

};


/// Generic ip port
template < typename T_PORT >
    class TEchoProtocol : 
        public CProtocol,
        public T_PORT
{
public:

    virtual void OnRegisterFunctions( CMsgCom *x_pMc )
    {
        // Call the base class
        CProtocol::OnRegisterFunctions( x_pMc );

        // Register interface functions
        T_PORT::RegisterFunctions( x_pMc );
    }

	virtual oexBOOL OnRead( oexINT x_nErr ) 
    {
        // Process the incomming data
        if ( !T_PORT::OnRead( x_nErr ) )
	        return oexFALSE;

        // Just echo the data
        Send( Recv() );
        
        return oexTRUE; 
    }

    /// Closes the session when the socket closes
	virtual oexBOOL OnClose( oexINT x_nErr )
    {
        CloseSession();

        return oexTRUE;
    }
};

