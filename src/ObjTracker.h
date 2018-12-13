#pragma once

#include "ofMain.h"

// for calculating velocity, record a number of samples of the ball position
// vel is the total displacement between these frames.
#define POS_SMAPLES 20

class ObjTracker {

public:

	void setup();
	void update(ofVec2f tracking, int _threshold);

	// points that are within threshold go into array
	ofVec2f pos[POS_SMAPLES];
	// other points go into a new array
	// new array is also filtered within threshold to first point in array.
	// if the second point is outside threshold then it becomes the new first point.
	ofVec2f newPos[POS_SMAPLES];
	// once we have POS_SMAPLES consecutive samples in the new array, switch to the new ball position
	int positionSwitcher;
	// int posInd; // index in arrays
	// int newPosInd;
	// int switcherInd;
	int threshold; // distance threshold for filtering
	ofVec2f vel; // velocity in x and y
	float mvel; // total velocity

};
