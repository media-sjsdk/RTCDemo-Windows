#pragma once
#include "Room.h"
#include "DeviceManager.h"
#include "RenderManager.h"
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <commdlg.h>
#include <map>

struct video_resulations{
	int width;
	int height;
	bool enableAdaptiveResolution;
};

static void TcharToChar(const TCHAR * tchar, char * _char)
{
	int iLength;

	iLength = WideCharToMultiByte(CP_ACP, 0, tchar, -1, NULL, 0, NULL, NULL);
	WideCharToMultiByte(CP_ACP, 0, tchar, -1, _char, iLength, NULL, NULL);
}

static WCHAR * charToWchar(const char *s)
{
	int w_nlen = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
	WCHAR *ret;
	ret = (WCHAR *)malloc(sizeof(WCHAR) * w_nlen);
	memset(ret, 0, sizeof(ret));
	MultiByteToWideChar(CP_ACP, 0, s, -1, ret, w_nlen);
	return ret;
}

class DeviceManagerCallback : public IDeviceManagerEventHandler
{
public:
	virtual void OnCameraStarted();
	virtual void OnCameraStoped();
	virtual void OnMicStartFailed();
	virtual void OnCameraStartFailed();
    void setlocalWindow(HWND hwnd);
	void setRenderManger(WindowsRenderManager* rendermanager);
public:
    void* localRender;
    HWND hVideoWinLocal;
    WindowsRenderManager* render_manger;

};

class RoomCallback : public shijie::IRoomEventHandler {
public:

	virtual void onRemoteUserJoined(const char* uid);

	virtual void onRemoteUserLeaved(const char* uid);
	virtual void onError(const char* format, int errorType);
	virtual void onRemoteVidStreamCreated(const char* uid, const char* streamId); //notify that remote video stream is coming
	virtual void onRemoteVidStreamRemoved(const char* uid, const char* streamId); //notify that remote video stream has stopped
	virtual void onRemoteVidResize(const char* uid, int width, int height);  //notify that frame size has been changed
	virtual void onLocalUserLeaved(int errorNo);

	virtual void onLocalUserJoined();

	virtual void onGetFirstAudioSample();
	virtual void onGetFirstVideoSample(const char* uid);
    virtual ~RoomCallback();


public:
    void setWindowsparentHWNDforRender(HWND hwnd,HINSTANCE hinst, TCHAR* szTile, TCHAR* Windowsclassname);
	void setWindowsForRender(std::vector<HWND> hWindows);
    void setRenderManger(WindowsRenderManager* rendermanager);
	void setlocalWindow(HWND hwnd);
	void setcurrentRole(shijie::Win32ClientRole currentrole);
	void ClearWindowsAndRenders();
    WindowsRenderManager* render_manger;
    std::map<std::string, HWND> remote_windows_uid;
    std::map<std::string, void*> remote_render_uid;
	std::map<HWND, bool> windows_bind_render;
    HWND hwnd_parent;
    HINSTANCE hinst_parent;
    TCHAR* szTile;
	TCHAR* Windowname;
	void * localRender;
	HWND hVideoWinLocal;
	shijie::Win32ClientRole current_role;
	std::string local_id;
};
