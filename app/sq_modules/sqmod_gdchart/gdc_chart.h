// gdc_chart.h

class CGdcChart
{
public:

	enum {
			 GDC_LINE,					// 0
			 GDC_AREA,					// 1
			 GDC_BAR,					// 2
			 GDC_HILOCLOSE,				// 3
			 GDC_COMBO_LINE_BAR,		// 4
			 GDC_COMBO_HLC_BAR,			// 5
			 GDC_COMBO_LINE_AREA,		// 6
			 GDC_COMBO_HLC_AREA,		// 7
			 GDC_3DHILOCLOSE,			// 8
			 GDC_3DCOMBO_LINE_BAR,		// 9
			 GDC_3DCOMBO_LINE_AREA,		// 10
			 GDC_3DCOMBO_HLC_BAR,		// 11
			 GDC_3DCOMBO_HLC_AREA,		// 12
			 GDC_3DBAR,					// 13
			 GDC_3DAREA,				// 14
			 GDC_3DLINE					// 15
		 };


public:

	_SQBIND_CLASS_CTOR_BEGIN( CGdcChart )
	_SQBIND_CLASS_CTOR_END( CGdcChart )

	/// Default constructor
	CGdcChart();

	/// Destructor
	virtual ~CGdcChart();

	/// Generates a chart and returns it
	sqbind::CSqMulti CreateChart( const sqbind::stdString &x_sType,
							      const sqbind::stdString &x_sParams,
							      const sqbind::stdString &x_sData,
							      sqbind::CSqImage *x_pImg );

	/// Creates a chart and saves it to disk
	int SaveChart(  const sqbind::stdString &x_sFile,
					const sqbind::stdString &x_sParams,
					const sqbind::stdString &x_sData );

private:

	/// Thread lock
	static oexLock				m_lock;

};
