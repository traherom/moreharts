#!/usr/bin/env python3
import sys
import argparse
import os
import subprocess
import tempfile
import re

class NotVideoError(Exception):
	def __init__(self, value):
		self.value = value
	def __str__(self):
		return repr(self.value)

def generate_dest_file(src, dest):
	"""
	Creates a valid destination file name from the given info.
	"""
	if not os.path.isfile(src):
		raise ValueError('Source must be a file')
	
	if os.path.isdir(dest):
		src_name = os.path.basename(src)
		dest += '/' + '.'.join(src_name.split('.')[:-1]) + '.mp4'

	return dest
		
def video_is_acceptable(src):
	"""
	Determines if the source is already in a Roku-playable format. Returns
	True if it is, False otherwise. Throws a NotVideoError if the source
	is not a (recognized) video file at all
	"""
	if not os.path.isfile(src):
		raise ValueError('Source to check must be a file')
	
	# Use ffmpeg to check codec
	print('\tChecking format... ', end='')
	p = subprocess.Popen(['ffmpeg', '-i', src], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	stdout, stderr = p.communicate()
	
	# Pull out the video codec
	m = re.search(': Video: (.+?),', str(stdout))
	if m is None:
		print('not a video')
		raise NotVideoError('Source is not a video file')
		
	print(m.group(1))
	
def move_video(src, dest):
	"""
	Moves the source video into the destination with an appropriate extension
	"""
	if not os.path.isfile(src):
		raise ValueError('Source must be a file')
		
	dest = generate_dest_file(src, dest)
	print('\tMoving to', dest)
	#shutil.move(src, dest)
	
def convert_video(src, dest):
	if not os.path.isfile(src):
		raise ValueError('Source to convert must be a file')

	STATUS_FIELD_WIDTH = 25
		
	print('\tConverting to temporary file... ' + ' '*STATUS_FIELD_WIDTH, end='')
	
	# Actually do the encoding into temp, then move it afterward
	temp_dest = generate_dest_file(src, tempfile.gettempdir())
	p = subprocess.Popen(['HandBrakeCLI', '-f', 'mp4', '-O', '-i', src, '-o', temp_dest], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

	# Keep a status message up so the user doesn't feel a sense of hopelessness
	exp = re.compile("^Encoding: task ([0-9]+) of ([0-9]+), ([0-9.]+) %(?: \\(.+ETA ([0-9]{2})h([0-9]{2})m([0-9]{2})s\\))?$")
	while p.poll() is None:
		line = str(p.stdout.read(100))[4:-1]
		m = exp.search(line)
		
		# Not all the messages from HandBrake actually match, we just care about
		# the long-running part
		if m is not None:
			# Erase old status then output new one. Include ETA if available
			print('\b'*STATUS_FIELD_WIDTH, end='')
			
			if m.group(4) is not None:
				print('{}% ETA {}h {}m {}s'.format(m.group(3).rjust(6), m.group(4), m.group(5), m.group(6)).ljust(STATUS_FIELD_WIDTH), end='')
			else:
				print('{}% ETA unknown'.format(m.group(3).rjust(6)).ljust(STATUS_FIELD_WIDTH), end='')
			sys.stdout.flush()
	
	# All done!
	print('\b'*STATUS_FIELD_WIDTH, 'done'.ljust(STATUS_FIELD_WIDTH))
			
	if p.returncode == 0:
		move_video(temp_dest, dest)

def main(argv=None):
	# We have the default as None rather than sys.argv
 	# just in case sys.argv were changed after startup
	if argv is None:
		argv = sys.argv

	# Parse args
	parser = argparse.ArgumentParser(description='Performs conversion of video files to be suitable for Roku')
	parser.add_argument('--first-only', action='store_true', help='Only convert the first video found in the source folder')
	parser.add_argument('--remove-source', action='store_true', help='Remove the source file after conversion')
	parser.add_argument('src', metavar='source', help='What file to convert. If a folder is given, all video files in it are converted.')
	parser.add_argument('dest', metavar='dest', default='.', nargs='?', help='Where to store converted file. May be a file or directory.')
	args = parser.parse_args()
	
	# Were we given a folder or files?
	if os.path.isdir(args.src):
		# Destination has to be a folder if we're working from a folder
		if not args.first_only and not os.path.isdir(args.dest):
			print('Destination must be a directory when using a source directory and no --first-only')
			exit()
		
		# Source and destination folders must be different
		if os.path.abspath(args.src) == os.path.abspath(args.dest):
			print('Source and destination directories may not be the same')
			exit()
		
		# Work with all files in directory
		src_list = os.listdir(args.src)
	else:
		# Work with a single file
		src_list = [args.src]
		
	# Run through each video file in the folder
	total_encoded = 0
	total_checked = 0
	for src in src_list:
		print('Working on {} ({}/{})'.format(src, total_checked + 1, len(src_list)))
		total_checked += 1
	
		# Is this video already in Roku-acceptable format?
		try:
			if not video_is_acceptable(src):
				convert_video(src, args.dest)
				
				# Remove source?
				if args.remove_source:
					print('\tRemoving source video... ', end='')
					os.unlink(src)
					print('done')
			else:
				print('Moving video to destination... ', end='')
				move_video(src, args.dest)
				print('done')
				
			# Keep totals up-to-date
			total_encoded += 1
			
		except NotVideoError:
			print('\tSkipping file')
			continue
				
		# Only do this loop once if we're doing, well, the --first-only
		if args.first_only:
			break

	print('Complete. Encoded', total_encoded, 'videos')
			
if __name__ == '__main__':
	main(sys.argv)

