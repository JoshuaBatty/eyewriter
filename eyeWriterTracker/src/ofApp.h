#ifndef _OF_APP
#define _OF_APP


#include "ofMain.h"

#include "trackingManager.h"	
#include "calibrationManager.h"
#include "buttonTrigger.h"
#include "typingScene.h"

#include "pongScene.h"


enum{
	
	MODE_TRACKING,	MODE_CALIBRATING,	MODE_TEST, MODE_DRAW,  MODE_TYPING, MODE_PONG

};

#include "eyePlotterofApp.h"

class ofApp : public ofBaseApp {

	public:

		ofApp();
		void setup();
		void update();
		void draw();

		void keyPressed  (int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void resized(int w, int h);

		//----- scenes

		trackingManager			TM;
		calibrationManager		CM;
	
		buttonTrigger			BT;
		eyePlotterofApp			eyeApp;
		typingScene				typeScene;
		pongScene				ponger;

	
	
		ofPoint eyeSmoothed;
		ofPoint screenPoint;
		float waitFpsTime;
		float timeSince;
	
		int mode; 
	
		bool bMouseSimulation;
		bool bMouseEyeInputSimulation;
	
		//------ drawing
		void drawHelp();
	
		static constexpr float rotSmooth = 0.9f;
	
};

#endif
