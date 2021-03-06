#pragma once

#include "ofMain.h"
#include "ofxGui.h"

class GuiApp: public ofBaseApp {
public:
	void setup();
	void update();
	void draw();

	ofParameterGroup parameters;

	ofParameter<float> mapScale;
	ofParameter<float> mapXoff;
	ofParameter<float> mapYoff;
	ofParameter<int> blobMin;
	ofParameter<int> blobMax;
	ofParameter<int> ballBlob;
	ofParameter<int> holeBlob;
	ofParameter<int> blobSizeRange;
	ofParameter<float> blobSquare;
	ofParameter<float> trackingThreshold;
	ofParameter<float> targetThreshold;
	ofParameter<float> avgWeighting;
	ofParameter<float> holeRad;
	ofParameter<float> ballRad;
	ofParameter<float> ballVelScalar;

	ofxPanel gui;
};