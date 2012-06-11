#pragma once
#include "CameraCtlHandler.h"

#include "OSGApp.h"
#include "sdlhandler.h"
#include <osg\ref_ptr>
#include <osg\CameraNode>

class OSGCameraCtlHandler :
	public CameraCtlHandler
{
public:
	OSGCameraCtlHandler(void);
	OSGCameraCtlHandler(OSGApp* myapp, osg::CameraNode* cam);
	~OSGCameraCtlHandler(void);

	virtual void update();

private:
	osg::CameraNode* camera;

	osg::Vec3 eyevec;
	osg::Vec3 towardsvec;
	osg::Vec3 upvec;
};

