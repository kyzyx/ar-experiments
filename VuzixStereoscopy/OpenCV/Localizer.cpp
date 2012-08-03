#include "Localizer.h"
#include <aruco.h>
#include <boarddetector.h>
#include <cvdrawingutils.h>

using namespace aruco;

Localizer::~Localizer(void)
{
}

void Localizer::SetCamera(Camera& cam, Mat& img, bool set) {
	if (!initialized) return;
	float prob = detector.detect(img);
	/*for (unsigned int i=0;i<detector.getDetectedMarkers().size();i++)
           detector.getDetectedMarkers()[i].draw(img,Scalar(0,0,255),1);*/
	if (prob > 0.2) {
		CvDrawingUtils::draw3dAxis(img,detector.getDetectedBoard(),camparam);
		if (set) 
			cam.SetRT(detector.getDetectedBoard().Rvec, detector.getDetectedBoard().Tvec);
		/*else
			cam.AvgPos(detector.getDetectedBoard().Rvec, detector.getDetectedBoard().Tvec);*/
	}
}