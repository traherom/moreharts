#!/usr/bin/env python3
import cherrypy
import pwdholder
import re
import random
import sys
import os
import binascii
import time
from mako.template import Template
from mako.lookup import TemplateLookup

class MainPage:
	def __init__(self, save_location='.'):
		# Directory location for password files
		if not os.path.isdir(save_location):
			raise IOError('Save location is not directory or does not exist')
			
		self.__save_location = save_location
		
		# Logged-in user tracking
		self.__curr_users = {}
		
		# Save template lookup info
		self.__lookup = TemplateLookup(directories=[os.path.join(os.path.dirname(__file__), 'templates')])

	@cherrypy.expose
	def index(self):
		"""
		Loads up an HTML5 page that uses the JSON calls to handle everything
		"""
		template = self.__lookup.get_template('index.html')
		return template.render()
	
	@cherrypy.expose
	def login(self, user, pw):
		"""
		Logs a user in and caches their password database
		"""
		# Get teh user
		try:
			pw_holder = self.__get_user(user, pw)
		except ValueError as e:
			template = self.__lookup.get_template('message.json')
			return template.render(success=False, message=str(e))
		except pwdholder.PwdFileError as e:
			template = self.__lookup.get_template('message.json')
			return template.render(success=False, message=str(e))
			
		# Create unused session ID
		sid = binascii.hexlify(os.urandom(32)).decode()
		
		# Save to cache with the start time of the session
		# Start time allows us to timeout sessions easily
		self.__curr_users[sid] = [pw_holder, time.time()] # Include IP? TBD
		
		# Yeah!
		template = self.__lookup.get_template('session_start.json')
		return template.render(success=True, sid=sid)
	
	@cherrypy.expose
	def logout(self, sid):
		"""
		Removes an active session. Only drops the user's database from
		cache if it was the last session for that user
		"""
		# Ensure it is saved
		self.__curr_users[sid].save_passwords()
		
		# And remove
		del(self.__curr_users[sid])
		
		template = self.__lookup.get_template('message.json')
		return template.render(success=True, message='Logged Out')
	
	@cherrypy.expose
	def get_password(self, sid, site):
		"""
		Retrieves a password from a user's database
		"""
		# Pull the info and send to user
		pw_holder = self.__curr_user[sid]
		site_user, site_pw = pw_holder.get_password(site)
		
		template = self.__lookup.get_template('site_info.json')
		return template.render(success=True, site=site, site_user=site_user, site_pw=site_pw)
		
	@cherrypy.expose
	def set_password(self, sid, site, site_user, site_pw):
		"""
		Sets/resets a password for a site
		"""
		# Set password and force a write
		pw_holder = self.__curr_user[sid]
		pw_holder.set_password(site, site_user, site_pw)
		pw_holder.save_passwords()
		
		# Yeah!
		template = self.__lookup.get_template('message.json')
		return template.render(success=False, message='User info not provided')
	
	@cherrypy.expose
	def generate_password(self, min_len=12, max_len=25, extra_chars=''):
		"""
		Simply returns a random password matching the
		requirements specified. Numbers and letters are
		always assumed, extra characters can be given
		"""
		charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
		charset += extra_chars
		
		length = random.randrange(min_len, max_len)
		pw = ''.join([random.choice(charset) for i in range(length)])
		
		# Return to user
		template = self.__lookup.get_template('password.json')
		return template.render(success=True, password=pw)
		
	def __get_user(self, user, pw):
		"""
		Opens a password file
		"""
		# Valid user name, character wise?
		if re.match('[a-zA-Z0-9_]+', user) is None:
			# Failed
			raise ValueError('Invalid user name')
		
		# Attempt to log the user in
		pw_loc = os.path.join(self.__save_location, user + '.pwdb')
		if not self.__isChildOf(self.__save_location, pw_loc):
			raise ValueError('Directory traversal possible')
		
		# Try to open
		return pwdholder.PasswordHolder(pw_loc, pw, False)
	
	def __isChildOf(self, parent_dir, child_path):
		"""
		Checks if the given path is a child of the parent. Returns
		True if it is, False otherwise
		"""
		if not os.path.isdir(parent_dir):
			raise ValueError('Only directories may be checked')
			
		parent_dir = os.path.abspath(parent_dir)
		child_path = os.path.abspath(child_path)
		
		if os.path.commonprefix([parent_dir, child_path]) == parent_dir:
			return True
		else:	
			return False

# WSGI
def application(environ, start_response):
	"""
	Run as WSGI application
	"""
	cherrypy.config.update(environ['configuration'])
	cherrypy.tree.mount(MainPage(), script_name=environ['SCRIPT_NAME'], config=environ['configuration'])
	return cherrypy.tree(environ, start_response)

# Standalone
def main(argv=None):
	"""
	Run password holder as a standalone cherrypy app
	"""
	if argv is None:
		argv = sys.argv
		
	cherrypy.config.update('prod.conf')
	cherrypy.tree.mount(MainPage(), '/', 'prod.conf')
	cherrypy.engine.start()
	cherrypy.engine.block()

if __name__ == '__main__':
	main(sys.argv)
	
