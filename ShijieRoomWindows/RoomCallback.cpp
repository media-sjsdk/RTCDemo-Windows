#pragma once
#include "stdafx.h"
#include "RoomCallback.h"
#include "Room.h"
#include "devicemanager.h"
#include "RenderManager.h"
#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>
#include <commdlg.h>
#include<iostream>

using namespace shijie;

void DeviceManagerCallback::setlocalWindow(HWND hwnd)
{
    hVideoWinLocal = hwnd;
}

void DeviceManagerCallback::setRenderManger(WindowsRenderManager* rendermanager)
{
     render_manger = rendermanager;
}

void DeviceManagerCallback::OnCameraStarted()
{
	std::string id = "";
	ShowWindow(hVideoWinLocal, SW_SHOW);
	localRender = render_manger->createRender(hVideoWinLocal, 640, 480);
	render_manger->bindRenderWithStream(localRender, id, false);

}

void DeviceManagerCallback::OnCameraStoped()
{
	std::string id = "";
	render_manger->unbindRenderWithStream(localRender, id);
	render_manger->destroyRender(localRender);
	ShowWindow(hVideoWinLocal, SW_HIDE);
	localRender = NULL;

}

void DeviceManagerCallback::OnMicStartFailed()
{

}

void DeviceManagerCallback::OnCameraStartFailed()
{

}

void RoomCallback::onRemoteUserJoined(const char* uid)
{

}

void RoomCallback::onRemoteUserLeaved(const char* uid)
{

}

void RoomCallback::onError(const char* format, int errorType)
{

}

void RoomCallback::onRemoteVidStreamCreated(const char* uid, const char* streamId) //notify that remote video stream is coming
{


}

void RoomCallback::onRemoteVidStreamRemoved(const char* uid, const char* streamId) //notify that remote video stream has stopped
{
	std::string id(uid);
	if(current_role == CLIENT_ROLE_VIEWER){
		if(localRender){
			render_manger->unbindRenderWithStream(localRender, id);
			render_manger->destroyRender(localRender);
			ShowWindow(hVideoWinLocal, SW_HIDE);
			localRender = NULL;
		}
		return;
	}
	auto it = remote_windows_uid.find(id);
	if (it != remote_windows_uid.end()) {
		ShowWindow(it->second,SW_HIDE);
		windows_bind_render[it->second] = false;
		remote_windows_uid.erase(id);
		auto render_it = remote_render_uid.find(id);
		if(render_it != remote_render_uid.end()) {
			render_manger->unbindRenderWithStream(render_it->second, id);
			render_manger->destroyRender(render_it->second);
			render_it->second = NULL;
			remote_render_uid.erase(id);
		}
	}
}

void RoomCallback::onRemoteVidResize(const char* uid, int width, int height)  //notify that frame size has been changed
{

}

void RoomCallback::onLocalUserLeaved(int errorNo)
{

}

void RoomCallback::onLocalUserJoined()
{

}

void RoomCallback::onGetFirstAudioSample()
{

}

void RoomCallback::onGetFirstVideoSample(const char* uid)
{
	std::string id(uid);
	if(current_role == CLIENT_ROLE_VIEWER){
		ShowWindow(hVideoWinLocal, SW_SHOW);
		local_id = id;
		localRender = render_manger->createRender(hVideoWinLocal, 640, 480);
		render_manger->bindRenderWithStream(localRender, id, false);
		return;
	}
	WCHAR* remotename = charToWchar(uid);
	for (auto it = windows_bind_render.begin(); it != windows_bind_render.end(); ++it){
		if (it->second == false) {
			HWND hwnd = it->first;
			SetWindowText(hwnd,remotename);
            remote_windows_uid.insert(std::make_pair(id, hwnd));
			void* remoteRender = render_manger->createRender(hwnd, 640, 360);
			render_manger->bindRenderWithStream(remoteRender, id, false);
			remote_render_uid.insert(std::make_pair(id, remoteRender));
			it->second = true;
			ShowWindow(hwnd, SW_SHOW);
			break;
		}
	}
	free(remotename);

}
void RoomCallback::setWindowsparentHWNDforRender(HWND hwnd,HINSTANCE hinst, TCHAR* szTile, TCHAR* Windowsclassname){
	 hwnd_parent = hwnd;
     hinst_parent = hinst;
	 szTile = szTile;
	 Windowname = Windowsclassname;
}

void RoomCallback::setRenderManger(WindowsRenderManager* rendermanager)
{
     render_manger = rendermanager;
}

void RoomCallback::setlocalWindow(HWND hwnd)
{
	hVideoWinLocal = hwnd;
}

void RoomCallback::ClearWindowsAndRenders(){
	for(auto it = remote_windows_uid.begin(); it != remote_windows_uid.end(); it++) {
		windows_bind_render[it->second] = false;
		ShowWindow(it->second,SW_HIDE);
		auto render_it = remote_render_uid.find(it->first);
		if(render_it != remote_render_uid.end()) {
			render_manger->unbindRenderWithStream(render_it->second, it->first);
			render_manger->destroyRender(render_it->second);
			render_it->second = NULL;
		}
	}
	remote_windows_uid.clear();
	remote_render_uid.clear();
	if(localRender != NULL) {
		ShowWindow(hVideoWinLocal, SW_HIDE);
		render_manger->unbindRenderWithStream(localRender, local_id);
		render_manger->destroyRender(localRender);
		localRender = NULL;
		local_id = "";
	}
}
void RoomCallback::setWindowsForRender(std::vector<HWND> hWindows){
	 windows_bind_render.clear();
	 for(int i =0; i < hWindows.size(); i++ ){
		 if(hWindows[i] != 0) {
			windows_bind_render.insert(std::make_pair(hWindows[i],false));
		 }
		 
	 }
}

void RoomCallback::setcurrentRole(shijie::Win32ClientRole currentrole)
{
	current_role = currentrole;
}

RoomCallback::~RoomCallback(){
	for(auto it = windows_bind_render.begin(); it != windows_bind_render.end(); it++){
			DestroyWindow(it->first);	
	}
	windows_bind_render.clear();
}
