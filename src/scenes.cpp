#include "Scenes.h"

void Scenes::setup(int _width, int _height) {
	width = _width;
	height = _height;
	offset = 0;
}


void Scenes::update(bool _gotTracking, bool _gotBallOrigin, ofVec2f _ballPos, ofVec2f _holePos, ofVec2f _ballOrigin) {
	
	gotTracking = _gotTracking;
	gotBallOrigin = _gotBallOrigin;
	ballPos = _ballPos;
	holePos = _holePos;
	ballOrigin = _ballOrigin;

	bx = ballPos.x;
	by = ballPos.y;
	hx = holePos.x;
	hy = holePos.y;
	bxo = ballOrigin.x;
	byo = ballOrigin.y;
	
	bhd = ofDist(bx, by, hx, hy);
	bod = ofDist(bxo, byo, hx, hy);
	bha = -glm::atan((bx-hx)/(by-hy));
	if(by-hy > 0) {
		bha += ofDegToRad(180.f);
	}

}


////// SIMPLE CIRCLE AND LINE
void Scenes::circleLine() {

	if(gotTracking) {
		ofPushMatrix();
		ofTranslate(bx, by);
		ofRotateRad(bha);
		ofNoFill();
		ofSetHexColor(0xffffff); 
		ofDrawCircle(0, 0, bhd);
		ofDrawLine(0, 0, 0, bhd);
		ofPopMatrix();
	}	
}


////// CONDENSING PARABOLAS
void Scenes::parabolas() {

	float holeMargin = 20;
	if(gotTracking && gotBallOrigin) {
		ofPushMatrix();
		ofTranslate(bx, by);
		ofRotateRad(bha);
		ofNoFill();
		ofSetHexColor(0xffffff); 
		int n = 20;
		float di = (bhd - holeMargin) / n;
		float a = 1-(bhd / bod);
		if(a < 0) a = 0;
		for (float y = di; y <= bhd - holeMargin;  y += di) {
			ofBeginShape();
			for (float x = -2000; x < 2000; x += 10) {
				float y1 = y + 0.01 * a * y/bhd * x * x;
				ofVertex(ofPoint(x,y1));
			}
			//ofDrawLine(x, y, x, y);
	 		ofEndShape();
		}
		ofPopMatrix();
	}	
}


////// CONCENTRIC CIRCLES
void Scenes::circles() {

	if(gotTracking) {
		ofNoFill();
		ofSetHexColor(0xffffff); 
		float a = bhd/10;
		if(bhd > 10) {
			ofPushMatrix();
			ofTranslate(bx, by);
			for (float i = 0; i < width*2; i+= a) {
				ofDrawCircle(0,0,i);
			}
			ofPopMatrix();
			ofPushMatrix();
			ofTranslate(hx, hy);
			for (float i = 0; i < width*2; i+= a) {
				ofDrawCircle(0,0,i);
			}
			ofPopMatrix();
		}
	}
}


/////// SCROLLING PARALLEL LINES
void Scenes::parallels() {

	int spacing = bhd/10;
	if(gotTracking) {
		ofPushStyle();
		ofNoFill();
		ofSetHexColor(0xffffff); 
		float a = bhd/10;
		if(bhd > 10) {
			ofPushMatrix();
			ofTranslate(bx, by);
			ofRotateRad(bha + ofDegToRad(90));
			for (float i = -2*width; i < 2*width; i+= spacing) {
				ofSetLineWidth(glm::abs(i/100));//10.0 * width/(i + offset)));
				ofDrawLine(-2*width, i + offset, 2*width, i + offset);
			}
			ofPopMatrix();
		}
		ofPopStyle();
	}
	offset += 2;
	if(offset > spacing)
		offset -= spacing;
}



//////// ROOM THAT ELEVATES WITH VELOCITY
void Scenes::elevator() {

	if(gotTracking) {
		ofNoFill();
		ofSetHexColor(0xffffff); 
		
		ofPushMatrix();
		ofPopMatrix();
	}

}
