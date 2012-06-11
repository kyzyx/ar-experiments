#define GL_GLEXT_PROTOTYPES
#include <GL/glew.h>
//#include <SDL_opengl.h>
#include "KinectGLApp.h"
#define USE_PBO 1

KinectGLApp::KinectGLApp(void)
{
	running = true;
	screen = NULL;
	width = 640;
	height = 480;
	flags = SDL_HWSURFACE | SDL_GL_DOUBLEBUFFER | SDL_OPENGL;
}


KinectGLApp::~KinectGLApp(void)
{
	if (imageData) delete [] imageData;
	glDeleteTextures(1, &textureId);
	glDeleteBuffers(2, pboIds);
}

bool KinectGLApp::onInit() {
	if (!initSDL()) return false;
	glewInit();
	char str[256];
	sprintf_s(str, "%p\n", glGenBuffers);
	OutputDebugString(str);

	kinect = KinectHandler();
	if (!kinect.initSensors()) return false;

	dataSize = kinect.getWidth()*kinect.getHeight()*kinect.bpp;
	imageData = new GLubyte[dataSize];

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
 
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
 
	SDL_GL_SetAttribute(SDL_GL_ACCUM_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ACCUM_ALPHA_SIZE, 8);
 
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 2);
	
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);

	if (USE_PBO) {
		// Initialize pbos
		glGenBuffersARB(npbos, pboIds);
		for (int i = 0; i < npbos; ++i) {
			glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[i]);
			glBufferData(GL_PIXEL_UNPACK_BUFFER, dataSize, 0, GL_STREAM_DRAW);
		}
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
	// Initialize texture
	glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, kinect.getWidth(), kinect.getHeight(), 0, GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*) imageData);
    glBindTexture(GL_TEXTURE_2D, 0);

	glClearColor(0,0,0,0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
	/*glDisable(GL_LIGHTING);*/

	glViewport(0, 0, 640, 480);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, 640, 480, 0, 1, -1);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	return true;
}


void KinectGLApp::onRender() {
	static int index = 0;
	index = (index+1)%npbos;
	int nextIndex = (index+1)%npbos;
	if (USE_PBO) {
		// Write current PBO to texture
		glBindTexture(GL_TEXTURE_2D, textureId);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[index]);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, kinect.getWidth(), kinect.getHeight(), GL_BGRA, GL_UNSIGNED_BYTE, 0);

		// Bind PBO to writ pixel values
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboIds[nextIndex]);
		// Map buffer into client memory
		glBufferData(GL_PIXEL_UNPACK_BUFFER, dataSize, 0, GL_STREAM_DRAW_ARB);
		GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		if (ptr) {
			kinect.update(ptr);
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release pointer to mapping buffer
		}
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}
	else {
		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, kinect.getWidth(), kinect.getHeight(), GL_BGRA, GL_UNSIGNED_BYTE, (GLvoid*)imageData);
		kinect.update(imageData);
	}
	// Clear buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// Draw texture
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);  
	//glColor3f(1, 0, 0);	
	glVertex3f(0, 0, 0);
		glTexCoord2f(1.0f, 0.0f);  
	//glColor3f(1, 1, 0);
	glVertex3f(kinect.getWidth(), 0, 0);
		glTexCoord2f(1.0f, 1.0f);  
		//glColor3f(1, 0, 1);
		glVertex3f(kinect.getWidth(), kinect.getHeight(), 0.0f);
		glTexCoord2f(0.0f, 1.0f);  
		//glColor3f(1, 1, 1);
		glVertex3f(0, kinect.getHeight(), 0.0f);
    glEnd();
	
	SDL_GL_SwapBuffers();
}
