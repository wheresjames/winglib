/*------------------------------------------------------------------
// ftp_session.cpp
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

CFtpSession::CFtpSession()
{
    // Passive mode
    m_uPasvPort = 3110 + os::CHqTimer::GetBootCount() % 1000;

    // Root folder
    m_sRoot = "c:/ftp";
}

oexBOOL CFtpSession::OnConnect( oexINT x_nErr ) 
{
    // Send greetings
    Tx().Write( "220 FTP Server Ready.\n" );

    return oexTRUE;
}

oexBOOL CFtpSession::OnRead( oexINT x_nErr )
{
    // Process the incomming data
    if ( !OnRead( x_nErr ) )
	    return oexFALSE;

    // Grab the data
    CStr8 sData = Rx().Read();

    // First token should be the command
    CStr8 sCmd = sData.ParseNextToken().ToUpper();

    // USER
    if ( sCmd == "USER" )
    {   m_sUser = sData.ParseNextToken();
        Tx().Write( CStr8( "331 Password required for " ) << m_sUser << ".\n" );
    } // end if

    // PASS
    else if ( sCmd == "PASS" )
    {   m_sUser = sData.ParseNextToken();
        Tx().Write( CStr8( "230 User \'" ) << m_sUser << "\' logged in at " << oexStrToStr8( CSysTime( 2 ).FormatTime( oexT( "GMT : %Y/%c/%d - %g:%m:%s.%l" ) ) ) << ".\n" );
    } // end else if

    // SYST
    else if ( sCmd == "SYST" )
        Tx().Write( "215 Windows_NT\n" );

    // PWD
    else if ( sCmd == "PWD" )
    {   if ( !m_sCurrent.Length() ) m_sCurrent = "/";
        Tx().Write( CStr8( "257 \"" ) << m_sCurrent << "\" is current directory.\n" );
    } // end else if

    // CWD
    else if ( sCmd == "CWD" )
    {   sData = sData.TrimWhiteSpace();        
        if ( *sData != '/' && *sData != '\\' )
            sData = CStr8::BuildPath( m_sCurrent, sData );
        if ( CFile::Exists( oexStr8ToStrPtr( CStr8::BuildPath( m_sRoot, sData ) ) ) )
            m_sCurrent = sData;
        Tx().Write( "250 CWD command successful.\n" );
    } // end else if

    // CDUP
    else if ( sCmd == "CDUP" )
    {   m_sCurrent.RParse( "/\\" );
        Tx().Write( "250 CDUP command successful.\n" );
    } // end else if

    // NOOP
    else if ( sCmd == "NOOP" )
        Tx().Write( "200 Ok.\n" );

    // NOOP
    else if ( sCmd == "REST" )
    {   m_asData.Destroy();
        Tx().Write( "200 Ok.\n" );
    } // end else if

    // PASV
    else if ( sCmd == "PASV" )
        CmdPasv();

    // TYPE
    else if ( sCmd == "TYPE" )
    {   m_sType = sData.ParseNextToken();
        Tx().Write( CStr8( "200 Type set to " ) << m_sType << ".\n" );
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
    {   sData = CStr8::BuildPath( CurrentPath(), sData.TrimWhiteSpace() );
        if ( CFile::Exists( oexStr8ToStrPtr( sData ) ) )
            CFile::Delete( oexStr8ToStrPtr( sData ) );
        Tx().Write( "250 DELE command successful.\n" );
    } // end if

    // QUIT
    else if ( sCmd == "QUIT" )
    {   Tx().Write( "221 Goodbye.\n" );
        Destroy();
    }

    // Error...
    else
        Tx().Write( CStr8( "500 \'" ) << sCmd << "\': Huh?\n" );

    return oexTRUE;
}

oexBOOL CFtpSession::CmdPasv()
{   
/*
    // See if the server is running already
    if ( !m_asData.IsConnected() && !m_asData.IsConnecting() )
    {
        // Kill port
        m_asData.Destroy();

        // Loop through port numbers
        if ( m_uPasvPort >= 4000 )
            m_uPasvPort = 3110;

        // +++

        // Attempt to start a server somewhere
//        while ( m_uPasvPort < 4000 && !m_asData.StartServer( m_uPasvPort ) )
//                m_uPasvPort++;

    } // end if

    // Did we get a server?
    if ( m_asData.IsConnected() || m_asData.IsConnecting() )
    {
        // Ensure the server gets on it's feet
        if ( !m_asData.WaitListen() )
        {   Tx().Write( oexT( "425 Error creating server.\n" ) );
            return oexFALSE;
        } // end if

        CStr sAddress = GetSocket()->LocalAddress().GetDotAddress().Replace( '.', ',' );
        Tx().Write( CStr8().Fmt( "227 Entering Passive Mode (%s,%lu,%lu).\n", 
                            sAddress.Ptr(), m_uPasvPort >> 8 & 0xff, m_uPasvPort & 0xff ) );
    } // end if

    else Tx().Write( "425 Error creating server.\n" );
*/
    return oexTRUE;
}
/*
TAutoServer< CFtpDataConnection >::t_SessionList::iterator CFtpSession::GetPassiveConnection()
{
    // Get the first session in the list
    TAutoServer< CFtpDataConnection >::t_SessionList::iterator it = 
        m_asData.SessionList().First();

    // Wait for client to connect
    if ( !it.IsValid() )
    {
        Tx().Write( "150 Waiting connection...\n" );

        os::CHqTimer to( oexTRUE );
        while ( 8 > to.ElapsedSeconds() && !it.IsValid() )
        {   os::CSys::Sleep( 15 );
            it = m_asData.SessionList().First();
        } // end while

    } // end if
    else Tx().Write( "125 Data connection already open; Transfer starting.\n" );

    // Did we get a connection?
    if ( !it.IsValid() )
        Tx().Write( "426 Timed out, I never heard from you!\n" );

    return it;
}
*/
oexBOOL CFtpSession::CmdList()
{       
/*    TAutoServer< CFtpDataConnection >::t_SessionList::iterator it = GetPassiveConnection();
    if ( !it.IsValid() ) return oexFALSE;

    CStr sStr;
   	CFindFiles ff;

    // Build file list
    if ( ff.FindFirst( CurrentPath().Ptr() ) )
    {
        do
        {
            // Format file information
            sStr << CStr8().Fmt( "%crwxrwxrwx    1 user     group %lu %s %s\n",
                                  ( ff.GetFileAttributes() & os::CBaseFile::eFileAttribDirectory ) ? 'd' : '-',
                                  (oexUINT)ff.FileData().llSize,
                                  oexStrToStr8( CSysTime( CSysTime::eFmtFile, ff.FileData().ftLastModified )
                                    .FormatTime( oexT( "%b %d %Y" ) ) ).Ptr(),
                                  ff.GetFileName().Ptr() );

        } while ( ff.FindNext() );

        // Write the file list to client
        it->Protocol()->Write( sStr );

    } // end if

    // Close the passive connection
    it->Close( 1000 );

    // All done
    Tx().Write( "226 Transfer complete.\n" );
*/
    return oexTRUE;
}

oexBOOL CFtpSession::CmdRetr( oexCSTR8 x_pFile )
{       
    CFile f;
    if ( !f.OpenExisting( oexStr8ToStrPtr( CStr8::BuildPath( CurrentPath(), x_pFile ) ) ).IsOpen() )
    {   Tx().Write( "451 Requested file action aborted; local error in processing.\n" );
        return oexTRUE;
    } // end if
/*
    TAutoServer< CFtpDataConnection >::t_SessionList::iterator it = GetPassiveConnection();
    if ( !it.IsValid() ) return oexFALSE;

    CStr sBlock;
    while ( ( sBlock = f.Read( 1024 ) ).Length() )
        it->Protocol()->Write( sBlock );

    // Close the passive connection
    it->Close( 1000 );

    // All done
    Tx().Write( "226 Transfer complete.\n" );
*/
    return oexTRUE;
}

oexBOOL CFtpSession::CmdStor( oexCSTR8 x_pFile )
{       
/*    TAutoServer< CFtpDataConnection >::t_SessionList::iterator it = GetPassiveConnection();
    if ( !it.IsValid() ) return oexFALSE;

    // Open the data file
    if ( !it->Protocol()->m_fData.CreateAlways( CFile::BuildPath( CurrentPath(), x_pFile ).Ptr() ).IsOpen() )
    {   Tx().Write( "451 Requested file action aborted; local error in processing.\n" );
        return oexTRUE;
    } // end if
    
    // All done
    Tx().Write( "226 Transfer complete.\n" );
*/
    return oexTRUE;
}