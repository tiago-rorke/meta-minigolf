
#include "GuiApp.h"

void GuiApp::setup(){

	parameters.setName("parameters");
	// initial values defined in settings.yml
	parameters.add(mapScale.set("map scale", 0, 0.85, 0.9));
	parameters.add(mapXoff.set("map offset x", 0, -50, 50));
	parameters.add(mapYoff.set("map offset y", 0, -100, 0));
	parameters.add(blobMin.set("min blob size", 0, 3, 150));
	parameters.add(blobMax.set("max blob size", 0, 150, 500));
	parameters.add(ballBlob.set("ball blob size", 0, 10, 100));
	parameters.add(holeBlob.set("hole blob size", 0, 150, 400));
	parameters.add(blobSizeRange.set("blob size range", 0, 0, 200));
	parameters.add(blobSquare.set("blob squareness", 0, 0, 1.0));
	parameters.add(trackingThreshold.set("tracking threshold", 0, 1, 500));
	parameters.add(targetThreshold.set("target threshold", 0, 1, 50));
	parameters.add(avgWeighting.set("average weighting", 0, 0, 1));	
	parameters.add(holeRad.set("holeRad", 0, 0, 50));	
	parameters.add(ballRad.set("ballRad", 0, 0, 50));	
	parameters.add(ballVelScalar.set("ballVelScalar", 0, 0, 20));	
	
	gui.setup(parameters);
	ofBackground(0);
	ofSetVerticalSync(false);
}

void GuiApp::update(){

}

void GuiApp::draw(){
	gui.draw();
}
