#include "WiimotePointTracker.h"
#include <Windows.h>
#include <wiiuse.h>
#include <wiiusecpp\wiiusecpp.h>

#include <vector>
#include <iostream>

using namespace std;
int LED_MAP[4] = {CWiimote::LED_1, CWiimote::LED_2, CWiimote::LED_3, CWiimote::LED_4};
WiimotePointTracker::WiimotePointTracker(void)
{
	int numFound = wii.Find(5);
	vector<CWiimote>& wiimotes = wii.Connect();
	if (wiimotes.empty()) {
		// ERROR! BAD!
		return;
	}
	wiimotes[0].SetLEDs(CWiimote::LED_1);

	for (int i = 0; i < 4; ++i) {
		coords[2*i] = -1; coords[2*i+1] = -1;
		visible[i] = 0;
	}
	updatethread = 0;

	InitializeCriticalSectionAndSpinCount(&cs,0x200);
}


WiimotePointTracker::~WiimotePointTracker(void)
{
	DeleteCriticalSection(&cs);
}

static DWORD WINAPI start(LPVOID args) {
	WiimotePointTracker* wpt = (WiimotePointTracker*) args;
	while (true) {
		wpt->update();
	}
}

void WiimotePointTracker::startTracking() {
	if (updatethread) return;
	CWiimote& wiimote = wii.GetWiimotes()[0];
	wiimote.IR.SetMode(CIR::ON);
	updatethread = CreateThread(NULL, 0, start, this, 0, 0);
}
bool WiimotePointTracker::stopTracking() {
	if (!updatethread) return false;
	bool ret;
	EnterCriticalSection(&cs);
	CWiimote& wiimote = wii.GetWiimotes()[0];
	wiimote.IR.SetMode(CIR::OFF);
	ret = TerminateThread(updatethread, 0);
	LeaveCriticalSection(&cs);
	return ret;
}

void WiimotePointTracker::update() {
	if (wii.Poll()) {
		CWiimote& wiimote = wii.GetWiimotes()[0];
		switch(wiimote.GetEvent()) {
			case CWiimote::EVENT_EVENT:
				{
					vector<CIRDot>& dots = wiimote.IR.GetDots();
					EnterCriticalSection(&cs);
					for(int i = 0; i < dots.size(); ++i) {
						if(dots[i].isVisible())	{
							dots[i].GetCoordinate(coords[2*i], coords[2*i+1]);
							visible[i] = 0;
						}
						else {
							++visible[i];
							coords[2*i] = -1;
							coords[2*i+1] = -1;
						}
						LeaveCriticalSection(&cs);
					}
					break;
				}
			default:
				break;
		}
	}
}


void WiimotePointTracker::getCoords(int result[8]) {
	EnterCriticalSection(&cs);
	for (int i = 0; i < 8; ++i) {
		result[i] = coords[i];
	}
	LeaveCriticalSection(&cs);
}