#pragma once
//#include <SDL_opengl.h>
#include <GL\glew.h>
#include "App.h"
#include "KinectHandler.h"

class KinectGLApp :
	public App
{
public:
	static const int npbos = 1;
	KinectGLApp(void);
	~KinectGLApp(void);

	bool onInit();
	void onRender();
private:
	int dataSize;
	unsigned int textureId;
	GLubyte* imageData;
	GLuint pboIds[1];
	KinectHandler kinect;
	void updateTexture();
};

