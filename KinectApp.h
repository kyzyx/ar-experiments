#pragma once
#include "osgapp.h"
class KinectApp :
	public OSGApp
{
public:
	KinectApp(void);
	~KinectApp(void);

	bool onInit();
	void onLoop();
private:
	KinectHandler kinect;
};

