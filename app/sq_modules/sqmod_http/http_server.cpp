// http_server.cpp

#include "stdafx.h"

/*
	sqmod_http_d.dll!CHttpServerImpl::SetCallback(CSqMsgQueue * x_pMsgQueue=0x1016c1d8, const std::basic_string<char,std::char_traits<char>,std::allocator<char> > & sServer="OnServerEvent", const std::basic_string<char,std::char_traits<char>,std::allocator<char> > & sSession="OnProcessRequest")  Line 6	C++
 	sqmod_http_d.dll!CHttpServer::SetCallback(CSqMsgQueue * x_pMsgQueue=0x1016c1d8, const std::basic_string<char,std::char_traits<char>,std::allocator<char> > & sServer="OnServerEvent", const std::basic_string<char,std::char_traits<char>,std::allocator<char> > & sSession="OnProcessRequest")  Line 77	C++
 	sqmod_http_d.dll!SqPlus::ReturnSpecialization<void>::Call<CHttpServer,CSqMsgQueue *,std::basic_string<char,std::char_traits<char>,std::allocator<char> > const &,std::basic_string<char,std::char_traits<char>,std::allocator<char> > const &>(CHttpServer & callee={...}, void (CSqMsgQueue *, const std::basic_string<char,std::char_traits<char>,std::allocator<char> > &, const std::basic_string<char,std::char_traits<char>,std::allocator<char> > &)* func=0x00e7127b, SQVM * v=0x00c6dbd0, int index=2)  Line 1089 + 0x85 bytes	C++
 	sqmod_http_d.dll!SqPlus::Call<CHttpServer,void,CSqMsgQueue *,std::basic_string<char,std::char_traits<char>,std::allocator<char> > const &,std::basic_string<char,std::char_traits<char>,std::allocator<char> > const &>(CHttpServer & callee={...}, void (CSqMsgQueue *, const std::basic_string<char,std::char_traits<char>,std::allocator<char> > &, const std::basic_string<char,std::char_traits<char>,std::allocator<char> > &)* func=0x00e7127b, SQVM * v=0x00c6dbd0, int index=2)  Line 1233 + 0x15 bytes	C++
 	sqmod_http_d.dll!SqPlus::DirectCallInstanceMemberFunction<CHttpServer,void (__thiscall CHttpServer::*)(CSqMsgQueue *,std::basic_string<char,std::char_traits<char>,std::allocator<char> > const &,std::basic_string<char,std::char_traits<char>,std::allocator<char> > const &)>::Dispatch(SQVM * v=0x00c6dbd0)  Line 1311 + 0x15 bytes	C++
 	sqengine_d.dll!SQVM::CallNative(SQNativeClosure * nclosure=0x00c7fea8, int nargs=4, int stackbase=5, SQObjectPtr & retval={...}, bool & suspend=false)  Line 1098 + 0xc bytes	C++
 	sqengine_d.dll!SQVM::Execute(SQObjectPtr & closure={...}, int target=3, int nargs=1, int stackbase=3, SQObjectPtr & outres={...}, unsigned int raiseerror=1, SQVM::ExecutionType et=ET_CALL)  Line 733 + 0x23 bytes	C++
 	sqengine_d.dll!SQVM::Call(SQObjectPtr & closure={...}, int nparams=1, int stackbase=3, SQObjectPtr & outres={...}, unsigned int raiseerror=1)  Line 1363 + 0x28 bytes	C++
 	sqengine_d.dll!sq_call(SQVM * v=0x00c6dbd0, int params=1, unsigned int retval=1, unsigned int raiseerror=1)  Line 923 + 0x37 bytes	C++
 	sqengine_d.dll!SqPlus::SquirrelFunction<void>::operator()()  Line 1544 + 0x11 bytes	C++
 	sqengine_d.dll!sqbind::CSqEngine::Execute(const char * x_pFunction=0x101160f8)  Line 533	C++
 	sqengine_d.dll!sqbind::CSqEngine::Load(const char * pScript=0x00c75800, bool bFile=true, bool bRelative=false, bool bStart=true)  Line 411	C++
 	sqengine_d.dll!CScriptThread::InitThread(void * x_pData=0x00000000)  Line 94 + 0x31 bytes	C++
 	sqengine_d.dll!oex::CThread::ThreadProc(void * x_pData=0x1016c1a0)  Line 83 + 0x11 bytes	C++
 	sqengine_d.dll!oex::os::CResource::ThreadProc(void * x_pData=0x00c755ac)  Line 272 + 0xf bytes	C++
 	sqengine_d.dll!oex::os::CResource::CThreadProcImpl::ThreadProc(void * x_pData=0x00c755ac)  Line 89 + 0xc bytes	C++
 	kernel32.dll!7c80b713() 	
 	[Frames below may be incorrect and/or missing, no symbols loaded for kernel32.dll]	
*/
void CHttpServerImpl::SetCallback( CSqMsgQueue *x_pMsgQueue, const std::tstring &sServer, const std::tstring &sSession )
{	m_pMsgQueue = x_pMsgQueue;
	m_sServer = sServer;
	m_sSession = sSession;
}

int CHttpServerImpl::Start( int nPort )
{
	// Set session callback
	m_server.SetSessionCallback( (oex::oexPVOID)CHttpServerImpl::_OnSessionCallback, this );

	// Start the server
	if ( !m_server.StartServer( nPort, CHttpServerImpl::_OnServerEvent, this ) )
		return 0;

	return 1;
}


oex::oexINT CHttpServerImpl::_OnServerEvent( oex::oexPVOID x_pData, oex::oexINT x_nEvent, oex::oexINT x_nErr,
										     oex::THttpServer< oex::os::CIpSocket, oex::THttpSession< oex::os::CIpSocket > > *x_pServer )
{
	CHttpServerImpl *pServer = (CHttpServerImpl*)x_pData;
	if ( !oexCHECK_PTR( pServer ) )
		return -1;

	if ( !oexCHECK_PTR( x_pServer ) )
		return -2;

	return pServer->OnServerEvent( x_pData, x_nEvent, x_nErr, x_pServer );
}

oex::oexINT CHttpServerImpl::_OnSessionCallback( oex::oexPVOID x_pData, oex::THttpSession< oex::os::CIpSocket > *x_pSession )
{
	CHttpServerImpl *pServer = (CHttpServerImpl*)x_pData;
	if ( !oexCHECK_PTR( pServer ) )
		return -1;

	if ( !oexCHECK_PTR( x_pSession ) )
		return -2;

	return pServer->OnSessionCallback( x_pData, x_pSession );
}

