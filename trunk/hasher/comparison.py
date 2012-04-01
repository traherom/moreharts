#!/usr/bin/env python3
import os
import argparse
import hashlib
import sys

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

def walk(left_base, right_base, relative_path):
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
		return False
	
	if os.path.isfile(left_path):
		# Hash it
		lhash = hash_file(left_path)
		rhash = hash_file(right_path)
		
		if lhash != rhash:
			print_diff(relative_path, 'Hash does not match')
			return False	
	elif os.path.isdir(left_path):
		# Oh, we're a directory. Compare all our children
		# Walk through left side and compare against same path in right side
		matches = True
		
		# Don't actually descend into a link to a directory
		if not os.path.islink(left_path):
			for child in sorted(os.listdir(left_path)):
				next_relative = os.path.join(relative_path, child)
				if not walk(left_base, right_base, next_relative):
					matches = False

		# Also, make sure the right side doesn't have extra files
		extra_right = list(set(os.listdir(right_path)) - set(os.listdir(left_path)))
		if extra_right:
			matches = False
			for extra in sorted(extra_right):
				print_diff(os.path.join(relative_path, extra), 'New on right')
				
		return matches
			
	return True

def main(argv):
	parser = argparse.ArgumentParser(description='''Displays differences between two directories''')
	parser.add_argument('left', metavar='left side', help='Directory to compare against. The "control" in the experiment, if you will')
	parser.add_argument('right', metavar='right side', help='Directory to compare')
	args = parser.parse_args()
	
	walk(args.left, args.right, '')

	return 0

if __name__ == '__main__':
	sys.exit(main(sys.argv))
	
