#pragma once

#include "ofxLibdcPtGrey.h"
#include "ofxOpenCv.h"
#include "ofThread.h"
#include "ofEvents.h"

#define INPUT_FIREFLY_WIDTH 752

class threadedPtGreyGrabber : public ofxLibdcPtGrey, public ofThread {
protected:
	int pollRate;
	void threadedFunction() {
		while(isThreadRunning()) {
			lock();
			update();			// we don't need to lock here if getThreadedPixels is called from "frameReceived"
			unlock();
			if(isFrameNew())
				ofNotifyEvent(frameReceived, *this);
			ofSleepMillis(pollRate);
		}
	}
	
public:
	
	ofEvent<ofxLibdcPtGrey> frameReceived;
	threadedPtGreyGrabber() {
		pollRate = 1;
	}
	template <class L>
	void setupGrabber(L* listener, bool useTexture = false, bool verbose = false) {
		addListener(listener);
		setUseTexture(useTexture);				
	}
	void startCapture( bool verbose = false){
		startThread(true);
	}
	void setPollRate(int pollRate) {
		this->pollRate = pollRate;
	}
	int getPollRate() {
		return pollRate;
	}
	template <class L>
	void addListener(L* listener) {
		using Callback = void (L::*)(ofxLibdcPtGrey &);
		ofAddListener(frameReceived, listener, static_cast<Callback>(&L::frameReceived));
	}
	void getThreadedPixels(ofxCvGrayscaleImage & grayImg, bool & bNeedUpdate) {
		lock();
		ofPixels& pixels = this->getPixels();
		if(pixels.isAllocated()){
			grayImg.setFromPixels(pixels);
			bNeedUpdate = true;
		}
		unlock();
	}
};
