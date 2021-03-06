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
		   0.1, 0, 0);
	tvec = (Mat_<double>(3,1) << 
		   0,0,500);
	vector<Point2f> p;
	cout << t.object << endl;
	cout << t.cam << endl;
	projectPoints(t.object, rvec, tvec, t.cam, noArray(), p);
	cout << p << endl;
	solvePnP(t.object,p,t.cam,noArray(),rr,tr,false,CV_P3P);
	
	cout << "-----------------------------------------" << endl;
	cout << rvec << endl;
	cout << rr << endl << "-----------------------------------------" << endl;
	cout << tvec << endl;
	cout << tr << endl;
	cout << "-----------------------------------------" << endl;
	return true;
}

bool testTrackWithManual() {
	Tracker t;
	Mat rvec, tvec, rr, tr; // FIXME
	vector<Point2f> p;
	p.push_back(Point2f(500,500));
	p.push_back(Point2f(650,566));
	p.push_back(Point2f(650,460));
	p.push_back(Point2f(800,510));

	cout << p << endl;
	solvePnP(t.object,p,t.cam,noArray(),rr,tr, false, CV_P3P);
	
	cout << "-----------------------------------------" << endl;
	cout << rvec << endl;
	cout << rr << endl << "-----------------------------------------" << endl;
	cout << tvec << endl;
	cout << tr << endl;
	cout << "-----------------------------------------" << endl;
	return true;
}

int tmain() {
	testTrackWithProject();
	testTrackWithManual();
	return 0;
}