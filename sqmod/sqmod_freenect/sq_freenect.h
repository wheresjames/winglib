// sq_freenect.h

class CSqFreenect
{
public:

	_SQBIND_CLASS_CTOR_BEGIN( CSqFreenect )
	_SQBIND_CLASS_CTOR_END( CSqFreenect )

	/// Default constructor
	CSqFreenect();

	/// Destructor
	virtual ~CSqFreenect();

	/// Releases resources
	void Destroy();

	/// Initializes output device information
	int Init();

	/// Returns the number of devices
	int getNumDevices();

	/// Opens the specified output stream
	sqbind::CSqMulti getDeviceList();

	/// Opens the specified device
	int OpenDevice( int i );

	/// Closes any open device
	int CloseDevice();
	
private:

	/// Initialization status
	int						m_nInit;

	/// Open device index
	int						m_nDev;

	/// freenect context
	freenect_context		*m_pfc;

	/// freenect device
	freenect_device			*m_pfd;

};
