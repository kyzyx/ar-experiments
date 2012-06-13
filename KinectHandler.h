#pragma once
#include <Windows.h>
#include <Ole2.h>
#include <NuiApi.h>

class KinectHandler
{
public:
	static const int        width  = 640;
    static const int        height = 480;
    static const int        bpp = 4;


	KinectHandler(void);
	~KinectHandler(void);
	bool initSensors();
	void update(BYTE* dest);
	int getWidth() const { return width; }
	int getHeight() const { return height; }


private:
	HANDLE depthEvent;
	HANDLE depthStream;
	INuiSensor* sensor;
	
	void processDepthData(BYTE* dest);
	void writeDepth(BYTE* dest, USHORT depth);
};

