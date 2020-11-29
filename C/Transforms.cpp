#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <cmath>
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
    transform(frame);
    return frame.data;
}

void WebcamCameraOutput::transform(Mat &img){}

WebcamCameraOutput::~WebcamCameraOutput(){
    cap.release();
    frame.release();
}

CannyCameraOutput::CannyCameraOutput(int threshold1, int threshold2){
    th1 = threshold1;
    th2 = threshold2;
}

void CannyCameraOutput::transform(Mat &img){
    cvtColor(img, gray, COLOR_RGB2GRAY);
    Canny(gray, gray, th1, th2);
    cvtColor(gray, img, COLOR_GRAY2RGB);
}

CannyCameraOutput::~CannyCameraOutput(){
    gray.release();
}

MatrixCameraOutput::MatrixCameraOutput():
chars("    .,:;oap!i1I)(}{OQXY#MW&8B"),
downscale_factor(10)
{
    size = Size(WIDTH / downscale_factor, HEIGHT / downscale_factor);
}


void MatrixCameraOutput::transform(Mat& frame)
{
    Mat gray;
    cvtColor(frame, gray, COLOR_BGR2GRAY);
    resize(gray, gray, size);
    double ratio = ((double)chars.size()) / 255;
    gray = gray * ratio;
    frame = Mat::zeros(Size(WIDTH, HEIGHT), CV_8UC3);
    string c;
    uint8_t val;
    Scalar green = Scalar(0,255,0);
    cout.flush();
    for (int row=0; row<gray.rows; row++){
        for (int col=0; col<gray.cols; col++){
            val = gray.at<uint8_t>(row, col);
            c = chars[val];
            putText(frame, c, Point(col * downscale_factor, row * downscale_factor), 
                    FONT_HERSHEY_DUPLEX, 0.4, green, 1
            );
        }
    }
}

MatrixCameraOutput::~MatrixCameraOutput(){}


DeformationMirrorCameraOutput::DeformationMirrorCameraOutput(float p, int r = HEIGHT / 4, int cx = HEIGHT / 2, int cy = WIDTH / 2){
    cout << "Initiated with r=" << r << '\n';
    map_X = Mat(HEIGHT, WIDTH, CV_32FC1);
    map_Y = Mat(HEIGHT, WIDTH, CV_32FC1);
    int d;
    for (int i=0; i<HEIGHT; i++){
        for (int j=0; j<WIDTH; j++){
            d = (i - cx) * (i - cx) + (j - cy) * (j - cy);
            // cout << "d=" << d << " vs " << r*r << '\n';

            if (d < (r * r)){
                // cout << i << " vs " << sign<int>(j - cy) * pow(((double)abs(j - cy)) / r, p) * r + cy;
                map_X.at<float>(i, j) = sign<int>(j - cy) * pow(((double)abs(j - cy)) / r, p) * r + cy;
                map_Y.at<float>(i, j) = sign<int>(i - cx) * pow(((double)abs(i - cx)) / r, p) * r + cx;
            }
            else{
                map_X.at<float>(i, j) = j;
                map_Y.at<float>(i, j) = i;
            }
        }
    }
    // cout << "map_X" << map_X << '\n';
}

void DeformationMirrorCameraOutput::transform(Mat& frame){
    remap(frame, frame, map_X, map_Y, INTER_LINEAR);
}

DeformationMirrorCameraOutput::~DeformationMirrorCameraOutput(){}

StaticImgCameraOutput::StaticImgCameraOutput(const string &imgPath, int width, int height){
    size = Size(width, height);
    Mat imgInput = imread(imgPath, IMREAD_COLOR);
    resize(imgInput, img, size);
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
    resize(buffer, frame, size);
    i++;
    return frame.data;
}

StaticVideoCameraOutput::~StaticVideoCameraOutput(){
    video.release();
    buffer.release();
    frame.release();
}