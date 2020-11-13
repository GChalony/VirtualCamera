# Virtual Camera project

This project aims at creating a fake camera device, which can be used by any application like Zoom, Skype etc... but _can be fed any data_. 

I have mostly two ideas in mind:
- replacing my camera with any video, either just of fun, or even to play a recording of me, so that I can leave my desk and fake still being here :wink:
- add Snapchat-like filters


## How it works

The base concept is a Linux kernel module named `v4l2loopback` which lets you create a virtual video device which can take data from other processes. 

I would eventually like to be able to use this module's API directly, but for now the easiest way was to use a python module named `virtualvideo`, which uses `ffmpeg` to feed the video stream to the kernel.

## Using it

There are several transforms defined in `transform.py` which transform the webcam image to whatever they want. The `mode.txt` file enables you to change the transform at runtime, by simply changing the first number. Run with:
```python
python3 customCam.py
```
 
## Reference

[StackOverflow thread listing solutions to view /dev/video2 output](https://superuser.com/questions/1330959/what-is-the-right-ffmpeg-output-format-for-a-v4l2-loopback-device).