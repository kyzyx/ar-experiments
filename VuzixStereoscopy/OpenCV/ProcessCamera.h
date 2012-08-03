#pragma once
#include <Windows.h>
#include <gl\GL.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include "Camera.h"
#include "Localizer.h"

class ProcessCamera
{
public:
	ProcessCamera(void);
	~ProcessCamera(void);

	void GetFrames(void);

	void RenderStereoBG(int width, int height);

	void SetCamera(Camera& cam) {
		lmat = Mat(limg);
		rmat = Mat(rimg);
		localizer->SetCamera(cam, lmat);
		localizer->SetCamera(cam, rmat, false);
	}

private:
	CvCapture* left_cam;
	CvCapture* right_cam;

	Mat lmat;
	Mat rmat;
	IplImage* limg;
	IplImage* rimg;
	bool lfail, rfail;

	Localizer* localizer;

	GLuint texId[2];
};

