/*********************************************************************************
	
	KinectA - Kinect Tracking Application / kinectApp.h
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

*********************************************************************************/

#ifndef _KINECT_APP
#define _KINECT_APP

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "ofxGuiApp.h"
#include "ofxOpenCv.h"
#include "filter.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"


class kinectApp : public ofxGuiApp, public ofxCvGrayscaleImage {

public:

	//---------- general functions & variables ----------//
	void	setup();
	void	update();
	void	draw();

	void	keyPressed  (int key);
	void	keyReleased(int key);
	void	mouseMoved(int x, int y );
	void	mouseDragged(int x, int y, int button);
	void	mousePressed(int x, int y, int button);
	void	mouseReleased(int x, int y, int button);
	void	windowResized(int w, int h);

	int		width, height;
	int		nrHand, nrBody, nrObjects, idHand[8], idBody[4], idObject[20];
	float	aHand[8], bHand[8], cHand[8], aBody[4], bBody[4], cBody[4], aObject[20], bObject[20], cObject[20], cObject2[20];

	int 	frames, fps, differenceTime;
	float	lastFPSlog;


	//---------- for drawing ----------//
	void	drawBack();
	void	drawCamView();
	void	drawAllHands();
	void	drawSkeletons();
	void	drawObjects();
	void	drawDetails();

	ofImage background;


	//---------- gui ----------//
	void	setupGui();
	void	handleGui(int parameterId, int task, void* data, int length);
	void    basicGui();
	void	objectGui();

	ofTrueTypeFont usedFont;

	string statusConfig;
	string statusHands;
	string statusSkeletons;
	string statusObjects;


	//---------- XML ----------//
	void savePersonalConfiguration();
	void loadPersonalConfiguration();
	void resetConfiguration();

	ofxXmlSettings XML;
	ofxXmlSettings XMLosc;


	//---------- ofxOpenNI ----------//
	void	setupScene();

	bool	isLive;

	void	exit();

	ofxOpenNI openNIDevices;
    
    void	userEvent(ofxOpenNIUserEvent & event);
	void	gestureEvent(ofxOpenNIGestureEvent & event);
	void	handEvent(ofxOpenNIHandEvent & event);

	ofxOpenNIUser			sceneUser;
	ofxOpenNIHand			sceneHandTracker;

	ofxCvGrayscaleImage userImg[4];
	ofxCvGrayscaleImage kinectImage;
	
	unsigned char*		getDepthPixels(int nearThreshold, int farThreshold);
	unsigned char*		maskPixels;



	//---------- blob tracking with ofxOpenCV ----------//
	void		objectGenerator();

	ofxCvContourFinder	contourFinder;

	filter*		filters;
	setfilter	processedImg;

	ofxCvGrayscaleImage	sourceImg;
	ofxCvGrayscaleImage	singleSourceImg[20];
	ofxCvGrayscaleImage	blobImg[20];
	ofPixels	objPix[20];

	float		objectX[20], objectY[20], objectZ[20];

	int			fullsize, grayVal[20], nearThreshold, farThreshold, minBlobSize, maxBlobSize;


	//---------- ofxOSC ----------//
	void			communicateViaOsc();

	ofxOscSender	sender;

	string			host;
	int				port;	
};

#endif
