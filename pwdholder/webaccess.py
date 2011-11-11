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
		self.session_length = 60 * 60 * 24 * 7
		self.max_sessions = 10
		
		# Logged-in user tracking
		self.__sessions = {}
		self.__users = {}
		
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
			return template.render(success=False, message='Invalid username/password')
		except pwdholder.PwdFileError as e:
			template = self.__lookup.get_template('message.json')
			return template.render(success=False, message='Invalid username/password')
		
		# Take this opportunity to expire old sessions
		self.expire_sessions(time.time() - self.session_length)
		
		# Create unused session ID
		while True:
			sid = binascii.hexlify(os.urandom(32)).decode()
			if sid not in self.__sessions:
				break
		
		# Was this user already logged in? If they are, then continue to use that object,
		# just associate it with the new session as well
		if user in self.__users:
			# Has this user exceeded the maximum number of active sessions?
			pw_holder, count = self.__users[user]
			if count >= 10:
				# They have! Remove their oldest session
				print(user, 'has exceeded their maximum number of logins')
				
				oldest_age = None
				oldest_sid = None
				for sid in self.__sessions:
					session = self.__sessions[sid]
					if session[1] == user and (oldest_age == None or oldest_age > session[2]):
						# Found a new oldest
						oldest_age = session[2]
						oldest_sid = sid
				
				# And drop it
				self.remove_session(oldest_sid)
				count = count - 1
				
			# Insert their new session
			print("Dupe login for {}, now at {} sessions".format(user, count + 1))
			self.__users[user] = (pw_holder, count + 1)
		else:
			# They weren't, so save this new db to the user's lookup
			print("New login for {}".format(user))
			self.__users[user] = (pw_holder, 1)
		
		# Save to cache with the start time of the session
		# Start time allows us to timeout sessions
		self.__sessions[sid] = [pw_holder, user, time.time()]
		
		# Yeah!
		template = self.__lookup.get_template('session_start.json')
		return template.render(success=True, sid=sid)
	
	@cherrypy.expose
	def logout(self, sid):
		"""
		Removes an active session. Only drops the user's database from
		cache if it was the last session for that user
		"""
		self.remove_session(sid)
		
		# Take this opportunity to expire old sessions
		self.expire_sessions(time.time() - self.session_length)
		
		# Success
		template = self.__lookup.get_template('message.json')
		return template.render(success=True, message='Logged Out')
	
	@cherrypy.expose
	def reload_passwords(self, sid):
		"""
		Forces cache to refresh from disk
		"""
		pw_holder = self.__sessions[sid][0]
		pw_holder.reload_passwords()
		
		template = self.__lookup.get_template('message.json')
		return template.render(success=False, message='Passwords reloaded')
	
	@cherrypy.expose
	def save_passwords(self, sid):
		"""
		Forces cache to flush to disk
		"""
		try:
			pw_holder = self.__sessions[sid][0]
		except KeyError as e:
			template = self.__lookup.get_template('message.json')
			return template.render(success=False, message='Your session has expired')
			
		pw_holder.save_passwords()
		
		template = self.__lookup.get_template('message.json')
		return template.render(success=False, message='Passwords saved')
		
	@cherrypy.expose
	def get_password(self, sid, site):
		"""
		Retrieves a password from a user's database
		"""
		try:
			pw_holder = self.__sessions[sid][0]
		except KeyError as e:
			template = self.__lookup.get_template('message.json')
			return template.render(success=False, message='Your session has expired')
			
		# Pull the info and send to user
		result = pw_holder.get_password(site)
		
		if result is not None:
			template = self.__lookup.get_template('site_info.json')
			return template.render(success=True, site=site, site_user=result[0], site_pw=result[1])
		else:
			# Not Found
			template = self.__lookup.get_template('message.json')
			return template.render(success=False, message='No password stored for site')
			
	@cherrypy.expose
	def set_password(self, sid, site, site_user, site_pw):
		"""
		Sets/resets a password for a site
		"""
		try:
			pw_holder = self.__sessions[sid][0]
		except KeyError as e:
			template = self.__lookup.get_template('message.json')
			return template.render(success=False, message='Your session has expired')
			
		# Set password and force a write
		pw_holder.set_password(site, site_user, site_pw)
		pw_holder.save_passwords()
		
		# Yeah!
		template = self.__lookup.get_template('message.json')
		return template.render(success=True, message='New password saved')
	
	@cherrypy.expose
	def generate_password(self, min_len=12, max_len=25, extra_chars='', session_id=None):
		"""
		Simply returns a random password matching the
		requirements specified. Numbers and letters are
		always assumed, extra characters can be given. 
		session_id may be given but is ignored.
		"""
		charset = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789"
		charset += extra_chars
		
		length = random.randrange(min_len, max_len)
		pw = ''.join([random.choice(charset) for i in range(length)])
		
		# Return to user
		template = self.__lookup.get_template('password.json')
		return template.render(success=True, password=pw)
	
	def expire_sessions(self, oldest_time):
		"""
		Expires sessions older than the given time
		"""
		for sid in list(self.__sessions.keys()):
			if self.__sessions[sid][2] < oldest_time:
				print('Expiring session', sid)
				self.remove_session(sid)
	
	def remove_session(self, sid):
		"""
		Removes the given session
		"""
		try:
			# Pull out session data and invalidate it
			pw_holder, user, start_time = self.__sessions[sid]
			del(self.__sessions[sid])
		
			# Ensure db is saved
			pw_holder.save_passwords()
		
			# Decrement and possibly remove user cache
			pw_holder, count = self.__users[user]
			if count > 1:
				self.__users[user] = (pw_holder, count - 1)
				print("Logout for {}, now at {} sessions".format(user, count - 1))
			else:
				del(self.__users[user])
				print("Final logout for {}".format(user))
			
			return True
			
		except KeyError as e:
			# Likely an invalid session, ignore and show normal success
			return False
	
	def __get_user(self, user, pw):
		"""
		Opens a password file for the given user
		"""
		# Valid user name, character wise?
		if re.match('[a-zA-Z0-9_]+', user) is None:
			# Failed
			raise ValueError('Invalid user name')
		
		# Attempt to log the user in
		pwdb_loc = os.path.join(self.__save_location, user + '.pwdb')
		if not self.__isChildOf(self.__save_location, pwdb_loc):
			raise ValueError('Directory traversal possible')
	
		# Try to open
		return pwdholder.PasswordHolder(pwdb_loc, master_pw=pw, new_db=False)
	
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
	
