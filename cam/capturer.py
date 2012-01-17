#!/usr/bin/env python
import Image
import sys
import random
import string
import os
import socket
import time
import opencv
from opencv import highgui 

def get_image(camera):
    img = highgui.cvQueryFrame(camera)
    #im = opencv.cvGetMat(im)
    return opencv.adaptors.Ipl2PIL(img) 

def main(argv):
    # Seconds between images
    delay = 3
    
    # Connection info for server
    if len(argv) == 0:
        conn_info = (argv[1], 8888)
    else:
        conn_info = ('127.0.0.1', 8888)
    
    # Generate an ID for ourselves
    id = ''.join(random.choice(string.ascii_uppercase) for x in range(2))
    img_path = os.path.join('/tmp', 'temp.jpg')
    print 'Capturer', id, 'sending to', conn_info
    
    # Open camera and start taking pictures
    camera = highgui.cvCreateCameraCapture(0)
    while True:
        # Get image and save as JPG
        img = get_image(camera)
        img.save(img_path, 'JPEG')
        
        # Send to server
        try:
            sock = socket.create_connection(conn_info, timeout=1)
            
            sock.send(id)
            with open(img_path, 'rb') as f:
                sock.send(f.read())
            
            sock.close()
        except socket.error as e:
            # Try again later, assume that the server is just down
            # but that the user gave us the right address
            print(e)
            
        # Don't overload ourselves here
        time.sleep(delay)
   
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))

