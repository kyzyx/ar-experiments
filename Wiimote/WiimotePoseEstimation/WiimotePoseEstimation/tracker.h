#pragma once
#include "Camera.h"
#include <opencv2\core\core.hpp>

using namespace cv;

class Tracker {
public:
	Mat cam;
	vector<Point3f> object;

	Camera base_camera;
	Tracker();
	Camera getPosition(vector<double>& coords);
};