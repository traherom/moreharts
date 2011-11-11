#!/usr/bin/env python3
import os
import sys
import base64
import argparse
import getpass
import io
import threading
import hashlib
from Crypto.Cipher import AES

class PasswordHolder:
	"""
	Primary controller of passwords, handles all password database reading,
	writing, etc
	"""
	MAGIC_VALUE = 'pwdholder'
	VERSION = 1
	SALT = 'FaNyyfcAkoNdh4fY6qNW'
	AES_MODE = AES.MODE_CFB
	
	def __init__(self, pwFilePath, master_pw=None, salt=None, newDB=False):
		"""
		Loads up the given password file and initializes the lookup table
		"""		
		# Create encryption key based on password and salt
		if master_pw is not None:
			h = hashlib.sha256()
			h.update(master_pw.encode())
			
			if salt is None:
				h.update(self.SALT.encode())
			else:
				h.update(salt.encode())
				
			self.__key = h.digest()
		else:
			self.__key = None
			
		# Load database
		self.__pwdb_lock = threading.Lock()
		self.__pwdb = {}
		self.reload_passwords(pwFilePath, newDB)
		
	def reload_passwords(self, pwFilePath=None, newDB=False):
		"""
		Loads up the password file into memory for fast reference
		"""
		# If no path was given, use the saved one. Otherwise, 
		# save the new path
		if pwFilePath is None:
			pwFilePath = self.__pwFilePath
		else:
			self.__pwFilePath = pwFilePath
		
		# Does the file exist already? If not, just save new passwords there
		# Alternatively, the user may ask us to not load from the file... IE, wipe it
		if not newDB and os.path.isfile(self.__pwFilePath):
			# Open and decrypt file if needed
			with open(self.__pwFilePath, 'rb') as f:
				pwdb = f.read()
			
			# Decrypt?
			if self.__key is not None:
				aes = AES.new(self.__key, self.AES_MODE)
				pwdb = aes.decrypt(pwdb)
				
			# Make it into a string from bytes
			try:
				pwdb = pwdb.decode()
			except UnicodeDecodeError as e:
				raise PwdFileError('Incorrect decryption key and/or no encryption applied')
			
			# Treat (possibly decrypted) DB as a file again
			with io.StringIO(pwdb) as f:
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
		
				with self.__pwdb_lock:
					# Blank current list
					self.__pwdb = {}
					
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
			
	def save_passwords(self):
		"""
		Flushes out passwords to disk
		"""
		# Write DB to an in-memory string, encrypt if needed, then dump to disk
		with io.StringIO() as f:
			# Version/magic value ID
			f.write(self.MAGIC_VALUE + ' ' + str(self.VERSION) + '\n')
		
			with self.__pwdb_lock:
				# Encode and write each url
				for url in self.__pwdb:
					user, pw = self.__pwdb[url]
					line = ':'.join([base64.b64encode(s.encode()).decode() for s in [url, user, pw]]) + '\n'
					f.write(line)
			
			# Save final string
			pwdb = f.getvalue()
			
		# Encrypt?
		if self.__key is not None:
			aes = AES.new(self.__key, self.AES_MODE)
			pwdb = aes.encrypt(pwdb)
		else:
			# Turn to bytes
			pwdb = pwdb.encode()

		# We're going to re-write the whole file
		with open(self.__pwFilePath, 'wb') as f:
			f.write(pwdb)

	def get_password(self, url):
		"""
		Fetches the user name and password for the given URL.
		Return None if there is no saved password for it
		"""
		try:
			with self.__pwdb_lock:
				return self.__pwdb[url]
		except KeyError as e:
			# No password currently saved for URL
			return None
			
	def set_password(self, url, user, pw):
		"""
		Saves the given password to our in-memory database.
		To flush it out to disk, save_passwords() must be called
		"""
		with self.__pwdb_lock:
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
	parser.add_argument('--salt', default=None, help='Sets salt to use for encryption key. If none given, uses builtin salt')
	parser.add_argument('--db', default='~/.pwholder', metavar='pwfile', help='Path to the password database file. Defaults to ~/.pwholder')
	parser.add_argument('url', metavar='url', help='URL of username/password to pull out')
	args = parser.parse_args()
	
	# Need key?
	if not args.no_encryption:
		master_pw = getpass.getpass('Master password: ')
	else:
		master_pw = None
	
	try:
		# Load passwords
		pwHolder = PasswordHolder(os.path.expanduser(args.db), master_pw, args.salt, args.new_database)
	
		if args.set:
			# Set password
			user = input('Username: ')
			pw = getpass.getpass('Password: ');
			pwHolder.set_password(args.url, user, pw)
			pwHolder.save_passwords()
		else:
			# Pull out password
			pw = pwHolder.get_password(args.url)
			if pw is not None:
				print('User:', pw[0])
				print('Password:', pw[1])
			else:
				print('No password saved for given URL')
				
		# If we made a new database, always save it
		if args.new_database:
			pwHolder.save_passwords()
			
	except PwdFileError as e:
		print(e)
		
if __name__ == '__main__':
	# Only need argparse if actually running standalone
	main(sys.argv)

