/*------------------------------------------------------------------
// vfs_ftp_session.cpp
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

#include "../oexlib.h"

OEX_USING_NAMESPACE

/* 
Code                Description

110                 Restart marker reply
120                 Service ready in nnn minutes
125                 Data connection already open; transfer starting
150                 File status ok; about to open data connection  
200                 Command ok
202                 Command not implemented; superfluous at this site
211                 System status or system help reply
212                 Directory status
213                 File status
214                 Help message
215                 NAME system type
220                 Service ready for new user
221                 Service closing control connection
225                 Data connection open; no transfer in progress
226                 Closing data connection
227                 Entering passive mode (h1,h2,h3,h4,p1,p2)
230                 User logged in
250                 Requested file action ok, proceed
257                 "PATHNAME" created
331                 User name ok, need password
332                 Need account for login
350                 Requested file action pending further information
421                 Service not available, closing control connection
425                 Can't open data connection
426                 Connection closing; transfer aborted
450                 Requested file action not taken
451                 Requested file action aborted; local error in processing
452                 Requested action not taken
500                 Syntax error, command unrecognised
501                 Syntax error in parameters or arguments
502                 Command not implemented
503                 Bad sequence of commands
504                 Command not implemented for that parameter
530                 Not logged in
532                 Need account for storing files
550                 Requested action aborted
551                 Requested action aborted; page type unknown
552                 Requested file action aborted; exceeded storage allocation
553                 Requested action not taken; file name not allowed

http://www.networksorcery.com/enp/protocol/ftp.htm

*/

CVfsFtpSession::CVfsFtpSession()
{
    // Passive mode
    m_uPasvPort = 3110 + os::CHqTimer::GetBootCount() % 500;

    // Root folder
    m_sRoot = "_root_";
}

oexBOOL CVfsFtpSession::OnConnect( oexINT x_nErr ) 
{
    // Send greetings
    Write( "220 Nebula Server Ready.\n" );

    return oexTRUE;
}

oexBOOL CVfsFtpSession::OnRead( oexINT x_nErr )
{
    // Process the incomming data
    if ( !TBufferedPort< CAutoSocket >::OnRead( x_nErr ) )
	    return oexFALSE;

    // Grab the data
    CStr8 sData = Rx().Read();

    // First token should be the command
    CStr8 sCmd = sData.ParseNextToken().ToUpper();

    // USER
    if ( sCmd == "USER" )
    {   m_sUser = sData.ParseNextToken();
        Write( CStr8( "331 Password required for " ) << m_sUser << ".\n" );
    } // end if

    // PASS
    else if ( sCmd == "PASS" )
    {   m_sPassword = sData.ParseNextToken();
        Write( CStr8( "230 User \'" ) << m_sUser << "\' logged in at " << oexStrToStr8( CSysTime( 2 ).FormatTime( oexT( "GMT : %Y/%c/%d - %g:%m:%s.%l" ) ) ) << ".\n" );
    } // end else if

    // SYST
    else if ( sCmd == "SYST" )
        Write( "215 Windows_NT\n" );

    // PWD
    else if ( sCmd == "PWD" )
    {   if ( !m_sCurrent.Length() )
            m_sCurrent = "/";
        Write( CStr8( "257 \"" ) << m_sCurrent << "\" is current directory.\n" );
    } // end else if

    // CWD
    else if ( sCmd == "CWD" )
    {   sData = sData.TrimWhiteSpace();        
        if ( *sData != '/' && *sData != '\\' )
            sData = CStr8::BuildPath( m_sCurrent, sData );
        m_sCurrent = sData;
        Write( "250 CWD command successful.\n" );
    } // end else if

    // MKD
    else if ( sCmd == "MKD" )
    {   sData = sData.TrimWhiteSpace();        
        if ( *sData != '/' && *sData != '\\' )
            sData = CStr8::BuildPath( m_sCurrent, sData );
        m_vfs.MakeFolder( CStr8::BuildPath( m_sRoot, sData ).Ptr() ); 
        Write( "250 MKD command successful.\n" );
    } // end else if

    // RMD
    else if ( sCmd == "RMD" )
    {   sData = sData.TrimWhiteSpace();        
        if ( *sData != '/' && *sData != '\\' )
            sData = CStr8::BuildPath( m_sCurrent, sData );
        m_vfs.Delete( CStr8::BuildPath( m_sRoot, sData ).Ptr() ); 
        Write( "250 RMD command successful.\n" );
    } // end else if

    // CDUP
    else if ( sCmd == "CDUP" )
    {   m_sCurrent.RParse( "/\\" );
        Write( "250 CDUP command successful.\n" );
    } // end else if

    // NOOP
    else if ( sCmd == "NOOP" )
        Write( "200 Ok.\n" );

    // NOOP
    else if ( sCmd == "REST" )
    {   m_nsData.Destroy();
        Write( "200 Ok.\n" );
    } // end else if

    // PASV
    else if ( sCmd == "PASV" )
        CmdPasv();

    // TYPE
    else if ( sCmd == "TYPE" )
    {   m_sType = sData.ParseNextToken();
        Write( CStr8( "200 Type set to " ) << m_sType << ".\n" );
    } // end else if

    // LIST
    else if ( sCmd == "LIST" )
        CmdList();

    // RETR
    else if ( sCmd == "RETR" )
        CmdRetr( sData.TrimWhiteSpace().Ptr() );

    // STOR
    else if ( sCmd == "STOR" )
        CmdStor( sData.TrimWhiteSpace().Ptr() );

    // DELE
    else if ( sCmd == "DELE" )
    {   sData = sData.TrimWhiteSpace();        
        if ( *sData != '/' && *sData != '\\' )
            sData = CStr8::BuildPath( m_sCurrent, sData );
        m_vfs.Delete( CStr8::BuildPath( m_sRoot, sData ).Ptr() ); 
        Write( "250 DELE command successful.\n" );
    } // end if

    // QUIT
    else if ( sCmd == "QUIT" )
    {   Write( "221 Goodbye.\n" );        
        CloseSession();
    }

    // Error...
    else
        Write( CStr8( "500 \'" ) << sCmd << "\': Huh?\n" );

    return oexTRUE;
}

oexBOOL CVfsFtpSession::CmdPasv()
{   
    oexBOOL bStarted = oexFALSE;

    // Loop through port numbers
    if ( m_uPasvPort >= 3900 )
        m_uPasvPort = 3110;

    while ( m_uPasvPort < 4000 && !bStarted )
    {
        // Next port
        m_uPasvPort++;

        // Ensure the thread is running
        if ( !m_nsData.IsRunning() )
            m_nsData.Start();

        // Reset server
        m_nsData.Queue( 0, oexCall( oexT( "Reset" ) ) );

        // Bind to port
        if ( m_nsData.Queue( 0, oexCall( oexT( "Bind" ), m_uPasvPort ) )
                .Wait( oexDEFAULT_TIMEOUT ).GetReply().ToInt() 

             && m_nsData.Queue( 0, oexCall( oexT( "Listen" ), 0 ) )
                .Wait( oexDEFAULT_TIMEOUT ).GetReply().ToInt() )

            bStarted = oexTRUE;

    } // end while
        
    // Did we get a server?
    if ( bStarted )
    {
        // Tell the client where the server is
        CStr8 sAddress = oexStrToStr8( LocalAddress().GetDotAddress().Replace( '.', ',' ) );
        Write( CStr8().Fmt( "227 Entering Passive Mode (%s,%lu,%lu).\n", 
                            sAddress.Ptr(), m_uPasvPort >> 8 & 0xff, m_uPasvPort & 0xff ) );
    } // end if
        
    else
        Write( "425 Error creating server.\n" );

    return oexTRUE;
}

CDispatch CVfsFtpSession::GetPassiveConnection()
{
    // Get the first session
    CDispatch session = m_nsData.GetSession( 0 );

    // Wait for client to connect
    if ( !session.IsConnected() )
    {
        Write( "150 Waiting connection...\n" );

        os::CHqTimer to( oexTRUE );
        while ( 8 > to.ElapsedSeconds() && !session.IsConnected() )
        {   os::CSys::Sleep( 15 );
            session = m_nsData.GetSession( 0 );
        } // end while

        // Did we get a connection?
        if ( !session.IsConnected() )
            Write( "426 Timed out, I never heard from you!\n" );

    } // end if

    else 
        Write( "125 Data connection already open; Transfer starting.\n" );

    return session;
}

oexBOOL CVfsFtpSession::CmdList()
{       
    // Get connection
    CDispatch session = GetPassiveConnection();

    if ( !session.IsConnected() )
    {   Write( "426 Connection dropped before sending data.\n" );
        return oexFALSE;
    } // end if

    CStr8 sStr;
    CPropertyBag8 pb = m_vfs.ReadIndex( CurrentPath() );

    // Create file list
    for ( CPropertyBag8::iterator itPb; pb.List().Next( itPb ); )
    {
        // Format file information
        sStr << CStr8().Fmt( "%crwxrwxrwx    1 user     group %lu %s %s\n",
                              ( itPb.Obj()[ "type" ].ToLong() == 2 ) ? 'd' : '-',
                              (oexUINT)itPb.Obj()[ "size" ].ToLong(),
                              oexStrToStr8Ptr( 
                                CSysTime( CSysTime::eFmtFile, 0 )
                                    .FormatTime( oexT( "%b %d %Y" ) ) ),
                              itPb.Node()->key.Ptr() );

    } // end for

    // Write the file list to client
    if ( sStr.Length() )
        session.Queue( 0, oexCall( oexT( "WriteStr" ), oexStr8ToStr( sStr ) ) );

    // Wait for the data to go out
    oexUINT nWritten = 0;
    oexCONST oexUINT uTimeout = 8;
    os::CHqTimer to( oexTRUE );
    double dTime = 0;
    
    // Attempt to write data
    while ( !nWritten && uTimeout > ( dTime = to.ElapsedSeconds() ) && session.IsConnected() )
    {
        nWritten = session.Queue( 0, oexCall( oexT( "WaitTxEmpty" ), 0 ) )
                            .Wait( uTimeout * 1000 ).GetReply().ToInt();
        
        if ( !nWritten )
            os::CSys::Sleep( 15 );

    } // end while

    // Shutdown all connections
    m_nsData.Queue( 0, oexCall( oexT( "Reset" ) ) ).Wait( oexDEFAULT_TIMEOUT );

    // All done
    if ( nWritten ) 
        Write( "226 Transfer complete.\n" );

    // Timeout?
    else if ( uTimeout < to.ElapsedSeconds() )
        Write( "426 Timed out, I'm tired of waiting for the data to go out\n" );

    // Connection dropped
    else
        Write( "426 Connection dropped while sending data.\n" );

    return oexTRUE;
}

oexBOOL CVfsFtpSession::CmdRetr( oexCSTR8 x_pFile )
{       
    CStr8 sContent = m_vfs.LoadFile( CStr8::BuildPath( CurrentPath(), x_pFile ).Ptr() );
    if ( !sContent.Length() )
    {   Write( "451 Requested file action aborted; local error in processing.\n" );
        return oexFALSE;
    } // end if

    // Get connection
    CDispatch session = GetPassiveConnection();

    if ( !session.IsConnected() )
        return oexFALSE;

    if ( sContent.Length() )
        session.Queue( 0, oexCall( oexT( "WriteStr" ), oexStr8ToStr( sContent ) ) );

    // Shutdown all connections
    m_nsData.Queue( 0, oexCall( oexT( "Reset" ) ) ).Wait( oexDEFAULT_TIMEOUT );

    // All done
    Write( "226 Transfer complete.\n" );

    return oexTRUE;
}

oexBOOL CVfsFtpSession::CmdStor( oexCSTR8 x_pFile )
{       
    // Get connection
    CDispatch session = GetPassiveConnection();

    if ( !session.IsConnected() )
        return oexFALSE;

    // Set the file name
    session.Queue( 0, oexCall( oexT( "SetFile" ), oexStr8ToStr( CStr8::BuildPath( CurrentPath(), x_pFile ) ) ) );

    // Open the vfs
    session.Queue( 0, oexCall( oexT( "OpenVfs" ),  oexStr8ToStr( m_vfs.GetRoot() ) ) );

    // Wait for data to be tx'ed
    while ( session.IsConnected() )
        os::CSys::Sleep( 15 );

    // All done
    Write( "226 Transfer complete.\n" );

    return oexTRUE;
}