/*------------------------------------------------------------------
// vfw2.hpp
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


// {4B2C1FAD-F5E6-4f4c-9D2B-941F85239BC6}
static const GUID IID_DsRenderer = 
{ 0x4b2c1fad, 0xf5e6, 0x4f4c, { 0x9d, 0x2b, 0x94, 0x1f, 0x85, 0x23, 0x9b, 0xc6 } };


//==================================================================
// CDsRenderer
//
/// Renders video from a DirectShow source
/**
	Encapsulates a DirectShow graph containing a video source.
	
*/
//==================================================================
class CDsRenderer : 
		public CBaseVideoRenderer
{
public:

	/// Constructor
	/**
		\param [in] pUnk			-	IUnknown pointer for obtaining the interface.  Can be NULL.
		\param [out] phr			-	Windows error code or S_OK if success.
	*/
	CDsRenderer( LPUNKNOWN pUnk,HRESULT *phr )
		: CBaseVideoRenderer( IID_DsRenderer, NAME( "CDsRenderer" ), pUnk, phr )
	{_STT();
		m_nWidth = 0;
		m_nHeight = 0;
		m_nScanWidth = 0;

		m_llFrame = 0;

		m_cbfOnFrame = oexNULL;
		m_pUser = oexNULL;
	}

	/// Destructor
	virtual ~CDsRenderer() 
	{_STT();
	}

	/// Verifys the compatiblity of the specified media type.
	/**
		\param [in] pmt		-	CMediaType object describing the desired media type and format.

		\return		Windows error code if error or unsupported media type.  
					S_OK if media description is supported.

		\see SetMediaType()
	*/
    virtual HRESULT CheckMediaType( const CMediaType *pmt )
	{_STT();
		VIDEOINFO *pvi = 0;
	    
		if( pmt == NULL )
			return oexERROR( ERROR_INVALID_PARAMETER, oexT( "CMediaType pointer is NULL" ) );

		// Reject the connection if this is not a video type
		if( *pmt->FormatType() != FORMAT_VideoInfo )
			return oexWARNING( ERROR_INVALID_PARAMETER, CStr().Fmt( oexT( "Unsupported video format type %s" ), 
																  oexStrToMb( CStr().GuidToString( (oexGUID*)pmt->FormatType() ).Ptr() ) ) );

		// Video only
		pvi = (VIDEOINFO*)pmt->Format();
		if( !IsEqualGUID( *pmt->Type(), MEDIATYPE_Video ) )
			return oexWARNING( ERROR_INVALID_PARAMETER, CStr().Fmt( oexT( "Unsupported video type %s" ), 
																  oexStrToMb( CStr().GuidToString( (oexGUID*)pmt->Type() ).Ptr() ) ) );

		// Only accept RGB24 video
		if ( !IsEqualGUID( *pmt->Subtype(), MEDIASUBTYPE_RGB24 ) )
			return oexWARNING( ERROR_INVALID_PARAMETER, CStr().Fmt( oexT( "RGB24 not supported, %s" ), 
																  oexStrToMb( CStr().GuidToString( (oexGUID*)pmt->Type() ).Ptr() ) ) );

		// Save video parameters
//		SetMediaType( pmt );

		return S_OK;
	}

	/// Sets the current media information.
	/**
		\param [in] pmt		-	CMediaType object describing the desired media type and format.

		\return Windows error code or S_OK if success

		\see CheckMediaType()
	*/
	virtual HRESULT SetMediaType( const CMediaType *pmt )
	{_STT();
		// Retrive the size of this media type
		VIDEOINFO *pviBmp = (VIDEOINFO *)pmt->Format();

		// Save video parameters
		m_nWidth = pviBmp->bmiHeader.biWidth;
		m_nHeight = abs( pviBmp->bmiHeader.biHeight );
		m_nScanWidth = CImage::GetScanWidth( m_nWidth, 24 );

		return S_OK;
	}
    
	/// Called by the CBaseVideoRenderer framework when a new sample is available.
	virtual HRESULT DoRenderSample( IMediaSample *pMediaSample )
	{_STT();
		// Do we have a callback function?
		if ( oexCHECK_PTR( m_cbfOnFrame ) )
		{
			CCaptureTmpl::SFrameInfo si;
			oexZeroMemory( &si, sizeof( si ) );

			si.lSize = sizeof( si );
			pMediaSample->GetPointer( (LPBYTE*)&si.pBuf );
			si.lImageSize = (long)pMediaSample->GetSize();
			si.lWidth = m_nWidth;
			si.lHeight = m_nHeight;
			si.llFrame = m_llFrame;

			// Make the callback
			m_cbfOnFrame( &si, m_pUser );

		} // end if	   

		// Count a frame
		m_llFrame++;

		return S_OK;
	}

	/// Returns the image width
	long GetWidth() { return m_nWidth; }

	/// Returns the image height
	long GetHeight() { return m_nHeight; }

	/// Set frame callback function
	void SetFrameCallback( CCaptureTmpl::cbf_OnFrame &f, oexPVOID u ) 
	{	m_cbfOnFrame = f; m_pUser = u; }

private:

	/// Video frame width
	oexINT								m_nWidth;

	/// Video frame height
	oexINT								m_nHeight;

	/// Video scan width
	oexINT								m_nScanWidth;

	/// The current frame number
	oexINT64							m_llFrame;

	/// Frame callback function pointer
	CCaptureTmpl::cbf_OnFrame			m_cbfOnFrame;

	/// User data passed to callback function
	oexPVOID							m_pUser;

};


class CDsCapture
{
public:

	static const REFERENCE_TIME c_rtTimeBase;

public:

	/// Constructor
	CDsCapture()
	{_STT();
#if defined( oexDEBUG )
		m_dwRotId = 0;
#endif

		m_nWidth = 320;
		m_nHeight = 240;
		m_dFrameRate = 15;

		m_dwBaseTime = 0;

		m_pVideoRenderer = NULL;

		m_hrInitialized = -1;

		m_bStop = FALSE;

	}

	/// Destructor
	virtual ~CDsCapture()
	{_STT();
		Destroy();
		if ( SUCCEEDED( m_hrInitialized ) )
		{	m_hrInitialized = -1;
			CoUninitialize();
		} // end if
	}

	oexRESULT Init()
	{_STT();
		// Already done that?
		if ( S_OK == m_hrInitialized )
			return S_OK;

		// Initialize COM
		m_hrInitialized = ::CoInitialize( NULL );
		if ( FAILED( m_hrInitialized ) )
			return oexERROR( m_hrInitialized, "CoInitialize() failed" );

		return S_OK;
	}
	
	/// Releases all resources
	void Destroy()
	{_STT();
		// Stop the graph
		Stop();

		// *** Capture

		// Lose video renderer pointer
		if ( m_pVideoRenderer )
			m_pVideoRenderer = NULL;

		// Release capture source filter
		if ( m_cpCaptureSource ) 
			m_cpCaptureSource.Release();

		// *** Graph

#if defined( oexDEBUG )
		// Remove from GraphEdit
		RemoveFromRot( m_dwRotId );
		m_dwRotId = 0;
#endif

		// Release interfaces
		if ( m_cpFilterGraph2.p ) 
			m_cpFilterGraph2.Release();
		if ( m_cpMediaControl.p ) 
			m_cpMediaControl.Release();
		if ( m_cpMediaPosition.p ) 
			m_cpMediaPosition.Release();
		if ( m_cpMediaEvent.p ) 
			m_cpMediaEvent.Release();
		if ( m_cpMediaFilter.p ) 
			m_cpMediaFilter.Release();
		if ( m_cpCaptureGraphBuilder2.p ) 
			m_cpCaptureGraphBuilder2.Release();
		if ( m_cpGraphBuilder.p ) 
			m_cpGraphBuilder.Release();
	}

	/// Initializes capture device
	/**
		\param [in] dwDevice	- Index of the capture device
		\param [in] dwSource	- Video source index
		\param [in[ cbfOnFrame	- Callback function that receives video frames

	*/
	HRESULT Capture( DWORD dwDevice, DWORD dwSource, oexINT nWidth, oexINT nHeight, CCaptureTmpl::cbf_OnFrame cbfOnFrame = 0, oexPVOID pUser = 0 )
	{_STT();
		HRESULT hr = -1;

		// Lose old graph
		Destroy();

		// iii Some things in Windows crash on sizes smaller than this!
		if ( 80 > nWidth || 60 > nHeight )
		{	Destroy(); return oexERROR( ERROR_INVALID_PARAMETER, CStr().Fmt( oexT( "Invalid capture size : %d x %d" ), nWidth, nHeight ) ); 
		} // end if

		// Create capture graph
		if ( FAILED( hr = Create() ) )
			return hr;

		// Create the capture filter
		if ( FAILED( hr = CreateCaptureFilter( dwDevice, &m_cpCaptureSource ) ) )
		{	Destroy(); return oexERROR( hr, oexT( "Error creating capture filter" ) ); 
		} // end if

		// Get the capture source
		if ( FAILED( hr = AddFilter( m_cpCaptureSource, "Capture Device" ) ) )
		{	Destroy(); return oexERROR( hr, oexT( "Error adding capture filter to graph" ) ); 
		} // end if

		if ( m_cpCaptureGraphBuilder2.p )
		{
			// Set the crossbar
			CComPtr< IAMCrossbar > apCrossbar;
			if ( SUCCEEDED( hr = m_cpCaptureGraphBuilder2->FindInterface(	&LOOK_UPSTREAM_ONLY, NULL, m_cpCaptureSource,
																			IID_IAMCrossbar, (void**)&apCrossbar) ) && apCrossbar )
			{	apCrossbar->Route( 0, dwSource );
				apCrossbar.Detach();
			} // end if		   

		} // end if

		// Save width and height
		m_nWidth = nWidth;
		m_nHeight = nHeight;

		// Set the filter size
		SetFilterSize( m_cpCaptureSource, nWidth, nHeight );

		// Set filter frame rate
		SetFilterFrameRate( m_cpCaptureSource, GetFrameRate() );

		// Create generic renderer
		hr = S_OK;
		m_pVideoRenderer = new CDsRenderer( NULL, &hr );
		if ( FAILED( hr ) )
		{	Destroy(); return oexERROR( hr, oexT( "Error creating video renderer" ) );
		} // end if
		if ( m_pVideoRenderer == NULL ) 
		{	Destroy(); return oexERROR( ERROR_OUTOFMEMORY, oexT( "Out of memory creating capture filter" ) );
		} // end if

		// Set frame callback function
		if ( cbfOnFrame ) 
			m_pVideoRenderer->SetFrameCallback( cbfOnFrame, pUser );

		// Add custom filter
		if ( FAILED( hr = AddFilter( m_pVideoRenderer, "CVidRenderer" ) ) )
		{	Destroy(); return oexERROR( hr, oexT( "Error adding renderer to filter graph" ) );
		} // end if

		// Render video
		if ( FAILED( hr = RenderStream( NULL, m_cpCaptureSource, NULL, m_pVideoRenderer ) ) )
		{
			// Create a color space converter
			IBaseFilter *pColorSpaceConverter;
			if FAILED( hr = AddFilter( CLSID_Colour, "Color Converter", &pColorSpaceConverter ) )
			{	Destroy(); return oexERROR( hr, oexT( "Error Creating color space filter" ) ); 
			} // end if

			// Attempt to render through the converter
			if ( FAILED( hr = RenderStream( NULL, m_cpCaptureSource, pColorSpaceConverter, m_pVideoRenderer ) ) )
			{	Destroy(); return oexERROR( hr, oexT( "Error adding renderer capture graph" ) ); 
			} // end if

		} // end if

		return S_OK;
	}
	
	/// Capture from file
	/**
		\param [in] pFile		- Video file name
		\param [in[ cbfOnFrame	- Callback function that receives video frames

	*/
	HRESULT Capture( oexCSTR pFile, CCaptureTmpl::cbf_OnFrame cbfOnFrame = 0, oexPVOID pUser = 0 )
	{_STT();
		HRESULT hr = -1;

		// Lose old graph
		Destroy();

		if ( !oexCHECK_PTR( pFile ) )
			return oexERROR( ERROR_INVALID_PARAMETER, oexT( "Invalid parameter" ) );

		// Create capture graph
		if ( FAILED( hr = Create() ) )
			return hr;

		// Add source file
		CComPtr< IBaseFilter > cpBf;
		if ( S_OK != ( hr = AddFileSource( pFile, &cpBf ) ) )
			return oexERROR( hr, CStr().Fmt( oexT( "Error opening file %s" ), oexStrToMbPtr( pFile ) ) );

		// Render any audio
	//	CComPtr< IBaseFilter > cpSnd;
	//	AddSound( &cpSnd );
	//	hr = RenderStream( NULL, cpBf, NULL, cpSnd );
		
		// Create generic renderer
		hr = S_OK;
		m_pVideoRenderer = new CDsRenderer( NULL, &hr );
		if ( FAILED( hr ) )
		{	Destroy(); return oexERROR( hr, CStr().Fmt( oexT( "Error creating renderer for file %s" ), oexStrToMbPtr( pFile ) ) );
		} // end if
		if ( m_pVideoRenderer == NULL ) 
		{	Destroy(); return oexERROR( ERROR_OUTOFMEMORY, CStr().Fmt( oexT( "Out of memory rendering file %s" ), oexStrToMbPtr( pFile ) ) );
		} // end if

		// Set frame callback function
		if ( cbfOnFrame ) 
			m_pVideoRenderer->SetFrameCallback( cbfOnFrame, pUser );

		// Add custom filter
		if ( FAILED( hr = AddFilter( m_pVideoRenderer, "CDsRenderer" ) ) )
		{	Destroy(); return oexERROR( hr, CStr().Fmt( oexT( "Error adding renderer to filter graph; %s" ), oexStrToMbPtr( pFile ) ) ); 
		} // end if

		// Render video
		if ( FAILED( hr = RenderStream( NULL, cpBf, NULL, m_pVideoRenderer ) ) )
		{
			// Create a color space converter
			IBaseFilter *pColorSpaceConverter;
			if FAILED( hr = AddFilter( CLSID_Colour, "Color Converter", &pColorSpaceConverter ) )
			{	Destroy(); return oexERROR( hr, CStr().Fmt( oexT( "Error creating color space converter; %s" ), oexStrToMbPtr( pFile ) ) ); 
			} // end if

			// Attempt to render through the converter
			if ( FAILED( hr = RenderStream( NULL, cpBf, pColorSpaceConverter, m_pVideoRenderer ) ) )
			{	Destroy(); return oexERROR( hr, CStr().Fmt( oexT( "Error rendering file playback graph; %s" ), oexStrToMbPtr( pFile ) ) );
			} // end if

		} // end if

		return S_OK;
	}

	/// Set capture width and height
	void SetSize( oexINT x_nWidth, oexINT x_nHeight )
	{	m_nWidth = x_nWidth; m_nHeight = x_nHeight; }

	/// Get capture width
	oexINT GetWidth() { return m_nWidth; }

	/// Get capture height
	oexINT GetHeight() { return m_nHeight; }

	/// Set capture frame rate
	void SetFrameRate( double dFrameRate ) { m_dFrameRate = dFrameRate; }

	/// Get capture frame rate
	double GetFrameRate() { return m_dFrameRate; }

	/// Enumerates specified filters
	/**
		\param [out] lstFilter		-	List of available filters
		\param [in] clsidType		-	Type of filters to list

		\return Windows Error code or S_OK if success.
	*/
	HRESULT EnumFilters( TAssoList< CStr, CStr > *pList, const GUID &clsidType )
	{_STT();
		HRESULT hr = -1;

		if ( pList == NULL ) return ERROR_INVALID_PARAMETER;

		// Ensure the list is empty
		pList->Destroy();

		CComPtr< ICaptureGraphBuilder > cpCgb;
		CComPtr< ICreateDevEnum > cpCreateDevEnum;
		CComPtr< IEnumMoniker > cpEm;
		CComPtr< IMoniker > cpDev;

		try
		{
			// Create the graph builder
			if ( hr = cpCgb.CoCreateInstance( CLSID_CaptureGraphBuilder ) != S_OK )
				return hr;

			// Create dev enumerator
			if ( hr = cpCreateDevEnum.CoCreateInstance( CLSID_SystemDeviceEnum ) != S_OK )
				return hr;

			// What to enumberate
			hr = cpCreateDevEnum->CreateClassEnumerator( clsidType, &cpEm, 0 );
			if ( !SUCCEEDED( hr ) || cpEm == NULL ) return hr;

			// Start of list
			cpEm->Reset();

		} // end try

		catch( ... ) 
		{ 	ASSERT( 0 ); 
			oexERROR( ERROR_ARENA_TRASHED, "Exception occured while enumerating capture devices.  Check for misbehaving capture device." );
			return ERROR_ARENA_TRASHED; 
		} // end catch

		try
		{
			DWORD dwFetched = 0, i = 0;
			while ( SUCCEEDED( cpEm->Next( 1, &cpDev, &dwFetched ) ) && cpDev != NULL )
			{
				try
				{
					CComPtr< IBindCtx > cpBindCtx;
					if ( SUCCEEDED( hr = CreateBindCtx( 0, &cpBindCtx ) ) )
					{
						LPOLESTR pMonikerName = NULL;
						if ( SUCCEEDED( cpDev->GetDisplayName( cpBindCtx, cpDev, &pMonikerName ) ) && pMonikerName )
						{
							CComPtr< IPropertyBag > cpPBag;
							hr = cpDev->BindToStorage( 0, 0, IID_IPropertyBag, (void**)&cpPBag );
							if ( SUCCEEDED( hr ) && cpPBag != NULL )
							{
								// Get friendly name
								CComVariant var; var.vt = VT_BSTR;
								hr = cpPBag->Read( L"FriendlyName", &var, NULL );

								// Add it to the list
								(*pList)[ oexStdString( (_bstr_t)pMonikerName ).c_str() ] 
									= oexStdString( (_bstr_t)var.bstrVal ).c_str(); 

								// Free string
								CoTaskMemFree( pMonikerName );

							} // end if	

						} // end if
					
					} // end if

					cpDev.Release();

				} // end try

				catch( ... ) 
				{	
					oexERROR( ERROR_ARENA_TRASHED, "Exception occured while enumerating capture devices.  Check for misbehaving capture device." );
				} // end catch

			} // end while	

		} // end try

		catch( ... ) 
		{	ASSERT( 0 ); 
			oexERROR( ERROR_ARENA_TRASHED, "Exception occured while enumerating capture devices.  Check for misbehaving capture device." );
			return ERROR_ARENA_TRASHED; 
		} // end catch

		return ERROR_SUCCESS;
	}

	/// Display properties dialog box for specified IUnknown interface
	HRESULT PropertiesDialog(HWND hParent, IUnknown *pUnk, LPCTSTR pCaption)
	{_STT();
		HRESULT hr = -1;

		// Get property page interface
		CComPtr< ISpecifyPropertyPages > cpSpp;
		if ( FAILED( hr = pUnk->QueryInterface( IID_ISpecifyPropertyPages, (void**)&cpSpp ) ) )
			return hr;

		CAUUID uidPages;
		if ( FAILED( hr = cpSpp->GetPages( &uidPages ) ) )
			return hr;

		// Show the property dialog
		hr = OleCreatePropertyFrame( hParent,
									 10, 10, _bstr_t( pCaption ), 1,
									 &pUnk, uidPages.cElems, uidPages.pElems,
									 0, 0, 0 );

		// Free the page memory
		CoTaskMemFree( uidPages.pElems );

		return hr;
	}

	// *** Graph
	
	/// Creates the capture graph
	/**
		\return Windows error code or S_OK if success.

		\see Attach()
	*/
	HRESULT Create()
	{_STT();
		HRESULT hr = -1;

		// Initialize
		if ( S_OK != ( hr = Init() ) )
			return hr;

		CComPtr< IGraphBuilder > cpGb;
		if ( FAILED( hr = CoCreateInstance(	CLSID_FilterGraph, NULL, 
											CLSCTX_INPROC, IID_IGraphBuilder, 
											(void**)&cpGb ) ) ) 
			return oexERROR( hr, "Creating filter graph" );

		// Create the Capture Graph Builder.
		CComPtr< ICaptureGraphBuilder2 > cpCgb2;
		hr = CoCreateInstance(	CLSID_CaptureGraphBuilder2, NULL,
								CLSCTX_INPROC_SERVER, IID_ICaptureGraphBuilder2, 
								(void **)&cpCgb2 );

		if ( FAILED( hr ) )
			oexWARNING( hr, "Unable to create IID_ICaptureGraphBuilder2 interface" );

		else
			// Set the filter graph
			if ( FAILED( hr = cpCgb2->SetFiltergraph( cpGb ) ) )
				oexWARNING( hr, "SetFiltergraph( IID_ICaptureGraphBuilder2 ) failed" );

		// Attach to the object
		return Attach( cpGb.Detach(), cpCgb2.Detach() );
	}

	/// Attaches to an existing capture graph
	/**
		\param [in] pGraphBuilder			-	Pointer to an initialized IID_IGraphBuilder instance
		\param [in] pCaptureGraphBuilder2	-	Pointer to an initialized IID_ICaptureGraphBuilder2 instance

		\return Windows error code or S_OK if success.

		\see Create(), Detach()
	*/
	HRESULT Attach( IGraphBuilder *pGraphBuilder, ICaptureGraphBuilder2 *pCaptureGraphBuilder2 )
	{_STT();
		HRESULT hr = -1;

		// Destroy the current graph
		Destroy();

		// Ensure valid IGraphBuilder pointer
		if ( pGraphBuilder == NULL ) 
			oexERROR( ERROR_INVALID_PARAMETER, "Parameter pGraphBuilder is NULL" );

		// Attach to the graph builder interface
		m_cpGraphBuilder.Attach( pGraphBuilder );

		// Capture from the graph
		if ( pCaptureGraphBuilder2 ) 
			m_cpCaptureGraphBuilder2.Attach( pCaptureGraphBuilder2 );

		hr = m_cpGraphBuilder->QueryInterface( IID_IMediaControl, (void**)& m_cpMediaControl );
		if ( FAILED( hr ) ) 
			oexERROR( hr, "Query IID_IMediaControl" );

		hr = m_cpGraphBuilder->QueryInterface( IID_IFilterGraph2, (void**)&m_cpFilterGraph2 );
		if ( FAILED( hr ) ) 
			oexERROR( hr, "Query IID_IFilterGraph2" );

		hr = m_cpGraphBuilder->QueryInterface( IID_IMediaPosition, (void**)& m_cpMediaPosition );
		if ( FAILED( hr ) ) 
			oexERROR( hr, "Query IID_IMediaPosition" );

		hr = m_cpGraphBuilder->QueryInterface( IID_IMediaEvent, (void**)&m_cpMediaEvent );
		if ( FAILED( hr ) ) 
			oexERROR( hr, "Query IID_IMediaEvent" );

		hr = m_cpGraphBuilder->QueryInterface( IID_IMediaFilter, (void**)&m_cpMediaFilter );
		if ( FAILED( hr ) ) 
			oexERROR( hr, "Query IID_IMediaFilter" );

#if defined ( oexDEBUG )
		// Add to GraphEdit
		hr = AddToRot( m_cpGraphBuilder, &m_dwRotId );
#endif

		return S_OK;
	}

	/// Add GraphEdit Table Entry
	/**
		\param [in] pUnkGraph	-	An initialized CLSID_FilterGraph object
		\param [in] pdwRotId	-	Unique id to identify the graph for GraphEdit
		\param [in] pStr		-	Name for the graph

		\return Windows error code or S_OK if success.

		\see RemoveFromRot()
	*/
	static HRESULT AddToRot( IUnknown *pUnkGraph, LPDWORD pdwRotId, LPCTSTR pStr = NULL )
	{_STT();
		HRESULT hr = -1;

		// Sanity checks
		if ( pUnkGraph == NULL ) return ERROR_INVALID_PARAMETER;

		// Get ROT pointer
		CComPtr< IRunningObjectTable > cpRot;
		if ( FAILED( hr = GetRunningObjectTable( 0, &cpRot ) ) ) return hr;

		// Name for ROT
		WCHAR strw[ 256 ]; char str[ 256 ] = { 0 };
		if ( pStr != NULL ) { strncpy( str, pStr, sizeof( str ) - 16 ); strcat( str, " - " ); }
		wsprintfW( strw, L"%sFilterGraph 0x%x pid 0x%x", str, (DWORD_PTR)pUnkGraph, GetCurrentProcessId() );

		// Add to Graph edit
		CComPtr< IMoniker > cpMoniker;
		if ( SUCCEEDED( hr = CreateItemMoniker( L"!", strw, &cpMoniker ) ) )
			cpRot->Register( 0, pUnkGraph, cpMoniker, pdwRotId );

		return hr;

	}

	/// Remove GraphEdit Table Entry
	/**
		\param [in] dwRotId		-	ID passed to AddToRot

		\return Windows error code or S_OK if success.

		\see AddToRot()
	*/
	static HRESULT RemoveFromRot( DWORD dwRotId )
	{_STT();
		HRESULT hr = -1;

		// Ensure valid ROT ID
		if ( dwRotId == MAXDWORD ) return ERROR_INVALID_DATA;

		// Get running object pointer
		CComPtr< IRunningObjectTable > cpRot;
		if ( FAILED( hr = GetRunningObjectTable( 0, &cpRot ) ) ) return hr;
		if ( cpRot == NULL ) return ERROR_INVALID_DATA;

		// Remove from graph edit
		cpRot->Revoke( dwRotId );

		return ERROR_SUCCESS;
	}

	/// Creates a capture device source filter
	/**
		\param [in] dwDevice	-	The zero based index of the capture device to create.
		\param [in] ppSrcFilter	-	Receives a pointer to the IBaseFilter interface of the
									newly created capture filter.  Must not be NULL.  An 
									outstanding reference on ppSrcFilter must be released.

		\return Windows Error code or S_OK if success.
	*/
	HRESULT CreateCaptureFilter( DWORD dwDevice, IBaseFilter **ppSrcFilter )
	{_STT();
		HRESULT hr = -1;

		try
		{
			// Sanity check
			if ( !ppSrcFilter) 
				return oexERROR( ERROR_INVALID_PARAMETER, "Invalid parameter" );

			*ppSrcFilter = NULL;
	  
			// Create the system device enumerator
			CComPtr< ICreateDevEnum > cpDevEnum;
			if ( FAILED( hr = CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
												IID_ICreateDevEnum, (void **) &cpDevEnum ) ) )
				return oexERROR( hr, "CoCreateInstance( CLSID_SystemDeviceEnum ) failed" );

			// Create an enumerator for the video capture devices
			CComPtr < IEnumMoniker > cpClassEnum;

			if ( FAILED( hr = cpDevEnum->CreateClassEnumerator( CLSID_VideoInputDeviceCategory, 
															   &cpClassEnum, 0 ) ) )
				return oexERROR( hr, "CreateClassEnumerator( CLSID_VideoInputDeviceCategory ) failed" );

			// If there are no enumerators for the requested type, then 
			// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
			if ( cpClassEnum == NULL ) 
				return oexERROR( -1, "CreateClassEnumerator( CLSID_VideoInputDeviceCategory ) returned NULL pointer" );

			// Start with the first device
			cpClassEnum->Reset();

			// Use the first video capture device on the device list.
			// Note that if the Next() call succeeds but there are no monikers,
			// it will return S_FALSE (which is not a failure).  Therefore, we
			// check that the return code is S_OK instead of using SUCCEEDED() macro.
			ULONG cFetched = 0;
			CComPtr< IMoniker > cpMoniker;	

			// Skip to the device we want
			if ( dwDevice ) 
				if ( FAILED( hr = cpClassEnum->Skip( dwDevice ) ) )
				return oexERROR( hr, CStr().Fmt( oexT( "IEnumMoniker::Skip( %d ) failed" ), (int)dwDevice ) );

			// Get the next device
			if ( FAILED( hr = cpClassEnum->Next( 1, &cpMoniker, &cFetched ) ) )
				return oexERROR( hr, "IEnumMoniker::Next() failed" );

			if ( cpMoniker == NULL ) 
				return oexERROR( hr, "cpMoniker is NULL" );

			// Bind Moniker to a filter object
			CComPtr< IBaseFilter > cpSrc;
			if ( FAILED( hr = cpMoniker->BindToObject( 0, 0, IID_IBaseFilter, (void**)&cpSrc ) ) )
				return oexERROR( hr, "IMoniker::BindToObject() failed" );

			// Save the pointer
			*ppSrcFilter = cpSrc.Detach();

			return S_OK;

		} catch( ... ) {}

		// Log the exception
		oexERROR( ERROR_ARENA_TRASHED, "Exception occured while enumerating capture devices.  Check for misbehaving capture device." );

		return E_FAIL;
	}

	/// Creates the specified filter from the GUID and adds it to the graph.
	/**
		\param [in] clsid			-	CLSID of the filter to add to the graph
		\param [out] ppSrcFilter	-	Pointer to the newly created IBaseFilter object

		\return		Windows error code or S_OK if success.
	*/
	HRESULT AddFilter(const GUID &clsid, LPCTSTR pName, IBaseFilter **ppSrcFilter = NULL )
	{_STT();
		HRESULT hr = -1;
		
		// Create the filter
		CComPtr< IBaseFilter > cpBf;
		if ( FAILED( hr = cpBf.CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER ) ) )
			return oexERROR( hr, "Failed to create filter" );

		// Add the filter to the graph
		if ( FAILED( hr = AddFilter( cpBf, pName ) ) )
			return hr;

		// Give up the pointer
		if ( ppSrcFilter != NULL ) *ppSrcFilter = cpBf.Detach();
		
		return S_OK;
	}

	///	Adds the specified filter to the graph
	/**
		\param [in] pFilter		-	Filter to add to the graph.
		\param [in] pName		-	Name for the graph.

		\return		Windows error code or S_OK if success.

		\warning 	The graph should not be running when calling this function.
					Call Stop(), then Start() when done.

		\see Stop(), Start(), IsRunning()
	*/
	HRESULT AddFilter( IBaseFilter *pFilter, LPCTSTR pName )
	{_STT();
		HRESULT hr = -1;

		// Ensure graph
		if ( !IsGraph() ) 
			return oexERROR( ERROR_BAD_ENVIRONMENT, "Graph Not Initialized" );

		// Add filter
		if ( pName == NULL ) pName = "";
		if ( FAILED( hr = m_cpGraphBuilder->AddFilter( pFilter, (CComBSTR)pName ) ) )
			return oexERROR( hr, CStr().Fmt( oexT( "Adding Filter to Graph : %s" ), pName ) );

		return S_OK;
	}

	/// Sets the size on the specified filter
	/**
		\param [in] pBaseFilter		-	Pointer to the IBaseFilter object for which the size will be set.
		\param [in] lWidth			-	New width for video frames.
		\param [in] lHeight			-	New height for video frames.

		\return Windows Error code or S_OK if success.
	*/
	HRESULT SetFilterSize(IBaseFilter *pBaseFilter, long lWidth, long lHeight)
	{_STT();
		HRESULT hr = -1;

		// Get stream config interface
		CComPtr< IAMStreamConfig > apIAMStreamConfig;
		if ( FAILED( hr = GetIAMStreamConfig( pBaseFilter, &apIAMStreamConfig ) ) )
			return hr;
		
		AM_MEDIA_TYPE *pmt;
		if ( FAILED( hr = apIAMStreamConfig->GetFormat( &pmt ) ) )
			return hr;

		// Modify the header to reflect the new size
		VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER*)pmt->pbFormat;
		pvi->bmiHeader.biWidth = lWidth;
		pvi->bmiHeader.biHeight = lHeight;
		pvi->bmiHeader.biSizeImage = DIBSIZE( pvi->bmiHeader );

		// Set the format 
		hr = apIAMStreamConfig->SetFormat( pmt );

		// Lose memory
		DeleteMediaType( pmt );

		return hr;
	}
	
	/// Sets the frame rate on the specified filter
	/**
		\param [in] pBaseFilter		-	Pointer to the IBaseFilter object for which the size will be set.
		\param [in] dRate			-	Frame rate.

		\return Windows Error code or S_OK if success.
	*/
	HRESULT SetFilterFrameRate( IBaseFilter *pBaseFilter, double dRate)
	{_STT();
		HRESULT hr = -1;

		// Get stream config interface
		CComPtr< IAMStreamConfig > apIAMStreamConfig;
		if ( FAILED( hr = GetIAMStreamConfig( pBaseFilter, &apIAMStreamConfig ) ) )
			return hr;
		
		AM_MEDIA_TYPE *pmt;
		if ( FAILED( hr = apIAMStreamConfig->GetFormat( &pmt ) ) )
			return hr;

		VIDEOINFOHEADER *pvi = (VIDEOINFOHEADER*)pmt->pbFormat;
		pvi->AvgTimePerFrame = c_rtTimeBase / dRate;

		// Set the format 
		hr = apIAMStreamConfig->SetFormat( pmt );

		// Lose memory
		DeleteMediaType( pmt );

		return hr;
	}

	/// Returns the IAMStreamConfig interface for the specified filter
	/**
		\param [in] pBaseFilter			-	Pointer to the IBaseFilter object for which the IID_IAMStreamConfig
											interface is to be retrieved.  Must not be NULL.
		\param [in] pIAMStreamConfig	-	Receives a pointer to the IID_IAMStreamConfig interface.
											Must not be NULL.

		\return Windows Error code or S_OK if success.
	*/
	HRESULT GetIAMStreamConfig(IBaseFilter *pBaseFilter, IAMStreamConfig **pIAMStreamConfig)
	{_STT();
		HRESULT hr = -1;
		
		if ( !m_cpCaptureGraphBuilder2.p )
			return E_NOTIMPL;

		if ( FAILED( hr = m_cpCaptureGraphBuilder2->FindInterface(	NULL, NULL, pBaseFilter,
																	IID_IAMStreamConfig,
																	(void**)pIAMStreamConfig ) ) )
			return hr;

		return S_OK;
	}

	/// Connects the specified filters
	/**
		\param [in] pSrc	-	The filter whose output pin will be connected.
								This filter must have an output pin.
		\param [in] pMid	-	This filter will be connected between pSrc and pDst.
								This filter must support both input and output connections.
		\param [in] pDst	-	The filter whose input pin will be connected.
								This filter must have an input pin.

		\return		Windows error code or S_OK if success.
	*/
	HRESULT RenderStream( const GUID *pType, IBaseFilter *pSrc, IBaseFilter *pMid, IBaseFilter *pDst )
	{_STT();
		if ( !m_cpCaptureGraphBuilder2.p )
			return E_NOTIMPL;

		ICaptureGraphBuilder2 *pCgb2 = m_cpCaptureGraphBuilder2;
		return m_cpCaptureGraphBuilder2->RenderStream( NULL, pType, pSrc, pMid, pDst );
	}

	/// Loops a file source
	/**
		\return Windows Error code or S_OK if success.
	*/
	HRESULT LoopStream()
	{_STT();
		if ( m_cpMediaEvent == NULL || m_cpMediaPosition == NULL ) 
			return ERROR_INVALID_DATA;

		long lEventCode;
		long lParam1;
		long lParam2;
		HRESULT hr;
	       
		// Check for completion events
		if ( FAILED( hr = m_cpMediaEvent->GetEvent(&lEventCode, (LONG_PTR *) &lParam1, (LONG_PTR *) &lParam2, 0) ) )
			return hr;

		// If we have reached the end of the media file, reset to beginning
		if (EC_COMPLETE == lEventCode) 
		{
			hr = m_cpMediaPosition->put_CurrentPosition( 0 );
		}

		// Free any memory associated with this event
		hr = m_cpMediaEvent->FreeEventParams(lEventCode, lParam1, lParam2);

		return S_OK;
	}

	/// Returns settings for the specified filter
	HRESULT GetFilterSettings(IBaseFilter *pBf, TMem< BYTE > *pMem )
	{_STT();
		HRESULT hr = -1;

		// Get the interface
		CComPtr< IAMVfwCompressDialogs > cpCompressDialogs;
		if ( FAILED( pBf->QueryInterface( IID_IAMVfwCompressDialogs, (void**)&cpCompressDialogs ) ) )
			return oexERROR( hr, oexT( "Query IID_IAMVfwCompressDialogs" ) );

		// Ensure valid pointer
		if( cpCompressDialogs == NULL ) 
			return oexERROR( ERROR_INVALID_DATA, oexT( "Query IID_IAMVfwCompressDialogs" ) );

		// Get the size of the buffer
		DWORD dwSize = cpCompressDialogs->SendDriverMessage( ICM_GETSTATE, NULL, 0 ); 
		if ( !dwSize ) 
			return oexERROR( ERROR_INVALID_DATA, oexT( "ICM_GETSTATE returned 0 size" ) );

		// Allocate buffer for settings
		if ( !pMem->OexNew( dwSize ).Ptr() ) 
			return oexERROR( ERROR_OUTOFMEMORY, oexT( "Allocating filter settings buffer" ) );

		// Get the filter settings
		if ( FAILED( hr = cpCompressDialogs->SendDriverMessage( ICM_GETSTATE, (long)pMem->Ptr(), dwSize ) ) )
			return oexERROR( hr, oexT( "Error retrieving filter settings" ) );

		return S_OK;
	}

	/// Applies settings to specified filter
	/**
		ICCOMPRESSFRAMES icf;
		SendDriverMessage( ICM_COMPRESS_FRAMES_INFO, (WPARAM)&icf, sizeof( icf ) ); 

		ICCOMPRESSFRAMES icf;
		cpCompressDialogs->SendDriverMessage( ICM_COMPRESS_FRAMES_INFO, (WPARAM)&icf, sizeof( icf ) ); 

		MPG4 hack (found on internet, have no idea if it works)
			*((double *)&state[8]) = 500.0 + Quality*(6000.0-500.0);
			state[20] = (unsigned char)KeyRate;
			state[16] = (unsigned char)(Quality*100);

	*/
	HRESULT SetFilterSettings( IBaseFilter *pBf, LPBYTE pBuf, DWORD dwSize )
	{_STT();
		HRESULT hr = -1;

		if ( !pBf ) 
			return oexERROR( ERROR_INVALID_PARAMETER, oexT( "pBf is NULL" ) );

		if ( !pBuf ) 
			return oexERROR( ERROR_INVALID_PARAMETER, oexT( "pBuf is NULL" ) );

		// Get the interface
		CComPtr< IAMVfwCompressDialogs > cpCompressDialogs;
		if ( FAILED( pBf->QueryInterface( IID_IAMVfwCompressDialogs, (void**)&cpCompressDialogs ) ) )
			return oexERROR( hr, oexT( "Query IID_IAMVfwCompressDialogs" ) );

		// Ensure valid pointer
		if( cpCompressDialogs == NULL ) 
			return oexERROR( ERROR_INVALID_DATA, oexT( "Query IID_IAMVfwCompressDialogs" ) );

		// Apply settings
		if ( FAILED( hr = cpCompressDialogs->SendDriverMessage( ICM_SETSTATE, (long)pBuf, dwSize ) ) ) 
			return oexERROR( hr, oexT( "Error applying filter settings" ) );

		return S_OK;
	}

	/// Add a source file to the graph
	/**
		\param [in] pFile			-	Audio/Video file to add to the graph
		\param [out] ppSrcFilter	-	Optional pointer to IBaseFilter object for file.
										Can be NULL

		\return Windows error code or S_OK if success.
	*/
	HRESULT AddFileSource(LPCTSTR pFile, IBaseFilter **ppSrcFilter)
	{_STT();
		HRESULT hr = -1;

		// Ensure file name is valid
		DWORD dwAttrib = ::GetFileAttributes( pFile );
		if ( INVALID_FILE_ATTRIBUTES == dwAttrib )
			return oexERROR( ERROR_INVALID_PARAMETER, CStr().Fmt( oexT( "GetFileAttributes() failed, no such file, %s" ), pFile ) );

		// Make sure it's not a directory
		if ( FILE_ATTRIBUTE_DIRECTORY & dwAttrib )
			return oexERROR( ERROR_INVALID_PARAMETER, CStr().Fmt( oexT( "Path is a directory, %s" ), pFile ) );

		// Filter name
		oexStdString sFilterName( "A/V File" );

		// Create source filter
		CComPtr< IBaseFilter > cpBaseFilter;
		if ( FAILED( hr = m_cpGraphBuilder->AddSourceFilter( (CComBSTR)pFile, 
															 (CComBSTR)sFilterName.c_str(), 
															 &cpBaseFilter ) ) )
			return oexERROR( hr, CStr().Fmt( oexT( "IGraphBuilder::AddSourceFilter() failed, %s" ), pFile ) );

		// Save pointer if needed
		if ( ppSrcFilter != NULL ) 
			*ppSrcFilter = cpBaseFilter.Detach();
		
		// Return the filter pointer
		return S_OK;
	}

	/// Adds the specified sound filter to the graph
	/**
		\param [out] ppBf 	-	Pointer to the newly created IBaseFilter 
								sound object.

		\return Windows error code or S_OK if success.
	*/
	HRESULT AddSound( IBaseFilter **ppBf )
	{_STT();
		HRESULT hr = -1;
		
		CComPtr< IBaseFilter > cpBf;

		hr = CreateSound( &cpBf );
		if ( FAILED( hr ) ) return hr;

		AddFilter( cpBf, oexT( "Direct Sound" ) );
		
		if ( ppBf != NULL ) *ppBf = cpBf.Detach();
		
		return S_OK;
	}

	/// Creates a sound filter in the graph
	/**
		\param [out] ppSrcFilter 	-	Pointer to the newly created IBaseFilter 
										sound object.

		\return Windows error code or S_OK if success.
	*/
	HRESULT CreateSound(IBaseFilter **ppSrcFilter)
	{_STT();
		HRESULT hr = CreateFilter( CLSID_DSoundRender, ppSrcFilter );
		
		if ( FAILED( hr ) ) 
			return oexERROR( hr, oexT( "Creating CLSID_DSoundRender" ) );

		return S_OK;
	}

	/// Creates the specified filter
	/**
		\param [in] clsid			-	CLSID of the filter to create
		\param [out] ppSrcFilter	-	Pointer to the newly created IBaseFilter object

		\return		Windows error code or S_OK if success.
	*/
	HRESULT CreateFilter(const GUID &clsid, IBaseFilter **ppSrcFilter)
	{_STT();
		HRESULT hr = -1;

		// Create the filter
		CComPtr< IBaseFilter > cpBf;
		if ( FAILED( hr = cpBf.CoCreateInstance( clsid, NULL, CLSCTX_INPROC_SERVER ) ) )
			return oexERROR( hr, oexT( "Creating filter" ) );

		// Give up the pointer
		*ppSrcFilter = cpBf.Detach();
		
		return S_OK;
	}

	/// Seeks to a particular time in the file
	/**
		\param [in] dwTime		-	Offset in seconds from the beginning of the stream
										to set the file pointer.  This interface is only
										accurate to one second.

		\return Windows Error code or S_OK if success.
	*/
	HRESULT Seek( DWORD dwTime, DWORD dwMilliseconds = 0 )
	{_STT();
		HRESULT hr = -1;

		if ( m_cpMediaPosition == NULL ) ERROR_INVALID_DATA;

		// Verify valid timestamp
		if ( dwTime < m_dwBaseTime ) return ERROR_INVALID_PARAMETER;

		if ( dwMilliseconds > 1000 ) dwMilliseconds = 0;

		// Calculate seek time
		LONGLONG llSeekTime = ( dwTime - m_dwBaseTime );// * TIME_BASE;
		//	llSeekTime = (LONGLONG)dwMilliseconds * ( TIME_BASE / (LONGLONG)1000 );

		// Set position
		if ( FAILED( hr = m_cpMediaPosition->put_CurrentPosition( llSeekTime ) ) )
			return oexERROR( hr, CStr().Fmt( oexT( "IMediaPosition::put_CurrentPosition( %d ) failed" ), (int)llSeekTime ) );

		return S_OK;
	}

	/// Returns non-zero if the IGraphBuilder pointer is not NULL
	BOOL IsGraph() { return ( m_cpGraphBuilder != NULL ); }

	/// Starts the graph
	HRESULT Start()
	{_STT();
		m_bStop = FALSE;
		if ( m_cpMediaControl == NULL ) 
			return ERROR_INVALID_DATA;
		HRESULT hr = m_cpMediaControl->Run();
		if ( FAILED( hr ) )
			return oexERROR( hr, oexT( "IMediaControl::Run() failed" ) );
		return S_OK;
	}

	/// Stops the graph
	HRESULT Stop()
	{_STT();
		m_bStop = TRUE;
		if ( m_cpMediaControl == NULL ) 
			return ERROR_INVALID_DATA;
		HRESULT hr = m_cpMediaControl->Stop();
		if ( FAILED( hr ) )
			return oexERROR( hr, oexT( "IMediaControl::Stop() failed" ) );
		return S_OK;
	}

	/// Returns non-zero if the graph is running
	BOOL IsRunning()
	{_STT();
		if ( m_bStop ) return FALSE;
		if ( !IsGraph() ) return FALSE;
		if ( m_cpMediaControl == NULL ) return FALSE;

		// Check the running state of the graph
		OAFilterState filterState;
		m_cpMediaControl->GetState( 500, &filterState );
		return ( filterState != State_Stopped );
	}

	/// Returns a pointer to the renderer
	CDsRenderer* GetVideoRenderer() { return m_pVideoRenderer; }

private:

	/// Result returned from CoInitialized()
	HRESULT								m_hrInitialized;

	/// Capture width
	oexINT								m_nWidth;

	/// Capture height
	oexINT								m_nHeight;

	/// Capture frame rate
	double								m_dFrameRate;

	/// Set to non-zero if graph should stop
	BOOL								m_bStop;

	/// Capture graph source
	CComPtr< IBaseFilter >				m_cpCaptureSource;

	/// Render video stream
	CDsRenderer							*m_pVideoRenderer;

	// *** Graph

	// ROT id for graph edit
#if defined( oexDEBUG )
	DWORD								m_dwRotId;
#endif

	// File base time
	DWORD								m_dwBaseTime;

	// Automatic interface pointers
	CComPtr< IGraphBuilder >			m_cpGraphBuilder;
	CComPtr< ICaptureGraphBuilder2 >	m_cpCaptureGraphBuilder2;
	CComPtr< IMediaPosition >			m_cpMediaPosition;
	CComPtr< IMediaControl >			m_cpMediaControl;
	CComPtr< IMediaFilter >				m_cpMediaFilter;
	CComPtr< IMediaEvent >				m_cpMediaEvent;
	CComPtr< IFilterGraph2 >			m_cpFilterGraph2;

};

const REFERENCE_TIME CDsCapture::c_rtTimeBase = (REFERENCE_TIME)10000000;

class CV4w2 : public CCaptureTmpl
{
public:


public:

	/// Default contructor
	CV4w2()
	{_STT();
		m_pFi = oexNULL;
		m_nWidth = 0;
		m_nHeight = 0;
		m_uFormat = 0;
		m_fFps = 0;
	}

	/// Destructor
	~CV4w2()
	{_STT();
		Destroy();
	}

	/// Static callback
	static oexRESULT _OnFrame( CCaptureTmpl::SFrameInfo *pFi, LPVOID pUser )
	{_STT();
		CV4w2 *pThis = (CV4w2*)pUser;
		if ( !pThis ) return ERROR_INVALID_PARAMETER;
		return pThis->OnFrame( pFi, pUser );
	}

	/// Handles video frame callbacks
	oexRESULT OnFrame( CCaptureTmpl::SFrameInfo *pFi, LPVOID pUser )
	{_STT();
/*
		// Wait on user to use old buffer
		m_sigReady.GetResetEvent().Wait();

		// Copy stuff
		m_sfi.lSize = pFi->lSize;
		m_sfi.lImageSize = pFi->lImageSize;
		m_sfi.llFrame = pFi->llFrame;
		m_sfi.lWidth = pFi->lWidth;
		m_sfi.lHeight = pFi->lHeight;
		m_sfi.lScanWidth = pFi->lScanWidth;
		m_img.MemCpy( (CBin::t_byte*)pFi->pBuf, pFi->lImageSize );
		m_sfi.pBuf = (oexPVOID)m_img.Ptr();
		m_pFi = &m_sfi;

		// New frame is ready
		m_sigReady.Signal();
*/

		m_pFi = pFi;
		m_sigReady.Signal();
		m_sigReady.GetResetEvent().Wait();
		m_pFi = oexNULL;

		return 0;
	}

	//==============================================================
	// Open()
	//==============================================================
	/**
		\param [in] x_uDevice	-	Device index
	*/
	virtual oexBOOL Open( oexUINT x_uType, oexUINT x_uDevice, oexUINT x_uSource, oexINT x_nWidth, oexINT x_nHeight, oexUINT x_uFormat, oexFLOAT x_fFps, oexBOOL x_bInit )
	{_STT();
		Destroy();

		// If width and height are zero, just wanting a query object
		if ( !x_nWidth && !x_nHeight )
			return oexTRUE;

		// Set the frame rate
		m_cap.SetFrameRate( x_fFps );

		HRESULT hr = m_cap.Capture( x_uDevice, x_uSource, x_nWidth, x_nHeight, CV4w2::_OnFrame, this );
		if ( S_OK != hr )
		{	oexERROR( hr, "Failed to open capture device" );
			return oexFALSE;
		} // end if

		// Save information
		m_nWidth = x_nWidth;
		m_nHeight = x_nHeight;
		m_uFormat = x_uFormat;
		m_fFps = x_fFps;

		return oexTRUE;
	}

	//==============================================================
	// Open()
	//==============================================================
	/**
		\param [in] x_pFile		- Video file to open
	*/
	virtual oexBOOL Open( oexUINT x_uType, oexCSTR x_pFile, oexINT x_nWidth, oexINT x_nHeight, oexUINT x_uFormat, oexFLOAT x_fFps, oexBOOL x_bInit )
	{_STT();
		if ( !oexCHECK_PTR( x_pFile ) )
		{	oexERROR( ERROR_INVALID_PARAMETER, "Invalid pointer" );
			return oexFALSE;
		} // end if

		Destroy();

		HRESULT hr = m_cap.Capture( x_pFile, CV4w2::_OnFrame, this );
		if ( S_OK != hr )
		{	oexERROR( hr, "Failed to open capture device" );
			return oexFALSE;
		} // end if

		// Save information
		m_nWidth = x_nWidth;
		m_nHeight = x_nHeight;
		m_uFormat = x_uFormat;
		m_fFps = x_fFps;

		return oexTRUE;
	}

	//==============================================================
	// Destroy()
	//==============================================================
	/// Closes any open capture device
	virtual oexBOOL Destroy()
	{_STT();
		// Close the capture device
		m_cap.Destroy();

		m_pFi = oexNULL;
		m_nWidth = 0;
		m_nHeight = 0;
		m_uFormat = 0;
		m_fFps = 0;

		return oexTRUE;
	}

public:

	//==============================================================
	// IsOpen()
	//==============================================================
	/// Returns non-zero if a capture device is open
	virtual oexBOOL IsOpen()
	{_STT();
		return m_cap.IsRunning();
	}

	//==============================================================
	// Init()
	//==============================================================
	/// Initializes capture system
	/**
	*/
	virtual oexBOOL Init()
	{_STT();
		return S_OK == m_cap.Init();
	}

	//==============================================================
	// GetSupportedFormats()
	//==============================================================
	virtual CStr GetSupportedFormats()
	{_STT();
		return oexT( "RGB3,YUYV" );
	}

	//==============================================================
	// GetFormatDescription()
	//==============================================================
	virtual CStr GetFormatDescription( oexUINT x_uFormat )
	{_STT();
		switch( x_uFormat )
		{
			case oexFOURCC( 'Y', 'U', 'Y', 'V' ) :
				return "YUYV - Y, U (Cb) and V (Cr)";

			case oexFOURCC( 'R', 'G', 'B', '3' ) :
				return "RGB3 - 24-bit / Red(8), Green(8), Blue(8)";

			default:
				break;
		};

		return CStr();
	}

	//==============================================================
	// StartCapture()
	//==============================================================
	virtual oexBOOL StartCapture()
	{_STT();
		return S_OK == m_cap.Start();
	}

	//==============================================================
	// StopCapture()
	//==============================================================
	/// Stops video capture
	virtual oexBOOL StopCapture()
	{_STT();
		return S_OK == m_cap.Stop();
	}

	//==============================================================
	// WaitForFrame()
	//==============================================================
	/// Waits for a new frame of video
	oexBOOL WaitForFrame( oexUINT x_uTimeout = 0 )
	{_STT();
		return ( CResource::waitSuccess == m_sigReady.Wait( x_uTimeout ) );
	}

	//==============================================================
	// ReleaseFrame()
	//==============================================================
	virtual oexBOOL ReleaseFrame()
	{_STT();
		m_sigReady.Reset();
		return oexTRUE;
	}

	//==============================================================
	// GetBuffer()
	//==============================================================
	/// Returns a pointer to the video buffer
	virtual oexPVOID GetBuffer()
	{_STT();
		if ( !m_pFi )
			return oexNULL;
		return m_pFi->pBuf;
	}

	//==============================================================
	// GetImageSize()
	//==============================================================
	/// +++ Should return the size of the video buffer
	virtual oexINT GetImageSize()
	{_STT();
		return CImage::GetScanWidth( m_nWidth, 24 ) * cmn::Abs( m_nHeight ); 
	}

	//==============================================================
	// GetBufferSize()
	//==============================================================
	oexINT GetBufferSize()
	{_STT();
		if ( !m_pFi )
			return 0;
		return m_pFi->lImageSize;
	}

	//==============================================================
	// GetImage()
	//==============================================================
	/// Returns an image object
	virtual CImage* GetImage()
	{_STT();
		return oexNULL; 
	}

	//==============================================================
	// GetWidth()
	//==============================================================
	/// Returns the image width
	virtual oexINT GetWidth()
	{	return m_nWidth; }

	//==============================================================
	// GetHeight()
	//==============================================================
	/// Returns the image height
	virtual oexINT GetHeight()
	{	return m_nHeight; }

	//==============================================================
	// GetFormat()
	//==============================================================
	/// Returns the image format
	virtual oexUINT GetFormat()
	{	return m_uFormat; }

	//==============================================================
	// GetFps()
	//==============================================================
	/// Returns the frame rate in frames per second
	virtual oexFLOAT GetFps()
	{	return m_fFps; }

	//==============================================================
	// GetFrame()
	//==============================================================
	/// Returns the current frame index
	virtual oexINT64 GetFrame()
	{	return m_llFrame; }

private:

	/// Name of the device
	CStr							m_sDeviceName;

	/// The current frame number
	oexINT64						m_llFrame;

	/// Image width
	oexINT							m_nWidth;

	/// Image height
	oexINT							m_nHeight;

	/// Image format
	oexUINT							m_uFormat;

	/// Frames per second
	oexFLOAT						m_fFps;

	/// The Direct Show capture class
	CDsCapture						m_cap;

	/// Holds image frame info during callback
	CCaptureTmpl::SFrameInfo		*m_pFi;

	/// Callback pointer to copy of frame pointer
	CCaptureTmpl::SFrameInfo		m_sfi;

	/// Callback copy of image buffer
	CBin							m_img;		

	/// Frame ready signal
	CSignal							m_sigReady;
	
};


