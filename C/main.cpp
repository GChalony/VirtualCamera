#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <linux/videodev2.h>
#include <fcntl.h>
#include <unistd.h>
#include <iostream>
#include "Transforms.h"

using namespace std;

#define FPS 30

#define HEIGHT 480
#define WIDTH 640
#define BYTES WIDTH * HEIGHT * 3

int setupVirtualCam(string const &vid_out){
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

	format.fmt.pix.width = WIDTH;
	format.fmt.pix.height = HEIGHT;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_BGR24;
	format.fmt.pix.sizeimage = BYTES;
	format.fmt.pix.field = V4L2_FIELD_NONE;
	if (ioctl(dev_fd, VIDIOC_S_FMT, &format) == -1){
		cout << "cannot setup video device\n";
	}

    cout << "Video setup with (" << HEIGHT << ", " << WIDTH << ", " << 3 << ") of size\n";
    return dev_fd;
}

void loop(CameraOutput &input, int output){
    unsigned char *data;
	int written;

    for(int i=0; true; i++){
		usleep(1.0f / FPS * 1000000.0f);
        data = input.nextData();
        written = write(output, data, BYTES);
		if (written < 0) {
            cerr << "ERROR: could not write to output device! " << written << endl;
            close(output);
            break;
        }
        if (i % FPS == 0){
            //cout << "\r" << i / FPS << "s";
            cout.flush();
        }     
    }
}

int main(){
    StaticVideoCameraOutput in = StaticVideoCameraOutput("../data/record.webm", WIDTH, HEIGHT);

    int out = setupVirtualCam("/dev/video2");

    loop(in, out);
}