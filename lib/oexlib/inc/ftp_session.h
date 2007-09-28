/*------------------------------------------------------------------
// ftp_session.h
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


//==================================================================
// CFtpDataConnection
//
/// FTP data connection
/**
*/
//==================================================================
class CFtpDataConnection : 
    public CProtocol,
    public TBufferedPort< CAutoSocket >
{
public:

	//==============================================================
	// OnRead()
	//==============================================================
	/// Called when new data arrives
	/**
		\param [in] x_nErr	-	Error code
		
		\return Non-zero if success
	
		\see 
	*/
    virtual oexBOOL OnRead( oexINT x_nErr )
    {
	    // Process the incomming data
        if ( !TBufferedPort::OnRead( x_nErr ) )
		    return oexFALSE;

        // Write out the data
        if ( m_fData.IsOpen() )
            m_fData.Write( Rx().Read() );

	    return oexTRUE;
    }

	//==============================================================
	// OnClose()
	//==============================================================
	/// Called when socket connection has been closed or aborted.
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnClose( oexINT x_nErr )
    {   CloseSession();
        return oexTRUE;
    }

private:

    /// Disk file
    CFile           m_fData;
};


//==================================================================
// CFtpSession
//
/// Encapsulates basic FTP server functionality
/**

*/
//==================================================================
class CFtpSession : 
    public CProtocol,
    public TBufferedPort< CAutoSocket >
{
public:

    /// FTP data connection
    typedef TNetServer< oex::CAutoSocket, CFtpDataConnection > t_FtpDataConnection;

public:
	
    /// Constructor
    CFtpSession();

    /// Destructor
    virtual ~CFtpSession() {}

	//==============================================================
	// OnRead()
	//==============================================================
	/// Called when new data arrives
	/**
		\param [in] x_nErr	-	Error code
		
		\return Non-zero if success
	
		\see 
	*/
    virtual oexBOOL OnRead( oexINT x_nErr );

	//==============================================================
	// OnConnect()
	//==============================================================
	/// Called when a connection is established
	/**
		\param [in] x_nErr	-	Error code
		
		\return Non-zero if success
	
		\see 
	*/
    virtual oexBOOL OnConnect( oexINT x_nErr );

	//==============================================================
	// OnClose()
	//==============================================================
	/// Called when socket connection has been closed or aborted.
	/**
		\param [in] nErr	-	Zero if no error, otherwise socket error value.

		\return Return non-zero if handled
	*/
	virtual oexBOOL OnClose( oexINT x_nErr )
    {   CloseSession();
        return oexTRUE;
    }

    /// Returns a reference to the users list
    CStrList& GetUserList() { return m_lstUsers; }

    /// Creates a passive connection
    t_FtpDataConnection::t_Session GetPassiveConnection();

    /// Sends file list
    oexBOOL CmdList();

    /// Passive mode
    oexBOOL CmdPasv();

    /// Return file
    oexBOOL CmdRetr( oexCSTR8 x_pFile );

    /// Saves the specified file
    oexBOOL CmdStor( oexCSTR8 x_pFile );

    /// Returns the current path
    CStr8 CurrentPath() 
    {   return CStr8( m_sRoot ).BuildPath( m_sCurrent ); }

    /// Sets the root folder
    CStr8& SetRoot( oexCSTR8 x_pRoot )
    {   return m_sRoot = x_pRoot; }

    /// Returns the root folder
    CStr8& GetRoot( oexCSTR8 x_pRoot )
    {   return m_sRoot = x_pRoot; }


private:

    /// Root folder
    CStr8                               m_sRoot;

    /// Current folder
    CStr8                               m_sCurrent;

    /// User list
    CStrList                            m_lstUsers;

    /// Currently logged in user
    CStr8                               m_sUser;

    /// Current users password string
    CStr8                               m_sPassword;

    /// Transfer type
    CStr8                               m_sType;

    /// Passive FTP port
    oexUINT                             m_uPasvPort;
    
    /// Server for passive mode
    t_FtpDataConnection                 m_nsData;
};

