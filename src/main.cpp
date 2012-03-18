
#include "kinectApp.h"
#include "ofMain.h"
#include "ofAppGlutWindow.h"

//========================================================================
int main( ){
//int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd){ // use this with project properties-linker-system-subsystem: Windows (/SUBSYSTEM:WINDOWS)

    ofAppGlutWindow window;
	ofSetupOpenGL(&window, 905, 730, OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new kinectApp());

}
