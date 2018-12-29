#pragma once

#include "ofMain.h"

// for calculating velocity, record a number of samples of the ball position
// vel is the total displacement between these frames.
#define POS_SAMPLES 10
#define POS_AVG_SAMPLES 3


class ObjTracker {

public:

	void setup();
	void update(ofVec2f tracking, int _threshold, float _avgWeighting);

	// points that are within threshold go into array
	ofVec2f pos[POS_SAMPLES];
	// other points go into a new array
	// new array is also filtered within threshold to first point in array.
	// if the second point is outside threshold then it becomes the new first point.
	ofVec2f newPos[POS_SAMPLES];
	// once we have POS_SAMPLES consecutive samples in the new array, switch to the new ball position
	ofVec2f vel[POS_SAMPLES]; // velocity
	ofVec2f spos; // average position
	ofVec2f svel; // average position
	//ofVec2f pSpos; // prev average position (for moving weighted average)
	ofVec2f acc; // acceleration (for predicting motion)
	int positionSwitcher;
	// int posInd; // index in arrays
	// int newPosInd;
	// int switcherInd;
	int threshold; // distance threshold for filtering
	int avgWeighting;
	float mvel; // total velocity
	bool lock; // prevent from switching to new position
	bool lost; // if last tracked position is not in pos[] array
	bool freeze; // don't update the position

	void setPos(ofVec2f _pos);

	// debugging
	int switchcounter;

};
