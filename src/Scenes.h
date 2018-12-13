#pragma once

#include "ofMain.h"

class Scenes {

public:

	int width, height;
	bool gotTracking;
	bool gotBallOrigin;
	ofVec2f ballPos;
	ofVec2f holePos;
	ofVec2f ballOrigin;
 	float bx, by, hx, hy, bxo, byo, bhd, bod, bha;
 	float offset;

 	void setup(int _width, int _height);
	void update(bool _gotTracking, bool _gotBallOrigin, ofVec2f _ballPos, ofVec2f _holePos, ofVec2f _ballOrigin);

	void circleLine();
	void parabolas();
	void circles();
	void parallels();
	void elevator();

};