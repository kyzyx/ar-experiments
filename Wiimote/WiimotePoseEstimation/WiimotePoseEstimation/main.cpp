#include <Windows.h>
#include "WiimotePointTracker.h"
#include "tracker.h"
#include "Camera.h"

#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glut.h>
#include <iostream>

using namespace std;

int width = 1024;
int height = 768;

void draw();

WiimotePointTracker* wpt = NULL;
Tracker* t = NULL;
vector<double> tcoords(8,-1);
int coords[8];


void reshape(int w, int h) {
	width = w;
	height = h;
}

void cleanup() {
	if (wpt) {
		wpt->stopTracking();
		delete wpt;
	}
	if (t) {
		delete t;
	}
	exit(0);
}
void key(unsigned char key, int x, int y) {
	switch(key){
		case 'q': // exit with q or Escape
		case VK_ESCAPE:
			cleanup();
			break;
		default:
			return;
	}
}

bool init(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(width, height);
	glutInitWindowPosition(0, 0);
	//glutInitWindowPosition(0, 0);
	glutCreateWindow("Hello, World");
    glutReshapeFunc(reshape);
	glutKeyboardFunc(key);
	glutDisplayFunc(draw);
    glutIdleFunc(draw);

	// Construct WiimotePointTracker
	wpt = new WiimotePointTracker();
	wpt->startTracking();

	t = new Tracker();
    return true;
}

void cleancoords() {
	// Assume highest, lowest point will be constant
	int left = -1, up = -1, down = -1, right = -1;
	int xmax = -1;
    int xmin = 2000;
	int ymax = -1;
    int ymin = 2000;
	for (int i = 0; i < 4; ++i) {
		if (coords[2*i] > xmax) {
			xmax = coords[2*i];
			right = i;
		}
		if (coords[2*i] < xmin) {
			xmin = coords[2*i];
			left = i;
		}
		if (coords[2*i+1] > ymax) {
			ymax = coords[2*i+1];
			up = i;
		}
		if (coords[2*i+1] < ymin) {
			ymin = coords[2*i+1];
			down = i;
		}
	}
	cout << left << up << down << right << endl;
	tcoords[0] = coords[2*left];
	tcoords[1] = coords[2*left+1];
	tcoords[2] = coords[2*up];
	tcoords[3] = coords[2*up+1];
	tcoords[4] = coords[2*down];
	tcoords[5] = coords[2*down+1];
	tcoords[6] = coords[2*right];
	tcoords[7] = coords[2*right+1];
}

void updateWiimote() {
	// Fetch from wiimotepointtracker
	wpt->getCoords(coords);
	glColor3f(1.0,1.0,1.0);
	for (int i = 0; i < 4; ++i) {
		glVertex3f(coords[2*i], height - coords[2*i+1], 0);
	}
	cleancoords();
}

void orthoCamera() {
	// Camera setup
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	orthoCamera();
	glBegin(GL_POINTS);
	updateWiimote();
	glEnd();
	Camera cam = t->getPosition(tcoords);
	cam.OpenGLCamera();
	cam.Print();
	glColor3f(1.0,0,0);
	glutWireTeapot(1.0);
	glutSwapBuffers();
}

int main(int argc, char* argv[]) {
	if (!init(argc, argv)) cleanup();
	// OpenGL setup
    glClearColor(0,0,0,0);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);

	glPointSize(4);
	glutMainLoop();
	return 0;
}