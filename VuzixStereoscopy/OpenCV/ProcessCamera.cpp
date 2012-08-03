#include "ProcessCamera.h"
#include <Windows.h>
#include <gl\GL.h>

ProcessCamera::ProcessCamera(void)
{
	left_cam = cvCaptureFromCAM(1);
	right_cam = cvCaptureFromCAM(2);
	if (!left_cam) {
		OutputDebugString("No left\n");
		exit(1);
	}
	if (!right_cam) {
		OutputDebugString("No right\n");
		exit(1);
	}
	localizer = new Localizer("board.yml", "vuzix.yml", 0.0355);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(2, texId);
	glBindTexture(GL_TEXTURE_2D, texId[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindTexture(GL_TEXTURE_2D, texId[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}


ProcessCamera::~ProcessCamera(void)
{
	cvReleaseCapture(&left_cam);
	cvReleaseCapture(&right_cam);
	if (localizer) delete localizer;
}

void ProcessCamera::GetFrames(void) {
	lfail = false;
	rfail = false;
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
}

void ProcessCamera::RenderStereoBG(int width, int height) {
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