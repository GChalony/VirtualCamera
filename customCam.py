import virtualvideo

import cv2

from transforms import ContoursTransform, VideoLoop, MatrixTransform

vidsrc = MatrixTransform()
fvd = virtualvideo.FakeVideoDevice()
fvd.init_input(vidsrc)
fvd.init_output(2, 1280, 720, fps=30)
fvd.run()
