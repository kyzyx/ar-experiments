#pragma once
#include <Windows.h>
#include <wiiuse.h>
#include <wiiusecpp\wiiusecpp.h>

class WiimotePointTracker
{
private:
	int coords[8];           // Most recent coordinates stored here
	bool visible[4];         // Whether the ith point is visible
	CWii wii;                // The wii object
	wiimote_t** wiiuse;      // The array of wiimote objects (only one supported)

	HANDLE updatethread;
	CRITICAL_SECTION cs;

public:
	WiimotePointTracker(void);
	~WiimotePointTracker(void);

	// Start/stop a thread that will update the IR camera coordinates
	void startTracking();
	bool stopTracking();

	// Force a fetch of IR camera coordinates
	void update();

	// Copies the current coordinates into result
	void getCoords(int result[8]);
};

