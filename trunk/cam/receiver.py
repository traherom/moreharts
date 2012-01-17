#!/usr/bin/env python
import sys
import os
import random
import string
import socket
import argparse

def generate_html(save_loc, capturers, interval=3):
    with open(os.path.join(save_loc, 'index.html'), 'w') as f:
        f.write('<html>\n<head>\n<title>Morehart Cams</title>\n')
        f.write('<meta http-equiv="refresh" content="{}">\n'.format(interval))
        f.write('</head>\n<body>\n')
        
        for k in capturers:
            f.write('<img alt="{}" src="{}" />\n'.format(k, os.path.basename(capturers[k])))
        
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
    
    # Create initial (empty) HTML page
    generate_html(args.save_loc, capturers, args.interval)
    
    # Start listening
    listenSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listenSock.bind(('', 8888))
    listenSock.listen(10)
    
    while True:
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
        id = clientSock.recv(2)
        if len(id) != 2:
            print('ID not given, dropping')
            continue
        
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
        try:
            save_file = capturers[id] 
        except KeyError as e:
            # Not seen before, create new entry
            save_file = ''.join(random.choice(string.ascii_uppercase) for x in range(10))
            save_file += '.jpg'
            save_file = os.path.join(args.save_loc, save_file)
            capturers[id] = save_file
            
            # Regenerate HTML file
            generate_html(args.save_loc, capturers, args.save_loc)
        
        # Save image
        with open(os.path.join(args.save_loc, save_file), 'wb') as f:
            f.write(msg)
            print('New image saved for ' + id)
           
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
    
