#include "Camera.h"
#include <opencv2\calib3d\calib3d.hpp>
#include <gl/GL.h>
#include <gl/GLU.h>

#include <iostream>
using namespace std;

void Camera::ApplyRT(Mat& rvec, Mat& tvec) {
	Mat r(3,3,CV_32FC1);
	Mat e = (Mat_<double>(3,1) << eye.x,eye.y,eye.z);
	Mat t = (Mat_<double>(3,1) << towards.x,towards.y,towards.z);
	Mat u = (Mat_<double>(3,1) << up.x,up.y,up.z);
	Rodrigues(rvec,r);
	e = r*e + tvec;
	t = r*t + tvec;
	u = r*u + tvec;
	eye = *(e.ptr<Point3f>(0));
	towards = *(t.ptr<Point3f>(0));
	up = *(u.ptr<Point3f>(0));
	
	towards *= 1/norm(towards);
	up *= 1/norm(up);
	right = towards.cross(up);
}

void Camera::OpenGLCamera(int stereoscopy, double separation) {
	// Set projection transformation
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective(yfov, xfov/yfov, 0.1, 1000);

  // Set camera transformation
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(eye.x, eye.y, eye.z, eye.x+towards.x, eye.y+towards.y, eye.z+towards.z, up.x, up.y, up.z);
}

void Camera::Print() {
	cout << "-----------------------" << endl;
	cout << "Eye:     " << eye.x << " " << eye.y << " " << eye.z << endl;
	cout << "Towards: " << towards.x << " " << towards.y << " " << towards.z << endl;
	cout << "Up:      " << up.x << " " << up.y << " " << up.z << endl;
	cout << "-----------------------" << endl;
}