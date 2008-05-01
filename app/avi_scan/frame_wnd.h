// FrameWnd.h: interface for the CFrameWnd class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FRAMEWND_H__17157451_4597_497E_A568_60C3007F3590__INCLUDED_)
#define AFX_FRAMEWND_H__17157451_4597_497E_A568_60C3007F3590__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

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

	oex::vid::CAviFile		m_cAviFile;

};

#endif // !defined(AFX_FRAMEWND_H__17157451_4597_497E_A568_60C3007F3590__INCLUDED_)
