// sq_usb.h

class CSqUsb
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqUsb )
	_SQBIND_CLASS_CTOR_END( CSqUsb )

	/// Default constructor
	CSqUsb();

	/// Destructor
	virtual ~CSqUsb();

	/// Releases resources
	void Destroy();

	/// Initializes output device information
	int Init();

	/// Opens the specified output stream
	sqbind::CSqMulti getDeviceList();

private:

	/// Initialization status
	int			m_nInit;


};
