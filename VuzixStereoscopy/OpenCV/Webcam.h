#pragma once
#include <Windows.h>

HRESULT SetCamera();
HRESULT SetExposureTime( bool AutoExposureOff, long Time);
HRESULT Setup();
void CleanupCOM();