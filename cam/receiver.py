#!/usr/bin/env python
import sys
import os
import random
import string
import re
import time
import socket
import argparse

def generate_html(save_loc, capturers, interval=3):
    with open(os.path.join(save_loc, 'index.html'), 'w') as f:
        f.write('<html>\n<head>\n<title>Morehart Cams</title>\n')
        f.write('<meta http-equiv="refresh" content="{}">\n'.format(interval))
        f.write('</head>\n<body>\n')
        
        time_limit = time.time() - 60
        for k in capturers:
            t, save_file = capturers[k]
            if time_limit < t:
                f.write('<img alt="{}" src="{}" />\n'.format(k, os.path.basename(save_file)))
        
        f.write('</body>\n</html>\n')

def main(argv):
    parser = argparse.ArgumentParser(description='Receives images from capturer and saves them for an HTML page')
    parser.add_argument('save_loc', metavar='save location', nargs='?', default='.', help='Location to save index.html and images')
    parser.add_argument('-p', '--port', default=8888, type=int, help='Port to listen on')
    parser.add_argument('-i', '--interval', default=3, type=int, help='Refresh interval for webpage (seconds)')
    args = parser.parse_args()
    
    print 'Saving to', args.save_loc
    
    # Keep record of all capturer's we're getting data from
    # They'll be giving us unique IDs the first time they 
    # send us an image
    capturers = {}
    
    # Start listening
    listenSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listenSock.bind(('', 8888))
    listenSock.listen(10)
    
    # Expire old cameras. Make sure we refresh our HTML right off the bat
    html_refresh_time = 0
    
    while True:
        # Expire old cameras
        curr_time = time.time()
        if html_refresh_time < curr_time:
            print('Regenerating HTML')
            generate_html(args.save_loc, capturers, args.interval)
            html_refresh_time = curr_time + 60
        
        # We're not going to both to multithread the handling here
        # The others can queue up, we know we won't have that many 
        # capturer's dumping into us at once, this ensures we don't
        # have to deal with locking or anything
        clientSock, addr = listenSock.accept()
        
        # Messages should be in the format:
        # 2 byte capturer ID
        # Image in JPG format
        #
        # Any inconsistencies we detect will cause the connection
        # to be dropped
        id = clientSock.recv(8)
        if len(id) != 8:
            print('ID not given, dropping')
            continue
        if re.match('^[a-zA-Z][a-zA-Z0-9 ]{7}$', id) is None:
            print('ID invalid, dropping')
            continue
        
        id = id.strip()
        
        msg = ''
        while True:
            data = clientSock.recv(1024)
            if data:
                msg += data
            else:
                break
        
        if len(msg) == 0:
            print('Message length mismatch, dropping')
            continue
    
        # Ensure it's a JPG (roughly, anyway)
        if msg[0:2] != '\xff\xd8':
            print('Not a JPEG, dropping:', msg[0:2])
            continue
        
        # Get our data on this capturer
        if id in capturers:
            # Update timestamp
            t, save_file = capturers[id]
            capturers[id] = (time.time(), save_file)
        else:
            # Regenerate HTML file with new sender
            save_file = os.path.join(args.save_loc, id + '.jpg')
            capturers[id] = (time.time(), save_file)
            generate_html(args.save_loc, capturers, args.interval)
            
        # Save image
        with open(os.path.join(args.save_loc, save_file), 'wb') as f:
            f.write(msg)
            print('New image saved for ' + id)
        
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
    
