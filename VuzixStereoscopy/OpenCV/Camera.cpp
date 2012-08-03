#include "Camera.h"
#include <opencv2\calib3d\calib3d.hpp>
#include <gl/GL.h>
#include <gl/GLU.h>

#include <iostream>
using namespace std;

const double DEGREES_TO_RADIANS = 0.0174532925;

static const Point3f defaulteye(0,0,0);
static const Point3f defaulttowards(0,0,1);
static const Point3f defaultup(0,-1,0);

void Camera::ApplyRT(Mat& rvec, Mat& tvec) {
	Mat r(3,3,CV_32FC1);
	Mat e = (Mat_<float>(3,1) << eye.x,eye.y,eye.z);
	Mat t = (Mat_<float>(3,1) << towards.x,towards.y,towards.z);
	Mat u = (Mat_<float>(3,1) << up.x,up.y,up.z);
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

void Camera::SetRT(Mat& rvec, Mat& tvec) {
	Mat r(3,3,CV_32FC1);
	Mat e = (Mat_<float>(3,1) << defaulteye.x,defaulteye.y,defaulteye.z);
	Mat t = (Mat_<float>(3,1) << defaulttowards.x,defaulttowards.y,defaulttowards.z);
	Mat u = (Mat_<float>(3,1) << defaultup.x,defaultup.y,defaultup.z);
	Rodrigues(rvec,r);
	
	transpose(r,r);
	e = r*-tvec;
	t = r*(t-tvec) - e;
	u = r*(u-tvec) - e;
	eye = *(e.ptr<Point3f>(0));
	towards = *(t.ptr<Point3f>(0));
	up = *(u.ptr<Point3f>(0));
	towards *= 1/norm(towards);
	up *= 1/norm(up);
	right = towards.cross(up);
}

void Camera::AvgPos(Mat& rvec, Mat& tvec) {
	Mat r(3,3,CV_32FC1);
	Mat e = (Mat_<float>(3,1) << defaulteye.x,defaulteye.y,defaulteye.z);
	Rodrigues(rvec,r);
	transpose(r,r);
	e = (e-r*tvec)*0.5;
	eye = *(e.ptr<Point3f>(0));
}

void Camera::OpenGLCamera(int stereoscopy, double separation) {
	double near_plane = 0.01;
	double far_plane = 1000.0;
	double aspect = xfov/(double) yfov;

	double rad = DEGREES_TO_RADIANS * yfov / 2;
	double topf = near_plane*tan(rad);
	double rightf = aspect*topf;
	double frustrumshift = eye_separation/2 * near_plane/focal_length;

	if (stereoscopy == PROJECTION) {
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, height, 0, 1, -1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		return;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (stereoscopy == CENTER) {
		gluPerspective(yfov, aspect, near_plane, far_plane);
	} else {
		float left, right;
		if (stereoscopy == LEFT) {
			left  = -rightf + frustrumshift;
			right =  rightf + frustrumshift;
		} else if (stereoscopy == RIGHT) {
			left  = -rightf - frustrumshift;
			right =  rightf - frustrumshift;
		}
		glFrustum(left, right, -topf, topf, near_plane, far_plane);
	}
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, 
		      eye.x + towards.x, eye.y + towards.y, eye.z + towards.z, 
			  up.x, up.y, up.z);

	if (stereoscopy == LEFT) {
		glViewport(0,0,width/2,height);
	} else if (stereoscopy == RIGHT) {
		glViewport(width/2,0,width/2,height);
	} else if (stereoscopy == CENTER) {
		glViewport(0,0,width,height);
	}
}

void Camera::Print() {
	cout << "-----------------------" << endl;
	cout << "Eye:     " << eye.x << " " << eye.y << " " << eye.z << endl;
	cout << "Towards: " << towards.x << " " << towards.y << " " << towards.z << endl;
	cout << "Up:      " << up.x << " " << up.y << " " << up.z << endl;
	cout << "-----------------------" << endl;
}