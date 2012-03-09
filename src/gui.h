#ifndef _GUI
#define _GUI

#include "kinectApp.h"
#include "filter.h"

class kinectApp;
class filter;


const string cam_view[] =
{
	"Infrared",
	"Depth",
	"Off"
};

bool ir, depth, off, skel, hands, objects, oscHands01, oscSkeletons01, oscSkeletons02, oscObjects01, oscObjects02, save, load, reset, network, resetOsc;

ofxGuiPanel	*camOptions, *trackOptions, *sendigViaOSC, *xmlConfig, *oscConfig, *camRotation, *objectTrackingOptions, *imageProcessingOptions;

enum {
	switchPanel01,
	switchButton01,
	switchButton02,
	switchButton03,
	switchButton04,
	switchButton05,	
	switchButton06,
	switchButton07,
	switchButton08,
	switchButton09,
	switchButton10,
	triggerButton01,
	triggerButton02,
	triggerButton03,
	triggerButton04,
	triggerButton05,
	triggerButton06,
	slider01,
	slider02,
	slider03,
	slider04,
	slider05,
	slider06,
	slider07,
	slider08,
	slider09,
	slider10
};

void kinectApp::setupGui(){

	depth = true;
	skel = false;
	hands = true;
	objects = false; 
	oscHands01 = false;
	oscSkeletons01 = false;
	oscSkeletons02 = false;
	oscObjects01 = false;
	oscObjects02 = false;

	save = false;
	load = false;
	reset = false;
	network = false;
	resetOsc = false;

	camOptions = gui->addPanel(0, "Camera Options", 355, 15, 12, OFXGUI_PANEL_SPACING);
	camOptions->addButton(switchButton01, "Connection", 10, 10, isLive, kofxGui_Button_Switch, "");
	camOptions->addSwitch(switchPanel01, "View", 120, 15, 0, 2, 1, &cam_view[0]);
	camOptions->mObjWidth = 150;
	camOptions->mObjHeight = 95;

	trackOptions = gui->addPanel(1, "Tracking Format", 520, 15, 12, OFXGUI_PANEL_SPACING);
	trackOptions->addButton(switchButton02, "Track Hands", 10, 10, hands, kofxGui_Button_Switch, "");
	trackOptions->addButton(switchButton03, "Track Skeletons", 10, 10, skel, kofxGui_Button_Switch, "");
	trackOptions->addButton(switchButton04, "Track Objects", 10, 10, objects, kofxGui_Button_Switch, "");
	trackOptions->mObjWidth = 155;
	trackOptions ->mObjHeight = 95;

	sendigViaOSC = gui->addPanel(2, "Communication via OSC", 355, 125, 12, OFXGUI_PANEL_SPACING);
	sendigViaOSC->addButton(switchButton06, "Hands' ID and central X-Y-Z-Position", 10, 10, oscHands01, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton07, "Skeletons' ID and central X-Y-Z-Position", 10, 10, oscSkeletons01, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton08, "Bone-Positions of Skeletons", 10, 10, oscSkeletons02, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton09, "Objects' ID and central X-Y-Z-Position", 10, 10, oscObjects01, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton10, "Width-Height-Size of Objects", 10, 10, oscObjects02, kofxGui_Button_Switch, "");
	sendigViaOSC->mObjWidth = 320;
	sendigViaOSC->mObjHeight = 150;

	xmlConfig = gui->addPanel(5, "Manage Settings", 690, 15, 12, OFXGUI_PANEL_SPACING);
	xmlConfig->addButton(triggerButton04, "Save Configuration", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	xmlConfig->addButton(triggerButton05, "Load Configuration", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	xmlConfig->addButton(triggerButton06, "Reset Configuration", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	xmlConfig->mObjWidth = 150;
	xmlConfig->mObjHeight = 95;
		
	oscConfig = gui->addPanel(5, "OSC Settings", 690, 125, 12, OFXGUI_PANEL_SPACING);
	oscConfig->addButton(triggerButton02, "Use my Network Setup", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	oscConfig->addButton(triggerButton03, "Use Localhost at Port 3333", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	oscConfig->mObjWidth = 275;
	oscConfig->mObjHeight = 75;

	camRotation= gui->addPanel(5, "Vertical Camera Rotation", 690, 215, 12, OFXGUI_PANEL_SPACING);
	camRotation->addSlider(slider10, "", 250, 15, -33, 33, rotation, kofxGui_Display_Int, 1);
	camRotation->mObjWidth = 275;
	camRotation->mObjHeight = 60;
	
	gui->forceUpdate(false);
	gui->activate(true);
}

void kinectApp::handleGui(int parameterId, int task, void* data, int length){
		
	switch(parameterId){
		
		case switchButton01:
			isLive = !isLive;
			break;
		case switchPanel01:
			if(length == sizeof(int)){
				cam_view[*(int*)data];
				if(*(int*)data == 0){ ir = true; depth = false; off = false; }
				if(*(int*)data == 1){ ir = false; depth = true; off = false; }
				if(*(int*)data == 2){ ir = false; depth = false; off = true; }
			}
			break;

		case switchButton02:
			hands = !hands;
			/*for (int deviceID = 0; deviceID < numDevices; deviceID++){
				for (int i = 0; i < 100; i++){
					if (hands && i == 0){ 
						openNIDevices[deviceID].addGestureGenerator();
						openNIDevices[deviceID].addHandsGenerator(); 
						openNIDevices[0].setMaxNumHands(nrHand);
						ofAddListener(openNIDevices[0].handEvent, this, &kinectApp::handEvent);
						openNIDevices[0].addAllHandFocusGestures();
						openNIDevices[0].setBaseHandClass(sceneHandTracker);
						if (i >= 100) { i = 1; }
					}
					if (!hands){ 
						openNIDevices[deviceID].removeGestureGenerator(); 
						openNIDevices[deviceID].removeHandsGenerator(); 
						i = 0;
					}
				}
			}
			if (hands){ sceneHandTracker.isTracking() == true; }
			if (!hands){ sceneHandTracker.isTracking() == false; }*/
			break;
		case switchButton03:
			skel = !skel;
			/*bUsers01 = openNIDevices[0].getNumTrackedUsers();
			for (int nID = 0; nID < bUsers01; nID++){
				//if (skel){ openNIDevices[0].resetUserTracking(nID, skel); }
				//if (!skel){ openNIDevices[0].resetUserTracking(nID, skel); }
				openNIDevices[0].resetUserTracking(nID, skel);
			}*/
			break;
		case switchButton04:
			objects = !objects;	
			if(objects == true){
			objectTrackingOptions = gui->addPanel(3, "Object Dimensions", 690, 295, 12, OFXGUI_PANEL_SPACING);
			//objectTrackingOptions->addSlider(slider01, "Nearest Distance", 250, 15, 1, sceneDepth.getMaxDepth(), nearThreshold, kofxGui_Display_Int, 1);
			//objectTrackingOptions->addSlider(slider02, "Furthest Distance", 250, 15, 1, sceneDepth.getMaxDepth(), farThreshold, kofxGui_Display_Int, 1);
			objectTrackingOptions->addSlider(slider03, "Min-Size of Object", 250, 15, 0, 200000, minBlobSize, kofxGui_Display_Int, 1);
			objectTrackingOptions->addSlider(slider04, "Max-Size of Object", 250, 15, 0, 200000, maxBlobSize, kofxGui_Display_Int, 1);
			objectTrackingOptions->mObjWidth = 275;
			objectTrackingOptions->mObjHeight = 205;
			
			imageProcessingOptions = gui->addPanel(4, "Image Processing", 690, 510, 12, OFXGUI_PANEL_SPACING);
			imageProcessingOptions->addButton(triggerButton01, "Capture Background", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
			imageProcessingOptions->addButton(switchButton05, "Add  Background", 10, 10, kofxGui_Button_Off, kofxGui_Button_Switch, "");
			imageProcessingOptions->addSlider(slider05, "Image Threshold", 175, 15, 0, 250, filters->threshold, kofxGui_Display_Int, 1); // max = 300 (200)
			imageProcessingOptions->addSlider(slider06, "Smooth Image", 50, 15, 0, 25, filters->smooth, kofxGui_Display_Int, 1); // max = 15
			imageProcessingOptions->addSlider(slider07, "Blur Edges", 150, 15, 0, 200, filters->highpBlur, kofxGui_Display_Int, 1);
			imageProcessingOptions->addSlider(slider08, "Reduce Noise", 100, 15, 0, 50, filters->highpNoise, kofxGui_Display_Int, 1); // max = 30
			imageProcessingOptions->addSlider(slider09, "Amplify Weak Areas", 200, 15, 1, 300, filters->amp, kofxGui_Display_Int, 1);
			imageProcessingOptions->mObjWidth = 275;
			imageProcessingOptions->mObjHeight = 280;
			}
			else if(objects == false){
				gui->mObjects.resize(6);
			}
			break;

		case switchButton06:
			oscHands01 = !oscHands01;
			break; 
		case switchButton07:
			oscSkeletons01 = !oscSkeletons01;
			break;
		case switchButton08:
			oscSkeletons02 = !oscSkeletons02;
			break;
		case switchButton09:
			oscObjects01 = !oscObjects01;
			break;
		case switchButton10:
			oscObjects02 = !oscObjects02;
			break;

		case triggerButton02:
			if(length == sizeof(bool))
				network = !network;
				if (network){
					XMLosc.loadFile("oscConfiguration.xml");
					host = XMLosc.getValue("OSCCONFIGURATION:NETWORK:HOST", "127.0.0.1");
					port = XMLosc.getValue("OSCCONFIGURATION:NETWORK:PORT", 3333);
					sender.setup(host, port);
				}
			break;
		case triggerButton03:
			if(length == sizeof(bool))
				resetOsc = !resetOsc;
				if (resetOsc){
					host = "127.0.0.1";
					port = 3333;
					sender.setup(host, port);
				}
			break;

		case triggerButton04:
			if(length == sizeof(bool))
				save = !save;
				if (save){
					savePersonalConfiguration();
					statusNetwork = "saved";
				}
			break;
		case triggerButton05:
			if(length == sizeof(bool))
				load = !load;
				if (load){
					loadPersonalConfiguration();
					statusNetwork = "loaded";
				}
			break;
		case triggerButton06:
			if(length == sizeof(bool))
				reset = !reset;
				if (reset){
					resetConfiguration();
					statusNetwork = "reset";
				}
			break;

		case slider01:
			if(length == sizeof(float)){
				nearThreshold = *(float*)data;
			}
			break;
		case slider02:
			if(length == sizeof(float)){
				farThreshold = *(float*)data;
			}
			break;
		case slider03:
			if(length == sizeof(float)){
				minBlobSize = *(float*)data;
			}
			break;
		case slider04:
			if(length == sizeof(float)){
				maxBlobSize = *(float*)data;
			}
			break;
		case triggerButton01:
			if(length == sizeof(bool))
				filters->background = !filters->background;
			break;

		case switchButton05:
			if(length == sizeof(bool))
				filters->all = !filters->all;
			break;
		case slider05:
			if(length == sizeof(float)){
				filters->threshold = *(float*)data;
			}
			break;
		case slider06:
			if(length == sizeof(float)){
				filters->smooth = *(float*)data;
			}
			break;
		case slider07:
			if(length == sizeof(float)){
				filters->highpBlur = *(float*)data;
			}
			break;
		case slider08:
			if(length == sizeof(float)){
				filters->highpNoise = *(float*)data;
			}
			break;
		case slider09:
			if(length == sizeof(float)){
				filters->amp = *(float*)data;
			}
			break;
		
		case slider10:
			if(length == sizeof(float)){
				rotation = *(float*)data;
			}
			break;
	}

}

void kinectApp::savePersonalConfiguration(){
	XML.setValue("PERSONALCONFIGURATION:CAMOPTIONS:CONNECTION", isLive);
	XML.setValue("PERSONALCONFIGURATION:CAMOPTIONS:VIEW:INFRARED", ir);
	XML.setValue("PERSONALCONFIGURATION:CAMOPTIONS:VIEW:DEPTH", depth);
	XML.setValue("PERSONALCONFIGURATION:CAMOPTIONS:VIEW:OFF", off);

	XML.setValue("PERSONALCONFIGURATION:TRACKOPTIONS:HANDS", hands);
	XML.setValue("PERSONALCONFIGURATION:TRACKOPTIONS:SKELETONS", skel);
	XML.setValue("PERSONALCONFIGURATION:TRACKOPTIONS:OBJECTS", objects);

	XML.setValue("PERSONALCONFIGURATION:OSCOPTIONS:HANDSIDXYZ",oscHands01);
	XML.setValue("PERSONALCONFIGURATION:OSCOPTIONS:SKELSIDXYZ", oscSkeletons01);
	XML.setValue("PERSONALCONFIGURATION:OSCOPTIONS:SKELSBONES", oscSkeletons02);
	XML.setValue("PERSONALCONFIGURATION:OSCOPTIONS:OBJECTSIDXYZ", oscObjects01);
	XML.setValue("PERSONALCONFIGURATION:OSCOPTIONS:OBJECTSDIMENSIONS", oscObjects02);

	XML.setValue("PERSONALCONFIGURATION:OBJECTTRACKOPTIONS:NEARTHRESH", nearThreshold);
	XML.setValue("PERSONALCONFIGURATION:OBJECTTRACKOPTIONS:FARTHRESH", farThreshold);
	XML.setValue("PERSONALCONFIGURATION:OBJECTTRACKOPTIONS:MINBLOBSIZE", minBlobSize);
	XML.setValue("PERSONALCONFIGURATION:OBJECTTRACKOPTIONS:MAXBLOBSIZE", maxBlobSize);
	
	XML.setValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:CAPTUREBG", filters->background);
	XML.setValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:ADDBG", filters->all);
	XML.setValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:THRESHOLD", filters->threshold);
	XML.setValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:SMOOTH", filters->smooth);
	XML.setValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:HIGHPASSBLUR", filters->highpBlur);
	XML.setValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:HIGHPASSNOISE", filters->highpNoise);
	XML.setValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:HIGHPASSAMP", filters->amp);
	
	XML.saveFile("personalConfiguration.xml");
}

void kinectApp::loadPersonalConfiguration(){

	gui->mObjects.resize(0);

	XML.loadFile("personalConfiguration.xml");

	isLive	=	XML.getValue("PERSONALCONFIGURATION:CAMOPTIONS:CONNECTION", 1);
	ir		=	XML.getValue("PERSONALCONFIGURATION:CAMOPTIONS:VIEW:INFRARED", 0);
	depth	=	XML.getValue("PERSONALCONFIGURATION:CAMOPTIONS:VIEW:DEPTH", 1);
	off		=	XML.getValue("PERSONALCONFIGURATION:CAMOPTIONS:VIEW:OFF", 0);

	int a, b;
	a = hands;
	b =	XML.getValue("PERSONALCONFIGURATION:TRACKOPTIONS:HANDS", 0);
	if (a == b){ hands = b; }
	//else if (a != b) { hands = b; sceneHandTracker.toggleTrackHands(); }
	hands	=	XML.getValue("PERSONALCONFIGURATION:TRACKOPTIONS:HANDS", 0);
	skel	=	XML.getValue("PERSONALCONFIGURATION:TRACKOPTIONS:SKELETONS", 0);
	objects	=	XML.getValue("PERSONALCONFIGURATION:TRACKOPTIONS:OBJECTS", 0);

	oscHands01		=	XML.getValue("PERSONALCONFIGURATION:OSCOPTIONS:HANDSIDXYZ",0);
	oscSkeletons01	=	XML.getValue("PERSONALCONFIGURATION:OSCOPTIONS:SKELSIDXYZ", 0);
	oscSkeletons02	=	XML.getValue("PERSONALCONFIGURATION:OSCOPTIONS:SKELSBONES", 0);
	oscObjects01	=	XML.getValue("PERSONALCONFIGURATION:OSCOPTIONS:OBJECTSIDXYZ", 0);
	oscObjects02	=	XML.getValue("PERSONALCONFIGURATION:OSCOPTIONS:OBJECTSDIMENSIONS", 0);

	nearThreshold		=	XML.getValue("PERSONALCONFIGURATION:OBJECTTRACKOPTIONS:NEARTHRESH", 500);
	farThreshold		=	XML.getValue("PERSONALCONFIGURATION:OBJECTTRACKOPTIONS:FARTHRESH", 1000);
	minBlobSize			=	XML.getValue("PERSONALCONFIGURATION:OBJECTTRACKOPTIONS:MINBLOBSIZE", 1500);
	maxBlobSize			=	XML.getValue("PERSONALCONFIGURATION:OBJECTTRACKOPTIONS:MAXBLOBSIZE", 10000);
	
	filters->background	=	XML.getValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:CAPTUREBG", 0);
	filters->all		=	XML.getValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:ADDBG", 0);
	filters->threshold	=	XML.getValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:THRESHOLD", 0);
	filters->smooth		=	XML.getValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:SMOOTH", 0);
	filters->highpBlur	=	XML.getValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:HIGHPASSBLUR", 0);
	filters->highpNoise	=	XML.getValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:HIGHPASSNOISE", 0);
	filters->amp		=	XML.getValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:HIGHPASSAMP", 1);

	camOptions = gui->addPanel(0, "Camera Options", 355, 15, 12, OFXGUI_PANEL_SPACING);
	camOptions->addButton(switchButton01, "Connection", 10, 10, isLive, kofxGui_Button_Switch, "");
	if( ir && !depth && !off ){ camOptions->addSwitch(switchPanel01, "View", 120, 15, 0, 2, 0, &cam_view[0]); }
	if( !ir && depth && !off ){ camOptions->addSwitch(switchPanel01, "View", 120, 15, 0, 2, 1, &cam_view[0]); }
	if( !ir && !depth && off ){ camOptions->addSwitch(switchPanel01, "View", 120, 15, 0, 2, 2, &cam_view[0]); }
	camOptions->mObjWidth = 150;
	camOptions->mObjHeight = 95;

	trackOptions = gui->addPanel(1, "Tracking Format", 520, 15, 12, OFXGUI_PANEL_SPACING);
	trackOptions->addButton(switchButton02, "Track Hands", 10, 10, hands, kofxGui_Button_Switch, "");
	trackOptions->addButton(switchButton03, "Track Skeletons", 10, 10, skel, kofxGui_Button_Switch, "");
	trackOptions->addButton(switchButton04, "Track Objects", 10, 10, objects, kofxGui_Button_Switch, "");
	trackOptions->mObjWidth = 155;
	trackOptions ->mObjHeight = 95;

	sendigViaOSC = gui->addPanel(2, "Communication via OSC", 355, 125, 12, OFXGUI_PANEL_SPACING);
	sendigViaOSC->addButton(switchButton06, "Hands' ID and central X-Y-Z-Position", 10, 10, oscHands01, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton07, "Skeletons' ID and central X-Y-Z-Position", 10, 10, oscSkeletons01, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton08, "Bone-Positions of Skeletons", 10, 10, oscSkeletons02, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton09, "Objects' ID and central X-Y-Z-Position", 10, 10, oscObjects01, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton10, "Width-Height-Size of Objects", 10, 10, oscObjects02, kofxGui_Button_Switch, "");
	sendigViaOSC->mObjWidth = 320;
	sendigViaOSC->mObjHeight = 150;

	xmlConfig = gui->addPanel(5, "Manage Settings", 690, 15, 12, OFXGUI_PANEL_SPACING);
	xmlConfig->addButton(triggerButton04, "Save Configuration", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	xmlConfig->addButton(triggerButton05, "Load Configuration", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	xmlConfig->addButton(triggerButton06, "Reset Configuration", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	xmlConfig->mObjWidth = 150;
	xmlConfig->mObjHeight = 95;
		
	oscConfig = gui->addPanel(5, "OSC Settings", 690, 125, 12, OFXGUI_PANEL_SPACING);
	oscConfig->addButton(triggerButton02, "Use my Network Setup", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	oscConfig->addButton(triggerButton03, "Use Localhost at Port 3333", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	oscConfig->mObjWidth = 275;
	oscConfig->mObjHeight = 75;

	camRotation= gui->addPanel(5, "Vertical Camera Rotation", 690, 215, 12, OFXGUI_PANEL_SPACING);
	camRotation->addSlider(slider10, "", 250, 15, -33, 33, rotation, kofxGui_Display_Int, 1);
	camRotation->mObjWidth = 275;
	camRotation->mObjHeight = 60;

	if(objects){
		objectTrackingOptions = gui->addPanel(3, "Object Dimensions", 690, 295, 12, OFXGUI_PANEL_SPACING);
		//objectTrackingOptions->addSlider(slider01, "Nearest Distance", 250, 15, 1, sceneDepth.getMaxDepth(), nearThreshold, kofxGui_Display_Int, 1);
		//objectTrackingOptions->addSlider(slider02, "Furthest Distance", 250, 15, 1, sceneDepth.getMaxDepth(), farThreshold, kofxGui_Display_Int, 1);
		objectTrackingOptions->addSlider(slider03, "Min-Size of Object", 250, 15, 0, 200000, minBlobSize, kofxGui_Display_Int, 1);
		objectTrackingOptions->addSlider(slider04, "Max-Size of Object", 250, 15, 0, 200000, maxBlobSize, kofxGui_Display_Int, 1);
		objectTrackingOptions->mObjWidth = 275;
		objectTrackingOptions->mObjHeight = 205;
			
		imageProcessingOptions = gui->addPanel(4, "Image Processing", 690, 510, 12, OFXGUI_PANEL_SPACING);
		imageProcessingOptions->addButton(triggerButton01, "Capture Background", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
		imageProcessingOptions->addButton(switchButton05, "Add  Background", 10, 10, kofxGui_Button_Off, kofxGui_Button_Switch, "");
		imageProcessingOptions->addSlider(slider05, "Image Threshold", 175, 15, 0, 250, filters->threshold, kofxGui_Display_Int, 1); // max = 300 (200)
		imageProcessingOptions->addSlider(slider06, "Smooth Image", 50, 15, 0, 25, filters->smooth, kofxGui_Display_Int, 1); // max = 15
		imageProcessingOptions->addSlider(slider07, "Blur Edges", 150, 15, 0, 200, filters->highpBlur, kofxGui_Display_Int, 1);
		imageProcessingOptions->addSlider(slider08, "Reduce Noise", 100, 15, 0, 50, filters->highpNoise, kofxGui_Display_Int, 1); // max = 30
		imageProcessingOptions->addSlider(slider09, "Amplify Weak Areas", 200, 15, 1, 300, filters->amp, kofxGui_Display_Int, 1);
		imageProcessingOptions->mObjWidth = 275;
		imageProcessingOptions->mObjHeight = 280;
	}
}

void kinectApp::resetConfiguration(){	
	
	gui->mObjects.resize(0);
	
	isLive	=	1;
	ir		=	0;
	depth	=	1;
	off		=	0;

	if (hands){
		hands	=	0;
		//sceneHandTracker.toggleTrackHands();
	}
	skel	=	0;
	objects	=	0;

	oscHands01		=	0;
	oscSkeletons01	=	0;
	oscSkeletons02	=	0;
	oscObjects01	=	0;
	oscObjects02	=	0;

	nearThreshold		=	500;
	farThreshold		=	1000;
	minBlobSize			=	1500;
	maxBlobSize			=	10000;
	
	filters->background	=	0;
	filters->all		=	0;
	filters->threshold	=	0;
	filters->smooth		=	0;
	filters->highpBlur	=	0;
	filters->highpNoise	=	0;
	filters->amp		=	1;

	camOptions = gui->addPanel(0, "Camera Options", 355, 15, 12, OFXGUI_PANEL_SPACING);
	camOptions->addButton(switchButton01, "Connection", 10, 10, isLive, kofxGui_Button_Switch, "");
	if( ir && !depth && !off ){ camOptions->addSwitch(switchPanel01, "View", 120, 15, 0, 2, 0, &cam_view[0]); }
	if( !ir && depth && !off ){ camOptions->addSwitch(switchPanel01, "View", 120, 15, 0, 2, 1, &cam_view[0]); }
	if( !ir && !depth && off ){ camOptions->addSwitch(switchPanel01, "View", 120, 15, 0, 2, 2, &cam_view[0]); }
	camOptions->mObjWidth = 150;
	camOptions->mObjHeight = 95;

	trackOptions = gui->addPanel(1, "Tracking Format", 520, 15, 12, OFXGUI_PANEL_SPACING);
	trackOptions->addButton(switchButton02, "Track Hands", 10, 10, hands, kofxGui_Button_Switch, "");
	trackOptions->addButton(switchButton03, "Track Skeletons", 10, 10, skel, kofxGui_Button_Switch, "");
	trackOptions->addButton(switchButton04, "Track Objects", 10, 10, objects, kofxGui_Button_Switch, "");
	trackOptions->mObjWidth = 155;
	trackOptions ->mObjHeight = 95;

	sendigViaOSC = gui->addPanel(2, "Communication via OSC", 355, 125, 12, OFXGUI_PANEL_SPACING);
	sendigViaOSC->addButton(switchButton06, "Hands' ID and central X-Y-Z-Position", 10, 10, oscHands01, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton07, "Skeletons' ID and central X-Y-Z-Position", 10, 10, oscSkeletons01, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton08, "Bone-Positions of Skeletons", 10, 10, oscSkeletons02, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton09, "Objects' ID and central X-Y-Z-Position", 10, 10, oscObjects01, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton10, "Width-Height-Size of Objects", 10, 10, oscObjects02, kofxGui_Button_Switch, "");
	sendigViaOSC->mObjWidth = 320;
	sendigViaOSC->mObjHeight = 150;

	xmlConfig = gui->addPanel(5, "Manage Settings", 690, 15, 12, OFXGUI_PANEL_SPACING);
	xmlConfig->addButton(triggerButton04, "Save Configuration", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	xmlConfig->addButton(triggerButton05, "Load Configuration", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	xmlConfig->addButton(triggerButton06, "Reset Configuration", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	xmlConfig->mObjWidth = 150;
	xmlConfig->mObjHeight = 95;
		
	oscConfig = gui->addPanel(5, "OSC Settings", 690, 125, 12, OFXGUI_PANEL_SPACING);
	oscConfig->addButton(triggerButton02, "Use my Network Setup", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	oscConfig->addButton(triggerButton03, "Use Localhost at Port 3333", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	oscConfig->mObjWidth = 275;
	oscConfig->mObjHeight = 75;

	camRotation= gui->addPanel(5, "Vertical Camera Rotation", 690, 215, 12, OFXGUI_PANEL_SPACING);
	camRotation->addSlider(slider10, "", 250, 15, -33, 33, rotation, kofxGui_Display_Int, 1);
	camRotation->mObjWidth = 275;
	camRotation->mObjHeight = 60;
	
}

#endif