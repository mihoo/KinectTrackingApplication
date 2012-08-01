########## KINECTA - KINECT TRACKING APPLICATION ##########

KinectA is an application for motion tracking via KinectSensor - including hand, skeleton and object tracking. It provides a clearly arranged interface with multiple configuration. Simple save and load functionalities are available for modifiable XML-files. Tracked information can be send via OSC to other hard- and software.  

Download the project folder on github (https://github.com/mihoo/KinectTrackingApplication). Download the compiled application under http://www.mihoo.de/kinecta.html 

KinectA - Kinect Tracking Application / Copyright (C) 2011-2012  Michaela Honauer a.k.a. mihoo (http://www.mihoo.de/)
This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details. 
You should have received a copy of the GNU General Public License along with this program. If not, see <http://www.gnu.org/licenses/>.



C++ FRAMEWORK AND ADD-ONS

KinectA has been built with openFrameworks (http://www.openframeworks.cc). Hand and skeleton tracking base on the ofxOpenNI wrapper (by Matthew Gingold, http://gingold.com.au). Object Tracking bases on blob tracking via the ofxOpenCv core add-on. The interface has been designed with the ofxGui extension (by alphakana/ Stefan Kirch). The XML functionalities base on the ofxXmlSettings core add-on (by Lee Thomason, www.grinninglizard.com) and the OSC communication has been built with the ofxOsc core add-on (by Damian Stewart).



INSTALLATION AND DRIVERS

WINDOWS
(tested on Windows7 64bit, but installed 32bit-Versions of OpenNI, SensorKinect and NITE)
1. Install unstable OpenNI Binaries (v1.5.4.0) from http://www.openni.org/Downloads/OpenNIModules.aspx
2. Install SensorKinect (v5.1.2.1) from https://github.com/avin2/SensorKinect (you find the install .exe inside the bin folder).
3. Install unstable NITE / OpenNI Middleware Binaries (v1.5.2.21) from http://www.openni.org/Downloads/OpenNIModules.aspx
4. If it doesn't work install the driver for Kinect Camera manually from your device manager (you find the drivers inside the platform folder of SensorKinect). Then restart your system.

MAC
(tested on OSX 10.7.4)
For systems based on 10.7.4, KinectA should run without the installation of additional drivers.



PERSONAL CONFIGURATION FILE

You find the "personalConfig.xml" inside the data folder. The configuration settings are saved to and loaded from this file.



OSC CONFIGURATION FILE

You find the "oscConfig.xml" inside the data folder. You can open it in any editor and change port or host information. The configuration settings are loaded from this file.



OSC ADDRESS SETTINGS

"/hands/start" contains no information, just indicates that hand tracking is active.
"/hands/ID-centralXYZ" contains one int (Id of the hand) and three floats (central x-, y- and z-position of the hand).

"/skeletons/start" contains no information, just indicates that skeleton tracking is active.
"/skeletons/ID-centralXYZ" contains one int (Id of the skel) and three floats (central x-, y- and z-position of the skel).
"/skeletons/bones/..." contains information of the bones in this scheme: Point A with x and y coordinate and point B with x and y coordinate. That means every bone consists of four floats (boneAx, boneAy, boneBx, boneBy). See following for details:
"/skeletons/bones/neckShoulder" contains 12 floats (neckAx, neckAy, neckBx, neckBy, leftShoulderAx, leftShoulderAy, leftShoulderBx, leftShoulderBy, rightShoulderAx, rightShoulderAy, rightShoulderBx, rightShoulderBy).
"/skeletons/bones/leftArm" contains 8 floats (leftUpperArmAx, leftUpperArmAy, leftUpperArmBx, leftUpperArmBy, leftLowerArmAx, leftLowerArmAy, leftLowerArmBx, leftLowerArmBy).
"/skeletons/bones/rightArm" contains 8 floats (rightUpperArmAx, rightUpperArmAy, rightUpperArmBx, rightUpperArmBy, rightLowerArmAx, rightLowerArmAy, rightLowerArmBx, rightLowerArmBy).
"/skeletons/bones/torsoHip" contains 20 floats (leftUpperTorsoAx, leftUpperTorsoAy, leftUpperTorsoBx, leftUpperTorsoBy, rightUpperTorsoAx, rightUpperTorsoAy, rightUpperTorsoBx, rightUpperTorsoBy, leftLowerTorsoAx, leftLowerTorsoAy, leftLowerTorsoBx, leftLowerTorsoBy, rightLowerTorsoAx, rightLowerTorsoAy, rightLowerTorsoBx, rightLowerTorsoBy, hipAx, hipAy, hipBx, hipBy).
"/skeletons/bones/leftLeg" contains 8 floats (leftUpperLegAx, leftUpperLegAy, leftUpperLegBx, leftUpperLegBy, leftLowerLegAx, leftLowerLegAy, leftLowerLegBx, leftLowerLegBy).
"/skeletons/bones/rightLeg" contains 8 floats (rightUpperLegAx, rightUpperLegAy, rightUpperLegBx, rightUpperLegBy, rightLowerLegAx, rightLowerLegAy, rightLowerLegBx, rightLowerLegBy).

"/objects/start" contains no information, just indicates that object tracking is active.
"/objects/ID-centralXYZ" contains one int (Id of the object) and three floats (central x-, y- and z-position of the object).
"/objects/width-height-size" contains three floats (width, height and size of the object).



FURTHER INFORMATION

http://kinecta.mihoo.de
