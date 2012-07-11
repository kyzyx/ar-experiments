#include <Windows.h>
#include "WiimotePointTracker.h"

#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glut.h>

int width = 1024;
int height = 768;

void draw();

WiimotePointTracker* wpt = NULL;
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
    return true;
}

void updateWiimote() {
	// Fetch from wiimotepointtracker
	wpt->getCoords(coords);
	for (int i = 0; i < 4; ++i) {
		glColor3f(0.33*i,0.33*i,0.33*i);
		glVertex3f(coords[2*i], height - coords[2*i+1], 0);
	}
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.f,0.f,1.f);
	glBegin(GL_QUADS);
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(0, 0, 0);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(width, 0, 0);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(width, height, 0.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(0, height, 0.0f);
	glEnd();
	glBegin(GL_POINTS);
	updateWiimote();
	glEnd();
	glutSwapBuffers();
}

int main(int argc, char* argv[]) {
	if (!init(argc, argv)) cleanup();
	// OpenGL setup
    glClearColor(0,0,0,0);
    glClearDepth(1.0f);
    glEnable(GL_TEXTURE_2D);

    // Camera setup
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, width, height, 0, 1, -1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	glPointSize(4);
	glutMainLoop();
	return 0;
}