
#pragma once

#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/videodev.h>
#include <linux/videodev2.h>
#include <sys/mman.h>

#include <errno.h>

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

	/// Capture device
	oex::vid::CCapture		m_cCapture;
};
