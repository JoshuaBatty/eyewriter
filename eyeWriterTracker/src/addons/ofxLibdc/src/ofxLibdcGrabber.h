/*
 ofxLibdcGrabber has all the features of ofxLibdc with the interface
 of ofBaseVideo. It uses the more traditional approach to capturing,
 which has the disadvantage of being slightly slower and requiring
 more memory -- but the advantage of being easy to swap out.
 
 ofxLibdcGrabber camera;
 ofImage currentFrame;
 camera.grabFrame();
 if(camera.isFrameNew()) {
 currentFrame.setFromPixels(camera.getPixels() ... );
 // do something with currentFrame
 }
 */

#pragma once

#include "ofxLibdc.h"

class ofxLibdcGrabber : public ofxLibdc, public ofBaseVideo {
public:
	ofxLibdcGrabber();
		ofTexture& getTextureReference();
		void setUseTexture(bool useTexture);
		ofPixels& getPixels() override;
		const ofPixels& getPixels() const override;
		unsigned char* getPixelsData();
		const unsigned char* getPixelsData() const;
		ofImage& getBuffer();
		void update() override;
		void grabFrame();
		void draw(float x, float y) const;
		void draw(float x, float y, float w, float h) const;
		float getWidth() const;
		float getHeight() const;
		bool isFrameNew() const override;
		void close() override;
		bool isInitialized() const override;
		bool setPixelFormat(ofPixelFormat pixelFormat) override;
		ofPixelFormat getPixelFormat() const override;
protected:
	ofImage buffer;
	mutable bool newFrame;
};
