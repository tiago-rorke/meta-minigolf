#include "Game.h"

void Game::setup(int _width, int _height) {
	width = _width;
	height = _height;
	offset = 0;
	holePlaced = false;
	ballPlaced = false;
	ready = false;
	transition = false;
	highlightVel = 10;
	highlightSpot = 20;
	highlightFill = highlightSpot;

	highlightBall = false;
	highlightBallVel = false;
	//hgihlightRad = highlightSpot;
}


void Game::update(
	bool _gotTracking, ofVec2f _ballPos, ofVec2f _holePos, ofVec2f _ballVel, 
	float _ballVelScalar, float _targetThreshold, bool _holeFreeze, float _ballRad, float _holeRad) {
	
	gotTracking = _gotTracking;
	ballPos = _ballPos;
	ballVel = _ballVel;
	holePos = _holePos;
	ballVelScalar = _ballVelScalar;
	targetThreshold = _targetThreshold;
	holeFreeze = _holeFreeze;
	ballRad = _ballRad;
	holeRad = _holeRad;

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
	bv = ballVel.length();

	if(transition) {
		highlightRad += highlightVel;
		if(highlightRad > highlightFill) {//2*width) {
			/*
			if(!holePlaced) {
				holePlaced = true;
			} else if(!ballPlaced) {
				ballPlaced = true;
				ready = true;
				highlightRad = highlightSpot;
			} else {
				changeScene();
				//ready = false;
			}
			*/
			changeScene();
			transition = false;
		}
	}


	if(!holePlaced && holeFreeze) {
		holePlaced = true;
		changeScene();
		scene = 0;
		/*
		if(holePos.distance(holeTarget) < targetThreshold) {
			transition = true;
			highlightFill = maxCornerDist(holePos, width, height);
		} else {
			highlightRad = highlightSpot;
		}*/
	} 


	if(!ballPlaced) {
		if(ballPos.distance(ballTarget) < targetThreshold) {
			ballPlaced = true;
			ballOrigin = ballPos;
			gotBallOrigin = true;
			ready = true;
			//transition = true;
			//highlightFill = maxCornerDist(ballPos, width, height);
		} else {
			highlightRad = highlightSpot;
		}
	} else if(ready) {
		if(ballPos.distance(holePos) < targetThreshold) {
			transition = true;
			ready = false;
			highlightRad = highlightSpot;
			highlightFill = maxCornerDist(holePos, width, height);
		}
	}

	//game.ready = true;

}

float Game::maxCornerDist(ofVec2f p, float w, float h) {
	float b[4]; 
	b[0] = p.distance(ofVec2f(0, 0));
	b[1] = p.distance(ofVec2f(w, 0));
	b[2] = p.distance(ofVec2f(w, h));
	b[3] = p.distance(ofVec2f(0, h));
	float a = 0;
	for(int i=0; i<4; i++) {
		a = b[i] > a ? b[i] : a;
	}
	return a;
}


void Game::draw() {

	if(ready && holePlaced) {
		switch(scene) {
			case 0:
				circleLine();
				break;
			case 1:
				circles();
				break;
			case 2:
				parabolas();
				break;
			case 3:
				parallels();
				break;
		}
	}

	if(holePlaced) {
		ofNoFill();
		ofSetHexColor(0xffffff); 
		ofDrawCircle(holePos.x, holePos.y, holeRad);
		
		float br;
		if(highlightBall) {
			if(highlightBallVel){
				br = ballRad + (bv * ballVelScalar);
			}else{
				br = ballRad;
			}
			ofDrawCircle(ballPos.x, ballPos.y, br);
			ofFill();
			ofSetHexColor(0x000000);
			ofDrawCircle(ballPos.x, ballPos.y, br - 2);
		}


		if(!ballPlaced) {
			ofNoFill();
			ofSetHexColor(0xffffff);
			//ofDrawCircle(ballPos.x, ballPos.y, highlightRad);
			ofDrawLine(ballPos.x, ballPos.y, ballTarget.x, ballTarget.y);
			ofDrawCircle(ballTarget.x, ballTarget.y, highlightRad);
			ofFill();
			ofSetHexColor(0x000000);
			ofDrawCircle(ballTarget.x, ballTarget.y, highlightRad-2);
			ofDrawCircle(ballPos.x, ballPos.y, br - 2);
		}		
		
	}

	ofFill();
	if(!holeFreeze) {
		ofSetHexColor(0xffffff);
	} else {
		ofSetHexColor(0x000000);
	}	
	ofDrawCircle(holePos.x, holePos.y, holeRad - 2);
	if(transition) {
		ofFill();
		ofSetHexColor(0xffffff);
		ofDrawCircle(holePos.x, holePos.y, highlightRad);
	}

	/*
	if(!holePlaced) {
		ofDrawCircle(holeTarget.x, holeTarget.y, highlightRad);
	} else
	} else
	*/	
}


void Game::changeScene() {

	/*
	holeTarget.x = ofRandom(width);
	holeTarget.y = ofRandom(height);
	*/

	while(true) {
		ballTarget.x = ofRandom(width - 100) + 50;
		ballTarget.y = ofRandom(height - 100) + 50;
		if(ballTarget.distance(holePos) > width/3) {
			break;
		}
	}

	ballPlaced = false;
	//holePlaced = false;
	
	scene++;
	scene = scene % 4;
	ready = false;

	highlightBall = true;
	highlightBallVel = false;

}



////// SIMPLE CIRCLE AND LINE
void Game::circleLine() {

	highlightBall = true;
	highlightBallVel = true;

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
void Game::parabolas() {

	highlightBall = false;
	highlightBallVel = false;

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
void Game::circles() {

	highlightBall = true;
	highlightBallVel = true;

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
void Game::parallels() {

	highlightBall = false;
	highlightBallVel = false;

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
void Game::elevator() {

	if(gotTracking) {
		ofNoFill();
		ofSetHexColor(0xffffff); 
		ofPushMatrix();
		ofTranslate(hx, hy);
		ofScale(1- 1/bv); /// ==================================== ????
		ofPopMatrix();
	}

}
