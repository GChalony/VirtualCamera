#include <string.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include "Transforms.h"

using namespace cv;
using namespace std;

uchar *CameraOutput::nextData(){
    return NULL;
}

WebcamCameraOutput::WebcamCameraOutput(){
    cap.open(0, CAP_ANY);

    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
    }

}
uchar *WebcamCameraOutput::nextData(){
    cap.read(frame);
    
    if (frame.empty()) {
        cerr << "ERROR! blank frame grabbed\n";
        return NULL;
    }
    return frame.data;
}

WebcamCameraOutput::~WebcamCameraOutput(){
    cap.release();
    frame.release();
}

StaticImgCameraOutput::StaticImgCameraOutput(const string &imgPath, int width, int height){
    size = Size(width, height);
    Mat imgInput = cv::imread(imgPath, cv::IMREAD_COLOR);
    cv::resize(imgInput, img, size);
}

uchar *StaticImgCameraOutput::nextData(){
    return img.data;
}

StaticImgCameraOutput::~StaticImgCameraOutput(){
    img.release();
}

StaticVideoCameraOutput::StaticVideoCameraOutput(const string &videoPath, int width, int height){
    size = Size(width, height);
    i = 0;
    video.open(videoPath);
    if (!video.isOpened()) {
        cerr << "ERROR! Unable to open camera\n";
    }

    n_frames = video.get(CAP_PROP_FRAME_COUNT);
    cout << "Number of frames: " << n_frames << endl;
}

uchar *StaticVideoCameraOutput::nextData(){
    cout << "\r" << i;
    cout.flush();
    if (i >= n_frames-10){
        cout << "Looped!\n";
        video.set(CAP_PROP_POS_FRAMES, 0);
        i = 0;
    }
    video.read(buffer);
    if (buffer.empty()) {
        cerr << "ERROR! blank frame grabbed\n";
        return NULL;
    }
    cv::resize(buffer, frame, size);
    i++;
    return frame.data;
}

StaticVideoCameraOutput::~StaticVideoCameraOutput(){
    video.release();
    buffer.release();
    frame.release();
}