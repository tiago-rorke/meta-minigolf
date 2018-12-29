#include "ObjTracker.h"

void ObjTracker::setup() {
	for (int i = 0; i < POS_SAMPLES; i++) {
		pos[i] = ofVec2f(0,0);
	}
	spos = ofVec2f(0,0);
	positionSwitcher = 0;
	lock = false;
	freeze = false;
	switchcounter = 0;
	lost = true;
}

void ObjTracker::update(ofVec2f tracking, int _threshold, float _avgWeighting) {

	threshold = _threshold;
	avgWeighting = _avgWeighting;
	if(!freeze) {

		// check pos is not too far away from the previous position
		if(pos[0].distance(tracking) < threshold) {		
			
			for(int i=POS_SAMPLES-1; i>0; i--) {
				pos[i] = pos[i-1];
			}
			pos[0] = tracking;
			lost = false;

			for(int i=POS_SAMPLES-1; i>0; i--) {
				vel[i] = vel[i-1];
			}
			vel[0] = (pos[0] - pos[1]); //[POS_SAMPLES-1]) / POS_SAMPLES;

		} else {

			// otherwise, predict the next position using acc
			//pos[0] = pos[1] + acc;
			lost = true;

			// and store the new position
			if(newPos[0].distance(tracking) < threshold) {
				for(int i=POS_SAMPLES-1; i>0; i--) {
					newPos[i] = newPos[i-1];
				}
				newPos[0] = tracking;
				positionSwitcher++;
			} else {
				newPos[0] = tracking;
				positionSwitcher = 0;			
			}
		}

		// if the new position is stable, change to that position
		if(positionSwitcher >= POS_SAMPLES-1 && !lock) {
			for(int i=0; i<POS_SAMPLES; i++) {
				pos[i] = newPos[i];
			}
			positionSwitcher = 0;

			// debugging
			switchcounter++;
			//cout << "switch: " << switchcounter << endl;
		}


		/// THIS IS BROKEN ======================================================//

		//spos.average(pos, POS_SAMPLES);

		//pSpos = spos;
		//float x = (avgWeighting * pos[0].x);// + ((1-avgWeighting) * pSpos.x);
		//float y = (avgWeighting * pos[0].y);// + ((1-avgWeighting) * pSpos.y);
		//spos = pos[0];
		//spos.scale(avgWeighting * spos.length());
		//pSpos.scale(1-avgWeighting * pSpos.length());
		//spos += pSpos;
		//spos = pos[0];
		//spos = ofVec2f(x,y);
		/*
		float x = 0;
		float y = 0;
		*/
		spos = ofVec2f(0,0);
		for(int i=0; i<POS_AVG_SAMPLES; i++) {
			spos += pos[i];
			//x += pos[i].x;
			//y += pos[i].y;
		}
		spos /= POS_AVG_SAMPLES;
		//x /= POS_SAMPLES;;
		//y /= POS_SAMPLES;;
		//spos.x = x;
		//spos.y = y;
		//spos = ofVec2f(100,100);// pos[0];
		
		svel = ofVec2f(0,0);
		for(int i=0; i<POS_SAMPLES; i++) {
			svel += vel[i];
		}
		svel /= POS_SAMPLES;

		//acc = vel[1] - vel[0];
		mvel = vel[0].length();

	}


}


void ObjTracker::setPos(ofVec2f _pos) {

	for (int i = 0; i < POS_SAMPLES; i++) {
		pos[i] = _pos;
	}

}