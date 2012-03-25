########## KINECTA - KINECT TRACKING APPLICATION ##########

Application for motion tracking via KinectSensor - including hand, skeleton and object tracking.
Hand and skeleton tracking base on the ofxOpenNI wrapper (by Matthew Gingold, http://gingold.com.au). 
Object Tracking bases on blob tracking via the ofxOpenCv wrapper.
KinectA provides a clearly arranged interface with multiple configuration. Simple save and load 
functionalities are available for modifiable XML-files. Tracked information can be send via OSC 
to other hard- and software. It has been built with openFrameworks (http://www.openframeworks.cc).



INSTALLATION AND DRIVERS

WINDOWS
(tested on Windows7, 64bit)
1. install unstable OpenNI Binaries (v1.5.2.7) from http://www.openni.org/Downloads/OpenNIModules.aspx
2. install SensorKinect (v5.1.0.25) from https://github.com/avin2/SensorKinect (you find the install file inside bin folder)
3. install unstable NITE / OpenNI Middleware Binaries (v1.5.2.7) from http://www.openni.org/Downloads/OpenNIModules.aspx
4. if it doesn't work restart your system

MAC
(tested on OSX 10.6.8)
1. install unstable OpenNI Binaries (v1.5.2.23) from http://www.openni.org/Downloads/OpenNIModules.aspx
2. install SensorKinect (v5.1.0.25) from https://github.com/avin2/SensorKinect (you find the install file inside folder)
3. install unstable NITE / OpenNI Middleware Binaries (v1.5.2.21) from http://www.openni.org/Downloads/OpenNIModules.aspx
4. if it doesn't work install latest MacPorts from http://www.macports.org/ and restart your system



PERSONAL CONFIGURATION FILE

You find the "personalConfig.xml" inside the data folder. 
The configuration settings are saved to and loaded from this file.



OSC CONFIGURATION FILE

You find the "oscConfig.xml" inside the data folder. 
You can open it in any editor and change port or host information.
The configuration settings are loaded from this file.



OSC ADDRESS SETTINGS

- "/hands/start" contains no information, just indicates that hand tracking is active.
- "/hands/ID-centralXYZ" contains one int (Id of the hand) and three floats (central x-, y- and z-position of the hand).

- "/skeletons/start" contains no information, just indicates that skeleton tracking is active.
- "/skeletons/ID-centralXYZ" contains one int (Id of the skel) and three floats (central x-, y- and z-position of the skel).
- "/skeletons/bones/..." contains information of the bones in this scheme: Point A with x and y coordinate and point B with 
  x and y coordinate. That means every bone consists of four floats (boneAx, boneAy, boneBx, boneBy). See following for details:
- "/skeletons/bones/neckShoulder" contains 12 floats (neckAx, neckAy, neckBx, neckBy, leftShoulderAx, leftShoulderAy, 
  leftShoulderBx, leftShoulderBy, rightShoulderAx, rightShoulderAy, rightShoulderBx, rightShoulderBy).
- "/skeletons/bones/leftArm" contains 8 floats (leftUpperArmAx, leftUpperArmAy, leftUpperArmBx, leftUpperArmBy, leftLowerArmAx, 
  leftLowerArmAy, leftLowerArmBx, leftLowerArmBy).
- "/skeletons/bones/rightArm" contains 8 floats (rightUpperArmAx, rightUpperArmAy, rightUpperArmBx, rightUpperArmBy, rightLowerArmAx, 
  rightLowerArmAy, rightLowerArmBx, rightLowerArmBy).
- "/skeletons/bones/torsoHip" contains 20 floats (leftUpperTorsoAx, leftUpperTorsoAy, leftUpperTorsoBx, leftUpperTorsoBy, 
  rightUpperTorsoAx, rightUpperTorsoAy, rightUpperTorsoBx, rightUpperTorsoBy, leftLowerTorsoAx, leftLowerTorsoAy, leftLowerTorsoBx, 
  leftLowerTorsoBy, rightLowerTorsoAx, rightLowerTorsoAy, rightLowerTorsoBx, rightLowerTorsoBy, hipAx, hipAy, hipBx, hipBy).
- "/skeletons/bones/leftLeg" contains 8 floats (leftUpperLegAx, leftUpperLegAy, leftUpperLegBx, leftUpperLegBy, leftLowerLegAx, 
  leftLowerLegAy, leftLowerLegBx, leftLowerLegBy).
- "/skeletons/bones/rightLeg" contains 8 floats (rightUpperLegAx, rightUpperLegAy, rightUpperLegBx, rightUpperLegBy, rightLowerLegAx, 
  rightLowerLegAy, rightLowerLegBx, rightLowerLegBy).

- "/objects/start" contains no information, just indicates that object tracking is active.
- "/objects/ID-centralXYZ" contains one int (Id of the object) and three floats (central x-, y- and z-position of the object).
- "/objects/width-height-size" contains three floats (width, height and size of the object).



FURTHER INFORMATION

http://www.mihoo.de/