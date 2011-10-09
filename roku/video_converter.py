#!/usr/bin/env python3
import sys
import os
import subprocess

def convert_video(src, dest):
	# Generate file name if needed. HanbBrake requires it
	if os.path.isdir(dest):
		src_name = os.path.basename(src)
		dest += '/' + '.'.join(src_name.split('.')[:-1]) + '.mp4'

	print('Converting', src, 'to', dest)
	#status = subprocess.call(['HandBrakeCLI', '-f', 'mp4', '-O', '-i', src, '-o', dest])

def main(argv=None):
	# We have the default as None rather than sys.argv
 	# just in case sys.argv were changed after startup
	if argv is None:
		argv = sys.argv

	if len(argv) != 3:
		print('Usage:', argv[0], '<unconverted> <converted>')
		exit()

	# Were we given a folder or files?
	src = argv[1]
	dest = argv[2]
	if os.path.isdir(src):
		# Run through each video file in the folder
		src_dir = src
		for src in os.listdir(src_dir):
			convert_video(src, dest)
	else:
		# Single file source
		convert_video(src, dest)

if __name__ == '__main__':
	main(sys.argv)

