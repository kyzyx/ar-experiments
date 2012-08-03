#pragma once
#include <opencv2\core\core.hpp>

using namespace cv;

class Camera {
private:	
	Point3f eye;
	Point3f towards;
	Point3f up;

	Point3f right;

	double xfov, yfov;

	double eye_separation;
	double focal_length;

	int width, height;
public:
	enum {CENTER, LEFT, RIGHT, PROJECTION};
	Camera() {
	}
	Camera(int w, int h, double x) 
		: width(w), height(h), xfov(x)
	{
		eye = Point3f(0,0,0);
		towards = Point3f(1,0,0);
		up = Point3f(0,1,0);
		right = Point3f(0,0,1);
		yfov = xfov*height/width;

		eye_separation = 0.06;
		focal_length = 1;
	}
	Camera(Point3f e, Point3f t, Point3f u)
		:eye(e), towards(t), up(u), xfov(45), yfov(33.25), width(640), height(480)
	{
			right = towards.cross(up);
			eye_separation = 0.06;
			focal_length = 1;
	}
	Camera(Point3f e, Point3f t, Point3f u, double x, double y)
		:eye(e), towards(t), up(u), xfov(x), yfov(y), width(640), height(480)
	{
			right = towards.cross(up);
			eye_separation = 0.06;
			focal_length = 1;
	}
	Camera(Point3f e, Point3f t, Point3f u, int w, int h, double x)
		:eye(e), towards(t), up(u), width(w), height(h), xfov(x)
	{
			yfov = xfov*height/width;
			right = towards.cross(up);
			eye_separation = 0.06;
			focal_length = 1;
	}
	Camera(Camera& c)
		:eye(c.eye), towards(c.towards), up(c.up), right(c.right), xfov(c.xfov), yfov(c.yfov)
	{
		eye_separation = 0.06;
		focal_length = 1;
	}

	double increaseSep(double d) { eye_separation += d; return eye_separation; }
	double increaseFocus(double f) { focal_length += f; return focal_length; }
	Point3f getEye()     const { return eye; }
	Point3f getTowards() const { return towards; }
	Point3f getUp()      const { return up; }
	Point3f getRight()   const { return right; }

	void ApplyRT(Mat& rvec, Mat& tvec);
	void SetRT(Mat& rvec, Mat& tvec);
	void AvgPos(Mat& rvec, Mat& tvec);
	void OpenGLCamera(int stereoscopy = CENTER, double separation = 0.);
	void Print();
};