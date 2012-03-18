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

	
	void amplify ( setfilter& mom, float level );
	void highpass (float blur01, float blur02 );
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
		//processFilt = 0;
	}

	int camWidth, camHeight, threshold, smooth;
	float highpBlur, highpNoise, amp;
	bool background, all, processFilt;
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
		//grayImg.setUseTexture(false);
		grayBg.allocate(camWidth, camHeight);		//Background Image
		//grayBg.setUseTexture(false);
		subtractBg.allocate(camWidth, camHeight);	//Background After subtraction
		//subtractBg.setUseTexture(false);
        grayDiff.allocate(camWidth, camHeight);		//Difference Image between Background and Source
		//grayDiff.setUseTexture(false);
		highpassImg.allocate(camWidth, camHeight);	//Highpass Image
		//highpassImg.setUseTexture(false);
		ampImg.allocate(camWidth, camHeight);		//Amplied Image
		//ampImg.setUseTexture(false);
		floatBgImg.allocate(camWidth, camHeight);	//ofxShortImage used for simple dynamic background subtraction
		//floatBgImg.setUseTexture(false);
	}

	void apply (setfilter& img){
		
		ofSetColor(255, 255, 255);

		grayImg = img;

		if (background){
            floatBgImg = img;
			cvConvertScale( floatBgImg.getCvImage(), grayBg.getCvImage(), 255.0f/65535.0f, 0 );
			grayBg.flagImageChanged();
            background = false;
        }

		if (all){
            //Background Subtraction
			cvSub(img.getCvImage(), grayBg.getCvImage(), img.getCvImage());
			//img.absDiff(grayBg, img);
        }
		img.flagImageChanged();

		if (smooth >= 0){ 
			img.blur((smooth * 2) + 1); //needs to be an odd number
			subtractBg = img; //for drawing
		}

		if (highpBlur > 0 || highpNoise > 0){
			img.highpass(highpBlur, highpNoise);
			highpassImg = img; //for drawing
		}
		img.flagImageChanged();

		if (amp >= 1){
			img.amplify(img, amp);
			ampImg = img; //for drawing
		}

		img.threshold(threshold); //Threshold
		
		grayDiff = img; //for drawing
	}

	void draw (){

		ofSetColor(255, 255, 255);

		grayDiff.draw(680, 0, camWidth, camHeight);
		
		grayImg.draw(682, 558, 312, 232);
		floatBgImg.draw(1006, 558, 312, 232);
		
		//if (processFilt){ 
		subtractBg.draw(680, 878, 208, 156); 
		highpassImg.draw(896, 878, 208, 156); 
		ampImg.draw(1112, 878, 208, 156);
		//}
		
	}
};


#endif