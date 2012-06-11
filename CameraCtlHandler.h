#pragma once
#include <SDL_opengl.h>

#include "OSGApp.h"
#include "sdlhandler.h"
#include <osg\ref_ptr>
#include <osg\CameraNode>

using namespace osg;

class OSGApp;

class CameraCtlHandler :
	public SDLHandler
{
public:
	CameraCtlHandler(void);
	CameraCtlHandler(OSGApp* myapp, CameraNode* cam);
	~CameraCtlHandler(void);

	void update();
	void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);

protected:
	double translateSpeed;
	double rotateSpeed;
	OSGApp* app;
	CameraNode* camera;

	Uint32 move;

	Vec3 eye;
	Vec3 towards;
	Vec3 up;
};

