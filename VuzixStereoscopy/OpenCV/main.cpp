#include <Windows.h>

#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glut.h>

#include <iwrstdrv.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

#include <cmath>
#include "Webcam.h"
#include "Camera.h"
#include "ProcessCamera.h"

Camera cam;
ProcessCamera* proc;
int width = 1024;
int height = 768;

HANDLE h;

bool fullscreen = false;

void draw();

void reshape(int w, int h) {
	width = w;
	height = h;
}

void cleanup() {
	//IWRSTEREO_SetStereo(h, false);
	CleanupCOM();
	if (proc) delete proc;
	exit(0);
}

void key(unsigned char key, int x, int y) {
	switch(key){
		case 'q': // exit with q or Escape
		case VK_ESCAPE:
			cleanup();
			break;
		case 'o':
			cam.increaseSep(0.005);
			break;
		case 'p':
			cam.increaseSep(-0.005);
			break;
		case 'k':
			cam.increaseFocus(0.5);
			break;
		case 'l':
			cam.increaseFocus(-0.5);
			break;
		case VK_RETURN:
			if (fullscreen) {
				glutReshapeWindow(1024,768);
				//IWRSTEREO_SetStereo(h, false);
			} else {
				glutFullScreen();
				//IWRSTEREO_SetStereo(h, true);
			}
			fullscreen = !fullscreen;
			break;
		default:
			return;
	}
	cout << cam.increaseFocus(0) << " " << cam.increaseSep(0) << endl;
}

bool init(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	//glutInitWindowPosition(1600, 0);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Hello, World");
    glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutDisplayFunc(draw);
    glutIdleFunc(draw);
    return true;
}

bool initVuzix() {
	return true;
	/*h = IWRSTEREO_Open();
	if (h == INVALID_HANDLE_VALUE) {
		DWORD e = GetLastError();
		if (e == ERROR_INVALID_FUNCTION) {
			// incorrect firmware
			return false;
		} else if (e == ERROR_DEV_NOT_EXIST) {
			// plug in!
			return false;
		}
	}
	return IWRSTEREO_SetStereo(h, true);*/
}

bool initOpenCV() {
	HRESULT hr = Setup();
	if (FAILED(hr)) return false;
	hr = SetCamera();
	//hr = SetExposureTime(true, 363);
	if (FAILED(hr)) return false;
	proc = new ProcessCamera();
	return true;
}

bool initGL() {
	cam = Camera(width, height, 45.0);
	glClearColor(0,0,0,0);
    glClearDepth(1.0f);
	return true;
}


void renderWorld() {
	static double r = 0;
    glPushMatrix();
	glTranslatef(0.0f,0.0f,0.0f);
	glPushMatrix();
	glRotatef(r, 0.,1.,0.);
	glColor3f(1.0,0.0,0.0);
	glutWireTeapot(0.05f);
	glPopMatrix();
	glPopMatrix();
	//r += 0.1;
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	proc->GetFrames();
	// Texture background;
	cam.OpenGLCamera(Camera::PROJECTION);
	proc->SetCamera(cam);
	proc->RenderStereoBG(width,height);
	cam.OpenGLCamera(Camera::LEFT);
	renderWorld();
	cam.OpenGLCamera(Camera::RIGHT);
	renderWorld();
	glutSwapBuffers();
}

int main(int argc, char* argv[]) {
	if (!init(argc, argv)) return 1;
	if (!initVuzix()) {
		cleanup();
		return 1;
	}
	if (!initGL()) {
		cleanup();
		return 1;
	}
	if (!initOpenCV()) {
		cleanup();
		return 1;
	}

	glutMainLoop();
	return 0;
}