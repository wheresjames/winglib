
#pragma once

#include <sys/ioctl.h>
#include <linux/types.h>
//#include <linux/videodev.h>
#include <linux/videodev2.h>

#include <errno.h>
using namespace oex;

class CV4lCapture
{
public:

	enum
	{
		// Automatic IO type selection
		eIoAuto,

		// use read/write system calls
		eIoReadWrite,

		// Async IO
		eIoAsync,

		// streaming IO ioctls
		eIoStreaming
	};

public:

	/// Default contructor
	CV4lCapture()
	{
		m_nFd = -1;
		m_nIoMode = eIoAuto;
		m_nLastError = 0;
	}

	/// Destructor
	~CV4lCapture()
	{
		Destroy();
	}

	/////////////////////////////////////////////////////////////////
	// Open()
	/////////////////////////////////////////////////////////////////
	/**
		\param [in] x_pDevice	-	Video device name
										- Example: /dev/video0
	*/
	oexBOOL Open( oexCSTR x_pDevice )
	{
		// Sanity checks
		if ( !x_pDevice || !*x_pDevice )
		{	m_sLastError = oexT( "Invalid device name" );
			return oexFALSE;
		} // end if

		struct stat st;
		if ( -1 == stat( oexStrToMbPtr( x_pDevice ), &st ) )
		{	m_nLastError = errno;
			m_sLastError = oexT( "Device name is invalid" );
			return oexFALSE;
		} // end if

		if ( !S_ISCHR( st.st_mode ) )
		{	m_nLastError = -1;
			m_sLastError = oexT( "Name does not specify a device" );
			return oexFALSE;
		} // end if

		// Attempt to open the device
		m_nFd = open( oexStrToMbPtr( x_pDevice ), O_RDWR | O_NONBLOCK, 0 );
		if ( -1 == m_nFd )
		{	m_nLastError = errno;
			m_sLastError = oexT( "Unable to open file" );
			return oexFALSE;
		} // end if

		// Save device name
		m_sDeviceName = x_pDevice;

		// Attempt to initialize the device
		if ( !Init() )
		{	Destroy();
			return oexFALSE;
		} // end if

		return oexTRUE;
	}

	/////////////////////////////////////////////////////////////////
	// Destroy()
	/////////////////////////////////////////////////////////////////
	/// Closes any open capture device
	oexBOOL Destroy()
	{
		if ( -1 == m_nFd )
			return oexTRUE;

		// Close the file
		close( m_nFd );

		m_nFd = -1;
	}

public:

	/// +++ Fix this function
	static int IoCtl ( int fd, int request, void * arg )
	{
		int r;
		do r = ioctl ( fd, request, arg );
		while ( - 1 == r && EINTR == errno );
		return r;
	}

public:

	/////////////////////////////////////////////////////////////////
	// IsOpen()
	/////////////////////////////////////////////////////////////////
	/// Returns non-zero if a capture device is open
	oexBOOL IsOpen()
	{	return -1 != m_nFd; }

	/////////////////////////////////////////////////////////////////
	// Init()
	/////////////////////////////////////////////////////////////////
	/// Initializes the device
	/**
	*/
	oexBOOL Init()
	{
		// Query device capabilities
		if ( -1 == IoCtl( m_nFd, VIDIOC_QUERYCAP, &m_cap ) )
		{
			m_nLastError = errno;

			if ( EINVAL == errno )
				m_sLastError = oexT( "Error " ), m_sLastError += errno,
				m_sLastError += oexT( " : VIDEOC_QUERYCAP : Invalid V4L2 device, " ),
				m_sLastError += m_nFd;
			else
				m_sLastError = oexT( "VIDEOC_QUERYCAP : Failure" );

			return oexFALSE;

		} // end if

		// Ensure it's actually a capture device
		if ( !( m_cap.capabilities & V4L2_CAP_VIDEO_CAPTURE ) )
		{	m_sLastError = oexT( "Not a capture device" );
			return oexFALSE;
		} // end if

		// +++ Detect io mode if eIoAuto
		m_nIoMode = eIoReadWrite;

		// Verify image transfer mode is supported
		switch( m_nIoMode )
		{
			case eIoReadWrite :

				if ( !( m_cap.capabilities & V4L2_CAP_READWRITE ) )
				{	m_sLastError = oexT( "Stream based IO not supported" );
					return oexFALSE;
				} // end if

				break;

			case eIoAsync :

				if ( !( m_cap.capabilities & V4L2_CAP_ASYNCIO ) )
				{	m_sLastError = oexT( "Stream based IO not supported" );
					return oexFALSE;
				} // end if

				m_sLastError = oexT( "+++ Sorry, not implemented" );
				return oexFALSE;

				break;

			case eIoStreaming :

				if ( !( m_cap.capabilities & V4L2_CAP_STREAMING ) )
				{	m_sLastError = oexT( "Stream based IO not supported" );
					return oexFALSE;
				} // end if

				break;

//			case IoUser :
//				break;

			default :
				m_sLastError = oexT( "Invalid mode" );
				return oexFALSE;
				break;

		} // end switch

		return oexTRUE;
	}

public:

	/////////////////////////////////////////////////////////////////
	// GetLastError()
	/////////////////////////////////////////////////////////////////
	/// Returns the last error code
	oexINT GetLastError()
	{	return m_nLastError; }

	/////////////////////////////////////////////////////////////////
	// SetLastError()
	/////////////////////////////////////////////////////////////////
	/// Sets the last error code
	/**
		\param [in] x_nErr	- Error code to set
	*/
	void SetLastError( oexINT x_nErr )
	{	m_nLastError = x_nErr; }

	/////////////////////////////////////////////////////////////////
	// GetLastErrorStr()
	/////////////////////////////////////////////////////////////////
	/// Returns the last error
	CStr& GetLastErrorStr()
	{	return m_sLastError; }

private:

	/// Name of the device
	CStr				m_sDeviceName;

	/// Device file descriptor
	oexINT				m_nFd;

	/// IO method
	oexINT				m_nIoMode;

	/// Description of the last error
	CStr				m_sLastError;

	/// Integer describing the last error
	oexINT				m_nLastError;

	/// Device capabilities
	v4l2_capability		m_cap;

	/// Cropping capabilities
	v4l2_cropcap		m_cropcap;

	/// Cropping info
	v4l2_crop			m_crop;

	/// Video format information
	v4l2_format			m_fmt;
};


//==================================================================
// CFrameWnd
//
/// Main window frame
/**

*/
//==================================================================
class CFrameWnd : public wxFrame
{
public:

	enum
	{
		WXIDM_Exit = wxID_HIGHEST + 1,
		WXIDM_Open
	};


public:
	bool StretchDraw(wxDC &x_dc, wxImage &x_img, wxRect &x_rect);

	/// Default Constructor
	CFrameWnd();

	/// Destructor
	virtual ~CFrameWnd();

	//==============================================================
	// CFrameWnd()
	//==============================================================
	/// Constructor
	/**
		\param [in] x_sTitle	-	Window title
		\param [in] x_ptWin		-	Default window position
		\param [in] x_sizeWin	-	Default window size

		\return

		\see
	*/
    CFrameWnd( const wxString &x_sTitle, const wxPoint &x_ptWin, const wxSize &x_sizeWin );

	void OnExit( wxCommandEvent &x_wxCe );

	void OnOpen( wxCommandEvent &x_wxCe );

	void OnPaint( wxPaintEvent& x_wxPe );

	void OnEraseBackground( wxEraseEvent& x_wxEe );

    DECLARE_EVENT_TABLE()

private:

	wxImage					m_cWxImg;

	CV4lCapture				m_v4lCap;

};
