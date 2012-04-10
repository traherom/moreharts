#!/usr/bin/env python3
import os
import argparse
import hashlib
import sys
import shutil

def prompt_for_action(settings, left_base, right_base, relative_path, default='i'):
	# Is there a saved action for this path?
	try:
		choice = settings[relative_path]
	except KeyError:
		# There isn't, ask the user
		promptStr = "Action? "
		if default == 'i':
			promptStr += '[c/d/I] '
		elif default == 'd':
			promptStr += '[c/D/i] '
		elif default == 'c':
			promptStr += '[C/d/i] '
	
		choice = input(promptStr)
		if not choice:
			choice = default
		
	# Normalize choice
	choice = choice.lower()
	if choice[0] == 'a':
		# We're supposed to always take this action in the future
		choice = choice[1]
		
		settings[relative_path] = choice

	# Compose full path
	left_full = os.path.join(left_base, relative_path)		
	right_full = os.path.join(right_base, relative_path)

	# Follow user's direction	
	if choice == 'd':
		# Delete
		print('Deleting', relative_path, 'from right')
		
		if os.path.isdir(right_full):
			shutil.rmtree(right_full)
		else:
			os.unlink(right_full)
		
	elif choice == 'c':
		# Copy from left to right
		print('Copying', relative_path, 'to right')
		if os.path.isdir(right_full):
			shutil.copytree(left_full, right_full)
		else:
			shutil.copy2(left_full, right_full)
			
	else:
		print('Ignoring', relative_path)
		
def hash_file(path):
	"""
	Computes the SHA1 hash the given file
	Returns the hex string of hash
	"""
	m = hashlib.sha1()
	with open(path, 'rb') as f:
		while True:
			sec = f.read(5120)
			if not sec:
				break
			m.update(sec)

	return m.hexdigest()
		
def print_diff(path, diff_type):
	print('{}: {}'.format(path, diff_type))

def path_type(path):
	if not os.path.lexists(path):
		return None
	
	if os.path.isfile(path):
		t = 'file'
	elif os.path.isdir(path):
		t = 'dir'
	elif os.path.ismount(path):
		t = 'mount'
	else:
		t = None
		
	if os.path.islink(path):
		if t is not None:
			return 'link to ' + t
		else:
			return 'broken link'
	else:
		return t

def walk(settings, left_base, right_base, relative_path):
	print('Working on {}'.format(relative_path), file=sys.stderr)

	# Construct paths we're actually comparing
	left_path = os.path.join(left_base, relative_path)
	right_path = os.path.join(right_base, relative_path)
		
	# Ensure they are the same type on both sides and act accordingly
	left_type = path_type(left_path)
	right_type = path_type(right_path)
	
	if right_type is None:
		print_diff(relative_path, 'Does not exist on right side')
		return False
	
	if left_type != right_type:
		print_diff(relative_path, 'Left is {}, right is {}'.format(left_type, right_type))
		prompt_for_action(settings, left_base, right_base, relative_path, 'd')
		return False
	
	if os.path.isfile(left_path):
		# Hash it
		lhash = hash_file(left_path)
		rhash = hash_file(right_path)
		
		if lhash != rhash:
			print_diff(relative_path, 'Hash does not match')
			prompt_for_action(settings, left_base, right_base, relative_path, 'c')
			return False
	elif os.path.isdir(left_path):
		# Oh, we're a directory. Compare all our children
		# Walk through left side and compare against same path in right side
		matches = True
		
		# Don't actually descend into a link to a directory
		if not os.path.islink(left_path):
			for child in sorted(os.listdir(left_path)):
				next_relative = os.path.join(relative_path, child)
				if not walk(settings, left_base, right_base, next_relative):
					matches = False

		# Also, make sure the right side doesn't have extra files
		extra_right = list(set(os.listdir(right_path)) - set(os.listdir(left_path)))
		if extra_right:
			matches = False
			for extra in sorted(extra_right):
				extra_rel = os.path.join(relative_path, extra)
				print_diff(extra_rel, 'New on right')
				
				# Delete new file?
				prompt_for_action(settings, left_base, right_base, extra_rel, 'd')
				
		return matches
			
	return True

def save_settings(path, settings):
	print("Saving settings...")
	with open(path, 'w') as f:
		for path, action in settings.items():
			f.write('{} {}\n'.format(path, action))

def load_settings(path):
	print("Loading settings...")
	settings = {}
	try:
		with open(path, 'r') as f:
			for line in f:
				# Skip blank lines (mostly the end)
				line = line.strip()
				if not line:
					continue
					
				path = line[:-2]
				action = line[-1]
				settings[path] = action
	except IOError as e:
		# We're likely creating a new settings file
		pass
		
	return settings

def main(argv):
	parser = argparse.ArgumentParser(description='Displays differences between two directories')
	parser.add_argument('--settings', '-s', help='File to save whether to always do ignore/copy/delete on certain paths')
	parser.add_argument('left', metavar='<left>', help='Directory to compare against. The "control" in the experiment, if you will')
	parser.add_argument('right', metavar='<right>', help='Directory to compare')
	args = parser.parse_args()
	
	# Read in file that contains our "always do X" settings
	settings = {}
	if args.settings:
		settings = load_settings(args.settings)
	
	# Do comparison
	walk(settings, args.left, args.right, '')

	# Save setting changes back
	if args.settings:
		save_settings(args.settings, settings)
		
	return 0

if __name__ == '__main__':
	sys.exit(main(sys.argv))
	
