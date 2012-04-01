/*********************************************************************************
	
	KinectA - Kinect Tracking Application / gui.h
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
	"None"
};

bool ir, depth, off;
bool hands, skel, objects;
bool save, load, reset;
bool network, resetOsc;
bool oscHands01, oscSkeletons01, oscSkeletons02, oscObjects01, oscObjects02;

ofxGuiPanel	*camOptions, *trackOptions, *sendigViaOSC, *xmlConfig, *oscConfig, *camRotation, *objectTrackingOptions, *backgProcessingOptions, *imageProcessingOptions, *showProcessedImages;

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


//--------------------------------------------------------------
void kinectApp::setupGui(){
	
	//-- initialize variables & classes --// 

	ir = false;
	depth = true;
	off = false;

	hands = false;
	skel = false;
	objects = false; 

	save = false;
	load = false;
	reset = false;

	network = false;
	resetOsc = false;

	oscHands01 = false;
	oscSkeletons01 = false;
	oscSkeletons02 = false;
	oscObjects01 = false;
	oscObjects02 = false;

	basicGui();
	
	gui->forceUpdate(false);
	gui->activate(true);
}

//--------------------------------------------------------------
void kinectApp::handleGui(int parameterId, int task, void* data, int length){
		
	switch(parameterId){
		
		//-- "Camera" menu --// 
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
		
		//-- "Tracking" menu --// 
		case switchButton02:
			hands = !hands;
			break;
		case switchButton03:
			skel = !skel;
			break;
		case switchButton04:
			objects = !objects;	
			if(objects == true){ objectGui(); }
			else if(objects == false){ gui->mObjects.resize(5); }
			break;

		//-- "Configuration" menu --// 
		case triggerButton04:
			if(length == sizeof(bool))
				save = !save;
				if (save){
					savePersonalConfiguration();
					statusConfig = "saved";
				}
			break;
		case triggerButton05:
			if(length == sizeof(bool))
				load = !load;
				if (load){
					loadPersonalConfiguration();
					statusConfig = "loaded";
				}
			break;
		case triggerButton06:
			if(length == sizeof(bool))
				reset = !reset;
				if (reset){
					resetConfiguration();
					statusConfig = "reset";
				}
			break;

		//-- "Port / Remote Address" menu --// 
		case triggerButton02:
			if(length == sizeof(bool))
				network = !network;
				if (network){
					XMLosc.loadFile("oscConfig.xml");
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
		
		//-- "Communication via OSC" menu --// 
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

		//-- object tracking: "Dimensioning / Distance" menu --//
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

		//-- object tracking: "Background" menu --//
		case triggerButton01:
			if(length == sizeof(bool))
				filters->background = !filters->background;
			break;
		case switchButton05:
			if(length == sizeof(bool))
				filters->all = !filters->all;
			break;
		
		//-- object tracking: "Image Filters" menu --//
		/*case slider05:
			if(length == sizeof(float)){
				filters->threshold = *(float*)data;
			}
			break;*/
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
		
		//-- object tracking: "Vertical Camera Rotation" menu --//
		//doesn't work on Windows, currently no driver support for cam rotation
		/*case slider10:
			if(length == sizeof(float)){
				rotation = *(float*)data;
			}
			break;*/
	}

}

//--------------------------------------------------------------
void kinectApp::savePersonalConfiguration(){

	//-- save settings in XML-file --//

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

	XML.setValue("PERSONALCONFIGURATION:BGPROCESSOPTIONS:CAPTUREBG", filters->background);
	XML.setValue("PERSONALCONFIGURATION:BGPROCESSOPTIONS:ADDBG", filters->all);
	
	//XML.setValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:THRESHOLD", filters->threshold);
	XML.setValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:SMOOTH", filters->smooth);
	XML.setValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:HIGHPASSBLUR", filters->highpBlur);
	XML.setValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:HIGHPASSNOISE", filters->highpNoise);
	XML.setValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:HIGHPASSAMP", filters->amp);
	
	XML.saveFile("personalConfig.xml");
}

//--------------------------------------------------------------
void kinectApp::loadPersonalConfiguration(){

	//-- load settings from XML-file --//

	gui->mObjects.resize(0);

	XML.loadFile("personalConfig.xml");

	isLive	=	XML.getValue("PERSONALCONFIGURATION:CAMOPTIONS:CONNECTION", 1);
	ir		=	XML.getValue("PERSONALCONFIGURATION:CAMOPTIONS:VIEW:INFRARED", 0);
	depth	=	XML.getValue("PERSONALCONFIGURATION:CAMOPTIONS:VIEW:DEPTH", 1);
	off		=	XML.getValue("PERSONALCONFIGURATION:CAMOPTIONS:VIEW:OFF", 0);

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
	
	filters->background	=	XML.getValue("PERSONALCONFIGURATION:BGPROCESSOPTIONS:CAPTUREBG", 0);
	filters->all		=	XML.getValue("PERSONALCONFIGURATION:BGPROCESSOPTIONS:ADDBG", 0);

	//filters->threshold	=	XML.getValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:THRESHOLD", 0);
	filters->smooth		=	XML.getValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:SMOOTH", 0);
	filters->highpBlur	=	XML.getValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:HIGHPASSBLUR", 0);
	filters->highpNoise	=	XML.getValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:HIGHPASSNOISE", 0);
	filters->amp		=	XML.getValue("PERSONALCONFIGURATION:IMGPROCESSOPTIONS:HIGHPASSAMP", 1);
	
	basicGui();

	if(objects){ objectGui(); }
}

//--------------------------------------------------------------
void kinectApp::resetConfiguration(){	
	
	//-- reset settings, same settings as at start of application --//

	gui->mObjects.resize(0);
	
	isLive	=	1;

	ir		=	0;
	depth	=	1;
	off		=	0;

	hands	=	0;
	skel	=	0;
	objects	=	0;

	save	=	0;
	load	=	0;
	reset	=	0;

	network =	0;
	resetOsc=	0;

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
	
	//filters->threshold	=	0;
	filters->smooth		=	0;
	filters->highpBlur	=	0;
	filters->highpNoise	=	0;
	filters->amp		=	1;

	basicGui();	
}

//--------------------------------------------------------------
void kinectApp::basicGui(){

	//-- initialize "Camera" menu --//
	camOptions = gui->addPanel(0, "Camera", 11, 10, 12, OFXGUI_PANEL_SPACING);
	camOptions->addButton(switchButton01, "Active", 10, 10, isLive, kofxGui_Button_Switch, "");
	if( ir && !depth && !off ){ camOptions->addSwitch(switchPanel01, "View", 81, 15, 0, 2, 0, &cam_view[0]); }
	if( !ir && depth && !off ){ camOptions->addSwitch(switchPanel01, "View", 81, 15, 0, 2, 1, &cam_view[0]); }
	if( !ir && !depth && off ){ camOptions->addSwitch(switchPanel01, "View", 81, 15, 0, 2, 2, &cam_view[0]); }
	camOptions->mObjWidth = 109;
	camOptions->mObjHeight = 95;
	
	//-- initialize "Tracking" menu --//
	trackOptions = gui->addPanel(1, "Tracking", 130, 10, 12, OFXGUI_PANEL_SPACING);
	trackOptions->addButton(switchButton02, "Hands", 10, 10, hands, kofxGui_Button_Switch, "");
	trackOptions->addButton(switchButton03, "Skeletons", 10, 10, skel, kofxGui_Button_Switch, "");
	trackOptions->addButton(switchButton04, "Objects", 10, 10, objects, kofxGui_Button_Switch, "");
	trackOptions->mObjWidth = 110;
	trackOptions ->mObjHeight = 95;
		
	//-- initialize "Configuration" menu --//
	xmlConfig = gui->addPanel(3, "Configuration", 250, 10, 12, OFXGUI_PANEL_SPACING);
	xmlConfig->addButton(triggerButton04, "Save to personalConfig.xml", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	xmlConfig->addButton(triggerButton05, "Load from personalConfig.xml", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	xmlConfig->addButton(triggerButton06, "Reset (Default Configuration)", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	xmlConfig->mObjWidth = 205;
	xmlConfig->mObjHeight = 95;
	
	//-- initialize "Port / Remote Address" menu --//
	oscConfig = gui->addPanel(4, "Port / Remote Address", 465, 10, 12, OFXGUI_PANEL_SPACING);
	oscConfig->addButton(triggerButton02, "Load from oscConfig.xml", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	oscConfig->addButton(triggerButton03, "Reset (Default Settings)", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	oscConfig->mObjWidth = 205;
	oscConfig->mObjHeight = 95;

	//-- initialize "Communication via OSC" menu --//
	sendigViaOSC = gui->addPanel(2, "Communication via OSC", 680, 10, 12, OFXGUI_PANEL_SPACING);
	sendigViaOSC->addButton(switchButton06, "/hands/ID-centralXYZ", 10, 10, oscHands01, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton07, "/skeletons/ID-centralXYZ", 10, 10, oscSkeletons01, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton08, "/skeletons/bones/... (README.txt)", 10, 10, oscSkeletons02, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton09, "/objects/ID-centralXYZ", 10, 10, oscObjects01, kofxGui_Button_Switch, "");
	sendigViaOSC->addButton(switchButton10, "/objects/width-height-size", 10, 10, oscObjects02, kofxGui_Button_Switch, "");
	sendigViaOSC->mObjWidth = 215;
	sendigViaOSC->mObjHeight = 150;

	//-- initialize "Vertical Camera Rotation" menu --//
	//doesn't work on Windows, currently no driver support for cam rotation
	/*camRotation= gui->addPanel(5, "Vertical Camera Rotation", 690, 215, 12, OFXGUI_PANEL_SPACING);
	camRotation->addSlider(slider10, "", 250, 15, -33, 33, rotation, kofxGui_Display_Int, 1);
	camRotation->mObjWidth = 275;
	camRotation->mObjHeight = 60;*/
}

//--------------------------------------------------------------
void kinectApp::objectGui(){

	//-- initialize "Dimensioning / Distance" menu --//
	objectTrackingOptions = gui->addPanel(6, "Dimensioning / Distance", 680, 195, 12, OFXGUI_PANEL_SPACING);
	objectTrackingOptions->addSlider(slider01, "Nearest Distance", 190, 15, 1, 10000, nearThreshold, kofxGui_Display_Int, 1);
	objectTrackingOptions->addSlider(slider02, "Furthest Distance", 190, 15, 1, 10000, farThreshold, kofxGui_Display_Int, 1);
	objectTrackingOptions->addSlider(slider03, "Min-Size of Object", 190, 15, 0, 200000, minBlobSize, kofxGui_Display_Int, 1);
	objectTrackingOptions->addSlider(slider04, "Max-Size of Object", 190, 15, 0, 200000, maxBlobSize, kofxGui_Display_Int, 1);
	objectTrackingOptions->mObjWidth = 210;
	objectTrackingOptions->mObjHeight = 205;

	//-- initialize "Background" menu --//
	backgProcessingOptions = gui->addPanel(7, "Background", 680, 405, 12, OFXGUI_PANEL_SPACING);
	backgProcessingOptions->addButton(triggerButton01, "Capture Background", 10, 10, kofxGui_Button_Off, kofxGui_Button_Trigger, "");
	backgProcessingOptions->addButton(switchButton05, "Remove Background", 10, 10, filters->all, kofxGui_Button_Switch, "");
	backgProcessingOptions->mObjWidth = 210;
	backgProcessingOptions->mObjHeight = 95;
	
	//-- initialize "Image Filters" menu --//
	imageProcessingOptions = gui->addPanel(8, "Image Filters", 680, 506, 12, OFXGUI_PANEL_SPACING);
	//imageProcessingOptions->addSlider(slider05, "Image Threshold", 175, 15, 0, 250, filters->threshold, kofxGui_Display_Int, 1); // max = 300 (200)
	imageProcessingOptions->addSlider(slider06, "Smooth Image", 190, 15, 0, 25, filters->smooth, kofxGui_Display_Int, 1); // max = 15
	imageProcessingOptions->addSlider(slider07, "Blur Edges", 190, 15, 0, 200, filters->highpBlur, kofxGui_Display_Int, 1);
	imageProcessingOptions->addSlider(slider08, "Reduce Noise", 190, 15, 0, 50, filters->highpNoise, kofxGui_Display_Int, 1); // max = 30
	imageProcessingOptions->addSlider(slider09, "Amplify weak Areas", 190, 15, 1, 300, filters->amp, kofxGui_Display_Int, 1);
	imageProcessingOptions->mObjWidth = 210;
	imageProcessingOptions->mObjHeight = 205;
}

#endif