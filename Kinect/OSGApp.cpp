#include <SDL.h>
#include <osg\PositionAttitudeTransform>
#include <osgUtil\SceneView>
#include <osgDB\ReadFile>
#include "OSGApp.h"
#include "OSGCameraCtlHandler.h"

#include <iostream>

using namespace osg;

OSGApp::OSGApp(void)
{
	running = true;
	screen = NULL;
	width = 640;
	height = 480;
	flags = SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL;
}

OSGApp::~OSGApp(void)
{
}

bool OSGApp::onInit() {
	if (!initSDL()) return false;

	sceneView = new osgUtil::SceneView();
	sceneView->setDefaults();
	
	// Set Camera
	camera = sceneView->getCamera();
	camera->setViewport(0, 0, width, height);
	camera->setProjectionMatrixAsPerspective(
		45.0, camera->getViewport()->aspectRatio(),
		0.1, 1000);
	camera->setClearColor(Vec4(0.2, 0.2, 0.4, 0.0));
	camera->setViewMatrixAsLookAt(
		Vec3(0, -5, 0),   // Eye
		Vec3(0, 0, 0),     // Towards
		Vec3(0, 0, 1));    // Up

	handler = new OSGCameraCtlHandler(this, sceneView->getCamera());

	// Construct scene
	root = new Group();

	//ref_ptr<Node> node = osgDB::readNodeFile("C:/OpenSceneGraph/data/cow.osg");
    // Phantom object
	ref_ptr<Node> node1 = osgDB::readNodeFile("hand.obj");
	if (!node1) {
        std::cerr << "Could not find object file" << std::endl;
		return false;
	}
    PositionAttitudeTransform* pat1 = new PositionAttitudeTransform();
    pat1->setPosition(Vec3d(0,-2,0));
    pat1->setAttitude(Quat(-1.57, Vec3d(1,0,0)));
    pat1->addChild(node1);
    // Make it a phantom!
    StateSet* ss = pat1->getOrCreateStateSet();
    ColorMask* cm = new ColorMask(false, false, false, false);
    ss->setAttributeAndModes(cm, StateAttribute::ON);
    ss->setRenderBinDetails(-1000, "RenderBin", StateSet::USE_RENDERBIN_DETAILS);

    // virtual object
	ref_ptr<Node> node = osgDB::readNodeFile("hand.obj");
	if (!node) {
        std::cerr << "Could not find object file" << std::endl;
		return false;
	}
	PositionAttitudeTransform* pat = new PositionAttitudeTransform();
    pat->setScale(Vec3d(2,2,2));
	pat->addChild(node);
	root->addChild(pat);
	root->addChild(pat1);

	/*Geode* geode = new Geode();
	geode->addDrawable(new ShapeDrawable(new Cone(Vec3(0,0,0), 1, 1)));
	root->addChild(geode);
	*/
	sceneView->init();
	sceneView->setSceneData(root.get());
	return true;
}

void OSGApp::onLoop() {
	sceneView->update();
}

void OSGApp::onRender() {
	sceneView->cull();
	sceneView->draw();
	SDL_GL_SwapBuffers();
}
