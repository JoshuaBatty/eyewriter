#include "blobResampler.h"

void blobResampler::resample(ofxCvBlob& blob, float pickupRatio, int minimumPts) {
	using Point = std::decay_t<decltype(blob.pts.front())>;

	const int currPts = static_cast<int>(blob.pts.size());
	const int newNumPts = std::max(static_cast<int>(pickupRatio * currPts), minimumPts);

	if (newNumPts >= currPts || currPts < 2) return;

	const float totalLength = getWholeLength(blob.pts);
	if (totalLength <= 0.0f) return;

	const float interval = totalLength / static_cast<float>(newNumPts - 1);
	float D = 0.0f;

	std::vector<Point> newPts;
	newPts.reserve(static_cast<size_t>(newNumPts));
	newPts.push_back(blob.pts.front());

	for (size_t i = 1; i < blob.pts.size(); ++i) {
		const Point& curr = blob.pts[i];
		const Point& prev = blob.pts[i - 1];
		float d = getLength(Point(curr - prev));

		while ((D + d) >= interval && d > 0.0f) {
			float t = (interval - D) / d;
			Point q = prev + t * (curr - prev);
			newPts.push_back(q);
			D = 0.0f;
			d -= (interval - D);
		}

		D += d;
	}

	if (newPts.size() == static_cast<size_t>(newNumPts - 1))
		newPts.push_back(blob.pts.back());

	blob.pts = std::move(newPts);
	blob.nPts = static_cast<int>(blob.pts.size());
}


//#include "blobResampler.h"
//
//void blobResampler::resample(ofxCvBlob & blob, float pickupRatio, int minimumPts){
//	
//	int newNumPts = MAX((int)(pickupRatio * blob.pts.size()), minimumPts);
//	
//	if (newNumPts < blob.pts.size()){
//	
//		float interval = getWholeLength(blob.pts) / (newNumPts - 1); // calc interval length
//		float D = 0.0;									// total distance, as we walk across
//		
//		vector <ofPoint> newPts;						// new pts
//		
//		//--- store first point since we'll never resample it out of existence
//		newPts.push_back(blob.pts.front());
//		for(int i = 1; i < (int)blob.pts.size(); i++){
//			
//			ofPoint currentPoint  = blob.pts[i];
//			ofPoint previousPoint = blob.pts[i-1];
//			float d = getLength(previousPoint - currentPoint);
//			if ((D + d) >= interval){
//				float qx = previousPoint.x + ((interval - D) / d) * (currentPoint.x - previousPoint.x);
//				float qy = previousPoint.y + ((interval - D) / d) * (currentPoint.y - previousPoint.y);
//				ofPoint point(qx, qy);
//				newPts.push_back(point);
//				blob.pts.insert(blob.pts.begin() + i, point);
//				D = 0.0;
//			}
//			else D += d;
//		}
//		
//		// somtimes we fall a rounding-error short of adding the last point, so add it if so
//		if (newPts.size() == (newNumPts - 1)){
//			newPts.push_back(blob.pts.back());
//		}
//		
//		blob.pts = newPts;
//		blob.nPts = blob.pts.size();
//	}
//
//}
//
//float blobResampler::getWholeLength(vector <ofPoint> & pts){
//	
//	float length = 0;
//	if (pts.size() > 1){
//		for (int i = 0; i < pts.size()-1; i++){
//			length += sqrt((pts[i] - pts[i+1]).x * (pts[i] - pts[i+1]).x +  (pts[i] - pts[i+1]).y * (pts[i] - pts[i+1]).y);
//		}
//	}
//	return length;
//}
//
//float blobResampler::getLength(ofPoint point){
//	
//	return sqrt(point.x * point.x + point.y * point.y);
//}
//
