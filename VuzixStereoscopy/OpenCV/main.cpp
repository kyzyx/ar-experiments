#include <Windows.h>

#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glut.h>

#include <iwrstdrv.h>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>

#include <cmath>
#include "Webcam.h"

enum {
	LEFT_EYE,
	RIGHT_EYE,
	BG
};
typedef struct vec {
	double x;
	double y;
	double z;
} Vec;

Vec eye;
Vec up;
Vec towards;

int width = 1024;
int height = 768;

HANDLE h;
GLuint texId[2];


const double DEGREES_TO_RADIANS = 0.0174532925;
bool fullscreen = false;

CvCapture* left_cam;
CvCapture* right_cam;

void draw();

void reshape(int w, int h) {
	width = w;
	height = h;
}

void cleanup() {
	cvReleaseCapture(&left_cam);
	cvReleaseCapture(&right_cam);
	IWRSTEREO_SetStereo(h, false);
	CleanupCOM();
	exit(0);
}
void key(unsigned char key, int x, int y) {
	switch(key){
		case 'q': // exit with q or Escape
		case VK_ESCAPE:
			cleanup();
			break;
		case VK_RETURN:
			if (fullscreen) {
				glutReshapeWindow(1024,768);
				IWRSTEREO_SetStereo(h, false);
			} else {
				glutFullScreen();
				IWRSTEREO_SetStereo(h, true);
			}
			fullscreen = !fullscreen;
			break;
		default:
			return;
	}
}

bool init(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(1600, 0);
	//glutInitWindowPosition(0, 0);
	glutCreateWindow("Hello, World");
    glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutDisplayFunc(draw);
    glutIdleFunc(draw);
    return true;
}

bool initVuzix() {
	h = IWRSTEREO_Open();
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
	return IWRSTEREO_SetStereo(h, true);
}

bool initOpenCV() {
	HRESULT hr = Setup();
	if (FAILED(hr)) return false;
	hr = SetCamera();
	//hr = SetExposureTime(true, 363);
	if (FAILED(hr)) return false;
	left_cam = cvCaptureFromCAM(1);
	right_cam = cvCaptureFromCAM(2);
	if (!left_cam) {
		OutputDebugString("No left\n");
		return false;
	}
	if (!right_cam) {
		OutputDebugString("No right\n");
		return false;
	}
	return true;
}

bool initGL() {
	eye.x = 0;
	eye.y = 0;
	eye.z = 0;
	towards.x = 1;
	towards.y = 0;
	towards.z = 0;
	up.x = 0;
	up.y = 1;
	up.z = 0;
	glEnable(GL_TEXTURE_2D);
	glGenTextures(2, texId);
	glBindTexture(GL_TEXTURE_2D, texId[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, texId[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);

	glClearColor(0,0,0,0);
    glClearDepth(1.0f);
	return true;
}

void setCamera(int e) {
	static float fovy = 45.0;
	static float near_plane = 1.0;
	static float far_plane = 1000.0;
	static float eye_separation = 0.14;
	static float focal_length = 4;
	static float aspect = width/(double) height;

	static float rad = DEGREES_TO_RADIANS * fovy / 2;
	static float topf = near_plane*tan(rad);
	static float rightf = aspect*topf;
	static float frustrumshift = eye_separation/2 * near_plane/focal_length;

	if (e == BG) {
		glViewport(0, 0, width, height);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(0, width, height, 0, 1, -1);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		return;
	}

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	float left, right;
	if (e == LEFT_EYE) {
		left  = -rightf + frustrumshift;
		right =  rightf + frustrumshift;
	} else if (e == RIGHT_EYE) {
		left  = -rightf - frustrumshift;
		right =  rightf - frustrumshift;
	}
	glFrustum(left, right, -topf, topf, near_plane, far_plane);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, 
		      eye.x + towards.x, eye.y + towards.y, eye.z + towards.z, 
			  up.x, up.y, up.z);

	if (e == LEFT_EYE) {
		glViewport(0,0,width/2,height);
	} else if (e == RIGHT_EYE) {
		glViewport(width/2,0,width/2,height);
	}
}

void renderWorld() {
	static double r = 0;
    glPushMatrix();
	glTranslatef(5.0f,0.0f,0.0f);
	glPushMatrix();
	glRotatef(r, 0.,1.,0.);
	glColor3f(1.0,0.0,0.0);
	glutWireTeapot(1.5f);
	glPopMatrix();
	glPopMatrix();
	r += 0.05;
}

void renderBG() {
	// Get images
	IplImage* limg = 0;
	IplImage* rimg = 0;
	bool lfail = false;
	bool rfail = false;
	if (!cvGrabFrame(left_cam)) {
		lfail = true;
		return;
	}
	if (!cvGrabFrame(right_cam)) {
		rfail = true;
		return;
	}
	limg = cvRetrieveFrame(left_cam);
	rimg = cvRetrieveFrame(right_cam);
	if (!limg || !rimg) return;
	glEnable(GL_TEXTURE_2D);
	glColor3f(1.,1.,1.);
	glLoadIdentity();
	if (!lfail) {
	glBindTexture(GL_TEXTURE_2D, texId[0]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, limg->width, limg->height, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, limg->imageData);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);  
		glVertex3f(0, 0, 0);
		glTexCoord2f(1.0f, 0.0f);  
		glVertex3f(width/2, 0, 0);
		glTexCoord2f(1.0f, 1.0f);  
		glVertex3f(width/2, height, 0.0f);
		glTexCoord2f(0.0f, 1.0f);  
		glVertex3f(0, height, 0.0f);
    glEnd();
	}
	if(!rfail) {
	glBindTexture(GL_TEXTURE_2D, texId[1]);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, rimg->width, rimg->height,0, GL_BGR_EXT, GL_UNSIGNED_BYTE, rimg->imageData);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);  
		glVertex3f(width/2, 0, 0);
		glTexCoord2f(1.0f, 0.0f);  
		glVertex3f(width, 0, 0);
		glTexCoord2f(1.0f, 1.0f);  
		glVertex3f(width, height, 0.0f);
		glTexCoord2f(0.0f, 1.0f);  
		glVertex3f(width/2, height, 0.0f);
    glEnd();
	}
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	// Texture background;
	setCamera(BG);
	renderBG();
	
	/*setCamera(LEFT_EYE);
	renderWorld();
	setCamera(RIGHT_EYE);
	renderWorld();*/
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