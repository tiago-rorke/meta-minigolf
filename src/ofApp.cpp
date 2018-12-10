#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void ofApp::setup(){
	ofSetVerticalSync(true);
	ofSetWindowPosition(1920,0);
	cam.setup(640, 480);

	// load settings
	FileStorage settings(ofToDataPath("settings.yml"), FileStorage::READ);
	if(settings.isOpened()) {
		gridCountX = settings["xCount"];
		gridCountY = settings["yCount"];
		floorWidth = settings["floorWidth"];
		floorHeight = settings["floorHeight"];
		ballSize = settings["ballSize"];
		wallSize = settings["wallSize"];
		imgWidth = settings["imgWidth"];
		gui->mapScale = settings["mapScale"];
		gui->mapXoff = settings["mapXoff"];
		gui->mapYoff = settings["mapYoff"];
		gui->blobMin = settings["blobMin"];
		gui->blobMax = settings["blobMax"];
		gui->ballBlob = settings["ballBlob"];
		gui->holeBlob = settings["holeBlob"];
		gui->blobSizeRange = settings["blobSizeRange"];
		gui->blobSquare = settings["blobSquare"];
	} else {
		// use default values
		cout << "cannot load settings.yml, using default values" << endl;
		gridCountX = 6;
		gridCountY = 4;
		floorWidth = 3500;
		floorHeight = 2000;
		ballSize = 60;
		wallSize = 32;
		imgWidth = 1024;
		gui->mapScale = 0.8665;
		gui->mapXoff = -8.5;
		gui->mapYoff = -28;
		gui->blobMin = 30;
		gui->blobMax = 450;
		gui->ballBlob = 50;
		gui->holeBlob = 340;
		gui->blobSizeRange = 100;
		gui->blobSquare = 0.8;
	}

	float floorGridHeight = floorHeight - 2*wallSize - ballSize;
	float gridHeight = (float(imgWidth)/floorWidth) * floorGridHeight;
	gridRes = gridHeight/(gridCountY-1);
	imgHeight = (floorHeight/floorWidth) * imgWidth;
	imgScale = imgWidth/floorWidth;
	camScale = imgWidth/cam.getWidth();
	
	updateOffsets();
	ballId = -1;
	holeId = -1;

	cout << "img size = " << imgWidth << " x " << imgHeight << endl;
	cout << "img offset = " << imgOffset.x << "," << imgOffset.y << endl;
	cout << "ballSize = " << ballSize << endl;
	cout << "wallSize = " << wallSize << endl;
	cout << "floorWidth = " << floorWidth << endl;
	cout << "floorHeight = " << floorHeight << endl;
	cout << "floorGridHeight = " << floorGridHeight << endl;
	cout << "gridHeight = " << gridHeight << endl;
	cout << "gridRes = " << gridRes << endl;
	cout << "mapScale = " << gui->mapScale << endl;
	cout << "mapXoff = " << gui->mapXoff << endl;
	cout << "mapYoff = " << gui->mapYoff << endl;
	
	// blobs
	getBkgd = true;
	bkgdCounter = 0;
	threshold = 80;

	// calibration
	calibration.setPatternSize(gridCountX, gridCountY);
	calibration.setSquareSize(gridRes);
	CalibrationPattern patternType;
	patternType = CHESSBOARD;
	calibration.setPatternType(patternType);
	gridIndex = 0;
	camCalibrated = false;

}


void ofApp::update(){
	
	updateOffsets();
	updateVision();
	updateTracking();

}


void ofApp::draw(){
	ofBackground(0,0,0);
	
	ofPushMatrix();
	ofTranslate(mapOffset.x, mapOffset.y);
	ofScale(gui->mapScale);

	if(!camCalibrated) {
		ofPushMatrix();
		ofScale(camScale);
		drawTracking();
		
		ofPopMatrix();
		ofTranslate(imgOffset.x, imgOffset.y);
		drawFloor();
		drawGrid();
		drawPlay();

	} else {

	}
	ofPopMatrix();

	//gui.draw();
}


// ------------------------------------------------------------- //


void ofApp::updateVision() {

	cam.update();
	if(cam.isFrameNew()) {
	   camColor.setFromPixels(cam.getPixels());
	   camGray = camColor;
	   if(!camCalibrated) {
			if(getBkgd) {
				camBkgd = camGray;
				bkgdCounter++;
				if(bkgdCounter > 1) { // looks like the first frame from the camera is black.
					getBkgd = false;
				}
			}
			camDiff.absDiff(camGray, camBkgd);
			camDiff.threshold(threshold);

			contourFinder.findContours(camDiff, gui->blobMin, gui->blobMax, 10, true);
		}
	}
}

void ofApp::updateOffsets() {

	float w = ofGetWidth();
	float h = ofGetHeight();
	mapOffset.x = (w - w*gui->mapScale)/2 + gui->mapXoff;
	mapOffset.y = (h - h*gui->mapScale)/2 + gui->mapYoff;
	imgOffset.x = w/2 - imgWidth/2;
	imgOffset.y = h/2 - imgHeight/2;

}


void ofApp::updateTracking() {
	
	ballId = -1;
	holeId = -1;
	for (int i = 0; i < contourFinder.nBlobs; i++){
		float a = contourFinder.blobs[i].area;
		float r = gui->blobSizeRange;
		float w = contourFinder.blobs[i].boundingRect.getWidth();
		float h = contourFinder.blobs[i].boundingRect.getHeight();
		float s = glm::abs(w/h);
		if(s >= gui->blobSquare) {
			float b = gui->ballBlob;
			if(a < b + r && a > b - r) {
				ballId = i;
			}
			b = gui->holeBlob;
			if(a < b + r && a > b - r) {
				holeId = i;
			}
		}
	}

	// update tracking results
	if(ballId >= 0 && holeId >= 0 && ballId < contourFinder.blobs.size() && holeId < contourFinder.blobs.size() ) {
		ballPos.x = contourFinder.blobs[ballId].boundingRect.getCenter().x;
		ballPos.y = contourFinder.blobs[ballId].boundingRect.getCenter().y;
		holePos.x = contourFinder.blobs[holeId].boundingRect.getCenter().x;
		holePos.y = contourFinder.blobs[holeId].boundingRect.getCenter().y;
		ballPos *= camScale;
		holePos *= camScale;
		ballPos -= imgOffset;
		holePos -= imgOffset;
	}

}


// ------------------------------------------------------------- //


void ofApp::drawTracking() {

	ofFill();
	ofSetHexColor(0xffffff);
	//camDiff.draw(0, 0);
	cam.draw(0, 0);
	//ofNoFill();
	//ofSetHexColor(0xff0000);
	//ofDrawRectangle(0,0,cam.getWidth(), cam.getHeight());
	
	//ofFill();
	ofSetHexColor(0xffffff);

	if(ballId >= 0) {
		float x = contourFinder.blobs[ballId].boundingRect.getCenter().x;
		float y = contourFinder.blobs[ballId].boundingRect.getCenter().y;
		contourFinder.blobs[ballId].draw(0,0);
		//ofDrawCircle(x,y,20);
		ofDrawBitmapString("BALL", x - 10, y - 10);
	}
	if(holeId >= 0) {
		float x = contourFinder.blobs[holeId].boundingRect.getCenter().x;
		float y = contourFinder.blobs[holeId].boundingRect.getCenter().y;
		contourFinder.blobs[holeId].draw(0,0);
		//ofDrawCircle(x,y,30);
		ofDrawBitmapString("HOLE", x-10, y-10);
	}

}


void ofApp::drawGrid() {

	ofFill();
	ofSetHexColor(0xffffff);
	// calc grid origin
	float gridWidth = ((gridCountX-1) * gridRes);
	float gridHeight = ((gridCountY-1) * gridRes);
	float ox = imgWidth/2 - gridWidth/2;
	float oy = imgHeight/2 - gridHeight/2;
	for (int i = 0; i < gridCountX; i++) {
		float x = ox + i * gridRes;
		ofDrawLine(x, oy, x, oy + gridHeight);
		for (int h = 0; h < gridCountY; h++)	{
			float y = oy + h * gridRes;
			ofDrawCircle(x, y, 5);
			if(i == 0) {
				ofDrawLine(ox, y, ox + gridWidth, y);
			}
		}
	}

	float x = gridIndex % gridCountX;
	float y = floor(gridIndex/gridCountX);
	ofNoFill();
	ofSetHexColor(0xff0000);
	ofDrawCircle(ox + x*gridRes, oy + y*gridRes, 20);
}


void ofApp::drawFloor() {

	ofFill();
	ofSetHexColor(0xffffff);
	float w = wallSize*imgScale;
	//ofDrawRectangle(0, 0, imgWidth, imgHeight);
	//ofSetHexColor(0x000000);
	//ofDrawRectangle(w, w, imgWidth - 2*w, imgHeight- 2*w);
	ofDrawRectangle(0, 0, imgWidth, w);
	ofDrawRectangle(0, 0, w, imgHeight);
	ofDrawRectangle(0, imgHeight-w, imgWidth, w);
	ofDrawRectangle(imgWidth-w, 0, w, imgHeight);
}


void ofApp::drawPlay() {

	ofFill();
	ofSetHexColor(0xff0000); 
	ofDrawCircle(ballPos.x, ballPos.y, 10);
	ofDrawCircle(holePos.x, holePos.y, 10);

	float d = ofDist(ballPos.x, ballPos.y, holePos.x, holePos.y);
	ofFill();
	ofSetHexColor(0xffffff); 
	//ofDrawCircle(holePos.x, holePos.y, d);

}


// ------------------------------------------------------------- //

void ofApp::keyPressed(int key){

	switch (key){
		case ' ':
			getBkgd = true;
			break;
		case 'f':
			ofToggleFullscreen();
			break;
		case 'p':
			gridIndex++;
			cout << gridIndex << endl;
			break;
	}
}



/*
// For porting from processing

class Hole {

	PVector pos;

	Hole(Tracking tracking) {
		pos = new PVector();
		update(tracking);
	}

	void update(Tracking tracking) {
		pos.x = tracking.hole.x;
		pos.y = tracking.hole.y;
	}

}



class Ball {
	
	PVector[] pos = new PVector[BALL_POS_SMAPLES];
	int ind; // index in pos array
	PVector vel; // velocity in x and y
	float mvel; // total velocity

	Ball(Tracking tracking) {
		for (int i = 0; i < BALL_POS_SMAPLES; i++) {
			pos[i] = new PVector();
			pos[i].x = tracking.ball.x;
			pos[i].y = tracking.ball.y;
		}
		vel = new PVector(0, 0);
		ind = 0;
	}

	void update(Tracking tracking) {
		pos[ind].x = tracking.ball.x;
		pos[ind].y = tracking.ball.y;
		ind++;
		ind = ind >= BALL_POS_SMAPLES ? 0 : ind;
		float dtime = BALL_POS_SMAPLES / FRAME_RATE;
		float dx = pos[0].x - pos[BALL_POS_SMAPLES-1].x;
		float dy = pos[0].y - pos[BALL_POS_SMAPLES-1].y;
		vel.x = dx/dtime;
		vel.y = dy/dtime;
		mvel = vel.mag();
	}

}
*/