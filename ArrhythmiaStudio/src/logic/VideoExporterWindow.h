#pragma once

#include <string>

class VideoExporterWindow {
public:
    bool isOpen = false;

    VideoExporterWindow();

    void onLayout();
private:
    std::string path = "MyVideo.mkv";
    int framerate = 60;
    int width = 1920;
    int height = 1080;
    float start = 0;
    float end = 5;
};
