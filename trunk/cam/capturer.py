#!/usr/bin/env python
import Image
import sys
import random
import string
import os
import socket
import time
import opencv
import argparse
from opencv import highgui 

def get_image(camera):
    img = highgui.cvQueryFrame(camera)
    #im = opencv.cvGetMat(im)
    return opencv.adaptors.Ipl2PIL(img) 

def main(argv):
    parser = argparse.ArgumentParser(description='Sends images from camera to server')
    parser.add_argument('id', help='Identifier for this capturer. Limited to 8 chars')
    parser.add_argument('host', default='localhost', nargs='?', help='Server to send images to')
    parser.add_argument('-p', '--port', default=8888, type=int, help='Server port to send images')
    parser.add_argument('-i', '--interval', default=3.0, type=float, help='Number of seconds between image captures')
    args = parser.parse_args()
    
    # Setting stuff
    conn_info = (args.host, args.port)
    img_path = os.path.join('/tmp', 'temp.jpg')
    
    # Make ID exactly 8 characters
    id = args.id[0:8]
    id += ' ' * (8 - len(id))
    
    print 'Capturer', id, 'sending to', args.host + ':' + str(args.port)
    
    # Open camera and start taking pictures
    camera = highgui.cvCreateCameraCapture(0)
    while True:
        # Get image and save as JPG
        img = get_image(camera)
        img.save(img_path, 'JPEG')
        
        # Send to server
        try:
            print('Sending image')
            sock = socket.create_connection(conn_info, timeout=1)
            
            sock.send(id)
            with open(img_path, 'rb') as f:
                while True:
                    data = f.read(1024)
                    if data:
                        sock.send(data)
                    else:
                        break
            
            sock.close()
        except socket.error as e:
            # Try again later, assume that the server is just down
            # but that the user gave us the right address
            print(e)
            
        # Don't overload ourselves here
        time.sleep(args.interval)
   
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))

