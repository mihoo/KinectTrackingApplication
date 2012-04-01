/*********************************************************************************
	
	KinectA - Kinect Tracking Application / filter.cpp
    Copyright (C) 2011-2012  Michaela Honauer a.k.a. mihoo (http://www.mihoo.de/)

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
	
*********************************************************************************

	This part of the software is orientated on the KinectCoreVision project 
	by Patricio Gonzalez Vivo (http://www.patriciognzalezvivo.com) which again
	bases on the CommunityCoreVision project of the NUI Group (http://nuigroup.com).

*********************************************************************************/


#include "filter.h"


//--------------------------------------------------------------------------------
void setfilter::amplify ( setfilter& mom, float level ) {

	//-- amplify weak areas --//
	float scalef = level / 104.0; //128.0f

	cvMul( mom.getCvImage(), mom.getCvImage(), cvImageTemp, scalef );
	swapTemp();
	flagImageChanged();
}

//--------------------------------------------------------------------------------
void setfilter::highpass ( float blur01, float blur02 ) {
	
	//-- blur original image --//
	if(blur01 > 0) cvSmooth( cvImage, cvImageTemp, CV_BLUR , (blur01 * 2) + 1);

	//-- original image - blur image = highpass image --//
	cvSub( cvImage, cvImageTemp, cvImageTemp );

	//-- blur highpass image to remove noise --//
	if(blur02 > 0) cvSmooth( cvImageTemp, cvImageTemp, CV_BLUR , (blur02 * 2) + 1);

	swapTemp();
	flagImageChanged();
}

//--------------------------------------------------------------------------------
void setfilter::operator =	( unsigned char* _pixels ) {
    
	//-- for using pixels --//
	setFromPixels( _pixels, width, height );
}

//--------------------------------------------------------------------------------
void setfilter::operator = ( const ofxCvGrayscaleImage& _mom ) {
   
	//-- for using a grayimage --//

	if(this != &_mom) {  
        ofxCvGrayscaleImage& mom = const_cast<ofxCvGrayscaleImage&>(_mom); 
            
        if( matchingROI(getROI(), mom.getROI()) ) {
            cvCopy( mom.getCvImage(), cvImage, 0 );           
            flagImageChanged();
        } 
		else { ofLog(OF_LOG_ERROR, "in =, ROI mismatch"); }
    } 
	else { ofLog(OF_LOG_WARNING, "in =, you are assigning a ofxCvGrayscaleImage to itself"); }
}

//--------------------------------------------------------------------------------
void setfilter::operator = ( const ofxCvColorImage& _mom ) {
    
	//-- for using a colorimage --//

    ofxCvColorImage& mom = const_cast<ofxCvColorImage&>(_mom); 
	
	if( matchingROI(getROI(), mom.getROI()) ) {
		cvCvtColor( mom.getCvImage(), cvImage, CV_RGB2GRAY );
        flagImageChanged();
	} 
	else { ofLog(OF_LOG_ERROR, "in =, ROI mismatch"); }
}

//--------------------------------------------------------------------------------
void setfilter::operator = ( const ofxCvFloatImage& _mom ) {
    
	//-- for using a floatimage --//

    ofxCvFloatImage& mom = const_cast<ofxCvFloatImage&>(_mom); 
	
	if( matchingROI(getROI(), mom.getROI()) ) {
        cvConvert( mom.getCvImage(), cvImage );        
        flagImageChanged();
	} 
	else { ofLog(OF_LOG_ERROR, "in =, ROI mismatch"); }
}