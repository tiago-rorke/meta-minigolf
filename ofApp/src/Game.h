#pragma once

#include "ofMain.h"

class Game {

public:

 	void setup(int _width, int _height);
	void update(
		bool _gotTracking, ofVec2f _ballPos, ofVec2f _holePos, ofVec2f _ballVel, 
		float _ballVelScalar, float _targetThreshold, bool _holeFreeze, float _ballRad, float _holeRad);
	void draw();

	// game states
	bool holePlaced;
	bool ballPlaced;
	bool ready;
	bool transition;
	int scene;

	// tracking
	bool gotTracking;
	bool gotBallOrigin;
	ofVec2f ballPos;
	ofVec2f holePos;
	ofVec2f ballOrigin;
 	ofVec2f ballVel;
 	float ballVelScalar;
 	float bx, by, hx, hy, bxo, byo, bhd, bod, bha, bv;
 	float targetThreshold;
 	bool holeFreeze;
 	float ballRad;
 	float holeRad;

	ofVec2f ballTarget;
	ofVec2f holeTarget;

	// drawing
	int width, height;
 	float offset;
 	float highlightRad;
 	float highlightSpot;
 	float highlightFill;
 	float highlightVel;
	bool highlightBall;
	bool highlightBallVel;

 	void changeScene();
	float maxCornerDist(ofVec2f p, float w, float h);

	// scenes
	void circleLine();
	void parabolas();
	void circles();
	void parallels();
	void elevator();

};