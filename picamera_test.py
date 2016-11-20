import picamera
import time

camera = picamera.PiCamera()
#camera.hflip = True    #convert left/right
#camera.vflip = True    #convert up/down
camera.resolution = (1280,720)

print('camera capture : start')

#capture 10 images for 20 seconds
for i in range(10):
    camera.capture('images/image{0:04d}.jpg'.format(i))
    time.sleep(2)

print('camera capture : end')
