#pragma once
#include <SDL.h>
#include <gl\glew.h>
#include "sdlhandler.h"

class CameraCtlHandler :
	public SDLHandler
{
public:
	CameraCtlHandler(void);
	CameraCtlHandler(App* myapp, int width, int height, int yfov);
	~CameraCtlHandler(void);

	virtual void update();
	void OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode);
	void OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode);
	void setCamera();

protected:
	enum {
        TRANS_UP    = 1,
        TRANS_DOWN  = 2,
        TRANS_LEFT  = 4,
        TRANS_RIGHT = 8,
        TRANS_FORWARD  = 16,
        TRANS_BACKWARD = 32,
        ROT_UP    = 64,
        ROT_DOWN  = 128,
        ROT_LEFT  = 256,
        ROT_RIGHT = 512
    };

	GLfloat translateSpeed;
	GLfloat rotateSpeed;
	App* app;

	Uint32 move;

	int w;
	int h;
	GLfloat p;
	GLfloat eye[3];
	GLfloat towards[3];
	GLfloat up[3];
	GLfloat right[3];
};

