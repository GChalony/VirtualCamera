#ifndef TRANSFORM
#define TRANSFORM
#include <string.h>
#include <opencv2/videoio.hpp>

// Camera resolution
#define HEIGHT 480 
#define WIDTH 640
#define BYTES WIDTH * HEIGHT * 3

class CameraOutput
{
    public:
        virtual uchar *nextData();
        virtual ~CameraOutput() {};
};

class WebcamCameraOutput: public CameraOutput
{
    public:
        WebcamCameraOutput();
        uchar *nextData();
        /* Transform Webcam input inplace.*/
        virtual void transform(cv::Mat &img);
        virtual ~WebcamCameraOutput();
    
    private:
        cv::VideoCapture cap;
        cv::Mat frame;
};

class CannyCameraOutput: public WebcamCameraOutput
{
    public:
        CannyCameraOutput(int, int);
        void transform(cv::Mat&);
        ~CannyCameraOutput();
        
    private:
        cv::Mat gray;
        int th1, th2;
};


class MatrixCameraOutput: public WebcamCameraOutput
{
    public:
        MatrixCameraOutput();
        void transform(cv::Mat&);
        ~MatrixCameraOutput();
    
    private:
        std::string chars;
        int downscale_factor;
        cv::Size size;
};

template <typename T> int sign(T val) {
    return (T(0) < val) - (val < T(0));
}

class DeformationMirrorCameraOutput: public WebcamCameraOutput
{
    public:
        DeformationMirrorCameraOutput(float p, int r, int cx, int cy);
        void transform(cv::Mat&);
        ~DeformationMirrorCameraOutput();
    
    private:
        cv::Mat map_X, map_Y;
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

#endif