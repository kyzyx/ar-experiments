#include "CameraCtlHandler.h"

CameraCtlHandler::CameraCtlHandler(void)
{
}

CameraCtlHandler::CameraCtlHandler(OSGApp* myapp, CameraNode* cam) {
	app = myapp;
	
	camera = cam;
	camera->getViewMatrixAsLookAt(eye, towards, up);
	towards = towards - eye; // Change to towards vector
	towards.normalize();
	up.normalize();
	
	move = 0;
	translateSpeed = 0.01;
	rotateSpeed = 0.05;
}

CameraCtlHandler::~CameraCtlHandler(void)
{
}

static enum {
	UP    = 1, 
	DOWN  = 2, 
	LEFT  = 4, 
	RIGHT = 8, 
	FORWARD  = 16, 
	BACKWARD = 32
};

void CameraCtlHandler::update() {
	Vec3 right = towards ^ up;
	if (move & UP) {
		eye += up*translateSpeed;
	}
	if (move & DOWN) {
		eye -= up*translateSpeed;
	}
	if (move & RIGHT) {
		eye += right*translateSpeed;
	}
	if (move & LEFT) {
		eye -= right*translateSpeed;
	}
	if (move & FORWARD) {
		eye += towards*translateSpeed;
	}
	if (move & BACKWARD) {
		eye -= towards*translateSpeed;
	}
	camera->setViewMatrixAsLookAt(eye, eye + towards, up);
}

void CameraCtlHandler::OnKeyDown(SDLKey sym, SDLMod mod, Uint16 unicode) {
	switch(sym) {
		case SDLK_LEFT:
			move |= LEFT;
			break;
		case SDLK_RIGHT:
			move |= RIGHT;
			break;
		case SDLK_UP:
			move |= FORWARD;
			break;
		case SDLK_DOWN:
			move |= BACKWARD;
			break;
	}
}

void CameraCtlHandler::OnKeyUp(SDLKey sym, SDLMod mod, Uint16 unicode) {
	switch(sym) {
		case SDLK_LEFT:
			move &= ~LEFT;
			break;
		case SDLK_RIGHT:
			move &= ~RIGHT;
			break;
		case SDLK_UP:
			move &= ~FORWARD;
			break;
		case SDLK_DOWN:
			move &= ~BACKWARD;
			break;
	}
}
