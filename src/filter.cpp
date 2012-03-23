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