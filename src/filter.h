#ifndef FILTER_H
#define FILTER_H

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCvGrayscaleImage.h"
#include "ofxCvGrayscaleImage.h"
#include "ofxCvFloatImage.h"

class setfilter : public ofxCvGrayscaleImage {

  public:

    setfilter(){};
	void operator = ( unsigned char* _pixels );
    void operator = ( const ofxCvGrayscaleImage& mom );
	void operator = ( const ofxCvColorImage& mom );
    void operator = ( const ofxCvFloatImage& mom );

	void highpass (int blur01, int blur02 );
	void amplify ( setfilter& mom, float level );
};

class filter {

  public:

    filter(){
		camWidth = 640;
		camHeight = 480;

		//filter values
		threshold = 0;
		smooth = 0;
		highpBlur = 0;
		highpNoise = 0;
		amp = 1;
		background = 0;
		all = 0;
	}

	int camWidth, camHeight, threshold, smooth, highpBlur, highpNoise, amp;
	bool background, all;
	ofxCvGrayscaleImage grayImg;	
	ofxCvGrayscaleImage grayBg;
    ofxCvGrayscaleImage subtractBg;
    ofxCvGrayscaleImage grayDiff;
	ofxCvGrayscaleImage highpassImg;
	ofxCvGrayscaleImage ampImg;
	ofxCvShortImage		floatBgImg;

	virtual void allocate( int w, int h ) = 0;
    virtual void apply(setfilter& img) = 0;
    virtual void draw() = 0;
};

class applyfilter : public filter {
  
  public:

    void allocate (int w, int h){
		camWidth = w;
        camHeight = h;
		grayImg.allocate(camWidth, camHeight);		//Gray Image
		grayBg.allocate(camWidth, camHeight);		//Background Image
		subtractBg.allocate(camWidth, camHeight);	//Background After subtraction
        grayDiff.allocate(camWidth, camHeight);		//Difference Image between Background and Source
		highpassImg.allocate(camWidth, camHeight);	//Highpass Image
		ampImg.allocate(camWidth, camHeight);		//Amplied Image
		floatBgImg.allocate(camWidth, camHeight);	//ofxShortImage used for simple dynamic background subtraction
	}

	void apply (setfilter& img){
		
		ofSetColor(255, 255, 255);

		grayImg = img;

		if (background == true){
            floatBgImg = img;
			//grayBg = floatBgImg;  // not yet implemented
			cvConvertScale( floatBgImg.getCvImage(), grayBg.getCvImage(), 255.0f/65535.0f, 0 );
			grayBg.flagImageChanged();
            background = false;
        }

		if (all == false){
            //Background Subtraction
			//cvSub(img.getCvImage(), grayBg.getCvImage(), img.getCvImage());
			img.absDiff(grayBg, img);
			img.flagImageChanged();
        }

		img.blur((smooth * 2) + 1); //needs to be an odd number
		subtractBg = img; //for drawing

		if (highpBlur > 0 || highpNoise > 0){
			img.highpass(highpBlur, highpNoise);
			highpassImg = img; //for drawing
		}

		//if (amp >= 1){
			img.amplify(img, amp);
			ampImg = img; //for drawing
		//}

		img.threshold(threshold); //Threshold
		
		grayDiff = img; //for drawing
	}

	void draw (){

		ofSetColor(255, 255, 255);

		grayDiff.draw(680, 560, camWidth, camHeight);
		
		grayImg.draw(682, 1078, 312, 232);
		floatBgImg.draw(1006, 1078, 312, 232); 

		subtractBg.draw(680, 1422, 208, 156); 
		highpassImg.draw(896, 1422, 208, 156); 
		ampImg.draw(1112, 1422, 208, 156); 
		
	}
};


#endif