/***************************************************************************
                       plotting.cpp  -  GDL routines for plotting
                             -------------------
    begin                : July 22 2002
    copyright            : (C) 2002-2011 by Marc Schellens et al.
    email                : m_schellens@users.sf.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "includefirst.hpp"
#include "plotting.hpp"

namespace lib {

  using namespace std;

  void surface( EnvT* e)
  {
    SizeT nParam=e->NParam( 1); 

    DDoubleGDL* zVal;
    DDoubleGDL* yVal;
    DDoubleGDL* xVal;
    //    DDoubleGDL* zValT;
    auto_ptr<BaseGDL> xval_guard;
    auto_ptr<BaseGDL> yval_guard;

    SizeT xEl;
    SizeT yEl;
    SizeT zEl;

    if (nParam == 2 || nParam > 3) {
      e->Throw( "Incorrect number of arguments.");
    }
    
    BaseGDL* p0 = e->GetNumericArrayParDefined( 0)->Transpose( NULL);
    auto_ptr<BaseGDL> p0_guard;
    zVal = static_cast<DDoubleGDL*>
      (p0->Convert2( DOUBLE, BaseGDL::COPY));
    p0_guard.reset( p0); // delete upon exit

    if(zVal->Dim(0) == 1)
      e->Throw( "Array must have 2 dimensions:"
			  +e->GetParString(0));    
    xEl = zVal->Dim(1);
    yEl = zVal->Dim(0);
    if (nParam == 1) {
      xVal = new DDoubleGDL( dimension( xEl), BaseGDL::INDGEN);
      xval_guard.reset( xVal); // delete upon exit
      yVal = new DDoubleGDL( dimension( yEl), BaseGDL::INDGEN);
      yval_guard.reset( yVal); // delete upon exit
    }

    if (nParam == 3) {
      
      xVal = e->GetParAs< DDoubleGDL>( 1);
      yVal = e->GetParAs< DDoubleGDL>( 2);

      if (xVal->Rank() > 2)
	e->Throw( "X, Y, or Z array dimensions are incompatible.");
      
      if (yVal->Rank() > 2)
	e->Throw( "X, Y, or Z array dimensions are incompatible.");
      
      if (xVal->Rank() == 1) {
	if (xEl != xVal->Dim(0))
	  e->Throw( "X, Y, or Z array dimensions are incompatible.");
	}
      
      if (yVal->Rank() == 1) {
	if (yEl != yVal->Dim(0))
	  e->Throw( "X, Y, or Z array dimensions are incompatible.");
      }

      if (xVal->Rank() == 2) {
	if((xVal->Dim(0) != xEl) && (xVal->Dim(1) != yEl))
	  e->Throw( "X, Y, or Z array dimensions are incompatible.");
      }
      
      if (yVal->Rank() == 2) {
	if((yVal->Dim(0) != xEl) && (yVal->Dim(1) != yEl))
	  e->Throw( "X, Y, or Z array dimensions are incompatible.");
      }
    }

    // !P 
    DLong p_background; 
    DLong p_noErase; 
    DLong p_color; 
    DLong p_psym; 
    DLong p_linestyle;
    DFloat p_symsize; 
    DFloat p_charsize; 
    DFloat p_thick; 
    DString p_title; 
    DString p_subTitle; 
    DFloat p_ticklen; 

    GetPData( p_background,
	      p_noErase, p_color, p_psym, p_linestyle,
	      p_symsize, p_charsize, p_thick,
	      p_title, p_subTitle, p_ticklen);

    // !X, !Y, !Z (also used below)
    static DStructGDL* xStruct = SysVar::X();
    static DStructGDL* yStruct = SysVar::Y();
    static DStructGDL* zStruct = SysVar::Z();
    DLong xStyle; 
    DLong yStyle; 
    DLong zStyle; 
    DString xTitle; 
    DString yTitle; 
    DString zTitle; 
    DFloat x_CharSize; 
    DFloat y_CharSize; 
    DFloat z_CharSize; 
    DFloat xMarginL; 
    DFloat xMarginR; 
    DFloat yMarginB; 
    DFloat yMarginF; 
    DFloat zMarginB; 
    DFloat zMarginT; 
    DFloat xTicklen;
    DFloat yTicklen;
    DFloat zTicklen;
    GetAxisData( xStruct, xStyle, xTitle, x_CharSize, xMarginL, xMarginR,
		 xTicklen);
    GetAxisData( yStruct, yStyle, yTitle, y_CharSize, yMarginB, yMarginF,
		 yTicklen);
    GetAxisData( zStruct, zStyle, zTitle, z_CharSize, zMarginB, zMarginT,
		 zTicklen);
    
    // [XY]STYLE
    e->AssureLongScalarKWIfPresent( "XSTYLE", xStyle);
    e->AssureLongScalarKWIfPresent( "YSTYLE", yStyle);
    e->AssureLongScalarKWIfPresent( "ZSTYLE", zStyle);

    // TITLE
    DString title = p_title;
    DString subTitle = p_subTitle;
    e->AssureStringScalarKWIfPresent( "TITLE", title);
    e->AssureStringScalarKWIfPresent( "SUBTITLE", subTitle);

    // AXIS TITLE
    e->AssureStringScalarKWIfPresent( "XTITLE", xTitle);
    e->AssureStringScalarKWIfPresent( "YTITLE", yTitle);
    e->AssureStringScalarKWIfPresent( "ZTITLE", zTitle);

    // MARGIN (in characters)
    static int xMarginEnvIx = e->KeywordIx( "XMARGIN"); 
    static int yMarginEnvIx = e->KeywordIx( "YMARGIN"); 
    static int zMarginEnvIx = e->KeywordIx( "ZMARGIN"); 
    BaseGDL* xMargin = e->GetKW( xMarginEnvIx);
    BaseGDL* yMargin = e->GetKW( yMarginEnvIx);
    BaseGDL* zMargin = e->GetKW( zMarginEnvIx);
    if( xMargin != NULL)
      {
	if( xMargin->N_Elements() > 2)
	  e->Throw( "Keyword array parameter XMARGIN"
		    " must have from 1 to 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* xMarginFl = static_cast<DFloatGDL*>
	  ( xMargin->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( xMarginFl);
	xMarginL = (*xMarginFl)[0];
	if( xMarginFl->N_Elements() > 1)
	  xMarginR = (*xMarginFl)[1];
      }
    if( yMargin != NULL)
      {
	if( yMargin->N_Elements() > 2)
	  e->Throw( "Keyword array parameter YMARGIN"
		    " must have from 1 to 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* yMarginFl = static_cast<DFloatGDL*>
	  ( yMargin->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( yMarginFl);
	yMarginB = (*yMarginFl)[0];
	if( yMarginFl->N_Elements() > 1)
	  yMarginF = (*yMarginFl)[1];
      }
    if( zMargin != NULL)
      {
	if( zMargin->N_Elements() > 2)
	  e->Throw( "Keyword array parameter ZMARGIN"
		    " must have from 1 to 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* zMarginFl = static_cast<DFloatGDL*>
	  ( zMargin->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( zMarginFl);
	zMarginB = (*zMarginFl)[0];
	if( zMarginFl->N_Elements() > 1)
	  zMarginT = (*zMarginFl)[1];
      }

    // x and y and z range
     DDouble xStart;// = xVal->min(); 
     DDouble xEnd;//   = xVal->max(); 
GetMinMaxVal( xVal, &xStart, &xEnd);
     DDouble yStart;// = yVal->min(); 
     DDouble yEnd;//   = yVal->max(); 
GetMinMaxVal( yVal, &yStart, &yEnd);
     DDouble zStart;// = zVal->min(); 
     DDouble zEnd;//   = zVal->max(); 
GetMinMaxVal( zVal, &zStart, &zEnd);

    //[x|y|z]range keyword
    static int zRangeEnvIx = e->KeywordIx("ZRANGE");
    static int yRangeEnvIx = e->KeywordIx("YRANGE");
    static int xRangeEnvIx = e->KeywordIx("XRANGE");
    BaseGDL* xRange = e->GetKW( xRangeEnvIx);
    BaseGDL* yRange = e->GetKW( yRangeEnvIx);
    BaseGDL* zRange = e->GetKW( zRangeEnvIx);
    
    if(xRange != NULL) 
      {
	if(xRange->N_Elements() != 2)
	  e->Throw("Keyword array parameter XRANGE"
		   " must have 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* xRangeF = static_cast<DFloatGDL*>
	  ( xRange->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( xRangeF);
	xStart = (*xRangeF)[0];
	xEnd = (*xRangeF)[1];
      }

    if(yRange != NULL)
      {
	if(yRange->N_Elements() != 2)
	  e->Throw("Keyword array parameter YRANGE"
		   " must have 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* yRangeF = static_cast<DFloatGDL*>
	  ( yRange->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( yRangeF);
	yStart = (*yRangeF)[0];
	yEnd = (*yRangeF)[1];
      }
    if(zRange != NULL)
      {
	if(zRange->N_Elements() != 2)
	  e->Throw("Keyword array parameter ZRANGE"
		   " must have 2 elements.");
	auto_ptr<DFloatGDL> guard;
	DFloatGDL* zRangeF = static_cast<DFloatGDL*>
	  ( zRange->Convert2( FLOAT, BaseGDL::COPY));
	guard.reset( zRangeF);
	zStart = (*zRangeF)[0];
	zEnd = (*zRangeF)[1];
      }

    // AC is it useful ? Why not for Y ?
    if(xEnd == xStart) xEnd=xStart+1;

    DDouble minVal = zStart;
    DDouble maxVal = zEnd;
    e->AssureDoubleScalarKWIfPresent( "MIN_VALUE", minVal);
    e->AssureDoubleScalarKWIfPresent( "MAX_VALUE", maxVal);


    bool xLog = e->KeywordSet( "XLOG");
    bool yLog = e->KeywordSet( "YLOG");
    bool zLog = e->KeywordSet( "ZLOG");
    if( xLog && xStart <= 0.0)
      Warning( "PLOT: Infinite x plot range.");
    if( yLog && yStart <= 0.0)
      Warning( "PLOT: Infinite y plot range.");
    if( zLog && zStart <= 0.0)
      Warning( "PLOT: Infinite z plot range.");

    DLong noErase = p_noErase;
    if( e->KeywordSet( "NOERASE")) noErase = 1;

    DDouble ticklen = p_ticklen;
    e->AssureDoubleScalarKWIfPresent( "TICKLEN", ticklen);

    // POSITION
    PLFLT xScale = 1.0;
    PLFLT yScale = 1.0;
    //    PLFLT scale = 1.0;
    static int positionIx = e->KeywordIx( "POSITION"); 
    DFloatGDL* pos = e->IfDefGetKWAs<DFloatGDL>( positionIx);
    /*
    PLFLT position[ 4] = { 0.0, 0.0, 1.0, 1.0};
    if( pos != NULL)
      {
      for( SizeT i=0; i<4 && i<pos->N_Elements(); ++i)
	position[ i] = (*pos)[ i];

      xScale = position[2]-position[0];
      yScale = position[3]-position[1];
      //      scale = sqrt( pow( xScale,2) + pow( yScale,2));
      }
    */

    // CHARSIZE
    DDouble charsize = p_charsize;
    e->AssureDoubleScalarKWIfPresent( "CHARSIZE", charsize);
    if( charsize <= 0.0) charsize = 1.0;
    //    charsize *= scale;

    // AXIS CHARSIZE
    DDouble xCharSize = x_CharSize;
    e->AssureDoubleScalarKWIfPresent( "XCHARSIZE", xCharSize);
    if( xCharSize <= 0.0) xCharSize = 1.0;

    DDouble yCharSize = y_CharSize;
    e->AssureDoubleScalarKWIfPresent( "YCHARSIZE", yCharSize);
    if( yCharSize <= 0.0) yCharSize = 1.0;
    //    yCharSize *= scale;

    DDouble zCharSize = z_CharSize;
    e->AssureDoubleScalarKWIfPresent( "ZCHARSIZE", zCharSize);
    if( zCharSize <= 0.0) zCharSize = 1.0;


    // THICK
    DFloat thick = p_thick;
    e->AssureFloatScalarKWIfPresent( "THICK", thick);

    GDLGStream* actStream = GetPlotStream( e); 
    
    // *** start drawing
    gkw_background(e, actStream);  //BACKGROUND   
    gkw_color(e, actStream);       //COLOR

    actStream->NextPlot( !noErase);
    if( !noErase) actStream->Clear();

    // plplot stuff
    // set the charsize (scale factor)
    DDouble charScale = 1.0;
    DLongGDL* pMulti = SysVar::GetPMulti();
    if( (*pMulti)[1] > 2 || (*pMulti)[2] > 2) charScale = 0.5;
    actStream->schr( 0.0, charsize * charScale);

#if 0
    // get subpage in mm
    PLFLT scrXL, scrXR, scrYB, scrYF;
    actStream->gspa( scrXL, scrXR, scrYB, scrYF); 
    PLFLT scrX = scrXR-scrXL;
    PLFLT scrY = scrYF-scrYB;
#endif

    // get char size in mm (default, actual)
    PLFLT defH, actH;
    actStream->gchr( defH, actH);

    // CLIPPING
    DDoubleGDL* clippingD=NULL;
    DLong noclip=0;
    e->AssureLongScalarKWIfPresent( "NOCLIP", noclip);
    if(noclip == 0)
      {
	static int clippingix = e->KeywordIx( "CLIP"); 
	clippingD = e->IfDefGetKWAs<DDoubleGDL>( clippingix);
      }
    

#if 0
    // viewport and world coordinates
    bool okVPWC = SetVP_WC( e, actStream, pos, clippingD, 
			    xLog, yLog,
			    xMarginL, xMarginR, yMarginB, yMarginT,
			    xStart, xEnd, minVal, maxVal);
    if( !okVPWC) return;
#endif
    

    //linestyle
    DLong linestyle = p_linestyle ;
    DLong temp_linestyle=0;
    e->AssureLongScalarKWIfPresent( "LINESTYLE", temp_linestyle);

    /*
    if((temp_linestyle > 0) && (temp_linestyle < 9) )
	linestyle=temp_linestyle;
    else if((linestyle > 0) && (linestyle < 9) )
	linestyle=linestyle+1;
    else 
	linestyle=1;
    */

    linestyle=temp_linestyle+1;

    // pen thickness for axis
    actStream->wid( 0);

    // axis
    string xOpt = "bcnst";
    string yOpt = "bcnstv";

    if( xLog) xOpt += "l";
    if( yLog) yOpt += "l";

#if 0    
    // axis titles
    actStream->schr( 0.0, actH/defH * xCharSize);
    actStream->mtex("b",3.5,0.5,0.5,xTitle.c_str());
    // the axis (separate for x and y axis because of charsize)
    actStream->box( xOpt.c_str(), 0.0, 0, "", 0.0, 0);

    actStream->schr( 0.0, actH/defH * yCharSize);
    actStream->mtex("l",5.0,0.5,0.5,yTitle.c_str());
    // the axis (separate for x and y axis because of charsize)
    actStream->box( "", 0.0, 0, yOpt.c_str(), 0.0, 0);
#endif

    // pen thickness for plot
    actStream->wid( static_cast<PLINT>(floor( thick-0.5)));


    // plot the data
    actStream->lsty(linestyle);

    actStream->vpor(0.0, 1.0, 0.0, .9);
    //    actStream->wind(-0.8, 0.8, -0.8, .8);
    actStream->wind( -1.0, 1.0, -0.9, 2.0 );
    //    actStream->wind(-1.0, 1.0, -1.0, 1.5);

    PLFLT alt = 33.0;
    DFloat alt_change = alt;
    e->AssureFloatScalarKWIfPresent( "AX", alt_change);
    alt=alt_change;

    PLFLT az = 30.0;
    DFloat az_change = az;
    e->AssureFloatScalarKWIfPresent( "AZ", az_change);
    az=az_change;

    actStream->w3d( 1.2, 1.2, 2.2, 
		    xStart, xEnd, yStart, yEnd, minVal, maxVal,
		    alt, az);

    actStream->box3( "bnstu", xTitle.c_str(), 0.0, 0,
		     "bnstu", yTitle.c_str(), 0.0, 0,
		     "bcdmnstuv", zTitle.c_str(), 0.0, 4 );


    // 1 DIM X & Y
    if (xVal->Rank() == 1 && yVal->Rank() == 1) {
      
      PLFLT** z;
      actStream->Alloc2dGrid(&z,xEl,yEl);
      for( SizeT ii=0; ii<xEl ; ii++) {
	for( SizeT jj=0; jj<yEl ; jj++) {
	  z[ii][jj] = (*zVal)[ii*yEl+jj];
	}
      }
      actStream->mesh(static_cast<PLFLT*> (&(*xVal)[0]), 
		      static_cast<PLFLT*> (&(*yVal)[0]), 
		      z, (long int) xEl, (long int) yEl, 3);
      //delete[] z;
      if (z != NULL) { free((void *) z); z = NULL; }
    }

    // 2 DIM X & Y
    if (xVal->Rank() == 2 && yVal->Rank() == 2) {

      PLFLT** z1 = new PLFLT*[xEl];
      PLFLT* xVec1 = new PLFLT[xEl];
      PLFLT* yVec1 = new PLFLT[xEl];

      for( SizeT j=0; j<yEl; j++) {
	for( SizeT i=0; i<xEl; i++) {
	  z1[i] = &(*zVal)[i*yEl+j];
	  xVec1[i] = (*xVal)[j*xEl+i];
	  yVec1[i] = (*yVal)[j*xEl+i];
	}
	lib::mesh_nr(xVec1, yVec1, z1, (long int) xEl, 1,1);
      }
      delete[] z1;
      delete[] xVec1;
      delete[] yVec1;

      //
      PLFLT** z2 = new PLFLT*[yEl];
      PLFLT* xVec2 = new PLFLT[yEl];
      PLFLT* yVec2 = new PLFLT[yEl];
      
      for( SizeT j=0; j<xEl; j++) {
	for( SizeT i=0; i< yEl; i++) {
	  z2[i] = &(*zVal)[j*yEl+i];       
	  xVec2[i] = (*xVal)[i*xEl+j];
	  yVec2[i] = (*yVal)[i*xEl+j];
	}
	lib::mesh_nr(xVec2, yVec2, z2, 1, (long int) yEl, 2);
      }
      delete[] z2;
      delete[] xVec2;
      delete[] yVec2;
    }

    // title and sub title
    actStream->schr( 0.0, 1.25*actH/defH);
    actStream->mtex("t",1.25,0.5,0.5,title.c_str());
    actStream->schr( 0.0, actH/defH); // charsize is reset here
    actStream->mtex("b",5.4,0.5,0.5,subTitle.c_str());
    
    actStream->lsty(1);//reset linestyle
    actStream->flush();


    // set ![XY].CRANGE
    set_axis_crange("X", xStart, xEnd);
    set_axis_crange("Y", yStart, yEnd);

    //set ![x|y].type
    set_axis_type("X",xLog);
    set_axis_type("Y",yLog);
  } // surface

} // namespace