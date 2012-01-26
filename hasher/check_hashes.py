#!/usr/bin/env python3
import sys
import os
import argparse

def check_hash(hashdb, hash):
    return False

def check_dir(hashdb, dir):
	pass

def main(argv):
    # Parse command line
    parser = argparse.ArgumentParser(description='Compares hashes of files on drive to hash database')
    parser.add_argument('--host', default='localhost', help='Host with the hash database')
    parser.add_argument('--port', type=int, default=3389, help='Port for database')
    parser.add_argument('-s', '-db', '--database', '--schema', default='hashes', help='Schema (database) on the server to use')
    parser.add_argument('-u', '--user', required=True, help='Username for database')
    parser.add_argument('-p', '--password', help='Password for database')
    parser.add_argument('-o', '--output', help='File to output results to')
    parser.add_argument('--nsrl-compare', type=bool, action='store_true', default=True, help='Check files against NIST NSRL hashes (known-good files)')
    parser.add_argument('--save-state', type=bool, action='store_true', default=False, help='Saves the hashes for this computer to the database')
    parser.add_argument('--check-state', type=bool, action='store_true', default=False, help='Checks the hashes found against saved state. Shows additions, removals, and changes')
    parser.add_argument('-i', '--identifier', help='Identifier for this computer')
    parser.add_argument('root_dir', meta='Root', nargs='+', help='Directory to start recursive hashing of files')
    args = parser.parse_args()
    
    # If the user wants us to do a state action, they must give the ID
    if (args.save_state or args.check_state) and args.identifier is None:
    	print('A system identifier must be given to save or check state')
    	return 1
    
    # Go through each directory the user requested
    for d in args.root_dir:
        check_dir(args.hashdb, d)
    
    return 0
    
if __name__ == '__main__':
    sys.exit(main(sys.argv))
    
