#pragma once
#include <aruco.h>
#include <boarddetector.h>
#include "Camera.h"

using namespace aruco;

class Localizer
{
public:
	Localizer(void){
		initialized = false;
	}
	Localizer(const string& boardconfig, const string& camfile, double msz) {
		cout << boardconfig << endl;
		cout << camfile << endl;
		initialized = true;
		board.readFromFile(boardconfig);
		camparam.readFromXMLFile(camfile);
		detector.setParams(board, camparam, msz);
	}
	~Localizer(void);

	void SetCamera(Camera& cam, Mat& img, bool set=true);
private:
	bool initialized;
	double markersize;
	BoardConfiguration board;
	BoardDetector detector;
	CameraParameters camparam;
};

