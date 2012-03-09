#ifndef _KINECT_APP
#define _KINECT_APP

#define USE_IR // Comment this to use RGB instead of infrared cam...

#include "ofxOpenNI.h"
#include "ofMain.h"
#include "ofxGuiApp.h"
#include "ofxOpenCv.h"
#include "filter.h"
#include "ofxOsc.h"
#include "ofxXmlSettings.h"

#define MAX_DEVICES 2

//class ofxOpenNI;
//class ofxOpenNIUser;
//class ofxOpenNIHand;

class kinectApp : public ofxGuiApp, public ofxCvGrayscaleImage {

public:

	// general
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


	// gui
	void	setupGui();
	void	handleGui(int parameterId, int task, void* data, int length);

	ofTrueTypeFont usedFont;
	
	string statusConnection;
	string statusNetwork;
	string statusHands;
	string statusSkeletons;
	string statusObjects;


	// XML
	void savePersonalConfiguration();
	void loadPersonalConfiguration();
	void resetConfiguration();

	ofxXmlSettings XML;
	ofxXmlSettings XMLosc;
	

	// drawing
	void	drawBack();
	void	drawCamView();
	void	drawAllHands();
	void	drawSkeletons();
	void	drawObjects();
	void	drawDetails();

	ofImage background;

	int		width, height;
			
	int		nrHand, nrBody, nrObjects;

	float	aHand[8], bHand[8], cHand[8], aBody[4], bBody[4], cBody[4], aObject[20], bObject[20], cObject[20], cObject2[20]; 
	int		idHand[8], idBody[4], idObject[20];


	// ofxOpenNI
	void	setupScene();

	bool	isLive;

	void exit();

	int numDevices, bUsers01;
	ofxOpenNI openNIDevices[MAX_DEVICES];
    
    void userEvent(ofxOpenNIUserEvent & event);
	//void gestureEvent(ofxOpenNIGestureEvent & event);
	void handEvent(ofxOpenNIHandEvent & event);

	ofxOpenNIUser sceneUser;
	ofxOpenNIHand sceneHandTracker;

	//ofxOpenNIContext	sceneContext;
	//ofxDepthGenerator	sceneDepth, sceneDepth2;

#ifdef USE_IR
	//ofxIRGenerator		sceneImage;
#else
	//ofxImageGenerator	sceneImage;
#endif

	//ofxHandGenerator	sceneHandTracker;
	//ofxUserGenerator	sceneUser;

#if defined (TARGET_OSX) //|| defined(TARGET_LINUX) // only working on Mac/Linux at the moment (but on Linux you need to run as sudo...)
	ofxHardwareDriver	hardware;

	int rotation; // added by mihoo, 2012-Feb-17
#endif

	int rotation; // added by mihoo, 2012-Feb-17

	ofImage		depthRangeMask;
	//ofTexture	allUserMasks, userMask[4];
	ofxCvGrayscaleImage kinectImage, userImg[4];


	// blob tracking with ofxOpenCV
	void	objectGenerator();

	ofxCvContourFinder	contourFinder;

	filter*			filters;
	setfilter		processedImg;

	ofxCvGrayscaleImage	sourceImg;
	ofxCvGrayscaleImage	singleSourceImg[20];
	//ofxCvGrayscaleImage	filteredImg;
	ofxCvGrayscaleImage	blobImg[20];

	unsigned char * singlePixels[20];
	ofPixels		objPix[20];
	ofColor			colOfPix[20];

	//ofColor newCol[4];

	float	objectX[20], objectY[20], objectZ[20], objectZ2[20];

	int		location, fullsize, grayVal;
	int		nearThreshold, farThreshold, minBlobSize, maxBlobSize;

	//float	x1, x2, x3;
	//float	y1, y2, y3; 

	// ofxOSC
	void			communicateViaOsc();

	ofxOscSender	sender;

	string			host;
	int				port;


	//FPS variables
	int 				frames;
	int  				fps;
	float				lastFPSlog;
	int					differenceTime;

};

#endif
