#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "GuiApp.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	
	//ofxPanel gui;
	shared_ptr<GuiApp> gui;

	// comupter vision
	ofVideoGrabber cam;
   ofxCvColorImage camColor;
   ofxCvGrayscaleImage camGray;
	ofxCvGrayscaleImage camBkgd;
   ofxCvGrayscaleImage camDiff;
	ofxCvContourFinder contourFinder;
	int threshold;
	bool getBkgd;
	int bkgdCounter;
	bool camCalibrated;
	int ballId; // blob ids
	int holeId;
	ofVec2f ballPos;
	ofVec2f holePos;
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
	float camScale; // ratio of camera image width to imgWidth
	int gridIndex; // for interating through grid to define vertex positions
	float gridRes; // size of grid cells in pixels for projection, at imgScale // "squareSize"
	
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
	void updateTracking();
	void drawGrid();
	void drawTracking();
	void drawFloor();
	void drawPlay();
		
};