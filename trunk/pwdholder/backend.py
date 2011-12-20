#!/usr/bin/env python3
import os
import sys
import base64
import argparse
import getpass
import io
import threading
import hashlib
import binascii
import re
from Crypto.Cipher import AES
import postgresql

class PasswordHolder:
	"""
	Primary controller of passwords, handles all password database reading,
	writing, etc
	"""
	AES_MODE = AES.MODE_CFB
	
	def __init__(self, conn_str, salt=None):
		"""
		Loads up the given password file and initializes the lookup table
		"""		
		# Connect to database
		self.__db = postgresql.open(conn_str)
		
		# Use a different salt than default?
		if salt is None:
			self.__salt = 'cHRKE9U8983hrkboerc'
		else:
			self.__salt = salt
	
	def get_db(self):
		"""
		Returns our DB connection, in case someone else wants to use it
		"""
		return self.__db
	
	def create_user(self, user, pw):
		"""
		Creates a new user in the database
		"""
		pw_hash = self.__password_hash(user, pw)
		create_login = self.__db.prepare('INSERT INTO users (name, masterpw) VALUES ($1, $2)')
		
		try:
			create_login(user, pw_hash)
		except postgresql.exceptions.UniqueError as e:
			raise PwdError('Username already exists')
	
	def __password_hash(self, user, pw):
		"""
		Helper to hash password
		"""
		hash = hashlib.sha256()
		hash.update(pw.encode())
		hash.update(self.__salt.encode())
		hash.update(user.encode())
		return hash.hexdigest()
	
	def login(self, user, pw):
		"""
		Attempts to log a user in via the given information. If the user is invalid,
		a UserError is raised. Returns the user ID
		"""
		# Attempt login
		pw_hash = self.__password_hash(user, pw)
		login = self.__db.prepare('SELECT id FROM users WHERE name=$1 AND masterpw=$2')
		user_id = login.first(user, pw_hash)
		
		if user_id is None:
			raise PwdError('Username or password not found')
		
		# Derive encryption key
		# TBD use real standard
		hash = hashlib.sha256()
		hash.update(pw.encode())
		hash.update(self.__salt.encode())
		hash.update(user.encode())
		hash.update(bytes(user_id))
		enc_key = hash.digest()
		
		return (user_id, enc_key)
	
	def get_user_info(self, user_id):
		"""
		Returns the username for the given user ID. Returns
		None if the user id doesn't exist.
		"""
		login = self.__db.prepare('SELECT name FROM users WHERE id=$1')
		return login.first(user_id)
	
	def encrypt(self, key, data):
		"""
		Helper to encrypt data with given key
		"""
		if type(key) == str:
			key = key.encode()
		aes = AES.new(key, self.AES_MODE)
		return aes.encrypt(data.encode())

	def decrypt(self, key, data):
		"""
		Helper to decrypt data with given key
		"""
		if type(key) == str:
			key = key.encode()
		aes = AES.new(key, self.AES_MODE)
		return aes.decrypt(data).decode()
	
	def __url_reduce(self, full_url):
		"""
		Reduces the given URL to something relatively consistent
		for a given site. IE, usually the domain. If it can't for
		some reason, the full url without query string will be returned.
		"""
		m = re.match("https?://([^/]*).*(\\?.*)?", full_url)
		if m is not None:
			return m.group(1)
		else:
			return full_url
	
	def get_password(self, user_id, site):
		"""
		Fetches tuple of the user name and password for the
		given URL. Password is still encrypted in tuple.
		Return None if there is no saved password for it
		"""
		getpass = self.__db.prepare('SELECT site_user, password FROM password WHERE user_id=$1 AND site=$2')
		row = getpass.first(user_id, self.__url_reduce(site))
		if row is not None:
			return (row['site_user'], row['password'])
		else:
			return None
			
	def set_password(self, user_id, site, site_user, site_hashed_pw):
		"""
		Saves the given password to database
		"""
		update = self.__db.prepare('UPDATE password SET site_user=$3, password=$4 WHERE site=$2 AND user_id=$1')
		if not update.first(user_id, self.__url_reduce(site), site_user, site_hashed_pw):
			# Update failed, must not have already been in database
			insert = self.__db.prepare('INSERT INTO password (user_id, site, site_user, password) VALUES ($1, $2, $3, $4)')
			if not insert.first(user_id, self.__url_reduce(site), site_user, site_hashed_pw):
				# This failed too. How sad
				return False
		
		return True

class PwdError(Exception):
	"""
	Thrown when an execption occurs with the password file
	"""
	def __init__(self, value):
		self.value = value
	
	def __str__(self):
		return self.value

