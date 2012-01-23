#!/usr/bin/env python3
import sys
import os
import argparse

def check_hash(hashdb, hash):
    return False

def check_dir(hashdb, dir):
    for f_name in 

def main(argv):
    # Parse command line
    parser = argparse.ArgumentParser(description='Compares hashes of files on drive to hash database')
    parser.add_argument('hashdb', meta='Hash DB', help='Text database of hashes to compare against')
    parser.add_argument('root_dir', meta='Root', nargs='+', help='Directory to recursively hash files in')
    parser.add_argument('-o', '--output', help='File to output results to')
    args = parser.parse_args()
    
    # Go through each directory the user requested
    for d in args.root_dir:
        check_dir(args.hashdb, d)
    
    return 0
    
if __name__ == '__main__':
    sys.exit(main(sys.argv))
    
