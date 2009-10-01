// gdc_chart.cpp

#include "stdafx.h"

#include "gdc.h"
#include "gdchart.h"
#include "gd/gd.h"

CGdcChart::CGdcChart()
{
	GDC_generate_gif = FALSE;
	GDC_hold_img = GDC_EXPOSE_IMAGE;
}

CGdcChart::~CGdcChart()
{
}

#define MAX_DIMENSIONS		2

sqbind::CSqMulti CGdcChart::GetChart( const sqbind::stdString &x_sType,
									  const sqbind::stdString &x_sParams,
									  const sqbind::stdString &x_sData )
{
#if !defined( OEX_ENABLE_XIMAGE )
	return sqbind::CSqMulti();
#else

	sqbind::CSqMulti mParams( x_sParams.c_str() );
	sqbind::CSqMulti mData( x_sData.c_str() );

	int nDataPts = 0;
	oex::TMem< oex::oexCSTR > memLabels;
	oex::TMem< float > memPts[ MAX_DIMENSIONS ];

	int nDimensions = 0;
	for ( sqbind::CSqMulti::iterator it = mData.begin();
		  nDimensions < MAX_DIMENSIONS && mData.end() != it; it++ )
	{
		// Use smallest size
		if ( !nDataPts )
		{
			nDataPts = it->second[ oexT( "data" ) ].size();
			if ( !nDataPts )
			{	oexERROR( 0, oexT( "No data" ) );
				return oexT( "" );
			} // end if

			if ( !memLabels.OexNew( nDataPts ).Ptr() )
			{	oexERROR( 0, oexMks( oexT( "Unable to allocate memory block, size = " ), nDataPts ) );
				return oexT( "" );
			} // end if

		} // end if

		// Allocate space for data
		if ( !memPts[ 0 ].OexNew( nDataPts ).Ptr() )
		{	oexERROR( 0, oexMks( oexT( "Unable to allocate memory block, size = " ), nDataPts ) );
			return oexT( "" );
		} // end if
		memPts[ 0 ].Zero();

		// Get data points
		int i = 0;
		for ( sqbind::CSqMulti::iterator itPt = it->second[ oexT( "data" ) ].begin();
			  i < nDataPts && it->second[ oexT( "data" ) ].end() != itPt; itPt++ )
		{
			// Grab pointer to label on first dimension
			if ( !nDimensions )
				memLabels[ i ] = itPt->first.c_str();

			// Grab point value
			memPts[ nDimensions ][ i++ ] = oexStrToFloat( itPt->second.str().c_str() );

		} // end for

		// Count a dimension
		nDimensions++;

	} // end for

	// Defaults
	GDC_BGColor = 0xffffff;
	GDC_GridColor = 0x000000;
	GDC_LineColor = 0x000000;
	GDC_PlotColor = 0x00ff00;
	GDC_ylabel_fmt = (char*)"%.3f";

	// User settings
	if ( mParams.isset( oexT( "col_bg" ) ) )
		GDC_BGColor = oexHtmlToRgb( mParams[ oexT( "col_bg" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_grid" ) ) )
		GDC_GridColor = oexHtmlToRgb( mParams[ oexT( "col_grid" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_labels" ) ) )
		GDC_LineColor = oexHtmlToRgb( mParams[ oexT( "col_labels" ) ].str().c_str() );
	if ( mParams.isset( oexT( "col_plot" ) ) )
		GDC_PlotColor = oexHtmlToRgb( mParams[ oexT( "col_plot" ) ].str().c_str() );

	// Write out the graph to the pipe
	out_graph( oexStrToLong( mParams[ oexT( "width" ) ].str().c_str() ),
	   		   oexStrToLong( mParams[ oexT( "height" ) ].str().c_str() ),
			   0, (GDC_CHART_T)oexStrToLong( mParams[ oexT( "type" ) ].str().c_str() ),
			   nDataPts, (char**)memLabels.Ptr(), nDimensions,
			   memPts[ 0 ].Ptr(), memPts[ 1 ].Ptr() );

	gdImagePtr graph = (gdImagePtr)GDC_image;
	if ( !oexCHECK_PTR( graph ) )
	{	oexERROR( 0, oexT( "Inavlid image pointer" ) );
		return oexT( "" );
	} // end if

	oex::CImage img;

	if ( !img.Create( graph->sx, graph->sy ) )
	{	oexERROR( 0, oexT( "Error allocating image buffer" ) );
		return oexT( "" );
	} // end if

	int sw = img.GetWidth();
	int sh = img.GetHeight();
//	int sz = ( img.GetWidth() * img.GetHeight() ) / 4;
	unsigned char **p = graph->pixels;

	for ( int y = 0, u = sh - 1; y < sh; y++, u-- )
		for ( int x = 0; x < sw; x++ )
		{	unsigned char b = p[ u ][ x ];
			img.SetPixel( x, y, oexRGB( graph->red[ b  & 0x03 ], graph->green[ b & 0x03 ], graph->blue[ b & 0x03 ] ) );
		} // end for

	gdImageDestroy( graph );
	GDC_image = NULL;

	oex::oexINT nSize;
	oex::oexPBYTE pBuf = oexNULL;
	oex::CStr sType = oexMks( x_sType.c_str() ).GetFileExtension();
	if ( !sType.Length() )
		sType = x_sType.c_str();
	if ( 0 >= img.GetFileType( sType.Ptr() ) )
		sType = oexT( "png" );

	// Encode image
	if ( 0 >= img.Encode( &pBuf, &nSize, sType.Ptr() ) || !pBuf || !nSize )
	{	oexERROR( 0, oexMks( oexT( "Failed to encode image, type = " ), x_sType.c_str() ) );
		return oexT( "" );
	} // end if

	sqbind::CSqMulti mImg;
	mImg[ oexT( "type" ) ].set( sType.Ptr() );
	mImg[ oexT( "width" ) ].set( oexMks( img.GetWidth() ).Ptr() );
	mImg[ oexT( "height" ) ].set( oexMks( img.GetHeight() ).Ptr() );

	oex::CStr sImg = oexBinToStr( oex::CStr8( (char*)pBuf, nSize ) );
	mImg[ oexT( "img" ) ].set( sqbind::stdString().assign( sImg.Ptr(), sImg.Length() ) );

	return mImg;
#endif
}

int CGdcChart::SaveChart(	const sqbind::stdString &x_sFile,
							const sqbind::stdString &x_sParams,
							const sqbind::stdString &x_sData )
{
	// Get the chart
	sqbind::CSqMulti mChart = GetChart( x_sFile, x_sParams, x_sData );
	if ( !mChart.isset( oexT( "img" ) ) )
		return -10;

	// Save image to disk
	oexFilePutContentsLen( x_sFile.c_str(), mChart[ oexT( "img" ) ].str().c_str(), mChart[ oexT( "img" ) ].str().length() );

	return 0;
}
