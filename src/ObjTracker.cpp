#include "ObjTracker.h"

void ObjTracker::setup() {
	for (int i = 0; i < POS_SMAPLES; i++) {
		pos[i] = ofVec2f(0,0);
	}
	positionSwitcher = 0;
}

void ObjTracker::update(ofVec2f tracking, int _threshold) {

	threshold = _threshold;
	
	if(pos[0].distance(tracking) < threshold) {
		
		for(int i=1; i<POS_SMAPLES; i++) {
			pos[i] = pos[i-1];
		}
		pos[0] = tracking;
		//cout << '|' ;

	} else {

		if(newPos[0].distance(tracking) < threshold) {
			for(int i=1; i<POS_SMAPLES; i++) {
				newPos[i] = newPos[i-1];
			}
			newPos[0] = tracking;
			positionSwitcher++;
		} else {
			newPos[0] = tracking;
			positionSwitcher = 0;			
		}
	}

	if(positionSwitcher >= POS_SMAPLES-1) {
		for(int i=0; i<POS_SMAPLES; i++) {
			pos[i] = newPos[i];
		}
		positionSwitcher = 0;
		//cout << "switch" << endl;
	}

	vel = pos[0] - pos[POS_SMAPLES-1];
	mvel = vel.length();



}