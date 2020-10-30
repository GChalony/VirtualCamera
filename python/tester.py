import cv2

from transforms import CannyTransform, ContoursTransform, MatrixTransform

TRANSFORM = MatrixTransform
trans = TRANSFORM()


while True:
    # Capture frame-by-frame
    ret, frame = trans.cap.read()

    # Our operations on the frame come here
    res = trans.transform(frame)

    # Display the resulting frame
    cv2.imshow('frame', res)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# When everything done, release the capture
trans.cap.release()
cv2.destroyAllWindows()
