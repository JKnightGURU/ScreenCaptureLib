#include "xshmgrabber.h"

#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <opencv2/core.hpp>
#include <iostream>
#include <sstream>
#include <iterator>

#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <regex>

std::string exec(const char* cmd)
{
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(& pclose)> pipe(popen(cmd, "r"), pclose);

    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }
    return result;
}

XShmGrabber::XShmGrabber() :
    is_stop_requested_(std::make_shared<std::atomic<bool>>(false))
{
    XInitThreads();
}

XShmGrabber::~XShmGrabber()
{
    *is_stop_requested_ = true;
    grab_thread_.join();
}

void XShmGrabber::init(GrabberCallback callback, int grabbing_fps)
{
    this->callback_     = callback;
    *is_stop_requested_ = false;

    std::vector<MonitorDesc> descs = monitors();

    for (size_t i = 0; i < descs.size(); i++)
        monitors_.push_back(std::make_shared<XShmScreenshot>(descs[i].offset_x_, descs[i].offset_y_, descs[i].width_, descs[i].height_));

    grab_thread_ = std::thread([grabbing_fps, this]() {
                                   for (size_t i = 0; i < monitors_.size(); i++)
                                       monitors_[i]->Init();

                                   while (!*is_stop_requested_)
                                   {
                                       std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();

                                       for (size_t i = 0; i < monitors_.size(); i++)
                                       {
                                           cv::Mat img;
                                           monitors_[i]->operator()(img);
                                           callback_(img, static_cast<int>(i));
                                       }
                                       std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

                                       int diff = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();

                                       std::this_thread::sleep_for(std::chrono::milliseconds(1000 / grabbing_fps - diff));
                                   }
                               });
}

std::vector<MonitorDesc> XShmGrabber::monitors()
{
    std::string result = exec("xrandr | grep \\ connected");//  \  | sed 's/[a-wy-ZA-WY-Z]*//g' | sed 's/[^0-9]/ /g'");

    size_t position = 0;

    std::vector<std::string> descriptions;

    while ((position = result.find('\n', position + 1)) != std::string::npos)
    {
        descriptions.push_back(result.substr(0, position));
        result = result.substr(position + 1);
    }

    descriptions.push_back(result);
    std::vector<MonitorDesc> descs;

    for (size_t i = 0; i < descriptions.size(); i++)
    {
        std::smatch match;

        std::regex reg("[0-9]+x[0-9]+\\+[0-9]+\\+[0-9]+");

        if (std::regex_search(descriptions[i], match, reg))
        {
            std::regex  replace("[x|+]");
            std::string replaced;

            replaced = std::regex_replace(match.str(), replace, " ");

            std::istringstream iss(replaced);
            std::vector<std::string> tokens;
            std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(tokens));

            const int TOKEN_COUNT = 4;

            if (tokens.size() != TOKEN_COUNT)
                continue;

            const int TOKEN_WIDTH_IND  = 0;
            const int TOKEN_HEIGHT_IND = 1;
            const int TOKEN_OFFSET_X   = 2;
            const int TOKEN_OFFSET_Y   = 3;

            MonitorDesc desc;
            desc.index_    = i;
            desc.width_    = stoi(tokens[TOKEN_WIDTH_IND]);
            desc.height_   = stoi(tokens[TOKEN_HEIGHT_IND]);
            desc.offset_x_ = stoi(tokens[TOKEN_OFFSET_X]);
            desc.offset_y_ = stoi(tokens[TOKEN_OFFSET_Y]);

            descs.push_back(desc);
        }
    }
    return descs;
}

XShmScreenshot::XShmScreenshot(int x, int y, int width, int height) :
    x_(x),
    y_(y),
    width_(width),
    height_(height)
{}

void XShmScreenshot::Init()
{
    this->display_ = XOpenDisplay(nullptr);
    root_          = DefaultRootWindow(display_);

    XGetWindowAttributes(display_, root_, &window_attributes_);
    screen_ = window_attributes_.screen;
    ximg_   = XShmCreateImage(display_,
                              DefaultVisualOfScreen(screen_),
                              DefaultDepthOfScreen(screen_),
                              ZPixmap,
                              nullptr,
                              &shminfo_,
                              width_,
                              height_);

    shminfo_.shmid    = shmget(IPC_PRIVATE, ximg_->bytes_per_line * ximg_->height, IPC_CREAT | 0777);
    shminfo_.shmaddr  = ximg_->data = (char*)shmat(shminfo_.shmid, 0, 0);
    shminfo_.readOnly = False;

    if (shminfo_.shmid < 0)
        puts("Fatal shminfo error!");
    ;
    XShmAttach(display_, &shminfo_);

    init_ = true;
}

void XShmScreenshot::operator()(cv::Mat& cv_img)
{
    if (init_)
        init_ = false;

    XLockDisplay(display_);
    XShmGetImage(display_, root_, ximg_, x_, y_, 0x00ffffff);
    XUnlockDisplay(display_);

    cv_img = cv::Mat(height_, width_, CV_8UC4, ximg_->data);
}

XShmScreenshot::~XShmScreenshot()
{
    if (!init_)
        XDestroyImage(ximg_);

    XShmDetach(display_, &shminfo_);
    shmdt(shminfo_.shmaddr);
    XCloseDisplay(display_);
}
