// gdc_chart.h

class CGdcChart
{
public:

	enum {
			 GDC_LINE,
			 GDC_AREA,
			 GDC_BAR,
			 GDC_HILOCLOSE,
			 GDC_COMBO_LINE_BAR,
			 GDC_COMBO_HLC_BAR,
			 GDC_COMBO_LINE_AREA,
			 GDC_COMBO_HLC_AREA,
			 GDC_3DHILOCLOSE,
			 GDC_3DCOMBO_LINE_BAR,
			 GDC_3DCOMBO_LINE_AREA,
			 GDC_3DCOMBO_HLC_BAR,
			 GDC_3DCOMBO_HLC_AREA,
			 GDC_3DBAR,
			 GDC_3DAREA,
			 GDC_3DLINE
		 };


public:

	/// Default constructor
	CGdcChart();

	/// Destructor
	virtual ~CGdcChart();

	/// Creates a chart and saves it to disk
	int SaveChart(  const sqbind::stdString &x_sFile,
					const sqbind::stdString &x_sParams,
					const sqbind::stdString &x_sLabels,
					const sqbind::stdString &x_sData1,
					const sqbind::stdString &x_sData2 );

};
