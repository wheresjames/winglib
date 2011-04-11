// sq_quickfix.h

class CSqQuickfix
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqQuickfix )
	_SQBIND_CLASS_CTOR_END( CSqQuickfix )

	/// Default constructor
	CSqQuickfix();

	/// Destructor
	virtual ~CSqQuickfix();

	/** \addtogroup CSqQuickfix
		@{
	*/

	/// Releases resources
	void Destroy();

	/// Initializes
	int Init( const sqbind::stdString &sSettings );

	/// Returns a string describing the last error
	sqbind::stdString getLastError() { return m_sLastError; }

	/** @} */
	
private:

	/// Last error that occured
	sqbind::stdString		m_sLastError;

	/// Fix settings
	FIX::SessionSettings	*m_pSs;

	/// FIX::Application derived object
	FIX::Application		*m_pApp;

	/// File store
	FIX::FileStoreFactory	*m_pFsf;

	/// Screen log
	FIX::ScreenLogFactory	*m_pLog;

	/// Acceptor
	FIX::SocketAcceptor		*m_pSa;

};
