#include "filter.h"

//--------------------------------------------------------------------------------
void setfilter::highpass ( int blur01, int blur02 ) {
	
	//Blur Original Image
	if(blur01 > 0) cvSmooth( cvImage, cvImageTemp, CV_BLUR , (blur01 * 2) + 1);

	//Original Image - Blur Image = Highpass Image
	cvSub( cvImage, cvImageTemp, cvImageTemp );

	//Blur Highpass to remove noise
	if(blur02 > 0) cvSmooth( cvImageTemp, cvImageTemp, CV_BLUR , (blur02 * 2) + 1);

	swapTemp();
	flagImageChanged();
}

//--------------------------------------------------------------------------------
void setfilter::amplify ( setfilter& mom, float level ) {

	float scalef = level / 320.0f;

	cvMul( mom.getCvImage(), mom.getCvImage(), cvImageTemp, scalef );
	swapTemp();
	flagImageChanged();
}

//--------------------------------------------------------------------------------
void setfilter::operator =	( unsigned char* _pixels ) {
    setFromPixels( _pixels, width, height );
}

//--------------------------------------------------------------------------------
void setfilter::operator = ( const ofxCvGrayscaleImage& _mom ) {
    if(this != &_mom) {  //check for self-assignment
        // cast non-const,  no worries, we will reverse any chages
        ofxCvGrayscaleImage& mom = const_cast<ofxCvGrayscaleImage&>(_mom); 
            
        if( matchingROI(getROI(), mom.getROI()) ) {
            cvCopy( mom.getCvImage(), cvImage, 0 );           
            flagImageChanged();
        } else {
            ofLog(OF_LOG_ERROR, "in =, ROI mismatch");
        }
    } else {
        ofLog(OF_LOG_WARNING, "in =, you are assigning a ofxCvGrayscaleImage to itself");
    }
}

//--------------------------------------------------------------------------------
void setfilter::operator = ( const ofxCvColorImage& _mom ) {
    // cast non-const,  no worries, we will reverse any chages
    ofxCvColorImage& mom = const_cast<ofxCvColorImage&>(_mom); 
	if( matchingROI(getROI(), mom.getROI()) ) {
		cvCvtColor( mom.getCvImage(), cvImage, CV_RGB2GRAY );
        flagImageChanged();
	} else {
        ofLog(OF_LOG_ERROR, "in =, ROI mismatch");
	}
}

//--------------------------------------------------------------------------------
void setfilter::operator = ( const ofxCvFloatImage& _mom ) {
    // cast non-const,  no worries, we will reverse any chages
    ofxCvFloatImage& mom = const_cast<ofxCvFloatImage&>(_mom); 
	if( matchingROI(getROI(), mom.getROI()) ) {
		//cvConvertScale( mom.getCvImage(), cvImage, 1.0f, 0);
        cvConvert( mom.getCvImage(), cvImage );        
        flagImageChanged();
	} else {
        ofLog(OF_LOG_ERROR, "in =, ROI mismatch");
	}
}