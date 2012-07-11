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
public:
	enum {CENTER, LEFT, RIGHT};
	Camera() {
	}
	Camera(Point3f e, Point3f t, Point3f u)
		:eye(e), towards(t), up(u), xfov(45), yfov(33.25)
	{
			right = towards.cross(up);
	}
	Camera(Point3f e, Point3f t, Point3f u, double x, double y)
		:eye(e), towards(t), up(u), xfov(x), yfov(y)
	{
			right = towards.cross(up);
	}
	Camera(Camera& c)
		:eye(c.eye), towards(c.towards), up(c.up), right(c.right), xfov(c.xfov), yfov(c.yfov)
	{
		;
	}

	Point3f getEye()     const { return eye; }
	Point3f getTowards() const { return towards; }
	Point3f getUp()      const { return up; }
	Point3f getRight()   const { return right; }

	void ApplyRT(Mat& rvec, Mat& tvec);
	void OpenGLCamera(int stereoscopy = CENTER, double separation = 0.);
	void Print();
};