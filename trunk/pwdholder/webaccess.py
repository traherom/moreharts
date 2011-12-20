#!/usr/bin/env python3
import cherrypy
import re
import random
import sys
import os
import binascii
import time
import string
import json

try:
	import backend
	import session
except ImportError as e:
	# Probably running under Apache, which screws with import dirs
	sys.path[0] = os.path.dirname(__file__)
	import backend
	import session

class MainPage:
	def __init__(self, conn_str):
		"""
		Sets up database connection and initializes basic settings
		"""
		self.__pwdb = backend.PasswordHolder(conn_str)
		self.__db = self.__pwdb.get_db()
		
		# Default settings for sessions
		self.__default_settings = {'server_enc' : True}

	@cherrypy.expose
	def index(self):
		"""
		Loads up an HTML5 page that uses the JSON calls to handle everything
		"""		
		f = open(os.path.join(os.path.dirname(__file__), 'static/index.html'), 'rb')
		return f.read()
	
	@cherrypy.expose
	def login(self, user, pw):
		"""
		Logs a user in and establishes their session
		"""
		sess = session.Session(self.__db, secure=True)
		
		# Get the user
		try:
			user_id, enc_key = self.__pwdb.login(user, pw)
		except (ValueError, backend.PwdError):
			return self.__send_json(success=False, invalid_credentials=True, message='Invalid username/password')
		
		# Save session settings
		# If server_enc is True, we do encryption and decryption. Otherwise we assume
		# the client will handle it
		for key in self.__default_settings:
			sess.set(key, self.__default_settings[key])
		sess.set('user_id', user_id)
		
		# Yeah!
		return self.__send_json(success=True, enc_key=binascii.hexlify(enc_key).decode())
	
	@cherrypy.expose
	def logout(self):
		"""
		Removes an active session. Only drops the user's database from
		cache if it was the last session for that user
		"""
		# Kill session
		sess = session.Session(self.__db)
		sess.expire()
		
		# Success
		return self.__send_json(success=True, message='Logged Out')
	
	@cherrypy.expose
	def encryption_mode(self, server_enc=None, client_enc=None):
		"""
		Sets the encryption mode of the current session and/or returns
		the current setting. If server_enc is given, encryption of passwords
		occurs on the server side. If client_enc is given, server assumes client
		is encrypting. If nothing is given, current setting is returned.
		"""
		sess = self.__check_login()
		if sess is None:
			return self.__send_json(success=False, login_needed=True, message='Your session has expired')
		
		# Set
		if client_enc is not None:
			sess.set('server_enc', False)
		elif server_enc is not None:
			sess.set('server_enc', True)
		
		# Get
		return self.__send_json(success=True, server_enc=sess.get('server_enc'))
	
	@cherrypy.expose
	def get_password(self, site, enc_key=None):
		"""
		Retrieves a password from database for a user
		"""
		sess = self.__check_login()
		if sess is None:
			return self.__send_json(success=False, login_needed=True, message='Your session has expired')
		
		# Pull the info and send to user
		result = self.__pwdb.get_password(sess.get('user_id'), site)
		if result is None:
			return self.__send_json(success=False, no_password=True, message='No password stored for site')
		
		user, pw = result
		
		# Decrypt for them?
		if sess.get('server_enc'):
			if enc_key is not None:
				pw = self.__decrypt(enc_key, pw)
			else:
				return self.__send_json(success=False, no_key_given=True, message='Session using server-side encryption, no encryption key given')
		
		# Send
		return self.__send_json(success=True, site=site, site_user=user, site_pw=pw)
	
	@cherrypy.expose
	def set_password(self, site, site_user, site_pw, enc_key=None):
		"""
		Sets/resets a password for a site
		"""
		sess = self.__check_login()
		if sess is None:
			return self.__send_json(success=False, message='Your session has expired')
		
		# Encrypt for them?
		if sess.get('server_enc'):
			if enc_key is not None:
				site_pw = self.__encrypt(enc_key, site_pw)
			else:
				return self.__send_json(success=False, no_key_given=True, message='Session using server-side encryption, no encryption key given')
		
		# Set password and force a write
		self.__pwdb.set_password(sess.get('user_id'), site, site_user, site_pw)
		
		# Yeah!
		return self.__send_json(success=True, message='New password saved')
	
	@cherrypy.expose
	def who_am_i(self):
		"""
		Lets a client retrieve the user name of the session they have
		"""
		sess = self.__check_login()
		if sess is None:
			return self.__send_json(success=False, message='Your session has expired')
		
		user = self.__pwdb.get_user_info(sess.get('user_id'))
		if user is not None:
			return self.__send_json(success=True, username=user)
		else:
			return self.__send_json(success=False, message='User ID not found')
	
	@cherrypy.expose
	def generate_password(self, min_len=12, max_len=25, extra_chars=''):
		"""
		Simply returns a random password matching the
		requirements specified. Numbers and letters are
		always assumed, extra characters can be given. 
		session_id may be given but is ignored.
		"""
		# Permitted characters
		charset = string.ascii_letters + string.digits + extra_chars
		
		# Length
		min_len = int(min_len)
		max_len = int(max_len)
		if min_len < max_len:
			length = random.randrange(min_len, max_len)
		else:
			length = min_len
		
		# Generate
		pw = ''.join([random.choice(charset) for i in range(length)])
		
		# Return to user
		return self.__send_json(success=True, password=pw)
	
	def __check_login(self, sess=None):
		"""
		Ensures the current user is logged in. Returns current
		session if they are, None otherwise
		"""
		# Create session if needed
		if sess is None:
			sess = session.Session(self.__db)
		
		# Does it have values?
		try:
			sess.get('user_id')			
			return sess
		except KeyError as e:
			return None
	
	def __encrypt(self, key, data):
		"""
		Encrypts given data using hex version of encryption key
		"""
		return self.__pwdb.encrypt(binascii.unhexlify(key.encode()), data)
		
	def __decrypt(self, key, data):
		"""
		Decrypts given data using hex version of encryption key
		"""
		return self.__pwdb.decrypt(binascii.unhexlify(key.encode()), data)
	
	def __send_json(self, **kwargs):
		"""
		Returns the given variables as a JSON file and sets the content-type
		appropriately. The caller still must return it to cherrypy though
		"""
		cherrypy.response.headers["Content-Type"] = "application/json"
		return json.dumps(kwargs).encode()

# WSGI
def application(environ, start_response):
	"""
	Run as WSGI application.
	
	WARNING: It appears that the app gets reloaded for each request under Apache. Process forking may be the cause.
	This prevents the app from actually working. However, fixing this would be entirely possible CherryPy sessions
	were used and the password files were decrypted each time. The recommended setup is to instead run it behind
	mod_rewrite
	"""
	cherrypy.config.update(environ['configuration'])
	cherrypy.tree.mount(MainPage(cherrypy.config['pwdb.connection']), script_name=environ['SCRIPT_NAME'], config=environ['configuration'])
	return cherrypy.tree(environ, start_response)

# Standalone
def main(argv=None):
	"""
	Run password holder as a standalone cherrypy app
	"""
	cherrypy.config.update('prod.conf')
	cherrypy.tree.mount(MainPage(cherrypy.config['pwdb.connection']), '/', 'prod.conf')
	cherrypy.engine.start()
	cherrypy.engine.block()
	return 0
	
if __name__ == '__main__':
	sys.exit(main(sys.argv))
	