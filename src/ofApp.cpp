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
	imitate(camUndistorted, cam);
	calibration.setPatternSize(gridCountX, gridCountY);
	calibration.setSquareSize(gridRes);
	CalibrationPattern patternType;
	patternType = CHESSBOARD;
	calibration.setPatternType(patternType);
	gridIndex = 0;
	camCalibrated = false;

	// vision
	setBallOrigin = false;
	gotBallOrigin = false;

	// graphics
	ofSetCircleResolution(100);

	// DEBUGGING
	toggleUndistort = true;

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

	ofPushMatrix();
	ofScale(camScale);
	drawTracking();	
	ofNoFill();
	/*
	ofDrawCircle(74,75.5, 10);
	ofDrawCircle(229,67, 10);
	ofDrawCircle(395.5,65.5, 10);
	ofDrawCircle(553,71, 10);
	ofDrawCircle(67,232, 10);
	ofDrawCircle(226.5,234, 10);
	ofDrawCircle(399.5,232.5, 10);
	ofDrawCircle(562.5,231.5, 10);
	ofDrawCircle(69,396, 10);
	ofDrawCircle(227.5,405.5, 10);
	ofDrawCircle(399,405.5, 10);
	ofDrawCircle(559,396, 10);
	*/
	/*
	ofDrawCircle(51,76, 10);
	ofDrawCircle(148,70.5, 10);
	ofDrawCircle(256,65.5, 10);
	ofDrawCircle(367.5,66.5, 10);
	ofDrawCircle(476.5,69.5, 10);
	ofDrawCircle(575.5,74, 10);
	ofDrawCircle(44,178, 10);
	ofDrawCircle(144.5,176, 10);
	ofDrawCircle(254.5,176, 10);
	ofDrawCircle(370.5,175.5, 10);
	ofDrawCircle(482.5,175, 10);
	ofDrawCircle(584,177, 10);
	ofDrawCircle(42,286, 10);
	ofDrawCircle(143.5,291, 10);
	ofDrawCircle(254.5,291, 10);
	ofDrawCircle(371.5,291.5, 10);
	ofDrawCircle(483.5,288.5, 10);
	ofDrawCircle(587,284.5, 10);
	ofDrawCircle(45.5,395, 10);
	ofDrawCircle(145,401, 10);
	ofDrawCircle(255.5,406, 10);
	ofDrawCircle(371,406.5, 10);
	ofDrawCircle(482,402, 10);
	ofDrawCircle(584,393.5, 10);
	*/
	ofPopMatrix();
	ofTranslate(imgOffset.x, imgOffset.y);
	//if(!camCalibrated) {
		//drawGrid();
	//}
	drawPlay();
	drawFloor();
	ofPopMatrix();

	//gui.draw();
}


// ------------------------------------------------------------- //


void ofApp::updateVision() {

	cam.update();

	if(camCalibrated) {
		calibration.undistort(toCv(cam), toCv(camUndistorted));
		camUndistorted.update();
	}

	if(cam.isFrameNew()) {
		if(!camCalibrated) {
	   	camColor.setFromPixels(cam.getPixels());
		} else {
	   	camColor.setFromPixels(camUndistorted.getPixels());			
		}
	   camGray = camColor;
//	   if(!camCalibrated) {
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
		gotTracking = true;
		if(setBallOrigin) {
			ballOrigin = ballPos;
			setBallOrigin = false;
			gotBallOrigin = true;
		}
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
		cout << "applying calibration..." << endl;
		updateCalibration();
		cout << "done" << endl;
		camCalibrated = true;
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

	ofFill();
	ofSetHexColor(0xffffff);
	//camDiff.draw(0, 0);
/*
	if(!camCalibrated) {
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
	*/
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

void ofApp::keyPressed(int key){

	switch (key){
		case ' ':
			getBkgd = true;
			break;
		case 'f':
			ofToggleFullscreen();
			break;
		case 'p':
			if(!camCalibrated) {
				addCalibrationPoint();
			}
			break;

		case 'w':
			toggleUndistort = !toggleUndistort;
			cout << toggleUndistort << endl;
			break;

		// calibration debugging
		case 'q':
			/*
			calibrationPoints.push_back(cv::Point2f(51,76));
			calibrationPoints.push_back(cv::Point2f(148,70.5));
			calibrationPoints.push_back(cv::Point2f(256,65.5));
			calibrationPoints.push_back(cv::Point2f(367.5,66.5));
			calibrationPoints.push_back(cv::Point2f(476.5,69.5));
			calibrationPoints.push_back(cv::Point2f(575.5,74));
			calibrationPoints.push_back(cv::Point2f(44,178));
			calibrationPoints.push_back(cv::Point2f(144.5,176));
			calibrationPoints.push_back(cv::Point2f(254.5,176));
			calibrationPoints.push_back(cv::Point2f(370.5,175.5));
			calibrationPoints.push_back(cv::Point2f(482.5,175));
			calibrationPoints.push_back(cv::Point2f(584,177));
			calibrationPoints.push_back(cv::Point2f(42,286));
			calibrationPoints.push_back(cv::Point2f(143.5,291));
			calibrationPoints.push_back(cv::Point2f(254.5,291));
			calibrationPoints.push_back(cv::Point2f(371.5,291.5));
			calibrationPoints.push_back(cv::Point2f(483.5,288.5));
			calibrationPoints.push_back(cv::Point2f(587,284.5));
			calibrationPoints.push_back(cv::Point2f(45.5,395));
			calibrationPoints.push_back(cv::Point2f(145,401));
			calibrationPoints.push_back(cv::Point2f(255.5,406));
			calibrationPoints.push_back(cv::Point2f(371,406.5));
			calibrationPoints.push_back(cv::Point2f(482,402));
			calibrationPoints.push_back(cv::Point2f(584,393.5));
			*/
			calibrationPoints.push_back(cv::Point2f(553,71));
			calibrationPoints.push_back(cv::Point2f(396.5,65.5));
			calibrationPoints.push_back(cv::Point2f(232,67));
			calibrationPoints.push_back(cv::Point2f(80,75.5));
			calibrationPoints.push_back(cv::Point2f(562.5,231.5));
			calibrationPoints.push_back(cv::Point2f(400.5,232.5));
			calibrationPoints.push_back(cv::Point2f(229.5,234));
			calibrationPoints.push_back(cv::Point2f(73,232));
			calibrationPoints.push_back(cv::Point2f(559,396));
			calibrationPoints.push_back(cv::Point2f(400,405.5));
			calibrationPoints.push_back(cv::Point2f(230.5,405.5));
			calibrationPoints.push_back(cv::Point2f(75,396));

			cout << "applying calibration..." << endl;
			updateCalibration();
			cout << "done" << endl;
			camCalibrated = true;
			gridIndex = 0;
			break;

		case 'b':
			setBallOrigin = true;
			break;



	}
}

void ofApp::mousePressed(int x, int y, int button){
	if(!camCalibrated) {
		addCalibrationPoint();
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


// =============================================================== //




void ofApp::drawPlay() {

	float bx = ballPos.x;
	float by = ballPos.y;
	float hx = holePos.x;
	float hy = holePos.y;
	float bxo = ballOrigin.x;
	float byo = ballOrigin.y;
	
	// tracking dots
	//ofFill();
	//ofSetHexColor(0xff0000); 
	//ofDrawCircle(bx, by, 5);
	//ofDrawCircle(hx, hy, 5);

	float bhd = ofDist(bx, by, hx, hy);
	float bod = ofDist(bxo, byo, hx, hy);
	float bha = -glm::atan((bx-hx)/(by-hy));
	if(by-hy > 0) {
		bha += ofDegToRad(180.f);
	}

	//cout << aba << endl;

	// --------------------------------------------

	////// SIMPLE CIRCLE AND LINE
	/*
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
	*/
	// --------------------------------------------

	////// CONDENSING PARABOLAS
	
	if(gotTracking && gotBallOrigin) {
		ofPushMatrix();
		ofTranslate(bx, by);
		ofRotateRad(bha);
		ofNoFill();
		ofSetHexColor(0xffffff); 
		int n = 20;
		float di = bhd / n;
		float a = 1-(bhd / bod);
		if(a < 0) a = 0;
		for (float y = di; y < bhd; y += di) {
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
	
	// --------------------------------------------


	////// CONCENTRIC CIRCLES
	/*
	if(gotTracking) {
		ofNoFill();
		ofSetHexColor(0xffffff); 
		float a = bhd/10;
		if(bhd > 10) {
			ofPushMatrix();
			ofTranslate(bx, by);
			for (float i = 0; i < imgWidth*2; i+= a) {
				ofDrawCircle(0,0,i);
			}
			ofPopMatrix();
			ofPushMatrix();
			ofTranslate(hx, hy);
			for (float i = 0; i < imgWidth*2; i+= a) {
				ofDrawCircle(0,0,i);
			}
			ofPopMatrix();
		}
	}

*/
	// --------------------------------------------
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