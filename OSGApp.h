#pragma once
#include "app.h"
#include <osg\CameraNode>
#include <osg\Geode>
#include <osg\ShapeDrawable>
#include <osgUtil\SceneView>

class OSGApp :
	public App
{
public:
	OSGApp(void);
	~OSGApp(void);
	bool onInit();
	void onLoop();
	void onRender();

private:
	osg::ref_ptr<osg::Group> root;
	osg::ref_ptr<osgUtil::SceneView> sceneView;
	osg::ref_ptr<osg::CameraNode> camera;
};