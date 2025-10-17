#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include <vector>
#include <cmath>

class blobResampler {
public:
	void resample(ofxCvBlob& blob, float pickupRatio, int minimumPts);

private:
	template<typename VecT>
	static float getLength(const VecT& v) {
		return std::hypot(v.x, v.y);
	}

	template<typename VecT>
	static float getWholeLength(const std::vector<VecT>& pts) {
		float length = 0.0f;
		for (size_t i = 0; i + 1 < pts.size(); ++i) {
			const auto& a = pts[i];
			const auto& b = pts[i + 1];
			float dx = b.x - a.x;
			float dy = b.y - a.y;
			length += std::hypot(dx, dy);
		}
		return length;
	}
};


//#pragma once
//
//#include "ofMain.h"
//#include "ofxOpenCv.h"
//
//class blobResampler {
//	
//public:
//	
//	void resample(ofxCvBlob & blob, float pickupRatio, int minimumPts);
//	float getLength(ofPoint point);
//	float getWholeLength(vector <ofPoint> & pts);
//	
//};
