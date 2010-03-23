/*------------------------------------------------------------------
// vfs_ftp_session.h
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
// CVfsFtpDataConnection
//
/// VFS FTP data connection
/**
*/
//==================================================================
class CVfsFtpDataConnection : 
    public CProtocol,
    public TBufferedPort< CAutoSocket >
{
public:

    ~CVfsFtpDataConnection()
    {
        // Save any data that needs to go to the vfs
        if ( m_vfs.IsOpen() && m_sFile.Length() && Rx().GetMaxRead() )
            m_vfs.SaveFile( m_sFile.Ptr(), Rx().Read() );
    }

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
        if ( !TBufferedPort< CAutoSocket >::OnRead( x_nErr ) )
		    return oexFALSE;

        // Grab the data from the buffer
//        if ( m_vfs.IsOpen() )
//            m_buf.Write( Rx().Read() );

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

    virtual void OnRegisterFunctions( CMsgCom *x_pMc )
    {
        if ( !x_pMc )
            return;

        // Call the base class
        CProtocol::OnRegisterFunctions( x_pMc );

        // Export functions
        x_pMc->oexMsgRegisterThisFunction( CVfsFtpDataConnection, WriteStr );
        x_pMc->oexMsgRegisterThisFunction( CVfsFtpDataConnection, WaitTxEmpty );
        x_pMc->oexMsgRegisterThisFunction( CVfsFtpDataConnection, SetFile );
        x_pMc->oexMsgRegisterThisFunction( CVfsFtpDataConnection, OpenVfs );
    }

    /// Exported Function
	oexBOOL WriteStr( CStr8 x_sStr )
    {	return Write( (oexPVOID)x_sStr.Ptr(), x_sStr.Length() ); }

    /// Returns non-zero if the tx'er is empty
    oexBOOL WaitTxEmpty( oexUINT x_uTimeout )
    {   return Tx().WaitEmpty( x_uTimeout ); }

    /// Opens a new file
    oexBOOL SetFile( CStr8 x_sFile )
    {   m_sFile = x_sFile; return oexTRUE; }

    /// Opens the VFS
    oexBOOL OpenVfs( CStr8 x_sRoot )
    {   return m_vfs.Open( x_sRoot.Ptr() );
    }


private:

    /// File name
    CStr8               m_sFile;

    /// VFS object
    CVirtualFs          m_vfs;
};


//==================================================================
// CVfsFtpSession
//
/// Encapsulates basic FTP server functionality for VFS
/**

*/
//==================================================================
class CVfsFtpSession : 
    public CProtocol,
    public TBufferedPort< CAutoSocket >
{
public:

    /// FTP data connection
    typedef TNetServer< OEX_NAMESPACE::CAutoSocket, CVfsFtpDataConnection > t_FtpDataConnection;

public:
	
    /// Constructor
    CVfsFtpSession();

    /// Destructor
    virtual ~CVfsFtpSession() 
    {
    }

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
    CStrList& GetUserList() 
    {   return m_lstUsers; }

    /// Creates a passive connection
    CDispatch GetPassiveConnection();

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

    /// Server for passive mode
    t_FtpDataConnection                 m_nsData;

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
   
    /// Virtual file system
    CVirtualFs                          m_vfs;
};

