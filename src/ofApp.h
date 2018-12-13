#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "GuiApp.h"
#include "ofxOsc.h"

#include "Scenes.h"
#include "ObjTracker.h"

// port for listening for osc for tracking
#define PORT_TRACKING 12345

// #define USE_CAM
#define USE_OSC_TRACKING
// #define USE_PROJECTION
// #define CALIBRATE
// #define DEBUG // draw tracking data etc...

// for calculating velocity, record a number of samples of the ball position
// vel is the total displacement between these frames.
#define POS_SMAPLES 5


class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	
	// ofxPanel gui;
	shared_ptr<GuiApp> gui;

	// osc  
	ofxOscReceiver oscTracking;

	// comupter vision
	ofVideoGrabber cam;
   ofxCvColorImage camColor;
   ofxCvGrayscaleImage camGray;
	ofxCvGrayscaleImage camBkgd;
   ofxCvGrayscaleImage camDiff;
	ofImage camUndistorted;
	ofxCvContourFinder contourFinder;
	int threshold;
	bool getBkgd;
	int bkgdCounter;
	bool trackingCalibrated;
	int ballId; // blob ids
	int holeId;
	ofVec2f ballPos;
	ofVec2f holePos;
	bool gotTracking;
	ofVec2f ballOrigin;
	bool setBallOrigin;
	bool gotBallOrigin;
	// in gui > 
	/*
	<int> blobMin;
	<int> blobMax;
	<int> ballBlob;
	<int> holeBlob;
	<int> blobSizeRange;
	<float> blobSquare;
	*/

	// dims for physical floor and elements
	float floorWidth; // inc. wall thickness
	float floorHeight;
	float ballSize;
	float wallSize;
	// dims for digital image (px) (before adjustments for projection mapping)
	int imgWidth;
	float imgHeight; // imgHeight is derived from imgWidth and floorHeight from settings.yml.
	float imgScale; // is used for calculating all other dims
	ofVec2f imgOffset;
	
	// Camera Calibration
	ofxCv::Calibration calibration;
	int gridCountX;
	int gridCountY;
	float trackingScale; // ratio of camera image width to imgWidth (or incomingtracking coords via osc)
	ofVec2f trackingOffset;
	int gridIndex; // for interating through grid to define vertex positions
	float gridResX; // size of grid cells in pixels for projection, at imgScale // originally "squareSize" from checkboard openCV example
	float gridResY;
	vector<cv::Point2f> calibrationPoints;
	
	// Projection mapping data
	ofVec2f mapOffset;
	// in gui > 
	/*
	<float> mapScale;
	<float> mapXoff;
	<float> mapYoff;
	*/

	void updateOffsets();
	void updateVision();
	bool updateTracking();
	void addCalibrationPoint();
	void updateCalibration();
	void drawGrid();
	void drawTracking();
	void drawFloor();
	void drawPlay();

	void updateOscTracking();

	// tracker object filters positions and calcs velocity
	ObjTracker ball;
	ObjTracker hole;

	Scenes scenes;
	int scene;

	// DEGBUGGIN

	bool toggleUndistort;

};
