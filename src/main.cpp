#include "ofMain.h"
#include "ofApp.h"
#include "GuiApp.h"
#include "ofAppGLFWWindow.h"

//========================================================================
int main( ){
	ofGLFWWindowSettings settings;

   settings.setSize(1024, 768);
	settings.setPosition(ofVec2f(1920,0));
	settings.resizable = false;
	settings.windowMode = OF_FULLSCREEN;
	shared_ptr<ofAppBaseWindow> mainWindow = ofCreateWindow(settings);

   settings.setSize(300, 600);
	settings.setPosition(ofVec2f(100,100));
	settings.resizable = false;
	settings.windowMode = OF_WINDOW;
	shared_ptr<ofAppBaseWindow> guiWindow = ofCreateWindow(settings);

	shared_ptr<ofApp> mainApp(new ofApp);
	shared_ptr<GuiApp> guiApp(new GuiApp);
	mainApp->gui = guiApp;

	ofRunApp(guiWindow, guiApp);
	ofRunApp(mainWindow, mainApp);
	ofRunMainLoop();

}
