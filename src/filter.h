/*********************************************************************************
	
	KinectA - Kinect Tracking Application / filter.h
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

#ifndef FILTER_H
#define FILTER_H

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCvGrayscaleImage.h"
#include "ofxCvGrayscaleImage.h"
#include "ofxCvFloatImage.h"


//--------------------------------------------------------------------------------
class setfilter : public ofxCvGrayscaleImage {

  public:

    setfilter(){};
		
	void amplify ( setfilter& mom, float level );
	void highpass (float blur01, float blur02 );

	void operator = ( unsigned char* _pixels );
    void operator = ( const ofxCvGrayscaleImage& mom );
	void operator = ( const ofxCvColorImage& mom );
    void operator = ( const ofxCvFloatImage& mom );
};

//--------------------------------------------------------------------------------
class filter {

  public:

    filter(){
		
		//-- initialize variables --// 
		
		camWidth = 640;
		camHeight = 480;

		threshold = 0;
		smooth = 0;
		highpBlur = 0;
		highpNoise = 0;
		amp = 1;
		background = 0;
		all = 0;
	}

	int		camWidth, camHeight, threshold, smooth;
	float	highpBlur, highpNoise, amp;
	bool	background, all;

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

//--------------------------------------------------------------------------------
class applyfilter : public filter {
  
  public:

	//-- assign filters --//
    void allocate (int w, int h){

		//-- initialize variables --// 

		camWidth = w;
        camHeight = h;

		grayImg.allocate(camWidth, camHeight);		//gray image
		grayBg.allocate(camWidth, camHeight);		//background image
		subtractBg.allocate(camWidth, camHeight);	//background after subtraction
        grayDiff.allocate(camWidth, camHeight);		//difference image between background & source
		highpassImg.allocate(camWidth, camHeight);	//highpass image
		ampImg.allocate(camWidth, camHeight);		//amplified image
		floatBgImg.allocate(camWidth, camHeight);	//simple background subtraction

	}

	//-- process filters --//
	void apply (setfilter& img){
		
		ofSetColor(255, 255, 255);

		//-- original image --//
		grayImg = img; //for drawing

		//-- capture background --//
		if (background){
            floatBgImg = img;
			cvConvertScale( floatBgImg.getCvImage(), grayBg.getCvImage(), 255.0f/65535.0f, 0 );
			grayBg.flagImageChanged();
            background = false;
        }

		//-- remove background --//
		if (all){ 
			cvSub(img.getCvImage(), grayBg.getCvImage(), img.getCvImage());
			//img.absDiff(grayBg, img);
        }
		img.flagImageChanged();

		//-- smooth image --//
		if (smooth >= 0){ 
			img.blur((smooth * 2) + 1); //needs to be an odd number
			subtractBg = img; //for drawing
		}

		//-- blur edges & reduce noise --//
		if (highpBlur > 0 || highpNoise > 0){
			img.highpass(highpBlur, highpNoise);
			highpassImg = img; //for drawing
		}
		img.flagImageChanged();

		//-- amplify weak areas --//
		if (amp >= 1){
			img.amplify(img, amp);
			ampImg = img; //for drawing
		}

		//-- image threshold --//
		img.threshold(threshold);
		
		//-- finally processed image --//
		grayDiff = img; //for drawing
	}

	//-- show steps of filter processing --//
	void draw (){

		ofSetColor(255, 255, 255);

		grayDiff.draw(680, 0, camWidth, camHeight);
		
		grayImg.draw(682, 558, 312, 232);
		floatBgImg.draw(1006, 558, 312, 232);
		
		subtractBg.draw(680, 878, 208, 156); 
		highpassImg.draw(896, 878, 208, 156); 
		ampImg.draw(1112, 878, 208, 156);		
	}
};


#endif