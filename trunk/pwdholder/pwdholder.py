#!/usr/bin/env python3
import os
import sys
import base64
import argparse
import getpass
import io

class PasswordHolder:
	"""
	Primary controller of passwords, handles all password database reading,
	writing, etc
	"""
	MAGIC_VALUE = 'pwdholder'
	VERSION = 1
	
	def __init__(self, pwFilePath, key=None, newDB=False):
		"""
		Loads up the given password file and initializes the lookup table
		"""
		self.reload_passwords(pwFilePath, key, newDB)
		
	def reload_passwords(self, pwFilePath=None, key=None, newDB=False):
		"""
		Loads up the password file into memory for fast reference
		"""
		# If no path was given, use the saved one. Otherwise, 
		# save the new path
		if pwFilePath is None:
			pwFilePath = self.__pwFilePath
		else:
			self.__pwFilePath = pwFilePath
		
		# Memory cache for passwords
		self.__pwdb = {}
		
		# Does the file exist already? If not, just save new passwords there
		# Alternatively, the user may ask us to not load from the file... IE, wipe it
		if not newDB and os.path.isfile(self.__pwFilePath):
			# Open and decrypt file if needed
			with open(self.__pwFilePath, 'rb') as f:
				pwdb = f.read()
			
			# Decrypt?
			if key is not None:
				pass
			
			# Treat (possibly decrypted) DB as a file again
			with io.StringIO(pwdb.decode()) as f:
				# Check magic value/version
				try:
					magic, version = f.readline().rstrip().split(' ', 1)
				except ValueError as e:
					# First line didn't even have a space... definitely not us
					raise PwdFileError('File is not a valid password store')
				except StopIteration as e:
					# Blank
					raise PwdFileError('File is empty, not a valid password store')
				
				if magic != self.MAGIC_VALUE:
					raise PwdFileError('File is not a valid password store')
				if int(version) > self.VERSION:
					raise PwdFileError('Password store version newer')
		
				# Read in all passwords
				try:
					line = f.readline()
					while line:
						# Split into url, user, and pw state and decode them all
						url, user, pw = [base64.b64decode(s.encode()).decode() for s in line.split(':')]
				
						# Save to memory cache
						self.__pwdb[url] = (user, pw)
						
						# Next!
						line = f.readline()
				except ValueError as e:
					raise PwdFileError('Password store appears to be corrupted')
		elif newDB:
			# Use a blank DB
			print('Creating new store')
		else:
			raise PwdFileError('Password store not found')
			
	def save_passwords(self, key=None):
		"""
		Flushes out passwords to disk
		"""
		# We're going to re-write the whole file
		with open(self.__pwFilePath, 'w') as f:
			# Version/magic value ID
			f.write(self.MAGIC_VALUE + ' ' + str(self.VERSION) + '\n')
		
			# Encode and write each url
			for url in self.__pwdb:
				user, pw = self.__pwdb[url]
				line = ':'.join([base64.b64encode(s.encode()).decode() for s in [url, user, pw]]) + '\n'
				f.write(line)

	def get_password(self, url):
		"""
		Fetches the user name and password for the given URL.
		Return None if there is no saved password for it
		"""
		try:
			return self.__pwdb[url]
		except KeyError as e:
			# No password currently saved for URL
			return None
			
	def set_password(self, url, user, pw):
		"""
		Saves the given password to our in-memory database.
		To flush it out to disk, save_passwords() must be called
		"""
		self.__pwdb[url] = (user, pw)

class PwdFileError(Exception):
	"""
	Thrown when an execption occurs with the password file
	"""
	def __init__(self, value):
		self.value = value
	
	def __str__(self):
		return self.value

def main(argv):
	"""
	Run pwdholder as a standalone app
	"""
	if argv is None:
		argv = sys.argv

	parser = argparse.ArgumentParser(description='Securely stores passwords for websites')
	parser.add_argument('--new-database', action='store_true', help='Removes all the passwords in the database')
	parser.add_argument('--set', action='store_true', help='Sets the username/password for the given URL')
	parser.add_argument('--no-encryption', action='store_true', help='Saves password database without encryption. Not recommended')
	parser.add_argument('--db', default='~/.pwholder', metavar='pwfile', help='Path to the password database file. Defaults to ~/.pwholder')
	parser.add_argument('url', metavar='url', help='URL of username/password to pull out')
	args = parser.parse_args()
	
	# Need key?
	if not args.no_encryption:
		key = getpass.getpass('Master password: ')
	else:
		key = None
	
	try:
		# Load passwords
		pwHolder = PasswordHolder(os.path.expanduser(args.db), key, args.new_database)
	
		if args.set:
			# Set password
			user = input('Username: ')
			pw = getpass.getpass('Password: ');
			pwHolder.set_password(args.url, user, pw)
			pwHolder.save_passwords(key)
		else:
			# Pull out password
			pw = pwHolder.get_password(args.url)
			if pw is not None:
				print('User:', pw[0])
				print('Password:', pw[1])
			else:
				print('No password saved for given URL')
	except PwdFileError as e:
		print(e)
		
if __name__ == '__main__':
	# Only need argparse if actually running standalone
	main(sys.argv)

