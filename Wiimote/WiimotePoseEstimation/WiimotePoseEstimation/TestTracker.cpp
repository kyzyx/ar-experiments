#include "tracker.h"
#include <opencv2\core\core.hpp>
#include <opencv2\calib3d\calib3d.hpp>
#include <iostream>

using namespace std;

bool testTrackWithProject() {
	// Back 
	Tracker t;
	Mat rvec, tvec, rr, tr; // FIXME
	rvec = (Mat_<double>(3,1) << 
		   0.1, 1, 0.5);
	tvec = (Mat_<double>(3,1) << 
		   30,10,7);
	vector<Point2f> p;
	cout << t.object << endl;
	cout << t.cam << endl;
	projectPoints(t.object, rvec, tvec, t.cam, noArray(), p);
	cout << p << endl;
	solvePnPRansac(t.object,p,t.cam,noArray(),rr,tr);
	
	cout << "-----------------------------------------" << endl;
	cout << rvec << endl;
	cout << rr << endl << "-----------------------------------------" << endl;
	cout << tvec << endl;
	cout << tr << endl;
	cout << "-----------------------------------------" << endl;
	return true;
}

bool testTrackWithManual() {
	return true;
}

int tmain() {
	testTrackWithProject();
	return 0;
}