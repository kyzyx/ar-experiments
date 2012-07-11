#include "Camera.h"
#include <opencv2\calib3d\calib3d.hpp>

void Camera::ApplyRT(Mat& rvec, Mat& tvec) {
	Mat r;
	Mat e(eye);
	Mat t(towards);
	Mat u(up);
	Rodrigues(rvec,r);
	multiply(r,e,e);
	multiply(r,t,t);
	multiply(r,u,u);
	add(tvec,e,e);
	add(tvec,t,t);
	add(tvec,u,u);
	eye = *(e.ptr<Point3f>(0));
	towards = *(t.ptr<Point3f>(0));
	up = *(u.ptr<Point3f>(0));
}

void Camera::OpenGLCamera(int eye, double separation) {
}
