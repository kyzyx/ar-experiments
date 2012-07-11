#include "tracker.h"
#include <opencv2\core\core.hpp>
#include <opencv2\calib3d\calib3d.hpp>
#include <opencv2\gpu\gpu.hpp>

using namespace cv;

Tracker::Tracker() {
	// TODO: Measure object distances
	object.push_back(Point3f(-50,150,200));
	object.push_back(Point3f(325,100,220));
	object.push_back(Point3f(250,20,220));
	object.push_back(Point3f(600,180,200));
	// TODO: measure directions relative to object
	// Camera params in object coordinate system
	base_camera = Camera(Point3f(0,0,0), Point3f(1,0,0), Point3f(0,1,0));
	// Wiimote camera intrinsics from http://idav.ucdavis.edu/~okreylos/ResDev/Wiimote/Technology.html
	cam = (Mat_<double>(3,3) << 
		   1280,    0, 512,
		      0, 1280, 384,
		      0,    0,   1);
}
Camera Tracker::getPosition(vector<double>& coords) {		
	// TODO: Get order!
	vector<Point2f> image;       // Image coordinates
	for (int i = 0; i < coords.size(); i += 2) {
		image.push_back(Point2f(coords[i], coords[i+1]));
	}

	Mat rvec, tvec, r;
	solvePnPRansac(object,image,cam,noArray(),rvec,tvec);
	//gpu::solvePnPRansac(object,image,cam,,rvec,tvec);

	// Filter rvec, tvec?
	Camera ret = base_camera;
	ret.ApplyRT(rvec, tvec);
	return ret;
}