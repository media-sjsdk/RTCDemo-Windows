#pragma once
#include <string>
class WindowsRenderManager {
public:

    WindowsRenderManager();
    virtual ~WindowsRenderManager();

    void* createRender(void* window, int displayWidth, int displayHeight);
    void destroyRender(void * render);
    void* getRender(std::string streamId);
    bool bindRenderWithStream(void* render, std::string streamId, bool disableLipSync);
    bool unbindRenderWithStream(void* render, std::string streamId);

private:
};
