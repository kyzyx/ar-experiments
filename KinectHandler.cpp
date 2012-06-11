#include "KinectHandler.h"
#include <Windows.h>
#include <NuiApi.h>
#include <NuiImageCamera.h>
#include <NuiSensor.h>

KinectHandler::KinectHandler(void)
{
}


KinectHandler::~KinectHandler(void)
{
}

bool KinectHandler::initSensors() {
	int numSensors;
	long result = -1;

	if (NuiGetSensorCount(&numSensors) < 0) return false;

	for (int i = 0; i < numSensors; ++i) {
		if (NuiCreateSensorByIndex(i, &sensor) < 0) continue;
		if (sensor->NuiStatus() == 0) break;
		sensor->Release();
	}
	if (sensor) {
		if (sensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_DEPTH | NUI_INITIALIZE_FLAG_USES_COLOR) == 0) {
			depthEvent = CreateEvent(NULL, true, false, NULL);
			result = sensor->NuiImageStreamOpen(
				NUI_IMAGE_TYPE_DEPTH,			 // Image type
				NUI_IMAGE_RESOLUTION_640x480,	 // Resolution
				0,								 // Image stream flags, e.g. near mode, player indices (?) min/max depth (?)
				2,								 // Number of frames to buffer
				depthEvent,
				&depthStream);
		}
	}
	if (result < 0 || !sensor) return false;
	return true;
}

void KinectHandler::update(BYTE* dest) {
	DWORD dwEvent = MsgWaitForMultipleObjects(1, &depthEvent, false, 0, QS_ALLINPUT);
	if (dwEvent != WAIT_OBJECT_0) return;
	if (!sensor) return;
	if (WaitForSingleObject(depthEvent, 0) == WAIT_OBJECT_0) {
		processDepthData(dest);
	}
}

void KinectHandler::processDepthData(BYTE* dest) {
	NUI_IMAGE_FRAME imageFrame;
	if (sensor->NuiImageStreamGetNextFrame(depthStream, 0, &imageFrame) < 0) return;
	INuiFrameTexture* texture = imageFrame.pFrameTexture;

	NUI_LOCKED_RECT LockedRect;

    // Lock the frame data so the Kinect knows not to modify it while we're reading it
    texture->LockRect(0, &LockedRect, NULL, 0);

    // Make sure we've received valid data
    if (LockedRect.Pitch != 0)
    {
        const USHORT* curr = (const USHORT *) LockedRect.pBits;
        const USHORT* dataEnd = curr + (width*height);          // end pixel is start + width*height - 1

		while (curr < dataEnd) {
            // discard the portion of the depth that contains only the player index
            USHORT depth = NuiDepthPixelToDepth(*curr);	
			writeDepth(dest, depth);
			dest += 4;
            ++curr;
		}
    }
    // We're done with the texture so unlock it
    texture->UnlockRect(0);

    // Release the frame
    sensor->NuiImageStreamReleaseFrame(depthStream, &imageFrame);
}

void KinectHandler::writeDepth(BYTE* dest, USHORT depth) {
	BYTE blue = 0, green = 0, red = 0;
	depth >>= 1;
	if (depth < 128) {
		blue = green = red = 255;
	} else {
		depth -= 128;
		if (depth < 256) {
			blue = (255 - depth);
			green = (255 - depth);
			red = 255;
		}
		else {
			depth -= 256;
			if (depth < 256) {
				red = (255 - depth);
				green = depth;
			}
			else {
				depth -= 256;
				if (depth < 256) {
					green = (255 - depth);
					blue = depth;
				}
				else {
					depth -= 256;
					blue = (255 - depth);
				}
			}
		}
	}
	*dest = blue;
	*(dest+1) = green;
	*(dest+2) = red;
}
