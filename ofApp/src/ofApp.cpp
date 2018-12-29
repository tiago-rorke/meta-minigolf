#include "ofApp.h"

using namespace ofxCv;
using namespace cv;


void ofApp::setup(){
	
	ofSetWindowTitle("meta minigolf");
	ofSetVerticalSync(true);

	oscSound.setup(HOST_SOUND, PORT_SOUND);

	#ifdef USE_OSC_TRACKING
		oscTracking.setup(PORT_TRACKING);
	#elif USE_CAM
		cam.setup(640, 480);
	#else
		ballPos = ofVec2f(100,100);
		holePos = ofVec2f(300,100);
		// this is doing wierd things
		//ballPos = ofVec2f(imgWidth/4, imgHeight/2);
		//holePos = ofVec2f(3*imgWidth/4, imgHeight/2);
	#endif

	#ifdef USE_PROJECTION
		ofSetWindowPosition(1920,0);
	#else
		ofSetWindowPosition(500,100);
	#endif


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
		gui->trackingThreshold = settings["trackingThreshold"];
		gui->targetThreshold = settings["targetThreshold"];
		gui->avgWeighting = settings["avgWeighting"];
		gui->ballRad = settings["ballRad"];
		gui->holeRad = settings["holeRad"];
		gui->ballVelScalar = settings["ballVelScalar"];

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

	#ifdef USE_CAM
		trackingScale = imgWidth/cam.getWidth();
		trackingOffset = ofVec2f(0,0);  // ???
	#else
		trackingScale = 1;
		trackingOffset = ofVec2f(0,0);
	#endif

	#ifdef CALIBRATE
		trackingCalibrated = false;
	#else
		trackingCalibrated = true;
	#endif


	float floorGridHeight = floorHeight - 2*wallSize - ballSize;
	float gridHeight = (float(imgWidth)/floorWidth) * floorGridHeight;
	gridResY = gridHeight/(gridCountY-1);
	float floorGridWidth = floorWidth - 2*wallSize - ballSize;
	float gridWidth = (float(imgWidth)/floorWidth) * floorGridWidth;
	gridResX = gridWidth/(gridCountX-1);

	imgHeight = (floorHeight/floorWidth) * imgWidth;
	imgScale = imgWidth/floorWidth;
	game.setup(imgWidth, imgHeight);
		
	updateOffsets();
	ballId = -1;
	holeId = -1;

	cout << "img size = " << imgWidth << " x " << imgHeight << endl;
	cout << "img offset = " << imgOffset.x << "," << imgOffset.y << endl;
	cout << "ballSize = " << ballSize << endl;
	cout << "wallSize = " << wallSize << endl;
	cout << "floorWidth = " << floorWidth << endl;
	cout << "floorHeight = " << floorHeight << endl;
	cout << "floor grid size = " << floorGridWidth << "x" << floorGridHeight << endl;
	cout << "grid size = " << gridWidth << "x" << gridHeight << endl;
	cout << "gridRes = " << gridResX << "x" << gridResY << endl;
	cout << "mapScale = " << gui->mapScale << endl;
	cout << "mapXoff = " << gui->mapXoff << endl;
	cout << "mapYoff = " << gui->mapYoff << endl;
	
	// blobs
	getBkgd = true;
	bkgdCounter = 0;
	threshold = 80;

	// calibration
	/*
	imitate(camUndistorted, cam);
	calibration.setPatternSize(gridCountX, gridCountY);
	calibration.setSquareSize(gridRes);
	CalibrationPattern patternType;
	patternType = CHESSBOARD;
	calibration.setPatternType(patternType);
	*/
	gridIndex = 0;
	// vision
	ball.setup();
	hole.setup();

	// graphics
	ofSetCircleResolution(100);


	// DEBUGGING
	toggleUndistort = true;

}


void ofApp::update(){
	
	#ifdef USE_OSC_TRACKING
	updateOscTracking();
	//updateSound(); // moved to inside updateOscTracking
	#endif
	#ifdef USE_CAM
		updateVision();
		if(updateCamTracking()) {
			gotTracking = true;
			/*
			if(setBallOrigin) {
				game.ballOrigin = ball.spos;
				setBallOrigin = false;
				gotBallOrigin = true;
			}
			*/
		}
	#else
	gotTracking = true;
	/*
	if(setBallOrigin) {
		game.ballOrigin = ballPos;
		setBallOrigin = false;
		gotBallOrigin = true;
	}
	*/
	#endif
	updateOffsets();
	if(ball.lost) {
		//???
	}
	if(ball.spos.distance(hole.spos) < gui->targetThreshold) {
		ball.setPos(hole.spos);
		ball.lock;
	}
	game.update(
		gotTracking, ball.spos, hole.spos, 
		ball.svel, gui->ballVelScalar, gui->targetThreshold, hole.freeze, gui->ballRad, gui->holeRad);
	if(game.transition) {
		hole.lock = true;
	} else {
		hole.lock = false;		
	}

}


void ofApp::draw(){
	ofBackground(0,0,0);
	
	ofPushMatrix();
	ofTranslate(mapOffset.x, mapOffset.y);
	ofScale(gui->mapScale);
	ofTranslate(imgOffset.x, imgOffset.y);

	#ifdef DEBUG
	ofPushMatrix();
	ofScale(trackingScale);
	drawTracking();
	ofPopMatrix();
	#endif
	if(!trackingCalibrated) {
		drawGrid();
	}
	drawPlay();
	drawFloor();
	ofPopMatrix();

	//gui.draw();
}


// ------------------------------------------------------------- //


void ofApp::updateVision() {

	cam.update();

	if(trackingCalibrated) {
		calibration.undistort(toCv(cam), toCv(camUndistorted));
		camUndistorted.update();
	}

	if(cam.isFrameNew()) {
		if(!trackingCalibrated) {
	   	camColor.setFromPixels(cam.getPixels());
		} else {
	   	camColor.setFromPixels(camUndistorted.getPixels());			
		}
	   camGray = camColor;
//	   if(!trackingCalibrated) {
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
	//	}
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


bool ofApp::updateCamTracking() {
	
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
		ballPos *= trackingScale;
		holePos *= trackingScale;
		ballPos += trackingOffset;
		holePos += trackingOffset;
		return true;
	} else {
		return false;
	}


}


void ofApp::addCalibrationPoint() {

	if(ballId >= 0) {
		cv::Point2f point(
			contourFinder.blobs[ballId].boundingRect.getCenter().x,
			contourFinder.blobs[ballId].boundingRect.getCenter().y
		);
		calibrationPoints.push_back(point);
		cout << gridIndex << ": " << point.x << "," << point.y << endl;
		gridIndex++;
	} else {
		cout << "cannot see ball" << endl;
	}

	if(gridIndex >= gridCountX * gridCountY) {
		//cout << "applying calibration..." << endl;
		//updateCalibration();
		//cout << "done" << endl;
		cout << "finished adding calibration points" << endl;
		trackingCalibrated = true;
		gridIndex = 0;
	}
	//cout << gridIndex << endl;
}


void ofApp::updateCalibration() {

	Size imgSize(cam.getWidth(), cam.getHeight());
	calibration.add(calibrationPoints, imgSize);
	calibration.calibrate();
	/*
	if(calibration.size() > startCleaning) {
		calibration.clean();
	}
	*/
	calibration.save("calibration.yml");
}

// ------------------------------------------------------------- //


void ofApp::drawTracking() {

	#ifdef USE_CAM

		ofFill();
		ofSetHexColor(0xffffff);
		//camDiff.draw(0, 0);

		if(!trackingCalibrated) {
			camColor.draw(0,0);
			//cam.draw(0, 0);
		} else {
			if(toggleUndistort) {
				camColor.draw(0,0);
				//camUndistorted.draw(0,0);
			} else {
				cam.draw(0, 0);
			}
		}
		
		//ofNoFill();
		//ofSetHexColor(0xff0000);s
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
	
	#else

		ofFill();
		ofSetHexColor(0x00FFFF); 
		ofDrawCircle(ball.spos.x, ball.spos.y, 8);
		ofDrawCircle(hole.spos.x, hole.spos.y, 13);
		ofDrawBitmapString("BALL", ball.spos.x - 10, ballPos.y - 10);
		ofDrawBitmapString("HOLE", hole.spos.x-10, hole.spos.y-10);

		ofSetHexColor(0xFF0000); 
		ofDrawCircle(ball.pos[0].x, ball.pos[0].y, 5);
		ofDrawCircle(hole.pos[0].x, hole.pos[0].y, 10);
		
		ofNoFill();
		ofSetHexColor(0xff00ff); 
		ofDrawCircle(ball.spos.x, ball.spos.y, ball.mvel);

	#endif

}


void ofApp::drawGrid() {

	ofFill();
	ofSetHexColor(0xffffff);
	// calc grid origin
	float gridWidth = ((gridCountX-1) * gridResX);
	float gridHeight = ((gridCountY-1) * gridResY);
	float ox = imgWidth/2 - gridWidth/2;
	float oy = imgHeight/2 - gridHeight/2;
	for (int i = 0; i < gridCountX; i++) {
		float x = ox + i * gridResX;
		ofDrawLine(x, oy, x, oy + gridHeight);
		for (int h = 0; h < gridCountY; h++)	{
			float y = oy + h * gridResY;
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
	ofDrawCircle(ox + x*gridResX, oy + y*gridResY, 20);
}


void ofApp::drawFloor() {

	ofFill();
	ofSetHexColor(0xffffff);
	float w = wallSize*imgScale;
	//ofDrawRectangle(0, 0, imgWidth, imgHeight);
	//ofSetHexColor(0x000000);
	//ofDrawRectangle(w, w, imgWidth - 2*w, imgHeight- 2*w);
	//ofDrawRectangle(0, 0, imgWidth, w);
	//ofDrawRectangle(0, 0, w, imgHeight);
	//ofDrawRectangle(0, imgHeight-w, imgWidth, w);
	//ofDrawRectangle(imgWidth-w, 0, w, imgHeight);

	ofBeginShape();
	float m = 0;
	ofVertex(m, m);
	ofVertex(imgWidth - m, m);
	ofVertex(imgWidth - m, imgHeight - m);
	ofVertex(m,imgHeight - m);
	ofNextContour(true);
	m = wallSize*imgScale;
	ofVertex(m, m);
	ofVertex(imgWidth - m, m);
	ofVertex(imgWidth - m, imgHeight - m);
	ofVertex(m,imgHeight - m);
	ofEndShape(true);
}


// ------------------------------------------------------------- //


void ofApp::drawPlay() {

	game.draw();

	// draw background mask
	ofFill();
	ofSetHexColor(0x000000); 
	ofBeginShape();
	float m = -500;
	ofVertex(m, m);
	ofVertex(imgWidth - m, m);
	ofVertex(imgWidth - m, imgHeight - m);
	ofVertex(m,imgHeight - m);
	ofNextContour(true);
	m = wallSize*imgScale + 10; // wallsize + margin to mask inside face of wall
	ofVertex(m, m);
	ofVertex(imgWidth - m, m);
	// inside face is not a problem on the bottom edge
	// because of the position of the projector
	ofVertex(imgWidth - m, imgHeight - m + 7);
	ofVertex(m,imgHeight - m + 7);
	ofEndShape(true);

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
			if(!trackingCalibrated) {
				addCalibrationPoint();
			}
			break;

		case 'w':
			toggleUndistort = !toggleUndistort;
			cout << toggleUndistort << endl;
			break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			game.scene = int(key) - 48;
			cout << "scene: " << game.scene << endl;
			break;

		// calibration debugging
		case 'q':
			cout << "applying calibration..." << endl;
			updateCalibration();
			cout << "done" << endl;
			trackingCalibrated = true;
			gridIndex = 0;
			break;

		case 'h':
			hole.freeze = !hole.freeze;
			break;

		// DEBUGGING --------------------

		case 'b':
			game.ballOrigin = game.ballPos;
			break;

		case 'z':
			cout << " ------------------------- " << endl;
			//cout << "holePlaced = " << game.holePlaced << endl;
			//cout << "ballPlaced = " << game.ballPlaced << endl;
			cout << "ready = " << game.ready << endl;
			cout << "transition = " << game.transition << endl;
			cout << "scene = " << game.scene << endl;
			cout << "ballPos = " << game.ballPos << endl;
			cout << "holePos = " << game.holePos << endl;
			cout << "ballOrigin = " << game.ballOrigin << endl;
			cout << "ballTarget = " << game.ballTarget << endl;
			cout << "holeTarget = " << game.holeTarget << endl;
			cout << " ------------------------- " << endl;
			break;

		case 'x':
			//cout << " ------------------------- " << endl;
			//for (int i = 0; i < POS_SAMPLES; i++){
			//	cout << ball.pos[i] << endl;
			//}
			cout << " ------------------------- " << endl;
			cout << ball.spos << endl;
			//cout << ball.pSpos << endl;
			cout << " ========================= " << endl;


	}
}

void ofApp::mousePressed(int x, int y, int button){
	#ifdef USE_CAM
		if(!trackingCalibrated) {
			addCalibrationPoint();
		}
	#else
		if(button > 0)
			holePos = ofVec2f(x,y);
		else
			ballPos = ofVec2f(x,y);
	#endif
}

void ofApp::mouseDragged(int x, int y, int button) { 

	#ifndef USE_CAM
		if(button > 0)
			holePos = ofVec2f(x,y);
		else
			ballPos = ofVec2f(x,y);
	#endif

}

// ============================================================

void ofApp::updateOscTracking() { 

	while(oscTracking.hasWaitingMessages()){

		ofxOscMessage m;
		oscTracking.getNextMessage(m);

		if(m.getAddress() == "/golf"){
			ballPos.x = m.getArgAsFloat(0);
			ballPos.y = m.getArgAsFloat(1);
			holePos.x = m.getArgAsFloat(2);
			holePos.y = m.getArgAsFloat(3);
			ballPos *= trackingScale;
			holePos *= trackingScale;
			ballPos += trackingOffset;
			holePos += trackingOffset;

			ball.update(ballPos, gui->trackingThreshold, gui->avgWeighting);
			hole.update(holePos, gui->trackingThreshold, gui->avgWeighting);

			updateSound();
		}

		//cout << m << endl;
	}
}


void ofApp::updateSound() {

	ofxOscMessage m1;
	m1.setAddress("/bhd");
	m1.addFloatArg(game.bhd);
	oscSound.sendMessage(m1, false);
	
	ofxOscMessage m2;
	m2.setAddress("/bod");
	m2.addFloatArg(game.bod);
	oscSound.sendMessage(m2, false);

	ofxOscMessage m3;
	m3.setAddress("/bha");
	m3.addFloatArg(game.bha);
	oscSound.sendMessage(m3, false);

	ofxOscMessage m4;
	m4.setAddress("/vel");
	m4.addFloatArg(ball.mvel);
	oscSound.sendMessage(m4, false);
	
	// cout << m1 << endl;
	// cout << m2 << endl;
	// cout << m3 << endl;
	// cout << m4 << endl;

}