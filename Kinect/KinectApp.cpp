#include "KinectApp.h"
#include "KinectHandler.h"
#include <osg\Geometry>
#include <osgUtil\SceneView>
#include <osg\ref_ptr>
#include <osg\Texture2D>

using namespace osg;

KinectApp::KinectApp(void)
{
	running = true;
	screen = NULL;
	width = 640;
	height = 480;
	flags = SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL;

	kinect = KinectHandler();
}


KinectApp::~KinectApp(void)
{
}

bool KinectApp::onInit() {
	if (!initSDL()) return false;

	kinect = KinectHandler();
	if (!kinect.initSensors()) return false;

	
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

	handler = new CameraCtlHandler(this, sceneView->getCamera());

	// Construct scene
	root = new Group();
	ref_ptr<Geode> geode = new Geode();
	Texture2D* texture = new Texture2D();
	texture->setDataVariance(Object::DYNAMIC);

	ImageStream* imagestream = new ImageStream();
	imagestream->allocateImage(kinect.getWidth(), kinect.getHeight(), 1, GL_BGRA, GL_UNSIGNED_BYTE, 1);
	imageStream->setDataVariance(Object::DYNAMIC);
	imageStream->play();
	imageStream->setImage(imageStream->s(), imageStream->t(), 1, 
		GL_TEXTURE_2D, GL_BGRA, GL_UNSIGNED_BYTE, kinect.getCurrentBuffer(), Image::NO_DELETE, 1);

	osg::ref_ptr<osg::Geometry> geometry = createTexturedQuadGeometry(
		Vec3f(0,0,0),
		Vec3f(kinect.getWidth()/2,0,0),
		Vec3f(0,kinect.getHeight()/2,0)
		0,0,1,1); // corner width height left bottom top right
	geode->addDrawable(geometry);
	geode->getOrCreateStateSet()->setTextureAttributeAndModes(0,texture,StateAttribute::ON);
	root->addChild(geode);

	sceneView->init();
	sceneView->setSceneData(root.get());
	return true;
}

void KinectApp::onLoop() {
	sceneView->update();
}