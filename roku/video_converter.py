#!/usr/bin/env python3
import sys
import argparse
import os
import subprocess
import tempfile
import re
import time
import shutil

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
		dest = os.path.join(dest, '.'.join(src_name.split('.')[:-1]) + '.mp4')

	return dest
		
def determine_action(src):
	"""
	Determines if the source is already in a Roku-playable format.
	Returns the function that should be called based on the results.
	Throws a NotVideoError if the source is not a (recognized) video
	file at all
	"""
	if not os.path.isfile(src):
		raise ValueError('Source to check must be a file.')
	
	# Use ffmpeg to check codec
	print('\tChecking format... ', end='')
	p = subprocess.Popen(['ffmpeg', '-i', src], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	stdout, stderr = p.communicate()
	
	# Video encoding needed?
	valid_codecs = ['mpeg4', 'mpeg4 (high)', 'mpeg4 (main)', 'h264', 'h264 (high)', 'h264 (main)']
	m = re.search(': Video: (.+?),', str(stdout))
	if m is not None:
		vcodec = m.group(1).lower()
		try:
			# Find in list. Throws exception if it's not there
			valid_codecs.index(vcodec)
			video_ok = True
		except ValueError as e:
			video_ok = False
	else:
		print('unable to determine video codec. Is this not a video?')
		raise NotVideoError('Source is not a video file')

	# Audio encoding needed?
	m = re.search(': Audio: (.+?),', str(stdout))
	if m is not None:
		acodec = m.group(1).lower()		
		if acodec == 'aac':
			audio_ok = True
		else:
			audio_ok = False
	else:
		print('unable to determine audio codec')
		raise NotVideoError('Source is not a video file')
		
	# Container format ok?
	ext = src.split('.')[-1].lower()
	if ext == 'mp4' or ext == 'm4v':
		container_ok = True
	else:
		container_ok = False
	
	# Determine the best, fastest action
	if audio_ok and video_ok and container_ok:
		# Just need to move it, everything is fine
		print('correct')
		return move_video
	elif audio_ok and video_ok and not container_ok:
		# Just need to change to an mp4 container
		print('container is', ext)
		return change_video_container
	elif video_ok and not audio_ok:
		# Need to reencode just the audio
		print('audio is', acodec)
		return transcode_audio
	elif not video_ok and audio_ok:
		# Need to just take care of the video
		print('video is', vcodec)
		return transcode_video
	else:
		# Bah. Do it all
		print('video is', vcodec, 'audio is', acodec)
		return transcode_both
	
def move_video(src, dest):
	"""
	Moves the source video into the destination with an appropriate extension
	"""
	if not os.path.isfile(src):
		raise ValueError('Source must be a file')
		
	dest = generate_dest_file(src, dest)
	print('\tMoving to', dest)
	shutil.move(src, dest)

def change_video_container(src, dest):
	"""
	Changes the video's container without fully reencoding
	"""
	if not os.path.isfile(src):
		raise ValueError('Source must be a file')
	
	STATUS_FIELD_WIDTH = 10
	
	print('\tChanging video container format' + ' ' * STATUS_FIELD_WIDTH, end='')
	
	# Actually do the change into temp, then move it afterward
	f, temp_dest = tempfile.mkstemp(suffix='.mp4')
	os.close(f)
	p = subprocess.Popen(['ffmpeg', '-i', src, '-y', '-vcodec', 'copy', '-acodec', 'copy', temp_dest], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	
	# "working" indicator
	wait_period = 0
	last_line = ''
	while p.poll() is None:
		print('\b' * STATUS_FIELD_WIDTH, end='')
		print('...'.rjust(wait_period + 3).ljust(STATUS_FIELD_WIDTH)[:STATUS_FIELD_WIDTH], end='')
		sys.stdout.flush()
		
		last_line = p.stdout.read(100)
		
		wait_period = (wait_period + 1) % STATUS_FIELD_WIDTH
		time.sleep(.5)
	
	print('\b' * STATUS_FIELD_WIDTH + '... done')
	
	if p.returncode == 0:
		move_video(temp_dest, generate_dest_file(src, dest))
	else:
		raise OSError('Error in changing container: ' + str(last_line)[4:-1])

def transcode_audio(src, dest):
	"""
	Changes the video's audio without touching the video. Mainly a
	massive speed advantage.
	"""
	if not os.path.isfile(src):
		raise ValueError('Source must be a file')
	
	STATUS_FIELD_WIDTH = 10
	
	print('\tTrascoding audio' + ' ' * STATUS_FIELD_WIDTH, end='')
	
	# Actually do the change into temp, then move it afterward
	f, temp_dest = tempfile.mkstemp(suffix='.mp4')
	os.close(f)
	p = subprocess.Popen(['ffmpeg', '-i', src, '-y', '-vcodec', 'copy', '-acodec', 'aac', '-strict', 'experimental', temp_dest], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	
	# "working" indicator as we run
	wait_period = 0
	last_line = ''
	while p.poll() is None:
		print('\b' * STATUS_FIELD_WIDTH, end='')
		print('...'.rjust(wait_period + 3).ljust(STATUS_FIELD_WIDTH)[:STATUS_FIELD_WIDTH], end='')
		sys.stdout.flush()
		
		last_line = p.stdout.read(100)
		
		wait_period = (wait_period + 1) % STATUS_FIELD_WIDTH
		time.sleep(.5)
	
	print('\b' * STATUS_FIELD_WIDTH + '... done')
	
	if p.returncode == 0:
		move_video(temp_dest, generate_dest_file(src, dest))
	else:
		raise OSError('Error in reencoding audio: ' + str(last_line)[4:-1])
	
def transcode_video(src, dest):
	"""
	Tells transcoder to just copy the audio track
	"""
	transcode(src, dest, False)
	
def transcode_both(src, dest):
	"""
	Merely calls handbrake to encode both the audio and the video
	"""
	transcode(src, dest, True)

def transcode(src, dest, do_audio = True):
	if not os.path.isfile(src):
		raise ValueError('Source to convert must be a file')

	STATUS_FIELD_WIDTH = 25
	
	if do_audio:
		audio_encoder = 'aac'
		print('\tTranscoding audio and video... ' + ' '*STATUS_FIELD_WIDTH, end='')
	else:
		audio_encoder = 'copy'
		print('\tTranscoding video... ' + ' '*STATUS_FIELD_WIDTH, end='')
	
	# Actually do the encoding into temp, then move it afterward
	f, temp_dest = tempfile.mkstemp(suffix='.mp4')
	os.close(f)
	p = subprocess.Popen(['HandBrakeCLI', '-f', 'mp4', '--decomb', '--encoder', 'x264', '--aencoder', audio_encoder, '-O', '-i', src, '-o', temp_dest], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

	# Keep a status message up so the user doesn't feel a sense of hopelessness
	exp = re.compile("^Encoding: task ([0-9]+) of ([0-9]+), ([0-9.]+) %(?: \\(.+ETA ([0-9]{2})h([0-9]{2})m([0-9]{2})s\\))?$")
	line = ''
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
		move_video(temp_dest, generate_dest_file(src, dest))
	else:
		raise OSError('Error in encoding. Unable to continue: ' + str(line)[4:-1])

def main(argv=None):
	# We have the default as None rather than sys.argv
 	# just in case sys.argv were changed after startup
	if argv is None:
		argv = sys.argv

	# Parse args
	parser = argparse.ArgumentParser(description='Performs conversion of video files to be suitable for Roku')
	parser.add_argument('--first-only', action='store_true', help='Only convert the first video found in the source folder')
	parser.add_argument('--remove-source', action='store_true', help='Remove the source file after conversion')
	parser.add_argument('--remove-failed', action='store_true', help='Remove the source file file if conversion fails. Cannot be undone!')
	parser.add_argument('--no-action', action='store_true', help='Only specify what primary action would be taken for each file, do not perform')
	parser.add_argument('--allow-audio-only', action='store_true', help='Allow converter to transcode only the audio for a video file. Much faster, but often poor sound quality')
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
		src_list = [os.path.join(args.src, f) for f in os.listdir(args.src)]
	else:
		# Work with a single file
		src_list = [args.src]
		
	# Run through each video file in the folder
	# Keep some status info handy
	total_src = len(src_list)
	total_checked = 0
	call_counts = {}
	start_time = time.time()
	
	for src in src_list:
		# Skip directories
		if os.path.isdir(src):
			total_src -= 1
			continue
	
		print('Working on {} ({}/~{})'.format(src, total_checked + 1, total_src))
		total_checked += 1
	
		try:
			# Do the fastest possible action to accomplish our goal
			# Keep track of how many times each action is/would be called
			action = determine_action(src)
			if action.__name__ in call_counts:
				acount, atime = call_counts[action.__name__]
			else:
				acount = 1
				atime = 0
			
			# Transcoding only the audio currently uses ffmpeg, which... well, sucks for AAC.
			# By default, we're going to swap out calls for just audio to do everything and allow
			# handbrake to make it nice sounding
			if not args.allow_audio_only and action == transcode_audio:
				action = transcode_both
			
			if not args.no_action:
				try:
					# Call and time action
					s = time.time()
					action(src, args.dest)
					atime += time.time() - s
					
					# Remove source? Source may no longer exist if
					# we merely did a move
					if args.remove_source and os.path.exists(src):
						print('\tRemoving source video... ', end='')
						os.unlink(src)
						print('done')
				except OSError as e:
					print('\tAction failed: ' + str(e))

					# Remove, we'll never successfully convert it.
					if args.remove_failed and os.path.exists(src):
						print('\tRemoving bad source video... ', end='')
						os.unlink(src)
						print('done')

					print('\tSkipping to next file')
					continue
			else:
				# Only simulating
				print('\tWould call:', action.__name__)
			
			# Save usage info
			call_counts[action.__name__] = (acount, atime)
			
		except NotVideoError:
			print('\tSkipping file')
			continue
				
		# Only do this loop once if we're doing, well, the --first-only
		if args.first_only:
			break

	# How much work did we do?
	end_time = time.time()
	print('Complete. Usage:')
	for action, utilization in call_counts.items():
		print(action, ':', utilization[0], 'times,', round(utilization[1], 3), 'seconds')
	print('Total:', total_checked, 'checked,', round(end_time - start_time, 3), 'seconds')
	
if __name__ == '__main__':
	main(sys.argv)
