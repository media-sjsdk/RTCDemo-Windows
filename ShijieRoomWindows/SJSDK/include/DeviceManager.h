#pragma once
#include <vector>
#include <string>
// #include "DeviceManager/AppDeviceManager.h"


class IDeviceManagerEventHandler{
public:
	IDeviceManagerEventHandler() {};
	~IDeviceManagerEventHandler() {};

	virtual void OnCameraStarted() {};
	virtual void OnCameraStoped() {};
	virtual void OnMicStartFailed() {};
	virtual void OnCameraStartFailed() {};
};

typedef enum{
	DEVICE_START_CAMERA_FAILED,
	DEVICE_CAMERA_CHANGE_MODE_FAILED,
	DEVICE_SET_CAMERA_FRAMERATE_FAILED,
	DEVICE_NO_CAMERA,
	DEVICE_SETUP_CAMERA_ERROR,
	DEVICE_FAILED_GET_VOICEPRO_STATE,
	DEVICE_START_MIC_FAILED,
	DEVICE_START_SPEAKER_FAILED,
	DEVICE_UNKNOWN_ERROR
} DeviceErrorType;

typedef enum {
	CAMERA,
	SCREEN
} VideoContentType;

class DeviceManagerObserverBridge;

class DeviceManager {
public:
	DeviceManager(IDeviceManagerEventHandler* cc);
	~DeviceManager();
	
	friend class DeviceManagerObserverBridge;

	bool setVideoOutputParam(int width, int height, int frameRate);
    bool setCameraParam(int width, int height, int frameRate);
    bool startCamera(bool start);
	bool switchCamera();
	bool muteSpeaker(bool mute);
	bool muteMicrophone(bool mute);
	void setSpeaker(bool enable);
	long getDeviceManagerInstance();
	//void enableRotation(bool enable);
	//void SetOrientation(int deviceOrientation, int UIOrientation);
    bool startSpeaker(bool start);
   
	void stopAudioDevice();
	bool startMicrophone(bool start);
    void setTargetVideo(int width, int height, int frameRate);
	//void startMixMusic(String path, bool loop);
	//long startPlayBackgroundMusic(String path, bool loop);
	//void stopPlayBackgroundMusic(long musicId);
	//void setAudioType(int audioType);
	//void playEffective(String path);
	
    //void pauseMixMusic();
    //void resumeMixMusic();
	
	//void enableMicMixMusic(bool enable);
	//void setBackgroundVolume(int volume);
	//void setLoopbackBackgroundVolume(int volume);
	//void setForegroundVolume(int volume);
	//void setVideoPreprocessParameters(float smoothLevel, float whiteLevel);
	//void setSmoothLevel(int smoothLevel);
    //int getSmoothLevel();
	//void setEyeZoomRatio(int eyeZoomRatio);
	//int getEyeZoomRatio();
	//void setFaceThinRatio(int faceThinRatio);
	//int getFaceThinRatio();
    //void setVideoFilter(String videoFilter);
    //void setVideoFilterIntensity(float videoFilterIntensity);
    //float getVideoFilterIntensity();
	//bool enableVideoPreprocess(bool enable);
	//bool  isSupportResolution(int width, int height, int framerate);
	//void setFocusPoint(float pointX, float pointY);
	//void setVideoZoomFactor(int zoomFactor);
	//bool isVideoZoomSupported();
	//int getSupportedVideoZoomMaxFactor();
	//int getCurrentVideoZoomFactor();
	//void setFlashState(bool state);
	//void startReverberation(int level);
	//void stopReverberation();
	//void stopMixMusic();
	//void EnableMicMixMusic(bool enable);
	//void loopbackAudio(bool enable);
	//bool addVideoPreprocessFilter(long filter, int filterType);
	//void removeVideoPreprocessFilter(long filter, int filterType);
	//bool setStickerPath(String path);
	//bool attachCallback(DeviceCallback cc);
	//bool disattachCallback();

private:
	IDeviceManagerEventHandler* handler_; 
};
