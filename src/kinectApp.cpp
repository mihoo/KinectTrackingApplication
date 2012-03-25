#include "kinectApp.h"
#include "gui.h"
#include "filter.h"


//--------------------------------------------------------------
void kinectApp::setup() {
	
	ofSetWindowTitle("KinectA");


	//-- initialize generals --// 
	width = 640;
	height = 480; 

	nrHand = 8;
	nrBody = 4;
	nrObjects = 20;	

	for (int i = 0; i < nrHand; i++){ idHand[i] = 0; }
	for (int i = 0; i < nrBody; i++){ idBody[i] = 0; }
	for (int i = 0; i < nrObjects; i++){ idObject[i] = 0; }
		
	ofBackground(50, 50, 50);
	background.loadImage("background/Interface.png");
	background.allocate(905, 730, OF_IMAGE_COLOR_ALPHA);


	//-- initialize gui classes & variables --// 
	usedFont.loadFont("fonts/Questrial-Regular.ttf", 9);
	usedFont.setLineHeight(14);

	statusConfig = "new";


	//-- initialize ofxOpenNI --// 
	isLive = true;

	setupScene();
	
	maskPixels = new unsigned char[width * height];
	kinectImage.allocate(width, height);
	kinectImage.setUseTexture(false);

	for (int i = 0; i < nrBody; i++){ userImg[i].allocate(width, height); }

	
	//-- initialize classes & variables for ofxOpenCV --// 
	filters = NULL;
	filters = new applyfilter();
	processedImg.allocate(640, 480);			
	processedImg.setUseTexture(false);			
	filters->allocate(640, 480);

	sourceImg.allocate(640, 480);				
	sourceImg.setUseTexture(false);		
	for (int i = 0; i < nrObjects; i++){
		singleSourceImg[i].allocate(640, 480);
		singleSourceImg[i].setUseTexture(false);	
		blobImg[i].allocate(640, 480);
		blobImg[i].setUseTexture(false);
	}

	fullsize = width * height;

	nearThreshold = 500;
	farThreshold  = 1000;

	minBlobSize = 1500;
	maxBlobSize = 10000;


	//-- initialize OSC --// 
	host = "127.0.0.1";
	port = 3333;
	sender.setup(host, port); 
}

//--------------------------------------------------------------
void kinectApp::setupScene() {

	ofSetLogLevel(OF_LOG_NOTICE);
    
    //openNIDevices.setLogLevel(OF_LOG_VERBOSE); // ofxOpenNI defaults to ofLogLevel, but you can force to any level
    openNIDevices.setup();
    openNIDevices.addDepthGenerator();
    //openNIDevices.addImageGenerator();
	openNIDevices.addInfraGenerator();
    openNIDevices.addUserGenerator();
	openNIDevices.addGestureGenerator();
	openNIDevices.addHandsGenerator();
	openNIDevices.addDepthThreshold(nearThreshold, farThreshold);
    openNIDevices.setRegister(true);
    openNIDevices.setMirror(true);
	openNIDevices.start();
	//openNIDevices.setDepthColoring(COLORING_GREY);
	    
    openNIDevices.setMaxNumUsers(nrBody); // default is 4
    //ofAddListener(openNIDevices.userEvent, this, &kinectApp::userEvent);
    sceneUser.setUseMaskTexture(true);
    //sceneUser.setUsePointCloud(true);
    //sceneUser.setPointCloudDrawSize(2);
    //sceneUser.setPointCloudResolution(2); 
    openNIDevices.setBaseUserClass(sceneUser); // this becomes the base class on which tracked users are created, allows you to set all tracked user properties to the same type easily
                                       
	openNIDevices.addAllHandFocusGestures();
	//ofAddListener(openNIDevices.gestureEvent, this, &kinectApp::gestureEvent);
	//ofAddListener(openNIDevices.handEvent, this, &kinectApp::handEvent);
	openNIDevices.setMaxNumHands(nrHand); // default is 1 ???

	openNIDevices.setBaseHandClass(sceneHandTracker);
	
	//width = openNIDevices.getWidth(); 
	//height = openNIDevices.getHeight();

}

//--------------------------------------------------------------
void kinectApp::update(){

	//-- update status --// 
	if(camOptions->mMouseIsDown  || trackOptions->mMouseIsDown || sendigViaOSC->mMouseIsDown || oscConfig->mMouseIsDown)
		{ statusConfig = "new"; }


	//-- update tracking nodes --// 
	if (isLive) {
		openNIDevices.update();


		//-- calculate FrameRate of camera --// 
		frames++;
		float time = ofGetElapsedTimeMillis();
		if (time > (lastFPSlog + 1000)) {
			fps = frames;
			frames = 0;
			lastFPSlog = time;
		}


		//-- update hand tracking --// 
		if (hands){
			openNIDevices.g_bIsHandsOn = true;	
			if(openNIDevices.currentTrackedHands.size() >= nrHand){
				openNIDevices.currentTrackedHands.clear();
				openNIDevices.currentTrackedHandIDs.clear();
			}
		}
		else if (!hands){ openNIDevices.g_bIsHandsOn = false; }
		

		//-- update skeleton tracking --// 
		if (skel){
			openNIDevices.g_bIsUserOn = true;					
			for (int i = 0; i < openNIDevices.getNumTrackedUsers(); i++){				
				userImg[i].getTextureReference() = openNIDevices.getTrackedUser(i).getMaskTextureReference();
			}	
		}
		else if (!skel){	
			for (int i = 0; i < openNIDevices.getNumTrackedUsers(); i++){ 
				if(openNIDevices.g_bIsUserOn == true) {
					openNIDevices.stopPoseDetection(openNIDevices.getTrackedUser(i).getXnID()); 
					openNIDevices.stopTrackingUser(openNIDevices.getTrackedUser(i).getXnID()); 
				}
			}
			openNIDevices.g_bIsUserOn = false;
		}


		//-- update object tracking --// 
		if (objects){
			if(objectTrackingOptions->mMouseIsDown || backgProcessingOptions->mMouseIsDown || imageProcessingOptions->mMouseIsDown){ statusConfig = "new"; }
			unsigned char * sourcePixels = getDepthPixels(nearThreshold, farThreshold);
			kinectImage.setFromPixels(sourcePixels, width, height);
			
			processedImg = kinectImage;				
			filters->apply(processedImg);

			contourFinder.findContours(processedImg, minBlobSize, maxBlobSize, nrObjects, false);

			objectGenerator();		
		}


		//-- communicate via OSC --// 
		communicateViaOsc();
	}
}

//--------------------------------------------------------------
void kinectApp::draw(){
	
	//-- background --// 
	drawBack(); 


	//-- show tracking results --// 
	ofSetColor(255, 255, 255);

	if (isLive) {
		drawCamView();

		if (hands) { drawAllHands(); }
		if (skel) { drawSkeletons(); }
		if (objects) { drawObjects(); }

		drawDetails();
	}


	ofSetColor (255, 255, 255);


	//-- show FrameRate --// 
	stringstream msgA;
	msgA
	//<< "FrameRate: " << ofToString(int(ofGetFrameRate())) << endl //uncomment this to get processed FrameRate
	<< "FrameRate: " << ofToString(fps) << "  " << endl;
	usedFont.drawString(msgA.str(), 16, 126);


	//-- calculate & display tracking status --// 
	if (isLive && skel) { statusSkeletons = ofToString(openNIDevices.getNumTrackedUsers()); }
	else { statusSkeletons = "0"; }
		
	if (isLive && hands) { statusHands = ofToString(openNIDevices.getNumTrackedHands()); }
	else { statusHands = "0"; }
		
	if (isLive && objects) { statusObjects = ofToString(contourFinder.blobs.size()); }
	else { statusObjects = "0"; }
	
	stringstream msgB;
	msgB
	<< "Hands: " << statusHands << endl
	<< "Skeletons: " << statusSkeletons << endl
	<< "Objects: " << statusObjects << endl;
	usedFont.drawString(msgB.str(), 136, 126);


	//-- show status of settings (new, saved, loaded or reset) --// 
	stringstream msgC;
	msgC
	<< "Settings: " << statusConfig << endl;
	usedFont.drawString(msgC.str(), 256, 126);	


	//-- show osc info --// 
	stringstream msgD;
	msgD
	<< "SEND DATA TO" << endl
	<< "Host: " << host << endl
	<< "Port: " << ofToString(port) << endl;
	usedFont.drawString(msgD.str(), 471, 126);
}

//--------------------------------------------------------------
void kinectApp::drawBack(){

	//-- gradient as background --//
	glBegin(GL_QUADS);  
	glColor3f( 0.40f, 0.40f, 0.40f ); 
	glVertex3f( 0.0f, 0.0f, 0.0f );  
	glVertex3f( ofGetWidth(), 0.0f, 0.0f );  
	glColor3f( 0.13f, 0.13f, 0.13f ); 
	glVertex3f( ofGetWidth(), ofGetHeight(), 0.0f );  
	glVertex3f( 0.0f, ofGetHeight(), 0.0f );  
	glEnd();
	

	//-- interface image --//
	ofEnableAlphaBlending();
	ofSetColor(255, 255, 255);
	background.draw(0, 0);
	ofDisableAlphaBlending();
}

//--------------------------------------------------------------
void kinectApp::drawCamView(){

	//-- differentiate between infrared-, depth- & none-view --// 
	ofPushMatrix();
	ofTranslate (15, 195);
	ofScale(0.5, 0.5, 0.5);

	if(ir == true){ openNIDevices.drawImage(0, 0, 640, 480); }

	if(depth == true){ openNIDevices.drawDepth(0, 0, 640, 480); }	
		
	if(off == true){ 
		ofFill();
		ofSetColor(0, 0, 0);
		ofRect(0, 0, 640, 480);
	}

	ofPopMatrix();
}

//--------------------------------------------------------------
void kinectApp::drawAllHands(){

	//-- show tracked hands --// 
	ofPushMatrix();
	ofTranslate (15, 195);
	ofScale(0.5, 0.5, 0.5);
	
	for (int i = 0; i < openNIDevices.getNumTrackedHands(); i++){
		if (aHand[i] > 0 && bHand[i] > 0){ openNIDevices.drawHands(0, 0, 640, 480); }
	}

	ofPopMatrix();


	//-- show coordinates of tracked hands --// 
	/*ofSetColor(255, 255, 255);
	stringstream msgHand[8];
	int msgHY[8];
	for (int i = 0; i < nrHand; i++){
		if ((int)idHand[i] >= 100) { idHand[i] = 0; }
		if ((int)aHand[i] > 1 || (int)aHand[i] < 0) { aHand[i] = 0; }
		if ((int)bHand[i] > 1 || (int)bHand[i] < 0) { bHand[i] = 0; }
		if ((int)cHand[i] > 1 || (int)cHand[i] < 0) { cHand[i] = 0; }
		msgHY[i] = 50+(i*20);
		msgHand[i]
			<< "HandNr " << ofToString(idHand[i]) << " : ( " << ofToString(aHand[i], 3) << " | " << ofToString(bHand[i], 3) << " | " << ofToString(cHand[i], 3) << " )" << endl;
		usedFont.drawString(msgHand[i].str(), 1000, msgHY[i]);
	}*/
}

//--------------------------------------------------------------
void kinectApp::drawSkeletons(){
	
	//-- show tracked skeletons --// 
	ofPushMatrix();
	ofTranslate (15, 195);
	ofScale(0.5, 0.5, 0.5);
	
	openNIDevices.drawSkeletons(0, 0, 640, 480);

	ofSetColor(255, 255, 255);
	ofFill();
	ofRect(2, 558, 312, 232);
	ofRect(326, 558, 312, 232);
	ofRect(2, 802, 312, 232);
	ofRect(326, 802, 312, 232);
	

	//-- show user masks --// 
	int numUsers = openNIDevices.getNumTrackedUsers();

	ofPushMatrix();
    ofEnableBlendMode(OF_BLENDMODE_ALPHA);
	for (int nID = 0; nID < numUsers; nID++){
		if (numUsers == 1){ userImg[0].draw(2, 558, 312, 232); }
		if (numUsers == 2){ userImg[0].draw(2, 558, 312, 232); userImg[1].draw(326, 558, 312, 232); }
		if (numUsers == 3){ userImg[0].draw(2, 558, 312, 232); userImg[1].draw(326, 558, 312, 232); userImg[2].draw(2, 802, 312, 232); }
		if (numUsers == 4){ userImg[0].draw(2, 558, 312, 232); userImg[1].draw(326, 558, 312, 232); userImg[2].draw(2, 802, 312, 232); userImg[3].draw(326, 802, 312, 232); }
    }
    ofDisableBlendMode();
    ofPopMatrix();


	ofPopMatrix();


	//-- show coordinates of tracked skeletons --//
	/*ofSetColor(255, 255, 255);
	stringstream msgBody[4];
	int msgBY[4];
	for (int i = 0; i < nrBody; i++){
		if ((int)aBody[i] > 1 || (int)aBody[i] < 0) { aBody[i] = 0; }
		if ((int)bBody[i] > 1 || (int)bBody[i] < 0) { bBody[i] = 0; }
		if ((int)cBody[i] > 1 || (int)cBody[i] < 0) { cBody[i] = 0; }
		msgBY[i] = 220+(i*20);
		msgBody[i]
			<< "BodyNr " << ofToString(idBody[i]) << " : ( " << ofToString(aBody[i], 3) << " | " << ofToString(bBody[i], 3) << " | " << ofToString(cBody[i], 3) << " )" << endl;
		usedFont.drawString(msgBody[i].str(), 1000, msgBY[i]);
	}*/			
}

//--------------------------------------------------------------
void kinectApp::drawObjects(){
	
	//-- show tracked objects & contours --// 
	ofPushMatrix();
	ofTranslate (15, 195);
	ofScale(0.5, 0.5, 0.5);

	contourFinder.draw(0, 0, 640, 480);

	ofSetColor(255, 255, 255);
	filters->draw();
		
	ofPopMatrix();

	
	//-- show coordinates of tracked hands --// 
	/*ofSetColor(255, 255, 255);
	stringstream msgObject[20];
	int msgOY[20];
	for (int i = 0; i < nrObjects; i++){
		if ((int)aObject[i] > 1 || (int)aObject[i] < 0) { aObject[i] = 0; }
		if ((int)bObject[i] > 1 || (int)bObject[i] < 0) { bObject[i] = 0; }
		if ((int)cObject[i] > 1 || (int)cObject[i] < 0) { cObject[i] = 0; }
		msgOY[i] = 310+(i*20);
		msgObject[i]
			//<< "ObjectNr " << ofToString(idObject[i]) << " : ( " << ofToString(aObject[i], 3) << " | " << ofToString(bObject[i], 3) << " | " << ofToString(cObject[i], 3) << " | " << ofToString(cObject2[i], 3) << " )" << endl;
			<< "ObjectNr " << ofToString(idObject[i]) << " : ( " << ofToString(aObject[i], 3) << " | " << ofToString(bObject[i], 3) << " | " << ofToString(cObject[i], 3) << " )" << endl;
		usedFont.drawString(msgObject[i].str(), 1000, msgOY[i]);
	}*/
}

//--------------------------------------------------------------
void kinectApp::drawDetails() {
	
	ofPushMatrix();
	ofTranslate (15, 195);
	
	//-- show details for hand tracking --// 
	if (!hands) { for (int i = 0; i < nrHand; i++){ idHand[i] = 0; aHand[i] = 0; bHand[i] = 0; cHand[i] = 0; } }
	else if(hands){
		for (int i = 0; i < openNIDevices.getNumTrackedHands(); i++){ 
			idHand[i] = openNIDevices.getTrackedHand(i).getID(); 
			aHand[i] = openNIDevices.getTrackedHand(i).getPosition().x / width;
			bHand[i] = openNIDevices.getTrackedHand(i).getPosition().y / height;
			cHand[i] = openNIDevices.getTrackedHand(i).getPosition().z / 10000;
							
			stringstream hID[8];
			ofSetColor(102, 153, 204); 
			hID[i] << ofToString(idHand[i]) << endl;
			if (idHand[i] < 10 && idHand[i] > 0) usedFont.drawString(hID[i].str(), (openNIDevices.getTrackedHand(i).getPosition().x)*0.5-3, (openNIDevices.getTrackedHand(i).getPosition().y)*0.5+4);	
			if (idHand[i] >= 10 && idHand[i] < 100) usedFont.drawString(hID[i].str(), (openNIDevices.getTrackedHand(i).getPosition().x)*0.5-7, (openNIDevices.getTrackedHand(i).getPosition().y)*0.5+4);ofSetColor(255, 255, 255);		
		}
	}
	
	//-- show details for skeleton tracking --// 
	if (!skel) { for (int i = 0; i < nrBody; i++){ idBody[i] = 0; aBody[i] = 0; bBody[i] = 0; cBody[i] = 0; } }
	else if(skel){
		for (int i = 0; i < openNIDevices.getNumTrackedUsers(); i++){
			idBody[i] = openNIDevices.getTrackedUser(i).getXnID(); //i+1;
			aBody[i] = openNIDevices.getTrackedUser(i).progPos.x;
			bBody[i] = openNIDevices.getTrackedUser(i).progPos.y;
			cBody[i] = openNIDevices.getTrackedUser(i).progPos.z;

			stringstream sID[4];
			ofSetColor(175, 175, 175);
			sID[i] << ofToString(idBody[i]) << endl;
			usedFont.drawString(sID[i].str(), (openNIDevices.getTrackedUser(i).getLimb(LIMB_NECK).getStartJoint().getProjectivePosition().x)*0.5-5, (openNIDevices.getTrackedUser(i).getLimb(LIMB_NECK).getStartJoint().getProjectivePosition().y)*0.5-10);
			ofSetColor(255, 255, 255);
		}
	}
	
	//-- show details for object tracking --// 
	if (!objects) { for (int i = 0; i < contourFinder.blobs.size(); i++){ idObject[i] = 0; aObject[i] = 0; bObject[i] = 0; cObject[i] = 0; } }
	if(objects){
		for (int i = 0; i < contourFinder.blobs.size(); i++){
			idObject[i] = i+1;
			aObject[i] = objectX[i] / width;
			bObject[i] = objectY[i] / height;
			cObject[i] = objectZ[i] / 10000; 
			
			stringstream oID[20];
			ofSetColor(204, 204, 204);
			oID[i] << ofToString(idObject[i]) << endl;
			usedFont.drawString(oID[i].str(), objectX[i]*0.5-5, objectY[i]*0.5+2);
			ofSetColor(255, 255, 255);		
		}
	}

	ofPopMatrix();
}

//--------------------------------------------------------------
unsigned char* kinectApp::getDepthPixels(int nearThreshold , int farThreshold) {
	
	//-- calculate depth pixels for further processing --// 
	const XnDepthPixel* objDepth = openNIDevices.getDepthGenerator().GetDepthMap();
	int numPixels = width * height;
	for(int i = 0; i < numPixels; i++, objDepth++) {
		if(*objDepth < farThreshold && *objDepth > nearThreshold) {
			maskPixels[i] = ofMap(*objDepth, nearThreshold, farThreshold, 255, 0);
		} 
		else { maskPixels[i] = 0; }
	}

	return maskPixels;
}

//--------------------------------------------------------------
void kinectApp::objectGenerator(){
	
	//-- assign depth pixels for processing of dimensions --// 
	unsigned char * rawPixels = getDepthPixels(0, 10000);
	sourceImg.setFromPixels(rawPixels, width, height);
	sourceImg.flagImageChanged();

	unsigned char * singlePixels[20];
	int mem[20], counter[20];


	//-- calculate dimensions of each found object --// 
	for (int i = 0; i < contourFinder.blobs.size(); i++) {
		//-- x- & y-dimension --// 
		objectX[i] = contourFinder.blobs[i].centroid.x;
		objectY[i] = contourFinder.blobs[i].centroid.y;	

		//-- calculate z-dimension --// 
		blobImg[i] = sourceImg;
		singleSourceImg[i] = sourceImg;
		blobImg[i].drawBlobIntoMe(contourFinder.blobs[i], 0);		
		singleSourceImg[i].absDiff(sourceImg, blobImg[i]);	
		singlePixels[i] = singleSourceImg[i].getPixels();
		objPix[i].setFromPixels(singlePixels[i], width, height, OF_IMAGE_GRAYSCALE);
		
		mem[i] = 0;
		counter[i] = 0;
		for (int loc = 0; loc < fullsize; loc++){ if (objPix[i][loc] != 0) { mem[i] = mem[i] + objPix[i][loc]; counter[i]++; } }
		grayVal[i] =  mem[i] / counter[i];
		objectZ[i] = ofMap(grayVal[i], 0, 255, 10000, 0);

		objPix[i].clear();
	}
	ofSetColor(255, 255, 255);	
}

//--------------------------------------------------------------
void kinectApp::communicateViaOsc(){
	
	//-- send hand data (ID, x-/y-/z-position) --//
	if(hands){
		ofxOscMessage oscHands;
		oscHands.setAddress("/hands/start");
		sender.sendMessage(oscHands);

		for (int i = 0; i < openNIDevices.getNumTrackedHands(); i++){
			if (openNIDevices.getTrackedHand(i).isTracking() == true){
				if (oscHands01){				
					int id = idHand[i];
					float x = aHand[i];
					float y = bHand[i];
					float z = cHand[i];

					ofxOscMessage oscHands01;
					oscHands01.setAddress("/hands/ID-centralXYZ");
					oscHands01.addIntArg(id);
					oscHands01.addFloatArg(x);
					oscHands01.addFloatArg(y);
					oscHands01.addFloatArg(z);
					sender.sendMessage(oscHands01);
				}
			}
		}
	}

	//-- send skeleton data --//
	if(skel){
		ofxOscMessage oscSkeletons;
		oscSkeletons.setAddress("/skeletons/start");
		sender.sendMessage(oscSkeletons);

		for (int i = 0; i < openNIDevices.getNumTrackedUsers(); i++){
			if (openNIDevices.getTrackedUser(i).isFound() == true){
				
				//-- ID, x-/y-/z-position --//
				if(oscSkeletons01){
					int id = idBody[i];
					float x = aBody[i];
					float y = bBody[i];
					float z = cBody[i];

					ofxOscMessage oscSkeletons01;
					oscSkeletons01.setAddress("/skeletons/ID-centralXYZ");
					oscSkeletons01.addIntArg(id);
					oscSkeletons01.addFloatArg(x);
					oscSkeletons01.addFloatArg(y);
					oscSkeletons01.addFloatArg(z);
					sender.sendMessage(oscSkeletons01);
				}
				
				//-- bones --//
				if(oscSkeletons02){
					float neck01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_NECK).getStartJoint().getProjectivePosition().x / width;
					float neck01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_NECK).getStartJoint().getProjectivePosition().y / height;		
					float neck02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_NECK).getEndJoint().getProjectivePosition().x / width;
					float neck02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_NECK).getEndJoint().getProjectivePosition().y / height;
					float shoulderLeft01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_SHOULDER).getStartJoint().getProjectivePosition().x / width;
					float shoulderLeft01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_SHOULDER).getStartJoint().getProjectivePosition().y / height;
					float shoulderLeft02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_SHOULDER).getEndJoint().getProjectivePosition().x / width;
					float shoulderLeft02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_SHOULDER).getEndJoint().getProjectivePosition().y / height;
					float shoulderRight01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_SHOULDER).getStartJoint().getProjectivePosition().x / width;
					float shoulderRight01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_SHOULDER).getStartJoint().getProjectivePosition().y / height;
					float shoulderRight02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_SHOULDER).getEndJoint().getProjectivePosition().x / width;
					float shoulderRight02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_SHOULDER).getEndJoint().getProjectivePosition().y / height;
					float upArmLeft01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_ARM).getStartJoint().getProjectivePosition().x / width;
					float upArmLeft01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_ARM).getStartJoint().getProjectivePosition().y / height;
					float upArmLeft02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_ARM).getEndJoint().getProjectivePosition().x / width;
					float upArmLeft02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_ARM).getEndJoint().getProjectivePosition().y / height;
					float upArmRight01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_ARM).getStartJoint().getProjectivePosition().x / width;
					float upArmRight01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_ARM).getStartJoint().getProjectivePosition().y / height;
					float upArmRight02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_ARM).getEndJoint().getProjectivePosition().x / width;
					float upArmRight02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_ARM).getEndJoint().getProjectivePosition().y / height;
					float lowArmLeft01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_ARM).getStartJoint().getProjectivePosition().x / width;
					float lowArmLeft01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_ARM).getStartJoint().getProjectivePosition().y / height;
					float lowArmLeft02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_ARM).getEndJoint().getProjectivePosition().x / width;
					float lowArmLeft02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_ARM).getEndJoint().getProjectivePosition().y / height;
					float lowArmRight01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_ARM).getStartJoint().getProjectivePosition().x / width;
					float lowArmRight01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_ARM).getStartJoint().getProjectivePosition().y / height;
					float lowArmRight02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_ARM).getEndJoint().getProjectivePosition().x / width;
					float lowArmRight02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_ARM).getEndJoint().getProjectivePosition().y / height;
					float upTorsoLeft01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_TORSO).getStartJoint().getProjectivePosition().x / width;
					float upTorsoLeft01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_TORSO).getStartJoint().getProjectivePosition().y / height;
					float upTorsoLeft02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_TORSO).getEndJoint().getProjectivePosition().x / width;
					float upTorsoLeft02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_TORSO).getEndJoint().getProjectivePosition().y / height;
					float upTorsoRight01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_TORSO).getStartJoint().getProjectivePosition().x / width;
					float upTorsoRight01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_TORSO).getStartJoint().getProjectivePosition().y / height;
					float upTorsoRight02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_TORSO).getEndJoint().getProjectivePosition().x / width;
					float upTorsoRight02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_TORSO).getEndJoint().getProjectivePosition().y / height;
					float lowTorsoLeft01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_TORSO).getStartJoint().getProjectivePosition().x / width;
					float lowTorsoLeft01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_TORSO).getStartJoint().getProjectivePosition().y / height;
					float lowTorsoLeft02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_TORSO).getEndJoint().getProjectivePosition().x / width;
					float lowTorsoLeft02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_TORSO).getEndJoint().getProjectivePosition().y / height;
					float lowTorsoRight01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_TORSO).getStartJoint().getProjectivePosition().x / width;
					float lowTorsoRight01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_TORSO).getStartJoint().getProjectivePosition().y / height;
					float lowTorsoRight02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_TORSO).getEndJoint().getProjectivePosition().x / width;
					float lowTorsoRight02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_TORSO).getEndJoint().getProjectivePosition().y / height;
					float hip01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_PELVIS).getStartJoint().getProjectivePosition().x / width;
					float hip01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_PELVIS).getStartJoint().getProjectivePosition().y / height;
					float hip02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_PELVIS).getEndJoint().getProjectivePosition().x / width;
					float hip02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_PELVIS).getEndJoint().getProjectivePosition().y / height;
					float upLegLeft01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_LEG).getStartJoint().getProjectivePosition().x / width;
					float upLegLeft01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_LEG).getStartJoint().getProjectivePosition().y / height;
					float upLegLeft02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_LEG).getEndJoint().getProjectivePosition().x / width;
					float upLegLeft02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_UPPER_LEG).getEndJoint().getProjectivePosition().y / height;
					float upLegRight01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_LEG).getStartJoint().getProjectivePosition().x / width;
					float upLegRight01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_LEG).getStartJoint().getProjectivePosition().y / height;
					float upLegRight02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_LEG).getEndJoint().getProjectivePosition().x / width;
					float upLegRight02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_UPPER_LEG).getEndJoint().getProjectivePosition().y / height;
					float lowLegLeft01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_LEG).getStartJoint().getProjectivePosition().x / width;
					float lowLegLeft01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_LEG).getStartJoint().getProjectivePosition().y / height;
					float lowLegLeft02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_LEG).getEndJoint().getProjectivePosition().x / width;
					float lowLegLeft02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_LEFT_LOWER_LEG).getEndJoint().getProjectivePosition().y / height;
					float lowLegRight01x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_LEG).getStartJoint().getProjectivePosition().x / width;
					float lowLegRight01y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_LEG).getStartJoint().getProjectivePosition().y / height;;
					float lowLegRight02x = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_LEG).getEndJoint().getProjectivePosition().x / width;
					float lowLegRight02y = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_RIGHT_LOWER_LEG).getEndJoint().getProjectivePosition().y / height;
					
					ofxOscBundle oscSkeletons02;

					//uncomment this example for sending z-dimension of bones 
					/*float neck01z = (float)openNIDevices.getTrackedUser(i).getLimb(LIMB_NECK).getStartJoint().getProjectivePosition().z ;
					float max_depth = openNIDevices.getDepthGenerator().GetDeviceMaxDepth();
					float depth_z = neck01z / max_depth;*/
				
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


	//-- send object data --//
	if(objects){
		ofxOscMessage oscObjects;
		oscObjects.setAddress("/objects/start");
		sender.sendMessage(oscObjects);

		for (int i = 0; i < contourFinder.blobs.size(); i++){ 

			//-- ID, x-/y-/z-position --//
			if(oscObjects01){
				int id = idObject[i];
				float x = aObject[i];
				float y = bObject[i];
				float z = cObject[i];

				ofxOscMessage oscObjects01;
				oscObjects01.setAddress("/objects/ID-centralXYZ");
				oscObjects01.addIntArg(id);
				oscObjects01.addFloatArg(x);
				oscObjects01.addFloatArg(y);
				oscObjects01.addFloatArg(z);
				sender.sendMessage(oscObjects01);
			}

			//-- width, height & area --//
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
	}
}

//--------------------------------------------------------------
void kinectApp::userEvent(ofxOpenNIUserEvent & event){
    ofLogNotice() << getUserStatusAsString(event.userStatus) << "for user" << event.id << "from device" << event.deviceID;
}

//--------------------------------------------------------------
void kinectApp::gestureEvent(ofxOpenNIGestureEvent & event){
    ofLogNotice() << event.gestureName << getGestureStatusAsString(event.gestureStatus) << "from device" << event.deviceID << "at" << event.timestampMillis;
}

//--------------------------------------------------------------
void kinectApp::handEvent(ofxOpenNIHandEvent & event){
    ofLogNotice() << event.id << getHandStatusAsString(event.handStatus) << "from device" << event.deviceID << "at" << event.timestampMillis;
}

//--------------------------------------------------------------
void kinectApp::exit(){
    // this often does not work -> it's a known bug -> but calling it on a key press or anywhere that isnt std::aexit() works
    // press 'x' to shutdown cleanly...
    //for (int deviceID = 0; deviceID < numDevices; deviceID++){
    openNIDevices.stop();
    //}
}

//--------------------------------------------------------------
void kinectApp::keyPressed(int key){

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

