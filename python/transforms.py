import cv2
import numpy as np
import virtualvideo

from modeReader import read_mode

class TransformVideoSource(virtualvideo.VideoSource):
    def __init__(self):
        self.cap = cv2.VideoCapture(0)
        _, first_frame = self.cap.read()
        self.img = first_frame
        size = first_frame.shape
        print(f"Initiated with {size=}")
        self._size = (size[1], size[0])

    def img_size(self):
        return self._size

    def fps(self):
        return 10

    @staticmethod
    def transform(self, img) -> np.ndarray:
        raise NotImplementedError

    def generator(self):
        while True:
            #processes the image a little bit
            ret, frame = self.cap.read()
            img = self.transform(frame)
            yield img


class NoTransform(TransformVideoSource):
    def transform(img):
        return img


class CannyTransform(TransformVideoSource):
    def transform(img) -> np.ndarray:
        img = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
        canny = cv2.Canny(img, 100, 200)
        return cv2.cvtColor(canny, cv2.COLOR_GRAY2RGB)


class ContoursTransform(TransformVideoSource):
    def transform(img) -> np.ndarray:
        img = cv2.cvtColor(img, cv2.COLOR_RGB2GRAY)
        canny = cv2.Canny(img, 50, 200)
        res = cv2.addWeighted(img, 0.5, canny, 0.5, 0)
        return cv2.cvtColor(res, cv2.COLOR_GRAY2BGR)

class MatrixTransform(TransformVideoSource):
    CHARS = list('    .,:;oap!i1I)(][}{OQXY#MW&8B')
    def img_size(self):
        return (*self._size, 3)

    def transform(src, scale=10):
        img = cv2.cvtColor(src, cv2.COLOR_BGR2GRAY)
        height, width = img.shape
        w, h = width//10, height//10
        # print(w, h, src)
        reduced = cv2.resize(img, (w, h))
        reduced = np.uint8(len(MatrixTransform.CHARS) / 256 * reduced)

        res = np.zeros(src.shape, dtype=np.uint8)
        for i in range(h):
            for j in range(w):
                char = MatrixTransform.CHARS[reduced[i, j]]
                pos = (j * scale, i * scale)
                cv2.putText(res, char, pos, cv2.FONT_HERSHEY_PLAIN, 1, (0, 255, 0), 1)

        return res

class VideoLoop(virtualvideo.VideoSource):
    def __init__(self, path):
        cap = cv2.VideoCapture(path)
        self.cap = cap
        self.n_frames = int(cap.get(cv2.CAP_PROP_FRAME_COUNT))
        print(f'{self.n_frames=}')
        width = int(cap.get(cv2.CAP_PROP_FRAME_WIDTH))
        height = int(cap.get(cv2.CAP_PROP_FRAME_HEIGHT))
        self._size = (width, height)
        print(f'{self._size=}')

    def img_size(self):
        return self._size

    def fps(self):
        return 10

    def generator(self):
        while True:
            for i in range(self.n_frames):
                ret, frame = self.cap.read()
                yield frame
            self.cap.set(cv2.CAP_PROP_POS_FRAMES, 0)



MODES_TO_TRANSFORM = {
        0: NoTransform,
        1: NoTransform,
        2: CannyTransform,
        3: ContoursTransform,
        4: MatrixTransform
}

class ModeChooser(virtualvideo.VideoSource):
    FREQ_UPDATE = 1  # Update every second


    def __init__(self, modes_to_transform=MODES_TO_TRANSFORM):
        self.modes = modes_to_transform
        self._mode = read_mode()
        self.cap = cv2.VideoCapture(0)
        _, first_frame = self.cap.read()
        self.img = first_frame
        size = first_frame.shape
        print(f"Initiated with {size=} and {self._mode}")
        self._size = (size[1], size[0], 3)

    def img_size(self):
        return self._size

    def fps(self):
        return 10

    def transform(self, img) -> np.ndarray:
        trans = self.modes[self._mode]
        return trans.transform(img)

    def generator(self):
        i = 0
        while True:
            #processes the image a little bit
            ret, frame = self.cap.read()
            img = self.transform(frame)
            yield img
            i += 1
            if i % (self.fps() * self.FREQ_UPDATE) == 0:
                # Update mode every second
                self._mode = read_mode()
                # print(f"{self._mode=}")


