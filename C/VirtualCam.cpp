#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>

#define IMAGE_PATH "../data/fish.jpg"
#define VIDEO_OUT "/dev/video2"
#define FPS 30

using namespace std;


int setupVirtualCam(string const &vid_out, cv::Mat &img, int &bytes){
    int width = img.cols, height = img.rows;
    bytes = 3 * height * width;

    struct v4l2_format format;


	int dev_fd = open(vid_out.c_str(), O_RDWR);
	if (dev_fd == -1) {
		cout << "cannot open video device\n";
        exit(-1);
	}
	format.type = V4L2_BUF_TYPE_VIDEO_OUTPUT;
	if (ioctl(dev_fd, VIDIOC_G_FMT, &format) == -1){
		cout << "cannot setup video device\n";
        exit(-1);
	}

	format.fmt.pix.width = width;
	format.fmt.pix.height = height;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
	format.fmt.pix.sizeimage = bytes;
	format.fmt.pix.field = V4L2_FIELD_NONE;
	if (ioctl(dev_fd, VIDIOC_S_FMT, &format) == -1){
		cout << "cannot setup video device\n";
	}


    cout << "Video setup with (" << height << ", " << width << ", " << img.channels() << ") of size\n";
    cout << "Size of buffer: " << bytes << endl;
    return dev_fd;
}

void run(int dev_out, cv::Mat &img, int &bytes){
    cout << "Running with bytes=" << bytes << endl;
    int count = 0;
    while (1){
        usleep(1.0f / FPS * 1000000.0f);
        size_t written = write(dev_out, img.data, bytes);
        if (written < 0) {
            cerr << "ERROR: could not write to output device!\n";
            close(dev_out);
            break;
        }
        count++;
        if (count % FPS == 0){
            cout << "\r" << count / FPS << "s";
            cout.flush();
        }
            
    }
}


int main(){
    cout << "Starting !\n";
    cv::Mat img = cv::imread(IMAGE_PATH, cv::IMREAD_COLOR);
    if (img.empty()){
        std::cout << "Image is empty\n";
        exit(-1);
    }
    string const vid = VIDEO_OUT;
    int bytes = 0;
    int dev_out = setupVirtualCam(vid, img, bytes);
    run(dev_out, img, bytes);
}