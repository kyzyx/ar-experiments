#include "Webcam.h"
#include <Windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <DShow.h>
#include <atlbase.h>
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }
#define SAFE_RELEASE_CCOMPTR(p) { if(p) { (p).Release(); } }

IAMCameraControl*	g_CCLeft = NULL;		// Left eye exposure time. 
IAMCameraControl*	g_CCRight = NULL;		// Right eye exposure time. 
IAMVideoProcAmp*    VPALeft = NULL;
IAMVideoProcAmp*    VPARight = NULL;
IBaseFilter					*g_pSrcFilterLeft=NULL; // Source Rendering filter
IBaseFilter					*g_pSrcFilterRight=NULL;// Source Rendering filter

HRESULT SetCC(IAMCameraControl* ctl, tagCameraControlProperty p, long n, bool AutoOff=true) {
	long Min, Max, Step, Default, Flags;
	HRESULT hr;
		// EXPOSURE: Get the range and default value. 
		hr = ctl->GetRange(p, &Min, &Max, &Step,
		&Default, &Flags);
		if( AutoOff ) {
			// Turn auto expose off.
			Flags &= ~CameraControl_Flags_Auto;
			Flags |= CameraControl_Flags_Manual;
		}
		else {
			// Turn auto expose on.
			Flags |= CameraControl_Flags_Auto;
			Flags &= ~CameraControl_Flags_Manual;
		}
		if (n > Min && n < Max) {
			hr = ctl->Set(p, n, Flags);
		} else {
			hr = ctl->Set(p, Default, Flags);
		}
		if (FAILED(hr)) return hr;
		return S_OK;
}

HRESULT SetVPA(IAMVideoProcAmp* ctl, tagVideoProcAmpProperty p, long n, bool AutoOff=true) {
	long Min, Max, Step, Default, Flags;
	HRESULT hr;
		// EXPOSURE: Get the range and default value. 
		hr = ctl->GetRange(p, &Min, &Max, &Step,
		&Default, &Flags);
		if( AutoOff ) {
			// Turn auto expose off.
			Flags &= ~VideoProcAmp_Flags_Auto;
			Flags |= VideoProcAmp_Flags_Manual;
		}
		else {
			// Turn auto expose on.
			Flags |= VideoProcAmp_Flags_Auto;
			Flags &= ~VideoProcAmp_Flags_Manual;
		}
		if (n > Min && n < Max) {
			hr = ctl->Set(p, n, Flags);
		} else {
			hr = ctl->Set(p, Default, Flags);
		}
		if (FAILED(hr)) return hr;
		return S_OK;
}

HRESULT SetCamera() {
	HRESULT hr;
	hr = SetVPA(VPALeft, VideoProcAmp_WhiteBalance, 0, false);
	hr = SetVPA(VPARight, VideoProcAmp_WhiteBalance, 0, false);
	
	hr = SetCC(g_CCLeft, CameraControl_Exposure, -4);
	if (FAILED(hr)) return hr;
	hr = SetCC(g_CCRight, CameraControl_Exposure, -4);
	if (FAILED(hr)) return hr;
	return hr;
}

HRESULT FindCaptureDevice(int eye, TCHAR *CameraFriendlyName, IBaseFilter ** ppSrcFilter)
{
HRESULT		hr;
IBaseFilter *pSrc = NULL;
CComPtr <IMoniker> pMoniker =NULL;
ULONG		cFetched;

    if (!ppSrcFilter)
        return E_POINTER;
    // Create the system device enumerator
    CComPtr <ICreateDevEnum> pDevEnum =NULL;
    hr = CoCreateInstance (CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC,
                           IID_ICreateDevEnum, (void **) &pDevEnum);
    if (FAILED(hr)) {
        return hr;
	    }
    // Create an enumerator for the video capture devices
    CComPtr <IEnumMoniker> pClassEnum = NULL;
    hr = pDevEnum->CreateClassEnumerator (CLSID_VideoInputDeviceCategory, &pClassEnum, 0);
    if (FAILED(hr)){
        return hr;
	    }
    // If there are no enumerators for the requested type, then 
    // CreateClassEnumerator will succeed, but pClassEnum will be NULL.
    if (pClassEnum == NULL) {
        return E_FAIL;
	    }
    // Note if the Next() call succeeds but there are no monikers,
    // it will return S_FALSE (which is not a failure).  Therefore, we
    // check that the return code is S_OK instead of using SUCCEEDED() macro.
	// Possible future camera id procedure...
	int		DeviceCount=0;
	bool	DeviceFound=false;
	while( S_OK == (pClassEnum->Next(1, &pMoniker, &cFetched) ) ) {
		IPropertyBag *pPropBag;
		hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
		if (SUCCEEDED(hr)){
			VARIANT varName;
			VariantInit(&varName);
			hr = pPropBag->Read(L"FriendlyName", &varName, 0);
			if (SUCCEEDED(hr)){
				TCHAR	DeviceName[MAX_PATH];
				WideCharToMultiByte(CP_ACP, 0, varName.bstrVal, -1, (LPSTR)DeviceName, MAX_PATH,  NULL, NULL);
				if( (strlen(CameraFriendlyName) == 0) || // Take 1st 2 capture devices.
					(strstr((char*)DeviceName,CameraFriendlyName)!=0) ) {
					if( eye == 0 ) {
						DeviceFound=true;
						break; 
						}
					else
					if( ++DeviceCount > 1 ) {
						DeviceFound=true;
						break; // Found second capture device.
						}
					}
				}
			}
		// Prepare to Grab next moniker...
		pMoniker = NULL;
		}
	// If we found our capture device...bind to it.
	if( DeviceFound ) {
		// Bind Moniker to a filter object
		hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)&pSrc);
		if (FAILED(hr)){
			return hr;
			}
		}
	else {
		return E_FAIL;
	}
    // Copy the found filter pointer to the output parameter.
    // Do NOT Release() the reference, since it will still be used
    // by the calling function.
    *ppSrcFilter = pSrc;
    return hr;
}


HRESULT Setup() {
	CoInitialize( NULL );
	HRESULT hr = FindCaptureDevice(0, "Vimicro", &g_pSrcFilterLeft);
	if (FAILED(hr)) return hr;
	hr = FindCaptureDevice(1, "Vimicro", &g_pSrcFilterRight);
	if (FAILED(hr)) return hr;
	g_pSrcFilterLeft->QueryInterface(IID_IAMCameraControl,(void**)&g_CCLeft);
	g_pSrcFilterRight->QueryInterface(IID_IAMCameraControl,(void**)&g_CCRight);
	g_pSrcFilterLeft->QueryInterface(IID_IAMVideoProcAmp,(void**)&VPALeft);
	g_pSrcFilterRight->QueryInterface(IID_IAMVideoProcAmp,(void**)&VPARight);
	return S_OK;
}

void CleanupCOM() {
	SAFE_RELEASE( g_CCLeft );
	SAFE_RELEASE( g_CCRight );
	SAFE_RELEASE(VPALeft);
	SAFE_RELEASE(VPARight);
	SAFE_RELEASE(g_pSrcFilterLeft);
    SAFE_RELEASE(g_pSrcFilterRight);
}