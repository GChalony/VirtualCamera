#include <string.h>
#include <opencv2/videoio.hpp>

class CameraOutput
{
    public:
        virtual uchar *nextData();
};

class WebcamCameraOutput: public CameraOutput
{
    public:
        WebcamCameraOutput();
        uchar *nextData();
        void transform();
        ~WebcamCameraOutput();
    
    private:
        cv::VideoCapture cap;
        cv::Mat frame;
};

class StaticImgCameraOutput: public CameraOutput
{
    public:
        StaticImgCameraOutput(const std::string &imgPath, int width, int height);
        uchar *nextData();
        ~StaticImgCameraOutput();
    
    private:
        cv::Size size;
        cv::Mat img;
};

class StaticVideoCameraOutput: public CameraOutput
{
    public: 
        StaticVideoCameraOutput(const std::string &videoPath, int width, int height);
        uchar *nextData();
        ~StaticVideoCameraOutput();

    private:
        cv::Size size;
        int i;
        cv::VideoCapture video;
        cv::Mat buffer, frame;
        int n_frames;
};

