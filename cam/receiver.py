#!/usr/bin/env python
import sys
import os
import random
import string
import socket

def generate_html(save_loc, capturers):
    with open(os.path.join(save_loc, 'index.html'), 'w') as f:
        f.write('<html><head><title>Morehart Cams</title>')
        f.write('<meta http-equiv="refresh" content="3">')
        f.write('</head><body>\n')
        
        for k in capturers:
            f.write('<img alt="')
            f.write(k)
            f.write('" src="')
            f.write(capturers[k])
            f.write('" />\n')
        
        f.write('</body></html>')

def main(argv):
    if len(argv) == 2:
       save_loc = argv[1]
    else:
       save_loc = '.'
    
    print 'Saving to', save_loc
    
    # Keep record of all capturer's we're getting data from
    # They'll be giving us unique IDs the first time they 
    # send us an image
    capturers = {}
    
    # Create initial (empty) HTML page
    generate_html(save_loc, capturers)
    
    # Start listening
    listenSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    listenSock.bind(('127.0.0.1', 8888))
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
            capturers[id] = save_file
            
            # Regenerate HTML file
            generate_html(save_loc, capturers)
        
        # Save image
        with open(os.path.join(save_loc, save_file), 'wb') as f:
            f.write(msg)
            print('New image saved for ' + id)
           
    return 0

if __name__ == '__main__':
    sys.exit(main(sys.argv))
    
