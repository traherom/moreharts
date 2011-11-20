#!/usr/bin/env python3
import os
import sys
import base64
import argparse
import getpass
import io
import threading
import backend
import configparser

def main(argv):
	"""
	Run pwdholder as a standalone app
	"""
	if argv is None:
		argv = sys.argv

	parser = argparse.ArgumentParser(description='Securely stores passwords for websites')
	parser.add_argument('--config', default='~/.pwholder', metavar='pwfile', help='Config file to use. Defaults to ~/.pwholder')
	parser.add_argument('--user', help='Provide username on command line, prompted for if not given')
	parser.add_argument('--password', help='Provide password on command line, prompted for if not given')
	parser.add_argument('--create-user', action='store_true', help='Creates a new user in the database')
	args = parser.parse_args()
	
	# Read config
	try:
		config = configparser.ConfigParser()
		config.read(os.path.expanduser(args.config))
		
		conn_str = config['database']['connection']
	except KeyError as e:
		if os.path.exists(args.config):
			print('Error in configuration file:', e)
		else:
			print('Ensure configure file', args.config, 'exists')
		sys.exit(1)
	
	# Connect to server
	pwdb = backend.PasswordHolder(conn_str)
	
	# Every action needs these:
	# Username
	if args.user is None:
		user = input('User: ')
	else:
		user = args.user
	
	# Password
	if args.password is None:
		master_pw = getpass.getpass('Password: ')
	else:
		master_pw = args.password
		
	# What action are we performing?
	if args.create_user:
		try:
			pwdb.create_user(user, master_pw)
			print('User "{}" created'.format(user))
		except backend.PwdError as e:
			print(e)
	else:
		print('Not yet implemented')
		
	return 0
		
if __name__ == '__main__':
	sys.exit(main(sys.argv))

