#include "OSGCameraCtlHandler.h"


OSGCameraCtlHandler::OSGCameraCtlHandler(void)
{
}

OSGCameraCtlHandler::OSGCameraCtlHandler(OSGApp* myapp, osg::CameraNode* cam) {
	app = myapp;
	
	camera = cam;
	camera->getViewMatrixAsLookAt(eyevec, towardsvec, upvec);
	towardsvec = towardsvec - eyevec; // Change to towards vector
	towardsvec.normalize();
	upvec.normalize();
	
	move = 0;
	translateSpeed = 0.01;
	rotateSpeed = 0.05;
}

OSGCameraCtlHandler::~OSGCameraCtlHandler(void)
{
}

void OSGCameraCtlHandler::update() {
	osg::Vec3 right = towardsvec ^ upvec;
	if (move & TRANS_UP) {
		eyevec += upvec*translateSpeed;
	}
	if (move & TRANS_DOWN) {
		eyevec -= upvec*translateSpeed;
	}
	if (move & TRANS_RIGHT) {
		eyevec += right*translateSpeed;
	}
	if (move & TRANS_LEFT) {
		eyevec -= right*translateSpeed;
	}
	if (move & TRANS_FORWARD) {
		eyevec += towardsvec*translateSpeed;
	}
	if (move & TRANS_BACKWARD) {
		eyevec -= towardsvec*translateSpeed;
	}
    if (move & ROT_UP) {
        towardsvec = towardsvec*cos(rotateSpeed) + upvec*sin(rotateSpeed);
        upvec = right ^ towardsvec;
    }
    if (move & ROT_DOWN) {
        towardsvec = towardsvec*cos(rotateSpeed) - upvec*sin(rotateSpeed);
        upvec = right ^ towardsvec;
    }
    if (move & ROT_LEFT) {
        towardsvec = towardsvec*cos(rotateSpeed) - right*sin(rotateSpeed);
    }
    if (move & ROT_RIGHT) {
        towardsvec = towardsvec*cos(rotateSpeed) + right*sin(rotateSpeed);
    }
	camera->setViewMatrixAsLookAt(eyevec, eyevec + towardsvec, upvec);
}