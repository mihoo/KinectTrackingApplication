#include "kinectApp.h"
#include "gui.h"
#include "filter.h"

//--------------------------------------------------------------
void kinectApp::setup() {
	
	ofSetWindowTitle("KinectApp");

	isLive = true;

	nearThreshold = 500;
	farThreshold  = 1000;

	minBlobSize = 1500;
	maxBlobSize = 10000;

	rotation = 0;

	nrHand = 8;
	nrBody = 4;
	nrObjects = 20;

	location = 0;
	fullsize = 640*480;

	setupScene();

	ofBackground(50, 50, 50);
	background.loadImage("background/Interface.png");
	background.allocate(1000, 840, OF_IMAGE_COLOR_ALPHA);

	for (int i = 0; i < nrHand; i++){
		idHand[i] = 0; 
	}

	for (int i = 0; i < nrBody; i++){
		idBody[i] = 0; 
	}

	for (int i = 0; i < nrObjects; i++){
		idObject[i] = 0; 
	}

	processedImg.allocate(640, 480);			//
	processedImg.setUseTexture(false);			//We don't need to draw this so don't create a texture
	sourceImg.allocate(640, 480);				//Source Image
	sourceImg.setUseTexture(false);				//We don't need to draw this so don't create a texture
	//filteredImg.allocate(640, 480);
	//filteredImg.setUseTexture(false);
	for (int i = 0; i < nrObjects; i++){
		singleSourceImg[i].allocate(640, 480);
		singleSourceImg[i].setUseTexture(false);
		
		blobImg[i].allocate(640, 480);
		blobImg[i].setUseTexture(false);
	}
	
	filters = new applyfilter();
	filters->allocate(640, 480);

	//user1Mask.allocate(640, 480, OF_IMAGE_GRAYSCALE);
	//user1.allocate(640, 480, GL_ALPHA);

	usedFont.loadFont("fonts/Questrial-Regular.ttf", 10);

	statusNetwork = "new";

	host = "127.0.0.1";
	port = 3333;
	sender.setup(host, port); 
}

//--------------------------------------------------------------
void kinectApp::setupScene() {

#if defined (TARGET_OSX) //|| defined(TARGET_LINUX) // only working on Mac/Linux at the moment (but on Linux you need to run as sudo...)
	hardware.setup();				// libusb direct control of motor, LED and accelerometers
	hardware.setLedOption(LED_OFF); // turn off the led just for yacks (or for live installation/performances ;-)
#endif

	/*
	sceneContext.setup();	// all nodes created by code -> NOT using the xml config file at all
	//sceneContext.setupUsingXMLFile();
	sceneDepth.setup(&sceneContext);
	sceneImage.setup(&sceneContext);
	sceneDepth2.setup(&sceneContext);
	sceneDepth2.setDepthColoring(COLORING_GREY);
	
	sceneUser.setup(&sceneContext);
	sceneUser.setUseMaskPixels(true);
	sceneUser.setMaxNumberOfUsers(nrBody);				// use this to set dynamic max number of users (NB: that a hard upper limit is defined by MAX_NUMBER_USERS in ofxUserGenerator)

	sceneHandTracker.setup(&sceneContext, nrHand);
	sceneHandTracker.stopTrackHands();

	sceneContext.toggleMirror();*/

	
	ofSetLogLevel(OF_LOG_NOTICE);
    
    numDevices = openNIDevices[0].getNumDevices();
    
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        //openNIDevices[deviceID].setLogLevel(OF_LOG_VERBOSE); // ofxOpenNI defaults to ofLogLevel, but you can force to any level
        openNIDevices[deviceID].setup();
        openNIDevices[deviceID].addDepthGenerator();
        //openNIDevices[deviceID].addImageGenerator();
		openNIDevices[deviceID].addInfraGenerator();
        openNIDevices[deviceID].addUserGenerator();
		openNIDevices[deviceID].addGestureGenerator();
		openNIDevices[deviceID].addHandsGenerator();
        openNIDevices[deviceID].setRegister(true);
        openNIDevices[deviceID].setMirror(true);
		openNIDevices[deviceID].start();
    }
    
    // NB: Only one device can have a user generator at a time - this is a known bug in NITE due to a singleton issue
    // so it's safe to assume that the fist device to ask (ie., deviceID == 0) will have the user generator...
    
    openNIDevices[0].setMaxNumUsers(nrBody); // default is 4
    ofAddListener(openNIDevices[0].userEvent, this, &kinectApp::userEvent);
  
    sceneUser.setUseMaskTexture(true);
    //sceneUser.setUsePointCloud(true);
    //sceneUser.setPointCloudDrawSize(2); // this is the size of the glPoint that will be drawn for the point cloud
    //sceneUser.setPointCloudResolution(2); // this is the step size between points for the cloud -> eg., this sets it to every second point
    openNIDevices[0].setBaseUserClass(sceneUser); // this becomes the base class on which tracked users are created
                                             // allows you to set all tracked user properties to the same type easily
                                             // and allows you to create your own user class that inherits from ofxOpenNIUser
	
	openNIDevices[0].addAllHandFocusGestures();
	//ofAddListener(openNIDevices[0].gestureEvent, this, &kinectApp::gestureEvent);
	//ofAddListener(openNIDevices[0].handEvent, this, &kinectApp::handEvent);

	openNIDevices[0].setMaxNumHands(nrHand);

	/*int num;
	num = openNIDevices[0].getMaxNumHands();
	cout << "maxNumHands: " << ofToString(num) << endl;*/


	//sceneHandTracker.isTracking();
	//openNIDevices[0].setBaseHandClass(sceneHandTracker);
	
	width = 640; //openNIDevices[0].getWidth();
	height = 480; //openNIDevices[0].getHeight();

	kinectImage.allocate(width, height);

	for (int i = 0; i < nrBody; i++){
		userImg[i].allocate(width, height);
	}
}

//--------------------------------------------------------------
void kinectApp::update(){

#ifdef TARGET_OSX // only working on Mac at the moment
	hardware.update();
#endif

	if (isLive) {
		
		/*
		// update all nodes
		sceneContext.update(); // end of this function call throws the ERROR "Could not open file mapping object" after running line 5934 in XnCppWrapper.h
		sceneDepth.update();  
		sceneImage.update();*/

		for (int deviceID = 0; deviceID < numDevices; deviceID++){
			openNIDevices[deviceID].update();
			//cout << "x: " << ofToString(sceneUser.getCenter().x) << "  y: " << ofToString(sceneUser.getCenter().y) << "  z: " << ofToString(sceneUser.getCenter().z) <<endl;
		}

		// Calculate FPS of Camera
		frames++;
		float time = ofGetElapsedTimeMillis();
		if (time > (lastFPSlog + 1000)) {
			fps = frames;
			frames = 0;
			lastFPSlog = time;
		}//End calculation

		/*
		if (objects){	
			sceneDepth2.update();
			//pixels();

			unsigned char * sourcePixels = sceneDepth.getDepthPixels(nearThreshold, farThreshold);	
			kinectImage.setFromPixels(sourcePixels, width, height);
			//kinectImage.flagImageChanged();
			processedImg = kinectImage;

			//processedImg = filteredImg;
			
			filters->apply(processedImg);

			contourFinder.findContours(processedImg, minBlobSize, maxBlobSize, nrObjects, false);

			objectGenerator();		
		}*/

		// update tracking nodes
		// demo getting pixels from user gen
		
		if (skel){
			if((bool)openNIDevices[0].getUserGenerator().IsGenerating() == false) { openNIDevices[0].getUserGenerator().StartGenerating(); }
			int numUsers = openNIDevices[0].getNumTrackedUsers();
			for (int nID = 0; nID < numUsers; nID++){				
				userImg[nID].getTextureReference() = openNIDevices[0].getTrackedUser(nID).getMaskTextureReference();
			}	
		}
		else if (!skel){
			if((bool)openNIDevices[0].getUserGenerator().IsGenerating() == true) { openNIDevices[0].getUserGenerator().StopGenerating(); }
		}

		communicateViaOsc();

	}

}

//--------------------------------------------------------------
void kinectApp::draw(){
	
	drawBack();

	ofSetColor(255, 255, 255);

	if (isLive) {
		
		drawCamView();

		if (hands) { drawAllHands(); }

		if (skel) { drawSkeletons(); }

		//if (objects == true) { drawObjects(); }

		drawDetails();

	}

	string statusHardware;
#ifdef TARGET_OSX // only working on Mac at the moment
	ofPoint statusAccelerometers = hardware.getAccelerometers();
	stringstream	statusHardwareStream;

	statusHardwareStream
	<< "ACCELEROMETERS:"
	<< " TILT: " << hardware.getTiltAngle() << "/" << hardware.tilt_angle
	<< " x - " << statusAccelerometers.x
	<< " y - " << statusAccelerometers.y
	<< " z - " << statusAccelerometers.z;

	statusHardware = statusHardwareStream.str();
#endif

	ofSetColor (255, 255, 255);
	usedFont.setLineHeight(14);

	statusConnection = (string)(isLive ? "On" : "Off");
	stringstream msgA;
	msgA
	//<< "FrameRate: " << ofToString(int(ofGetFrameRate())) << "  " << statusHardware << endl
	<< "FrameRate: " << ofToString(fps) << "  " << statusHardware << endl
	//<< endl
	<< "Connection: " << statusConnection << endl
	//<< endl
	<< "Settings: " << statusNetwork << endl;
	usedFont.drawString(msgA.str(), 850, 30);
	
	/*if (isLive && hands) { statusHands = ofToString(sceneHandTracker.getNumTrackedHands()); }
	else { statusHands = "0"; }*/
	
	for (int deviceID = 0; deviceID < numDevices; deviceID++){
		if (isLive && skel) { statusSkeletons = ofToString(openNIDevices[0].getNumTrackedUsers()); }
		else { statusSkeletons = "0"; }
		if (isLive && hands) { statusHands = ofToString(openNIDevices[0].getNumTrackedHands()); }
		else { statusHands = "0"; }
	}
	/*if (isLive && objects) { statusObjects = ofToString(contourFinder.blobs.size()); }
	else { statusObjects = "0"; }
	stringstream msgB;
	msgB
	<< "( Hands: " << statusHands << " ) ( Skeletons: " << statusSkeletons << " ) ( Objects: " << statusObjects << " )" << endl;
	usedFont.drawString(msgB.str(), 690, 820);*/

	stringstream msgB;
	msgB
	<< "( Hands: " << statusHands << " ) ( Skeletons: " << statusSkeletons << " )" << endl;
	usedFont.drawString(msgB.str(), 690, 820);

	stringstream msgC;
	msgC
	<< "Host: " << host << endl
	<< "Port: " << ofToString(port) << endl;
	usedFont.drawString(msgC.str(), 850, 85);
	
}

//--------------------------------------------------------------
void kinectApp::drawBack(){

	// gradient as background
	glBegin(GL_QUADS);  
	glColor3f( 0.40f, 0.40f, 0.40f );  //79
	glVertex3f( 0.0f, 0.0f, 0.0f );  
	glVertex3f( ofGetWidth(), 0.0f, 0.0f );  
	glColor3f( 0.13f, 0.13f, 0.13f );  //37
	glVertex3f( ofGetWidth(), ofGetHeight(), 0.0f );  
	glVertex3f( 0.0f, ofGetHeight(), 0.0f );  
	glEnd();
	
	// interface image
	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255);
	background.draw(0, 0);
	ofDisableAlphaBlending();

}

//--------------------------------------------------------------
void kinectApp::drawCamView(){

	glPushMatrix();
	ofTranslate (15, 35);
	glScalef(0.5, 0.5, 0.5);

	if(ir == true){
		//sceneImage.draw(0, 0, 640, 480);
		openNIDevices[0].drawImage(0, 0, 640, 480);
	}

	if(depth == true){
		//sceneDepth.draw(0, 0, 640, 480);
		openNIDevices[0].drawDepth(0, 0, 640, 480);
	}	
		
	if(off == true){ 
		ofFill();
		ofSetColor(0, 0, 0);
		ofRect(0, 0, 640, 480);
	}

	glPopMatrix();

}

//--------------------------------------------------------------
void kinectApp::drawAllHands(){

	glPushMatrix();
	ofTranslate (15, 35);
	glScalef(0.5, 0.5, 0.5);
	//sceneHandTracker.drawHands();
	for (int deviceID = 0; deviceID < numDevices; deviceID++){
		openNIDevices[deviceID].drawHands(0, 0, 640, 480);
	}
	glPopMatrix();

	ofSetColor(255, 255, 255);
	stringstream msgHand[8];
	int msgHY[8];
	for (int i = 0; i < nrHand; i++){
		if ((int)aHand[i] > 1 || (int)aHand[i] < 0) { aHand[i] = 0; }
		if ((int)bHand[i] > 1 || (int)bHand[i] < 0) { bHand[i] = 0; }
		if ((int)cHand[i] > 1 || (int)cHand[i] < 0) { cHand[i] = 0; }
		msgHY[i] = 50+(i*20);
		msgHand[i]
			<< "HandNr " << ofToString(idHand[i]) << " : ( " << ofToString(aHand[i], 3) << " | " << ofToString(bHand[i], 3) << " | " << ofToString(cHand[i], 3) << " )" << endl;
		usedFont.drawString(msgHand[i].str(), 1000, msgHY[i]);
	}
}

//--------------------------------------------------------------
void kinectApp::drawSkeletons(){
	
	ofPushMatrix();
	ofTranslate (15, 35);
	ofScale(0.5, 0.5, 0.5);
	
	for (int deviceID = 0; deviceID < numDevices; deviceID++){
		openNIDevices[deviceID].drawSkeletons(0, 0, 640, 480);
	}

	ofSetColor(255, 255, 255);
	ofFill();
	ofRect(0, 560, 640, 480);

	ofRect(2, 1078, 312, 232);
	ofRect(326, 1078, 312, 232);
	ofRect(2, 1346, 312, 232);
	ofRect(326, 1346, 312, 232);
	
	int numUsers = openNIDevices[0].getNumTrackedUsers();

	ofPushMatrix();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	
	for (int nID = 0; nID < numUsers; nID++){
		if (numUsers == 1){ userImg[0].draw(2, 1078, 312, 232); }
		if (numUsers == 2){ userImg[0].draw(2, 1078, 312, 232); userImg[1].draw(326, 1078, 312, 232); }
		if (numUsers == 3){ userImg[0].draw(2, 1078, 312, 232); userImg[1].draw(326, 1078, 312, 232); userImg[2].draw(2, 1346, 312, 232); }
		if (numUsers == 4){ userImg[0].draw(2, 1078, 312, 232); userImg[1].draw(326, 1078, 312, 232); userImg[2].draw(2, 1346, 312, 232); userImg[3].draw(326, 1346, 312, 232); }
    }
    ofDisableBlendMode();
    ofPopMatrix();

	ofPopMatrix();

	ofSetColor(255, 255, 255);
	stringstream msgBody[4];
	int msgBY[4];
	for (int i = 0; i < nrBody; i++){
		if ((int)aBody[i] > 1 || (int)aBody[i] < 0) { aBody[i] = 0; }
		if ((int)bBody[i] > 1 || (int)bBody[i] < 0) { bBody[i] = 0; }
		if ((int)cBody[i] > 1 || (int)cBody[i] < 0) { cBody[i] = 0; }
		msgBY[i] = 240+(i*20);
		msgBody[i]
			<< "BodyNr " << ofToString(idBody[i]) << " : ( " << ofToString(aBody[i], 3) << " | " << ofToString(bBody[i], 3) << " | " << ofToString(cBody[i], 3) << " )" << endl;
		usedFont.drawString(msgBody[i].str(), 1000, msgBY[i]);
	}
			
}

//--------------------------------------------------------------
/*
void kinectApp::drawObjects(){
	
	glPushMatrix();
	ofTranslate (15, 35);
	glScalef(0.5, 0.5, 0.5);

	contourFinder.draw(0, 0, 640, 480);

	ofSetColor(255, 255, 255);
	filters->draw();
	//kinectImage.draw(682, 1078, 312, 232);
		
	glPopMatrix();

	ofSetColor(255, 255, 255);
	stringstream msgObject[20];
	int msgOY[20];
	for (int i = 0; i < nrObjects; i++){
		msgOY[i] = 350+(i*20);
		msgObject[i]
			//<< "ObjectNr " << ofToString(idObject[i]) << " : ( " << ofToString(aObject[i], 3) << " | " << ofToString(bObject[i], 3) << " | " << ofToString(cObject[i], 3) << " | " << ofToString(cObject2[i], 3) << " )" << endl;
			<< "ObjectNr " << ofToString(idObject[i]) << " : ( " << ofToString(aObject[i], 3) << " | " << ofToString(bObject[i], 3) << " | " << ofToString(cObject[i], 3) << " )" << endl;
		usedFont.drawString(msgObject[i].str(), 1000, msgOY[i]);
	}
}
*/

//--------------------------------------------------------------
void kinectApp::drawDetails() {
	
	ofPushMatrix();
	ofTranslate (15, 35);
	
	if (!hands) { for (int i = 0; i < nrHand; i++){
		idHand[i] = 0; aHand[i] = 0; bHand[i] = 0; cHand[i] = 0; } 
	}
	else if(hands){
		for (int i = 0; i < openNIDevices[0].getNumTrackedHands(); i++){ // i < sceneHandTracker.tracked_hands.size();
			idHand[i] = openNIDevices[0].getTrackedHand(i).getID(); //sceneHandTracker.getID(); //(int)sceneHandTracker.tracked_hands[i]->nID; 
			//aHand[i] = sceneHandTracker.tracked_hands[i]->progPos.x;
			//bHand[i] = sceneHandTracker.tracked_hands[i]->progPos.y;
			//cHand[i] = sceneHandTracker.tracked_hands[i]->progPos.z;
			aHand[i] = openNIDevices[0].getTrackedHand(i).getPosition().x / width;
			bHand[i] = openNIDevices[0].getTrackedHand(i).getPosition().y / height;
			cHand[i] = openNIDevices[0].getTrackedHand(i).getPosition().z / 10000;
	
			//if ((int)sceneHandTracker.tracked_hands[i]->nID > 99 || (int)sceneHandTracker.tracked_hands[i]->nID < 0) { idHand[i] = 0; }
			
			//if ((int)sceneHandTracker.tracked_hands[i]->progPos.x > 1 || (int)sceneUser.tracked_users[i]->progPos.x < 0) { aHand[i] = 0; }
			//if ((int)sceneHandTracker.tracked_hands[i]->progPos.y > 1 || (int)sceneUser.tracked_users[i]->progPos.y < 0) { bHand[i] = 0; }
			//if ((int)sceneHandTracker.tracked_hands[i]->progPos.z > 1 || (int)sceneUser.tracked_users[i]->progPos.z < 0) { cHand[i] = 0; }
									
			stringstream hID[8];
			ofSetColor(102, 153, 204); 
			hID[i] << ofToString(idHand[i]) << endl;
			if (idHand[i] < 10) usedFont.drawString(hID[i].str(), (openNIDevices[0].getTrackedHand(i).getPosition().x)*0.5-4, (openNIDevices[0].getTrackedHand(i).getPosition().y)*0.5+5);
			
			if (idHand[i] >= 10) usedFont.drawString(hID[i].str(), (openNIDevices[0].getTrackedHand(i).getPosition().x)*0.5-8, (openNIDevices[0].getTrackedHand(i).getPosition().y)*0.5+5);ofSetColor(255, 255, 255);		
		}
	}
	
	if (!skel) { for (int i = 0; i < nrBody; i++){ 
		idBody[i] = 0; aBody[i] = 0; bBody[i] = 0; cBody[i] = 0; } 
	}
	else if(skel){
		for (int i = 0; i < openNIDevices[0].getNumTrackedUsers(); i++){
			idBody[i] = openNIDevices[0].getTrackedUser(i).getXnID(); //i+1;
			aBody[i] = openNIDevices[0].getTrackedUser(i).progPos.x;
			bBody[i] = openNIDevices[0].getTrackedUser(i).progPos.y;
			cBody[i] = openNIDevices[0].getTrackedUser(i).progPos.z;

			stringstream sID[4];
			ofSetColor(175, 175, 175);
			sID[i] << ofToString(idBody[i]) << endl;
			usedFont.drawString(sID[i].str(), (openNIDevices[0].getTrackedUser(i).getLimb(LIMB_NECK).getStartJoint().getProjectivePosition().x)*0.5-5, (openNIDevices[0].getTrackedUser(i).getLimb(LIMB_NECK).getStartJoint().getProjectivePosition().y)*0.5-10);
			ofSetColor(255, 255, 255);
		}
	}
	
	/*
	if (!objects) { for (int i = 0; i < contourFinder.blobs.size(); i++){
		idObject[i] = 0; aObject[i] = 0; bObject[i] = 0; cObject[i] = 0; } //cObject2[i] = 0; 
	}
	if(objects == true){
		for (int i = 0; i < contourFinder.blobs.size(); i++){ // i < nrObjects; ?
			idObject[i] = i+1;
			aObject[i] = objectX[i] / width;
			bObject[i] = objectY[i] / height;
			cObject[i] = objectZ[i] / 10000; 
			//cObject2[i] = objectZ2[i] / 10000; 

			//if ((int)idObject[i] > nrObjects || (int)idObject[i] < 0) { idObject[i] = 0; }
			if ((int)aObject[i] > 1 || (int)aObject[i] < 0) { aObject[i] = 0; }
			if ((int)bObject[i] > 1 || (int)bObject[i] < 0) { bObject[i] = 0; }
			if ((int)cObject[i] > 1 || (int)cObject[i] < 0) { cObject[i] = 0; }
			//if ((int)cObject2[i] > 1 || (int)cObject2[i] < 0) { cObject2[i] = 0; }

			if (contourFinder.blobs.size() > 0){
				stringstream oID[20];
				ofSetColor(204, 204, 204);
				oID[i] << ofToString(idObject[i]) << endl;
				usedFont.drawString(oID[i].str(), objectX[i]*0.5-5, objectY[i]*0.5+2);
				ofSetColor(255, 255, 255);				
			}
		}
	}*/

	ofPopMatrix();

}

//--------------------------------------------------------------
/*
void kinectApp::objectGenerator(){

	unsigned char * rawPixels = sceneDepth.getDepthPixels(0, sceneDepth.getMaxDepth());
	sourceImg.setFromPixels(rawPixels, width, height);
	sourceImg.flagImageChanged();

	for (int i = 0; i < contourFinder.blobs.size(); i++) { //blobs.size() is not equal to nrObjects
				blobImg[i] = sourceImg;
				singleSourceImg[i] = sourceImg;

				objectX[i] = contourFinder.blobs[i].centroid.x;
				objectY[i] = contourFinder.blobs[i].centroid.y;
				
				blobImg[i].drawBlobIntoMe(contourFinder.blobs[i], 0);
				
				singleSourceImg[i].absDiff(sourceImg, blobImg[i]);
				
				singlePixels[i] = singleSourceImg[i].getPixels();
				objPix[i].setFromPixels(singlePixels[i], 640, 480, OF_IMAGE_GRAYSCALE);

				if (location == fullsize ){ location = 0; }
				else { location++; }

				grayVal = objPix[i][location];

				if (grayVal > 0) { colOfPix[i].set(grayVal); objectZ[i] = ofMap(colOfPix[i].r, 0, 255, 10000, 0);}
				else if (objPix[i].getColor(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y).r > 0) {
					colOfPix[i].set(objPix[i].getColor(contourFinder.blobs[i].centroid.x, contourFinder.blobs[i].centroid.y)); 
					objectZ[i] = ofMap(colOfPix[i].r, 0, 255, 10000, 0);
				}

				ofSetColor(255, 255, 255);	
				
				objectX[i] = contourFinder.blobs[i].centroid.x;
				objectY[i] = contourFinder.blobs[i].centroid.y;
	}
}
*/

//--------------------------------------------------------------
void kinectApp::communicateViaOsc(){
	
	/*if(hands){
		ofxOscMessage oscHands;
		oscHands.setAddress("/hands/start");
		sender.sendMessage(oscHands);

		for (int i = 0; i < sceneHandTracker.tracked_hands.size(); i++){
			if (sceneHandTracker.tracked_hands[i]->isBeingTracked){
				if (oscHands01){				
					int id = idHand[i];
					float x = aHand[i];
					float y = bHand[i];
					float z = cHand[i];

					ofxOscMessage oscHands01;
					oscHands01.setAddress("/hands/ID-XYZ");
					oscHands01.addIntArg(id);
					oscHands01.addFloatArg(x);
					oscHands01.addFloatArg(y);
					oscHands01.addFloatArg(z);
					sender.sendMessage(oscHands01);
				}
			}
		}
	}*/


	if(skel){
		ofxOscMessage oscSkeletons;
		oscSkeletons.setAddress("/skeletons/start");
		sender.sendMessage(oscSkeletons);

		for (int i = 0; i < openNIDevices[0].getNumTrackedUsers(); i++){
			if (openNIDevices[0].getTrackedUser(i).isFound() == true){
				if(oscSkeletons01){
					int id = idBody[i];
					float x = aBody[i];
					float y = bBody[i];
					float z = cBody[i];

					ofxOscMessage oscSkeletons01;
					oscSkeletons01.setAddress("/skeletons/ID-XYZ");
					oscSkeletons01.addIntArg(id);
					oscSkeletons01.addFloatArg(x);
					oscSkeletons01.addFloatArg(y);
					oscSkeletons01.addFloatArg(z);
					sender.sendMessage(oscSkeletons01);
				}
				
				if(oscSkeletons02){
					float neck01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_NECK).getStartJoint().getProjectivePosition().x / width;
					float neck01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_NECK).getStartJoint().getProjectivePosition().y / height;
					
					float neck02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_NECK).getEndJoint().getProjectivePosition().x / width;
					float neck02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_NECK).getEndJoint().getProjectivePosition().y / height;
					float shoulderLeft01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_SHOULDER).getStartJoint().getProjectivePosition().x / width;
					float shoulderLeft01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_SHOULDER).getStartJoint().getProjectivePosition().y / height;
					float shoulderLeft02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_SHOULDER).getEndJoint().getProjectivePosition().x / width;
					float shoulderLeft02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_SHOULDER).getEndJoint().getProjectivePosition().y / height;
					float shoulderRight01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_SHOULDER).getStartJoint().getProjectivePosition().x / width;
					float shoulderRight01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_SHOULDER).getStartJoint().getProjectivePosition().y / height;
					float shoulderRight02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_SHOULDER).getEndJoint().getProjectivePosition().x / width;
					float shoulderRight02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_SHOULDER).getEndJoint().getProjectivePosition().y / height;
					float upArmLeft01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_ARM).getStartJoint().getProjectivePosition().x / width;
					float upArmLeft01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_ARM).getStartJoint().getProjectivePosition().y / height;
					float upArmLeft02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_ARM).getEndJoint().getProjectivePosition().x / width;
					float upArmLeft02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_ARM).getEndJoint().getProjectivePosition().y / height;
					float upArmRight01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_ARM).getStartJoint().getProjectivePosition().x / width;
					float upArmRight01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_ARM).getStartJoint().getProjectivePosition().y / height;
					float upArmRight02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_ARM).getEndJoint().getProjectivePosition().x / width;
					float upArmRight02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_ARM).getEndJoint().getProjectivePosition().y / height;
					float lowArmLeft01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_ARM).getStartJoint().getProjectivePosition().x / width;
					float lowArmLeft01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_ARM).getStartJoint().getProjectivePosition().y / height;
					float lowArmLeft02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_ARM).getEndJoint().getProjectivePosition().x / width;
					float lowArmLeft02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_ARM).getEndJoint().getProjectivePosition().y / height;
					float lowArmRight01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_ARM).getStartJoint().getProjectivePosition().x / width;
					float lowArmRight01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_ARM).getStartJoint().getProjectivePosition().y / height;
					float lowArmRight02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_ARM).getEndJoint().getProjectivePosition().x / width;
					float lowArmRight02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_ARM).getEndJoint().getProjectivePosition().y / height;
					float upTorsoLeft01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_TORSO).getStartJoint().getProjectivePosition().x / width;
					float upTorsoLeft01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_TORSO).getStartJoint().getProjectivePosition().y / height;
					float upTorsoLeft02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_TORSO).getEndJoint().getProjectivePosition().x / width;
					float upTorsoLeft02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_TORSO).getEndJoint().getProjectivePosition().y / height;
					float upTorsoRight01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_TORSO).getStartJoint().getProjectivePosition().x / width;
					float upTorsoRight01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_TORSO).getStartJoint().getProjectivePosition().y / height;
					float upTorsoRight02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_TORSO).getEndJoint().getProjectivePosition().x / width;
					float upTorsoRight02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_TORSO).getEndJoint().getProjectivePosition().y / height;
					float lowTorsoLeft01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_TORSO).getStartJoint().getProjectivePosition().x / width;
					float lowTorsoLeft01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_TORSO).getStartJoint().getProjectivePosition().y / height;
					float lowTorsoLeft02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_TORSO).getEndJoint().getProjectivePosition().x / width;
					float lowTorsoLeft02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_TORSO).getEndJoint().getProjectivePosition().y / height;
					float lowTorsoRight01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_TORSO).getStartJoint().getProjectivePosition().x / width;
					float lowTorsoRight01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_TORSO).getStartJoint().getProjectivePosition().y / height;
					float lowTorsoRight02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_TORSO).getEndJoint().getProjectivePosition().x / width;
					float lowTorsoRight02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_TORSO).getEndJoint().getProjectivePosition().y / height;
					float hip01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_PELVIS).getStartJoint().getProjectivePosition().x / width;
					float hip01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_PELVIS).getStartJoint().getProjectivePosition().y / height;
					float hip02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_PELVIS).getEndJoint().getProjectivePosition().x / width;
					float hip02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_PELVIS).getEndJoint().getProjectivePosition().y / height;
					float upLegLeft01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_LEG).getStartJoint().getProjectivePosition().x / width;
					float upLegLeft01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_LEG).getStartJoint().getProjectivePosition().y / height;
					float upLegLeft02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_LEG).getEndJoint().getProjectivePosition().x / width;
					float upLegLeft02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_LEG).getEndJoint().getProjectivePosition().y / height;
					float upLegRight01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_LEG).getStartJoint().getProjectivePosition().x / width;
					float upLegRight01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_LEG).getStartJoint().getProjectivePosition().y / height;
					float upLegRight02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_LEG).getEndJoint().getProjectivePosition().x / width;
					float upLegRight02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_LEG).getEndJoint().getProjectivePosition().y / height;
					float lowLegLeft01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_LEG).getStartJoint().getProjectivePosition().x / width;
					float lowLegLeft01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_LEG).getStartJoint().getProjectivePosition().y / height;
					float lowLegLeft02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_LEG).getEndJoint().getProjectivePosition().x / width;
					float lowLegLeft02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_LEG).getEndJoint().getProjectivePosition().y / height;
					float lowLegRight01x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_LEG).getStartJoint().getProjectivePosition().x / width;
					float lowLegRight01y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_LEG).getStartJoint().getProjectivePosition().y / height;;
					float lowLegRight02x = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_LEG).getEndJoint().getProjectivePosition().x / width;
					float lowLegRight02y = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_LEG).getEndJoint().getProjectivePosition().y / height;
					
					ofxOscBundle oscSkeletons02;

					float neck01z = (float)openNIDevices[0].getTrackedUser(i).getLimb(LIMB_NECK).getStartJoint().getProjectivePosition().z ;
					float max_depth = openNIDevices[0].getDepthGenerator().GetDeviceMaxDepth();
					float depth_z = neck01z / max_depth;
				
					ofxOscMessage neckShoulder;
					neckShoulder.setAddress("/skeletons/bones/neckShoulder");
					neckShoulder.addFloatArg(neck01x);
					neckShoulder.addFloatArg(neck01y);
					//neckShoulder.addFloatArg(neck01z);
					neckShoulder.addFloatArg(neck02x);
					neckShoulder.addFloatArg(neck02y);
					neckShoulder.addFloatArg(shoulderLeft01x);
					neckShoulder.addFloatArg(shoulderLeft01y);
					neckShoulder.addFloatArg(shoulderLeft02x);
					neckShoulder.addFloatArg(shoulderLeft02y);
					neckShoulder.addFloatArg(shoulderRight01x);
					neckShoulder.addFloatArg(shoulderRight01y);
					neckShoulder.addFloatArg(shoulderRight02x);
					neckShoulder.addFloatArg(shoulderRight02y);
					oscSkeletons02.addMessage(neckShoulder);

					ofxOscMessage leftArm;
					leftArm.setAddress("/skeletons/bones/leftArm");
					leftArm.addFloatArg(upArmLeft01x);
					leftArm.addFloatArg(upArmLeft01y);
					leftArm.addFloatArg(upArmLeft02x);
					leftArm.addFloatArg(upArmLeft02y);
					leftArm.addFloatArg(lowArmLeft01x);
					leftArm.addFloatArg(lowArmLeft01y);
					leftArm.addFloatArg(lowArmLeft02x);
					leftArm.addFloatArg(lowArmLeft02y);
					oscSkeletons02.addMessage(leftArm);

					ofxOscMessage rightArm;
					rightArm.setAddress("/skeletons/bones/rightArm");
					rightArm.addFloatArg(upArmRight01x);
					rightArm.addFloatArg(upArmRight01y);
					rightArm.addFloatArg(upArmRight02x);
					rightArm.addFloatArg(upArmRight02y);
					rightArm.addFloatArg(lowArmRight01x);
					rightArm.addFloatArg(lowArmRight01y);
					rightArm.addFloatArg(lowArmRight02x);
					rightArm.addFloatArg(lowArmRight02y);
					oscSkeletons02.addMessage(rightArm);
				
					ofxOscMessage torsoHip;
					torsoHip.setAddress("/skeletons/bones/torsoHip");
					torsoHip.addFloatArg(upTorsoLeft01x);
					torsoHip.addFloatArg(upTorsoLeft01y);
					torsoHip.addFloatArg(upTorsoLeft02x);
					torsoHip.addFloatArg(upTorsoLeft02y);
					torsoHip.addFloatArg(upTorsoRight01x);
					torsoHip.addFloatArg(upTorsoRight01y);
					torsoHip.addFloatArg(upTorsoRight02x);
					torsoHip.addFloatArg(upTorsoRight02y);
					torsoHip.addFloatArg(lowTorsoLeft01x);
					torsoHip.addFloatArg(lowTorsoLeft01y);
					torsoHip.addFloatArg(lowTorsoLeft02x);
					torsoHip.addFloatArg(lowTorsoLeft02y);
					torsoHip.addFloatArg(lowTorsoRight01x);
					torsoHip.addFloatArg(lowTorsoRight01y);
					torsoHip.addFloatArg(lowTorsoRight02x);
					torsoHip.addFloatArg(lowTorsoRight02y);
					torsoHip.addFloatArg(hip01x);
					torsoHip.addFloatArg(hip01y);
					torsoHip.addFloatArg(hip02x);
					torsoHip.addFloatArg(hip02y);
					oscSkeletons02.addMessage(torsoHip);
					
					ofxOscMessage leftLeg;
					leftLeg.setAddress("/skeletons/bones/leftLeg");
					leftLeg.addFloatArg(upLegLeft01x);
					leftLeg.addFloatArg(upLegLeft01y);
					leftLeg.addFloatArg(upLegLeft02x);
					leftLeg.addFloatArg(upLegLeft02y);
					leftLeg.addFloatArg(lowLegLeft01x);
					leftLeg.addFloatArg(lowLegLeft01y);
					leftLeg.addFloatArg(lowLegLeft02x);
					leftLeg.addFloatArg(lowLegLeft02y);
					oscSkeletons02.addMessage(leftLeg);
					
					ofxOscMessage rightLeg;
					rightLeg.setAddress("/skeletons/bones/rightLeg");
					rightLeg.addFloatArg(upLegRight01x);
					rightLeg.addFloatArg(upLegRight01y);
					rightLeg.addFloatArg(upLegRight02x);
					rightLeg.addFloatArg(upLegRight02y);
					rightLeg.addFloatArg(lowLegRight01x);
					rightLeg.addFloatArg(lowLegRight01y);
					rightLeg.addFloatArg(lowLegRight02x);
					rightLeg.addFloatArg(lowLegRight02y);
					oscSkeletons02.addMessage(rightLeg);
					
					sender.sendBundle(oscSkeletons02);
				}
			}
		}
	}

	/*if(objects){
		ofxOscMessage oscObjects;
		oscObjects.setAddress("/objects/start");
		sender.sendMessage(oscObjects);

		for (int i = 0; i < contourFinder.blobs.size(); i++){ 
			if(oscObjects01){
				int id = idObject[i];
				float x = aObject[i];
				float y = bObject[i];
				float z = cObject[i];

				ofxOscMessage oscObjects01;
				oscObjects01.setAddress("/objects/ID-XYZ");
				oscObjects01.addIntArg(id);
				oscObjects01.addFloatArg(x);
				oscObjects01.addFloatArg(y);
				oscObjects01.addFloatArg(z);
				sender.sendMessage(oscObjects01);
			}

			if(oscObjects02){
				float w = contourFinder.blobs[i].boundingRect.width / width;
				float h = contourFinder.blobs[i].boundingRect.height / height;
				float a = contourFinder.blobs[i].area / (width*height);

				ofxOscMessage oscObjects02;
				oscObjects02.setAddress("/objects/width-height-size");
				oscObjects02.addFloatArg(w);
				oscObjects02.addFloatArg(h);
				oscObjects02.addFloatArg(a);
				sender.sendMessage(oscObjects02);
			}
		}
	}*/
}

//--------------------------------------------------------------
void kinectApp::userEvent(ofxOpenNIUserEvent & event){
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
/*void kinectApp::gestureEvent(ofxOpenNIGestureEvent & event){
    ofLogNotice() << event.gestureName << getGestureStatusAsString(event.gestureStatus) << "from device" << event.deviceID << "at" << event.timestampMillis;
}*/

//--------------------------------------------------------------

//--------------------------------------------------------------
void kinectApp::handEvent(ofxOpenNIHandEvent & event){
    ofLogNotice() << event.id << getHandStatusAsString(event.handStatus) << "from device" << event.deviceID << "at" << event.timestampMillis;
}

void kinectApp::exit(){
    // this often does not work -> it's a known bug -> but calling it on a key press or anywhere that isnt std::aexit() works
    // press 'x' to shutdown cleanly...
    for (int deviceID = 0; deviceID < numDevices; deviceID++){
        openNIDevices[deviceID].stop();
    }
}

//--------------------------------------------------------------
void kinectApp::keyPressed(int key){

	switch (key) {
#ifdef TARGET_OSX // only working on Mac at the moment
		case 357: // up key
			hardware.setTiltAngle(hardware.tilt_angle++);
			break;
		case 359: // down key
			hardware.setTiltAngle(hardware.tilt_angle--);
			break;
#endif

	}
}

//--------------------------------------------------------------
void kinectApp::keyReleased(int key){

}

//--------------------------------------------------------------
void kinectApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void kinectApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void kinectApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void kinectApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void kinectApp::windowResized(int w, int h){

}

