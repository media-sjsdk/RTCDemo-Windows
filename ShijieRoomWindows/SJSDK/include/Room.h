#pragma once
#include <vector>
#include <string>

namespace xplatform_util
{
    class ThreadAllocator;
}
class ConferenceClient;
namespace shijie{

enum Win32ClientRole {
    CLIENT_ROLE_PROXY,
    CLIENT_ROLE_COHOST,
    CLIENT_ROLE_VIEWER,
    CLIENT_ROLE_ATTENDEE
};
struct Win32RoomProfile {
    bool joinWithoutVideo;
    Win32ClientRole role;
    bool startMixer;
    int videoWidth;
    int videoHeight;
    bool enableAdaptiveResolution; 
    std::string serverAddress;
};

class IRoomEventHandler {
public:
	IRoomEventHandler() {};
	~IRoomEventHandler() {};
	virtual void onRemoteUserJoined(const char* uid) {}

	virtual void onRemoteUserLeaved(const char* uid) {}

	virtual void onError(const char* format, int errorType) {}

	virtual void onRemoteAudioStreamCreated(const char* uid) {}

	virtual void onRemoteAudioStreamRemoved(const char* uid) {}

	virtual void onRemoteVidStreamCreated(const char* uid, const char* streamId) {} 

	virtual void onRemoteVidStreamRemoved(const char* uid, const char* streamId) {} 

	virtual void onRemoteVidResize(const char* uid, int width, int height) {} 

	virtual void onLocalUserLeaved(int errorNo) {}

	virtual void onLocalUserJoined() {}

	virtual void onGetFirstAudioSample() {}

	virtual void onGetFirstVideoSample(const char* uid) {}

	virtual void onSpeakingStatusChanged(const char* uid, bool isSpeaking) {}
	
	virtual void onUsersVolumeChanged(std::vector<std::string> participantsName, std::vector<int> participantsVolume) {}

	virtual void onPublisherError(std::string serviceErrorType, std::string subErrorType, std::string urlId) {}

	virtual void OnVideoChannelNotWriteable(std::string channel_id) {}

	virtual void OnAudioChannelNotWriteable(std::string channel_id) {}

	virtual void OnPublisherSessionStarted(std::string urlId) {}

	virtual void OnPublisherSessionStoped(std::string urlId) {}

	virtual void onRemoteSourceAdded(std::string uid, uint32_t streamId) {}

	virtual void onRemoteSourceRemoved(std::string uid, uint32_t streamId) {}
};

typedef enum{
	KShowIfNotStaticallyViewed,
	KShowEvenIfStaticallyViewed
} DynamicViewPolicy;

class RoomObserverBridge;
class ConferenceObserverBridge;

class Room {
public:
	Room(long deviceManager, std::string uid, int appId, std::string AppName,IRoomEventHandler* cc);
	~Room();
	
	friend class RoomObserverBridge;

	bool joinRoom(std::string roomid ,std::string localName, Win32RoomProfile roomProfile, std::string key);
	void leaveRoom();

	bool muteMicrophone();
	bool unMuteMicrophone();
	bool muteVideo();
	bool unMuteVideo();
	bool startVideo();
	bool stopVideo();
	static std::string getEngineVersion();
	int setEncoderMaxBitRate(int bitrate);
	static int getEngineVersionInt();
	void setResolution(int Width, int Height, int framerate);
	void setUserPlayoutVolume(std::string userId, double volume);
	void muteUserAudio(std::string userId, bool mute);
	int getUserPlayoutVolume(std::string userId);

private:
	long deviceManager_;
	xplatform_util::ThreadAllocator* m_threadAllocater;

    // using raw ptr to avoid include the scoped_ptr for API header file.
    ConferenceClient* client_;
    ConferenceObserverBridge* conferenceObserverBridge_;
};
}
