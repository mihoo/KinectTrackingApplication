/*********************************************************************************
	
	KinectA - Kinect Tracking Application / main.cpp
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
