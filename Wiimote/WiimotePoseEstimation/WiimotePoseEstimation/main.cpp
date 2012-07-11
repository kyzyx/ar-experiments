#include <Windows.h>

#include <gl\GL.h>
#include <gl\GLU.h>
#include <gl\glut.h>

#include <wiiuse.h>
#include <wiiusecpp\wiiusecpp.h>

#include <vector>

using namespace std;

int width = 1024;
int height = 768;
bool tracking = false;

CWii wii;
wiimote_t** wiiuse;

int threshold = 5;

int x[4];
int y[4];
int invisible[4];

int LED_MAP[4] = {CWiimote::LED_1, CWiimote::LED_2, CWiimote::LED_3, CWiimote::LED_4};
void draw();

void reshape(int w, int h) {
	width = w;
	height = h;
}

void cleanup() {
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

	// Wiimote initialization
    int numFound = wii.Find(5);
	vector<CWiimote>& wiimotes = wii.Connect();
	if (wiimotes.empty()) return false;
	wiimotes[0].SetLEDs(CWiimote::LED_1);

	for (int i = 0; i < 4; ++i) {
		x[i] = -1; y[i] = -1;
		invisible[i] = 0;
	}
    return true;
}

void updateWiimote() {
	vector<CWiimote>& wiimotes = wii.GetWiimotes();
	if (wiimotes.empty()) {
		cleanup();
		return;
	}
	
	CWiimote& wiimote = wiimotes[0];
	if (wii.Poll()) {
		switch (wiimote.GetEvent()){
			case CWiimote::EVENT_EVENT:
				if (wiimote.Buttons.isJustPressed(CButtons::BUTTON_B)) {
					// wm.SetMotionSensingMode(CWiimote::OFF);
					wiimote.IR.SetMode(CIR::OFF);
				} else if (wiimote.Buttons.isJustPressed(CButtons::BUTTON_A)) {
					// wm.SetMotionSensingMode(CWiimote::ON);
					wiimote.IR.SetMode(CIR::ON);
				}
				/*if(wiimote.isUsingACC())
				{
					float pitch, roll, yaw, a_pitch, a_roll;
					wiimote.Accelerometer.GetOrientation(pitch, roll, yaw);
					wiimote.Accelerometer.GetRawOrientation(a_pitch, a_roll);
					printf("%s wiimote roll = %f [%f]\n", prefixString, roll, a_roll);
					printf("%s wiimote pitch = %f [%f]\n", prefixString, pitch, a_pitch);
					printf("%s wiimote yaw = %f\n", prefixString, yaw);
				}*/
				 if(wiimote.isUsingIR()) {
					tracking = true;
					/*printf("Num IR Dots: %i\n", wiimote.IR.GetNumDots());
					printf("IR State: %u\n", wiimote.IR.GetState());*/

					vector<CIRDot>& dots = wiimote.IR.GetDots();

					for(int i = 0; i < dots.size(); ++i) {
						if(dots[i].isVisible())	{
							dots[i].GetCoordinate(x[i], y[i]);
							invisible[i] = 0;
						}
						else {
							++invisible[i];
						}
					}
				}
				 else {
					tracking = false;
				 }
                break;

            case CWiimote::EVENT_STATUS:
                // TODO: Status?
                break;
            case CWiimote::EVENT_DISCONNECT:
            case CWiimote::EVENT_UNEXPECTED_DISCONNECT:
                cleanup();
                break;
			default:
				break;
		}
	}
	for (int i = 0; i < 4; ++i) {
		if (invisible[i] > threshold) continue;
		glColor3f(0.33*i,0.33*i,0.33*i);
		glVertex3f(x[i], height - y[i], 0);
	}
}

void draw() {
	glClear(GL_COLOR_BUFFER_BIT);
	if (tracking) {
		glColor3f(1.f,0.f,0.f);
	} else {
		glColor3f(0.f,0.f,1.f);
	}
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
	// draw
	glutSwapBuffers();
}

int main(int argc, char* argv[]) {
	if (!init(argc, argv)) return 1;
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