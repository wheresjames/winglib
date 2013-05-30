
#pragma once

//------------------------------------------------------------------
// COM Error codes
//------------------------------------------------------------------

#define _OEXCOM_S_OK				0x00000000L
#define _OEXCOM_S_FALSE				0x00000001L
#define _OEXCOM_E_NOTIMPL			0x80004001L
#define _OEXCOM_E_NOINTERFACE		0x80004002L
#define _OEXCOM_E_POINTER			0x80004003L
#define _OEXCOM_E_ABORT				0x80004004L
#define _OEXCOM_E_FAIL				0x80004005L
#define _OEXCOM_E_NOAGGREGATION		0x80040110L
#define _OEXCOM_E_HANDLE			0x80070006L
#define _OEXCOM_E_OUTOFMEMORY		0x8007000EL
#define _OEXCOM_E_INVALIDARG		0x80070057L
#define _OEXCOM_E_UNEXPECTED		0x8000FFFFL


//------------------------------------------------------------------
// COM types
//------------------------------------------------------------------

#if !defined ( OEX_CPU_64 )
#	define _OEXCOM_STDCALL				__stdcall
#else
#	define _OEXCOM_STDCALL
#endif
#define _OEXCOM_STDAPI				extern "C" _OEXCOM_HRESULT _OEXCOM_STDCALL

// Types
#define _OEXCOM_HRESULT				oex::oexULONG
#define _OEXCOM_IID					oex::oexGUID
#define _OEXCOM_REFIID				oex::oexGUID*
#define _OEXCOM_CLSID				oex::oexGUID
#define _OEXCOM_ULONG				oex::oexULONG
#define _OEXCOM_LONG				oex::oexLONG
#define _OEXCOM_ULONGLONG			oex::oexUINT64
#define _OEXCOM_LONGLONG			oex::oexINT64
#define _OEXCOM_WPARAM				oex::oexLONG
#define _OEXCOM_LPARAM				oex::oexLONG
#define _OEXCOM_UINT				oex::oexUINT
#define _OEXCOM_INT					oex::oexINT
#define _OEXCOM_WORD				oex::oexSHORT
#define _OEXCOM_DWORD				oex::oexUINT
#define _OEXCOM_BOOL				oex::oexINT
#define _OEXCOM_BYTE				oex::oexUCHAR
#define _OEXCOM_TRUE				1
#define _OEXCOM_FALSE				0
#define _OEXCOM_OLECHAR				oex::oexCHARW
#define _OEXCOM_LPOLESTR			_OEXCOM_OLECHAR*
#define _OEXCOM_LPCOLESTR			const _OEXCOM_OLECHAR*
#define _OEXCOM_CLIPFORMAT			_OEXCOM_WORD
#define _OEXCOM_LPRECT				_OEXCOM_RECT*
#define _OEXCOM_LPCRECT				const _OEXCOM_RECT*
#define _OEXCOM_LPOLEVERB			_OEXCOM_OLEVERB*

// Handles
#define _OEXCOM_MAKE_HANDLE( n )	struct n##__ { int n; }; typedef struct n##__ *n
_OEXCOM_MAKE_HANDLE( _OEXCOM_HANDLE );
_OEXCOM_MAKE_HANDLE( _OEXCOM_HWND );
_OEXCOM_MAKE_HANDLE( _OEXCOM_HBITMAP );
_OEXCOM_MAKE_HANDLE( _OEXCOM_HMETAFILEPICT );
_OEXCOM_MAKE_HANDLE( _OEXCOM_HENHMETAFILE );
_OEXCOM_MAKE_HANDLE( _OEXCOM_HGLOBAL );

// Class declarations
class _OEXCOM_IAdviseSink;
class _OEXCOM_IMoniker;
class _OEXCOM_IRunningObjectTable;
class _OEXCOM_IStorage;
class _OEXCOM_IStream;
class _OEXCOM_IUnknown;

// Structs
struct _OEXCOM_SIZEL { _OEXCOM_INT cx, cy; };
struct _OEXCOM_PALETTEENTRY { _OEXCOM_BYTE peRed; _OEXCOM_BYTE peGreen; _OEXCOM_BYTE peBlue; _OEXCOM_BYTE peFlags; };
struct _OEXCOM_LOGPALETTE { _OEXCOM_WORD palVersion; _OEXCOM_WORD palNumEntries; _OEXCOM_PALETTEENTRY palPalEntry[1]; };
struct _OEXCOM_POINT { _OEXCOM_LONG x; _OEXCOM_LONG y; };
struct _OEXCOM_FILETIME { _OEXCOM_DWORD dwLowDateTime; _OEXCOM_DWORD dwHighDateTime; };
struct _OEXCOM_MSG { _OEXCOM_HWND hwnd; _OEXCOM_UINT message; _OEXCOM_WPARAM wParam; _OEXCOM_LPARAM lParam; _OEXCOM_DWORD time; _OEXCOM_POINT pt; };
typedef _OEXCOM_MSG* _OEXCOM_LPMSG;
union _OEXCOM_LARGE_INTEGER
{ 	struct { _OEXCOM_DWORD LowPart; _OEXCOM_LONG HighPart; };
	struct { _OEXCOM_DWORD LowPart; _OEXCOM_LONG HighPart; } u;
	_OEXCOM_LONGLONG QuadPart;
};
union _OEXCOM_ULARGE_INTEGER
{	struct { _OEXCOM_DWORD LowPart; _OEXCOM_DWORD HighPart; };
	struct { _OEXCOM_DWORD LowPart; _OEXCOM_DWORD HighPart; } u;
	_OEXCOM_ULONGLONG QuadPart;
};
struct _OEXCOM_STATSTG
{	_OEXCOM_LPOLESTR		pwcsName;
	_OEXCOM_DWORD			type;
	_OEXCOM_ULARGE_INTEGER	cbSize;
	_OEXCOM_FILETIME		mtime;
	_OEXCOM_FILETIME		ctime;
	_OEXCOM_FILETIME		atime;
	_OEXCOM_DWORD			grfMode;
	_OEXCOM_DWORD			grfLocksSupported;
	_OEXCOM_CLSID			clsid;
	_OEXCOM_DWORD			grfStateBits;
	_OEXCOM_DWORD			reserved;
};
struct _OEXCOM_BIND_OPTS
{	_OEXCOM_DWORD cbStruct;
	_OEXCOM_DWORD grfFlags;
	_OEXCOM_DWORD grfMode;
	_OEXCOM_DWORD dwTickCountDeadline;
};
struct _OEXCOM_DVTARGETDEVICE
{	_OEXCOM_DWORD tdSize;
	_OEXCOM_WORD  tdDriverNameOffset;
	_OEXCOM_WORD  tdDeviceNameOffset;
	_OEXCOM_WORD  tdPortNameOffset;
	_OEXCOM_WORD  tdExtDevmodeOffset;
	_OEXCOM_BYTE  tdData[1];
};
struct _OEXCOM_FORMATETC
{	_OEXCOM_CLIPFORMAT		cfFormat;
	_OEXCOM_DVTARGETDEVICE	*ptd;
	_OEXCOM_DWORD			dwAspect;
	_OEXCOM_LONG			lindex;
	_OEXCOM_DWORD			tymed;
};
struct _OEXCOM_STGMEDIUM
{	_OEXCOM_DWORD    tymed;
	union
	{	_OEXCOM_HBITMAP       hBitmap;
		_OEXCOM_HMETAFILEPICT hMetaFilePict;
		_OEXCOM_HENHMETAFILE  hEnhMetaFile;
		_OEXCOM_HGLOBAL       hGlobal;
		_OEXCOM_LPOLESTR      lpszFileName;
		_OEXCOM_IStream       *pstm;
		_OEXCOM_IStorage      *pstg;
	};
	_OEXCOM_IUnknown *pUnkForRelease;
};
struct _OEXCOM_STATDATA
{	_OEXCOM_FORMATETC   formatetc;
	_OEXCOM_DWORD       advf;
	_OEXCOM_IAdviseSink *pAdvSink;
	_OEXCOM_DWORD       dwConnection;
};
struct _OEXCOM_RECT
{	_OEXCOM_LONG left;
	_OEXCOM_LONG top;
	_OEXCOM_LONG right;
	_OEXCOM_LONG bottom;
};
struct _OEXCOM_OLEVERB
{	_OEXCOM_LONG		lVerb;
	_OEXCOM_LPOLESTR	lpszVerbName;
	_OEXCOM_DWORD		fuFlags;
	_OEXCOM_DWORD		grfAttribs;
};

// IID
extern const _OEXCOM_IID _OEXCOM_IID_zeros;
extern const _OEXCOM_IID _OEXCOM_IID_IUnknown;
extern const _OEXCOM_IID _OEXCOM_IID_IClassFactory;
extern const _OEXCOM_IID _OEXCOM_IID_IOleObject;
extern const _OEXCOM_IID _OEXCOM_IID_IDispatch;
extern oex::oexLONG _g_oex_lRefCount;

//------------------------------------------------------------------
// COM functions
//------------------------------------------------------------------

#define _OEXCOM_CMPGUID			oex::guid::CmpGuid
#define _OEXCOM_NEW				OexAllocConstruct
#define _OEXCOM_DELETE			OexAllocDelete

//------------------------------------------------------------------
// OEX Macros for COM
//------------------------------------------------------------------
#define _OEXCOM_IUnknown_Interface() \
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL QueryInterface( oexCONST _OEXCOM_IID& iid,void** ppv ) = 0; \
	virtual _OEXCOM_ULONG _OEXCOM_STDCALL AddRef() = 0; \
	virtual _OEXCOM_ULONG _OEXCOM_STDCALL Release() = 0; \

#define _OEXCOM_IUnknown_Implement() \
	virtual _OEXCOM_ULONG _OEXCOM_STDCALL QueryInterface( oexCONST _OEXCOM_IID& iid,void** ppv ) \
	{	if ( !ppv ) return _OEXCOM_E_INVALIDARG; \
		*ppv = 0; \
		if ( _OEXCOM_CMPGUID( &_OEXCOM_IID_IUnknown, &iid ) ) \
		{	AddRef(); *ppv = (void*)this; return _OEXCOM_S_OK; } \
		return OnQueryInterface( iid, ppv ); \
	} \
	virtual _OEXCOM_ULONG _OEXCOM_STDCALL AddRef() \
	{	oexIncrement( &_g_oex_lRefCount ); \
		return (_OEXCOM_ULONG)oexIncrement( &_m_ref._m_ref_count ); } \
	virtual _OEXCOM_ULONG _OEXCOM_STDCALL Release() \
	{	oexDecrement( &_g_oex_lRefCount ); \
		_OEXCOM_ULONG c = oexDecrement( &_m_ref._m_ref_count ); \
		if ( !c ) _OEXCOM_DeleteInstance( this ); \
		return c; \
	} \
	template< typename T > static T* _OEXCOM_CreateInstance( const _OEXCOM_IID &iid ) \
		{ T *p = _OEXCOM_NEW< T >(); if ( p ) p->_m_iid = iid, p->AddRef(); return p; } \
	template< typename T > static void _OEXCOM_DeleteInstance( T *p ) { if ( p ) _OEXCOM_DELETE( p ); } \
	_OEXCOM_IReference _m_ref; \
	_OEXCOM_IID _m_iid;

#define _OEXCOM_BEGIN_INTERFACE_MAP \
	_OEXCOM_IUnknown_Implement(); \
	virtual _OEXCOM_ULONG OnQueryInterface( oexCONST _OEXCOM_IID& iid, void** ppv ) \
	{	if ( !ppv ) return _OEXCOM_E_INVALIDARG;
#define _OEXCOM_END_INTERFACE_MAP return _OEXCOM_E_NOINTERFACE; }
#define _OEXCOM_DECLARE_INTERFACE( n, c ) \
	if ( _OEXCOM_CMPGUID( &n, &iid ) ) \
	{	AddRef(); \
		*ppv = (void*)(c*)this; \
		return _OEXCOM_S_OK; \
	}
#define _OEXCOM_DECLARE_INTERFACE_OBJECT( n, c, o ) \
	if ( _OEXCOM_CMPGUID( &n, &iid ) ) \
	{	if ( !o ) \
			o = COleObject::_OEXCOM_CreateInstance<COleObject>( iid ); \
		o->AddRef(); \
		*ppv = (void*)(c*)o; \
		return _OEXCOM_S_OK; \
	}


//------------------------------------------------------------------
// COM Interfaces
//------------------------------------------------------------------
class _OEXCOM_IReference
{
public:
	_OEXCOM_IReference() : _m_ref_count( 0 ) { }
	_OEXCOM_LONG _m_ref_count;
};


//------------------------------------------------------------------
// IUnknown
//------------------------------------------------------------------
class _OEXCOM_IUnknown
{
public:
	_OEXCOM_IUnknown_Interface();
};


//------------------------------------------------------------------
// IClassFactory
//------------------------------------------------------------------
class _OEXCOM_IClassFactory : public _OEXCOM_IUnknown
{
public:
	_OEXCOM_IUnknown_Interface();

	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL CreateInstance( _OEXCOM_IUnknown* pUnkOuter, const _OEXCOM_IID& iid, void** ppv ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL LockServer( _OEXCOM_BOOL bLock ) = 0;

};

template< typename T_CLASS >
	class _OEXCOM_CClassFactory : public _OEXCOM_IClassFactory
{
public:

	_OEXCOM_BEGIN_INTERFACE_MAP
		_OEXCOM_DECLARE_INTERFACE( _OEXCOM_IID_IClassFactory, _OEXCOM_CClassFactory )
	_OEXCOM_END_INTERFACE_MAP

	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL CreateInstance( _OEXCOM_IUnknown* pUnkOuter, const _OEXCOM_IID& iid, void** ppv )
	{
		if ( !ppv )
			return _OEXCOM_E_INVALIDARG;

		*ppv = oexNULL;

		if ( pUnkOuter )
			return _OEXCOM_E_NOAGGREGATION;

		T_CLASS *p = _OEXCOM_CreateInstance< T_CLASS >( iid );
		if ( !p )
			return _OEXCOM_E_OUTOFMEMORY;

		_OEXCOM_HRESULT hr = p->QueryInterface( iid, ppv );
		if ( _OEXCOM_S_OK != hr || !*ppv )
			p->Release();

		return hr;
	}

	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL LockServer( _OEXCOM_BOOL bLock )
	{
		if ( bLock )
			oexIncrement( &_g_oex_lRefCount );
		else
			oexDecrement( &_g_oex_lRefCount );

		return _OEXCOM_S_OK;
	}

};


//------------------------------------------------------------------
// IEnumString
//------------------------------------------------------------------
class _OEXCOM_IEnumString : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Next( 	_OEXCOM_ULONG celt,
													_OEXCOM_LPOLESTR *rgelt,
													_OEXCOM_ULONG *pceltFetched ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Skip( _OEXCOM_ULONG celt ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Reset() = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Clone( _OEXCOM_IEnumString **ppenum ) = 0;
};


//------------------------------------------------------------------
// IEnumMoniker
//------------------------------------------------------------------
class _OEXCOM_IEnumMoniker : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Next(	_OEXCOM_ULONG celt,
													_OEXCOM_IMoniker **rgelt,
													_OEXCOM_ULONG *pceltFetched ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Skip( _OEXCOM_ULONG celt ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Reset() = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Clone( _OEXCOM_IEnumMoniker **ppenum ) = 0;
};


//------------------------------------------------------------------
// IRunningObjectTable
//------------------------------------------------------------------
class _OEXCOM_IRunningObjectTable : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Register(	_OEXCOM_DWORD grfFlags,
														_OEXCOM_IUnknown *punkObject,
														_OEXCOM_IMoniker *pmkObjectName,
														_OEXCOM_DWORD *pdwRegister ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Revoke( _OEXCOM_DWORD dwRegister ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL IsRunning( _OEXCOM_IMoniker *pmkObjectName ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetObject(	_OEXCOM_IMoniker *pmkObjectName,
														_OEXCOM_IUnknown **ppunkObject ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL NoteChangeTime(	_OEXCOM_DWORD dwRegister,
															_OEXCOM_FILETIME *pfiletime ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetTimeOfLastChange(	_OEXCOM_IMoniker *pmkObjectName,
																	_OEXCOM_FILETIME *pfiletime ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL EnumRunning( _OEXCOM_IEnumMoniker **ppenumMoniker ) = 0;
};


//------------------------------------------------------------------
// IBindCtx
//------------------------------------------------------------------
class _OEXCOM_IBindCtx : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL RegisterObjectBound( _OEXCOM_IUnknown *punk ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL RevokeObjectBound( _OEXCOM_IUnknown *punk ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL ReleaseBoundObjects() = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetBindOptions( _OEXCOM_BIND_OPTS *pbindopts ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetBindOptions( _OEXCOM_BIND_OPTS *pbindopts ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetRunningObjectTable( _OEXCOM_IRunningObjectTable **pprot ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL RegisterObjectParam( _OEXCOM_LPOLESTR pszKey, _OEXCOM_IUnknown *punk ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetObjectParam( _OEXCOM_LPOLESTR pszKey, _OEXCOM_IUnknown **ppunk ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL EnumObjectParam( _OEXCOM_IEnumString **ppenum ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL RevokeObjectParam( _OEXCOM_LPOLESTR pszKey ) = 0;
};

//------------------------------------------------------------------
// ISequentialStream
//------------------------------------------------------------------
class _OEXCOM_ISequentialStream : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Read( void *pv, _OEXCOM_ULONG cb, _OEXCOM_ULONG *pcbRead ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Write( const void *pv, _OEXCOM_ULONG cb, _OEXCOM_ULONG *pcbWritten ) = 0;
};


//------------------------------------------------------------------
// IStream
//------------------------------------------------------------------
class _OEXCOM_IStream : public _OEXCOM_ISequentialStream
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Seek( 	_OEXCOM_LARGE_INTEGER dlibMove,
													_OEXCOM_DWORD dwOrigin,
													_OEXCOM_ULARGE_INTEGER *plibNewPosition ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetSize( _OEXCOM_ULARGE_INTEGER libNewSize ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL CopyTo(	_OEXCOM_IStream *pstm,
													_OEXCOM_ULARGE_INTEGER cb,
													_OEXCOM_ULARGE_INTEGER *pcbRead,
													_OEXCOM_ULARGE_INTEGER *pcbWritten ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Commit( _OEXCOM_DWORD grfCommitFlags ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Revert() = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL LockRegion(	_OEXCOM_ULARGE_INTEGER libOffset,
														_OEXCOM_ULARGE_INTEGER cb,
														_OEXCOM_DWORD dwLockType ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL UnlockRegion(	_OEXCOM_ULARGE_INTEGER libOffset,
															_OEXCOM_ULARGE_INTEGER cb,
															_OEXCOM_DWORD dwLockType ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Stat( _OEXCOM_STATSTG *pstatstg, _OEXCOM_DWORD grfStatFlag ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Clone( _OEXCOM_IStream **ppstm ) = 0;
};


//------------------------------------------------------------------
// IPersist
//------------------------------------------------------------------
class _OEXCOM_IPersist : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetClassID( _OEXCOM_CLSID *pClassID ) = 0;
};


//------------------------------------------------------------------
// IPersistStream
//------------------------------------------------------------------
class _OEXCOM_IPersistStream : public _OEXCOM_IPersist
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL IsDirty() = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Load( _OEXCOM_IStream *pStm ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Save( _OEXCOM_IStream *pStm, _OEXCOM_BOOL fClearDirty ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetSizeMax( _OEXCOM_ULARGE_INTEGER *pcbSize ) = 0;
};


//------------------------------------------------------------------
// IMoniker
//------------------------------------------------------------------
class _OEXCOM_IMoniker : public _OEXCOM_IPersistStream
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL BindToObject( 	_OEXCOM_IBindCtx *pbc,
															_OEXCOM_IMoniker *pmkToLeft,
															_OEXCOM_REFIID riidResult,
															void **ppvResult ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL BindToStorage(	_OEXCOM_IBindCtx *pbc,
															_OEXCOM_IMoniker *pmkToLeft,
															_OEXCOM_REFIID riid,
															void **ppvObj ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Reduce(	_OEXCOM_IBindCtx *pbc,
													_OEXCOM_DWORD dwReduceHowFar,
													_OEXCOM_IMoniker **ppmkToLeft,
													_OEXCOM_IMoniker **ppmkReduced ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL ComposeWith(	_OEXCOM_IMoniker *pmkRight,
															_OEXCOM_BOOL fOnlyIfNotGeneric,
															_OEXCOM_IMoniker **ppmkComposite ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Enum(	_OEXCOM_BOOL fForward,
													_OEXCOM_IEnumMoniker **ppenumMoniker ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL IsEqual( _OEXCOM_IMoniker *pmkOtherMoniker ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Hash( _OEXCOM_DWORD *pdwHash ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL IsRunning(	_OEXCOM_IBindCtx *pbc,
														_OEXCOM_IMoniker *pmkToLeft,
														_OEXCOM_IMoniker *pmkNewlyRunning ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetTimeOfLastChange( 	_OEXCOM_IBindCtx *pbc,
																	_OEXCOM_IMoniker *pmkToLeft,
																	_OEXCOM_FILETIME *pFileTime ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Inverse( _OEXCOM_IMoniker **ppmk ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL CommonPrefixWith(	_OEXCOM_IMoniker *pmkOther,
																_OEXCOM_IMoniker **ppmkPrefix) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL RelativePathTo(	_OEXCOM_IMoniker *pmkOther,
															_OEXCOM_IMoniker **ppmkRelPath ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetDisplayName(	_OEXCOM_IBindCtx *pbc,
															_OEXCOM_IMoniker *pmkToLeft,
															_OEXCOM_LPOLESTR *ppszDisplayName ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL ParseDisplayName(	_OEXCOM_IBindCtx *pbc,
																_OEXCOM_IMoniker *pmkToLeft,
																_OEXCOM_LPOLESTR pszDisplayName,
																_OEXCOM_ULONG *pchEaten,
																_OEXCOM_IMoniker **ppmkOut ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL IsSystemMoniker( _OEXCOM_DWORD *pdwMksys ) = 0;
};


//------------------------------------------------------------------
// IParseDisplayName
//------------------------------------------------------------------
class _OEXCOM_IParseDisplayName : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL ParseDisplayName(	_OEXCOM_IBindCtx *pbc,
																_OEXCOM_LPOLESTR pszDisplayName,
																_OEXCOM_ULONG *pchEaten,
																_OEXCOM_IMoniker **ppmkOut ) = 0;
};


//------------------------------------------------------------------
// IEnumUnknown
//------------------------------------------------------------------
class _OEXCOM_IEnumUnknown : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Next( 	_OEXCOM_ULONG celt,
													_OEXCOM_IUnknown **rgelt,
													_OEXCOM_ULONG *pceltFetched ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Skip( _OEXCOM_ULONG celt ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Reset() = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Clone( _OEXCOM_IEnumUnknown **ppenum ) = 0;
};


//------------------------------------------------------------------
// IOleContainer
//------------------------------------------------------------------
class _OEXCOM_IOleContainer : public _OEXCOM_IParseDisplayName
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL EnumObjects( _OEXCOM_DWORD grfFlags, _OEXCOM_IEnumUnknown **ppenum ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL LockContainer( _OEXCOM_BOOL fLock ) = 0;
};


//------------------------------------------------------------------
// IOleClientSite
//------------------------------------------------------------------
class _OEXCOM_IOleClientSite : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SaveObject() = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetMoniker( _OEXCOM_DWORD dwAssign,
														_OEXCOM_DWORD dwWhichMoniker,
														_OEXCOM_IMoniker **ppmk ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetContainer( _OEXCOM_IOleContainer **ppContainer ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL ShowObject() = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL OnShowWindow( _OEXCOM_BOOL fShow ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL RequestNewObjectLayout() = 0;
};


//------------------------------------------------------------------
// IEnumFORMATETC
//------------------------------------------------------------------
class _OEXCOM_IEnumFORMATETC : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Next( 	_OEXCOM_ULONG celt,
													_OEXCOM_FORMATETC *rgelt,
													_OEXCOM_ULONG *pceltFetched ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Skip( _OEXCOM_ULONG celt ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Reset() = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Clone( _OEXCOM_IEnumFORMATETC **ppenum ) = 0;
};


//------------------------------------------------------------------
// IAdviseSink
//------------------------------------------------------------------
class _OEXCOM_IAdviseSink : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL OnDataChange( _OEXCOM_FORMATETC *pFormatetc, _OEXCOM_STGMEDIUM *pStgmed ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL OnViewChange( _OEXCOM_DWORD dwAspect, _OEXCOM_LONG lindex) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL OnRename( _OEXCOM_IMoniker *pmk) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL OnSave() = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL OnClose() = 0;
};


//------------------------------------------------------------------
// IEnumSTATDATA
//------------------------------------------------------------------
class _OEXCOM_IEnumSTATDATA : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Next(	_OEXCOM_ULONG celt,
													_OEXCOM_STATDATA *rgelt,
													_OEXCOM_ULONG *pceltFetched ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Skip( _OEXCOM_ULONG celt ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Reset() = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Clone( _OEXCOM_IEnumSTATDATA **ppenum ) = 0;
};


//------------------------------------------------------------------
// IDataObject
//------------------------------------------------------------------
class _OEXCOM_IDataObject : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetData(	_OEXCOM_FORMATETC *pformatetcIn,
														_OEXCOM_STGMEDIUM *pmedium ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetDataHere(	_OEXCOM_FORMATETC *pformatetc,
															_OEXCOM_STGMEDIUM *pmedium ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL QueryGetData(	_OEXCOM_FORMATETC *pformatetc ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetCanonicalFormatEtc(	_OEXCOM_FORMATETC *pformatectIn,
																	_OEXCOM_FORMATETC *pformatetcOut ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetData(	_OEXCOM_FORMATETC *pformatetc,
														_OEXCOM_STGMEDIUM *pmedium,
														_OEXCOM_BOOL fRelease ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL EnumFormatEtc(	_OEXCOM_DWORD dwDirection,
															_OEXCOM_IEnumFORMATETC **ppenumFormatEtc ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL DAdvise(	_OEXCOM_FORMATETC *pformatetc,
														_OEXCOM_DWORD advf,
														_OEXCOM_IAdviseSink *pAdvSink,
														_OEXCOM_DWORD *pdwConnection ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL DUnadvise( _OEXCOM_DWORD dwConnection ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL EnumDAdvise( _OEXCOM_IEnumSTATDATA **ppenumAdvise ) = 0;
};


//------------------------------------------------------------------
// IEnumOLEVERB
//------------------------------------------------------------------
class _OEXCOM_IEnumOLEVERB : public _OEXCOM_IUnknown
{
public:
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Next(	_OEXCOM_ULONG celt,
													_OEXCOM_LPOLEVERB rgelt,
													_OEXCOM_ULONG *pceltFetched ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Skip( _OEXCOM_ULONG celt ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Reset() = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Clone( _OEXCOM_IEnumOLEVERB **ppenum ) = 0;
};


//------------------------------------------------------------------
// IOleObject
//------------------------------------------------------------------
class _OEXCOM_IOleObject : public _OEXCOM_IUnknown
{
public:
	_OEXCOM_IUnknown_Interface();

	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetClientSite( _OEXCOM_IOleClientSite *pClientSite ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetClientSite(	_OEXCOM_IOleClientSite **ppClientSite ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetHostNames( 	_OEXCOM_LPCOLESTR szContainerApp,
															_OEXCOM_LPCOLESTR szContainerObj ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Close( _OEXCOM_DWORD dwSaveOption ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetMoniker(	_OEXCOM_DWORD dwWhichMoniker,
														_OEXCOM_IMoniker *pmk ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetMoniker(	_OEXCOM_DWORD dwAssign,
														_OEXCOM_DWORD dwWhichMoniker,
														_OEXCOM_IMoniker **ppmk ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL InitFromData(	_OEXCOM_IDataObject *pDataObject,
															_OEXCOM_BOOL fCreation,
															_OEXCOM_DWORD dwReserved ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetClipboardData(	_OEXCOM_DWORD dwReserved,
																_OEXCOM_IDataObject **ppDataObject ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL DoVerb(	_OEXCOM_LONG iVerb,
													_OEXCOM_LPMSG lpmsg,
													_OEXCOM_IOleClientSite *pActiveSite,
													_OEXCOM_LONG lindex,
													_OEXCOM_HWND hwndParent,
													_OEXCOM_LPCRECT lprcPosRect ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL EnumVerbs( _OEXCOM_IEnumOLEVERB **ppEnumOleVerb ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Update( void ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL IsUpToDate( void ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetUserClassID( _OEXCOM_CLSID *pClsid) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetUserType( _OEXCOM_DWORD dwFormOfType,
														 _OEXCOM_LPOLESTR *pszUserType ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetExtent(	_OEXCOM_DWORD dwDrawAspect,
														_OEXCOM_SIZEL *psizel ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetExtent(	_OEXCOM_DWORD dwDrawAspect,
														_OEXCOM_SIZEL *psizel ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Advise(	_OEXCOM_IAdviseSink *pAdvSink,
													_OEXCOM_DWORD *pdwConnection ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Unadvise( _OEXCOM_DWORD dwConnection ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL EnumAdvise( _OEXCOM_IEnumSTATDATA **ppenumAdvise ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetMiscStatus(	_OEXCOM_DWORD dwAspect,
															_OEXCOM_DWORD *pdwStatus ) = 0;
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetColorScheme( _OEXCOM_LOGPALETTE *pLogpal ) = 0;
};

class _OEXCOM_COleObject : public _OEXCOM_IOleObject
{
public:
	_OEXCOM_BEGIN_INTERFACE_MAP
		_OEXCOM_DECLARE_INTERFACE( _OEXCOM_IID_IOleObject, _OEXCOM_COleObject )
	_OEXCOM_END_INTERFACE_MAP

	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetClientSite( _OEXCOM_IOleClientSite *pClientSite ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetClientSite(	_OEXCOM_IOleClientSite **ppClientSite ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetHostNames( 	_OEXCOM_LPCOLESTR szContainerApp,
															_OEXCOM_LPCOLESTR szContainerObj ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Close( _OEXCOM_DWORD dwSaveOption ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetMoniker(	_OEXCOM_DWORD dwWhichMoniker,
														_OEXCOM_IMoniker *pmk ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetMoniker(	_OEXCOM_DWORD dwAssign,
														_OEXCOM_DWORD dwWhichMoniker,
														_OEXCOM_IMoniker **ppmk ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL InitFromData(	_OEXCOM_IDataObject *pDataObject,
															_OEXCOM_BOOL fCreation,
															_OEXCOM_DWORD dwReserved ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetClipboardData(	_OEXCOM_DWORD dwReserved,
																_OEXCOM_IDataObject **ppDataObject ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL DoVerb(	_OEXCOM_LONG iVerb,
													_OEXCOM_LPMSG lpmsg,
													_OEXCOM_IOleClientSite *pActiveSite,
													_OEXCOM_LONG lindex,
													_OEXCOM_HWND hwndParent,
													_OEXCOM_LPCRECT lprcPosRect ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL EnumVerbs( _OEXCOM_IEnumOLEVERB **ppEnumOleVerb ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Update( void ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL IsUpToDate( void ) { return _OEXCOM_E_NOTIMPL; };
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetUserClassID( _OEXCOM_CLSID *pClsid) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetUserType( _OEXCOM_DWORD dwFormOfType,
														 _OEXCOM_LPOLESTR *pszUserType ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetExtent(	_OEXCOM_DWORD dwDrawAspect,
														_OEXCOM_SIZEL *psizel ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetExtent(	_OEXCOM_DWORD dwDrawAspect,
														_OEXCOM_SIZEL *psizel ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Advise(	_OEXCOM_IAdviseSink *pAdvSink,
													_OEXCOM_DWORD *pdwConnection ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL Unadvise( _OEXCOM_DWORD dwConnection ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL EnumAdvise( _OEXCOM_IEnumSTATDATA **ppenumAdvise ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL GetMiscStatus(	_OEXCOM_DWORD dwAspect,
															_OEXCOM_DWORD *pdwStatus ) 
	{ return _OEXCOM_E_NOTIMPL; }
	virtual _OEXCOM_HRESULT _OEXCOM_STDCALL SetColorScheme( _OEXCOM_LOGPALETTE *pLogpal ) 
	{ return _OEXCOM_E_NOTIMPL; }
};

//------------------------------------------------------------------
// COM Exported functions
//------------------------------------------------------------------
#define _OEXCOM_BEGIN_FACTORY \
_OEXCOM_STDAPI DllGetClassObject( const _OEXCOM_IID &clsid, const _OEXCOM_IID &iid, void** ppv ) \
{	if ( 0 );

#define _OEXCOM_CREATE_OBJECT( n, c ) \
	else if ( _OEXCOM_CMPGUID( &n, &clsid ) ) \
	{	_OEXCOM_CClassFactory< c > *p = _OEXCOM_CClassFactory< c >::_OEXCOM_CreateInstance< _OEXCOM_CClassFactory< c > >(iid); \
		if ( !p ) return _OEXCOM_E_OUTOFMEMORY; \
		_OEXCOM_HRESULT hr = p->QueryInterface( iid, ppv ); \
		p->Release(); \
		return hr; \
	}

#define _OEXCOM_END_FACTORY return _OEXCOM_E_NOTIMPL; }

