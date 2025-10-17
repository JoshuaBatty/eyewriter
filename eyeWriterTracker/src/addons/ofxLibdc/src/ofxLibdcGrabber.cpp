#include "ofxLibdcGrabber.h"

namespace{
bool pixelFormatToImageType(ofPixelFormat pixelFormat, ofImageType& imageType){
	switch(pixelFormat){
		case OF_PIXELS_GRAY:
			imageType = OF_IMAGE_GRAYSCALE;
			return true;
		case OF_PIXELS_RGB:
			imageType = OF_IMAGE_COLOR;
			return true;
		default:
			return false;
	}
}

ofPixelFormat imageTypeToPixelFormat(ofImageType imageType){
	switch(imageType){
		case OF_IMAGE_GRAYSCALE: return OF_PIXELS_GRAY;
		case OF_IMAGE_COLOR: return OF_PIXELS_RGB;
		default: return OF_PIXELS_RGB;
	}
}
} // namespace

ofxLibdcGrabber::ofxLibdcGrabber() :
	newFrame(false) {
}

ofTexture& ofxLibdcGrabber::getTextureReference() {
	return buffer.getTexture();
}

void ofxLibdcGrabber::setUseTexture(bool useTexture) {
	buffer.setUseTexture(useTexture);
}

ofPixels& ofxLibdcGrabber::getPixels() {
	return buffer.getPixels();
}

const ofPixels& ofxLibdcGrabber::getPixels() const {
	return buffer.getPixels();
}

unsigned char* ofxLibdcGrabber::getPixelsData() {
	if(buffer.isAllocated()){
		return buffer.getPixels().getData();
	}
	return nullptr;
}

const unsigned char* ofxLibdcGrabber::getPixelsData() const {
	if(buffer.isAllocated()){
		return buffer.getPixels().getData();
	}
	return nullptr;
}

ofImage& ofxLibdcGrabber::getBuffer() {
	return buffer;
}

void ofxLibdcGrabber::update() {
	grabFrame();
}

void ofxLibdcGrabber::grabFrame() {
	if(camera == nullptr){
		newFrame = false;
		return;
	}
	newFrame = grabVideo(buffer);
}

void ofxLibdcGrabber::draw(float x, float y) const {
	buffer.draw(x, y);
}

void ofxLibdcGrabber::draw(float x, float y, float w, float h) const {
	buffer.draw(x, y, w, h);
}

float ofxLibdcGrabber::getWidth() const {
	return ofxLibdc::getWidth();
}

float ofxLibdcGrabber::getHeight() const {
	return ofxLibdc::getHeight();
}

bool ofxLibdcGrabber::isFrameNew() const {
	if(newFrame) {
		newFrame = false;
		return true;
	}
	return false;
}

void ofxLibdcGrabber::close() {
	if(camera == nullptr){
		return;
	}
	setTransmit(false);
	flushBuffer();
	dc1394_capture_stop(camera);
	dc1394_camera_free(camera);
	camera = nullptr;
	buffer.clear();
	newFrame = false;
}

bool ofxLibdcGrabber::isInitialized() const {
	return camera != nullptr;
}

bool ofxLibdcGrabber::setPixelFormat(ofPixelFormat pixelFormat) {
	ofImageType targetType;
	if(!pixelFormatToImageType(pixelFormat, targetType)){
		ofLogWarning("ofxLibdcGrabber") << "setPixelFormat - unsupported pixel format " << pixelFormat;
		return false;
	}

	if(static_cast<ofImageType>(getImageType()) == targetType){
		return true;
	}

	setImageType(static_cast<int>(targetType));

	if(buffer.isAllocated()){
		buffer.clear();
		buffer.allocate(getWidth(), getHeight(), targetType);
	}

	if(isInitialized()){
		applySettings();
		flushBuffer();
	}

	return true;
}

ofPixelFormat ofxLibdcGrabber::getPixelFormat() const {
	return imageTypeToPixelFormat(static_cast<ofImageType>(getImageType()));
}
