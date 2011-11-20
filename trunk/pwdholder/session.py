import cherrypy
import time
import random
import string
import datetime
import binascii
try:
	import cPickle
except ImportError:
	import pickle

class Session:
	"""
	CherryPy sessions with Postgres seem... broken. At least I can't
	figure out how to get them to load properly, it dies on a seemingly
	improper "self.get_db()" call... in a class method, where self doesn't
	exist.
	"""
	def __init__(self, db, cookie_name='session', path='/', age_limit=604800, secure=False):
		"""
		Open existing session if the user has a cookie or
		set new cookie if they do not have one yet
		if 
		"""
		self.__db = db
		self.__cookie_name = cookie_name
		
		delta = datetime.timedelta(seconds=age_limit)
		self.__expiration = datetime.datetime.now() + delta
		
		self.__values = {}
		self.__is_changed = False
		
		# Get SID
		try:
			self.__sid = cherrypy.request.cookie[cookie_name].value
		except KeyError:
			# No session yet, generate random sid and send
			self.__sid = ''.join([random.choice(string.ascii_letters + string.digits) for i in range(32)])
			cherrypy.response.cookie[cookie_name] = self.__sid
			cherrypy.response.cookie[cookie_name]['path'] = path
			cherrypy.response.cookie[cookie_name]['max-age'] = delta.total_seconds()
			cherrypy.response.cookie[cookie_name]['expires'] = self.__expiration.strftime('%a, %d %b %Y %M:%H:%S %Z')
			if secure:
				cherrypy.response.cookie[cookie_name]['secure'] = True
		
		# Load from database
		self.load()
	
	def __del__(self):
		"""
		Saves session (if needed) before kill instance
		"""
		if self.__is_changed:
			self.save()
	
	def load(self):
		"""
		(Re)loads current session from the database. If changes have been made since the last load,
		they are discarded
		"""
		# Empty session by default
		self.__values = {}
		self.__is_changed = False
		
		# Get from database
		load = self.__db.prepare('SELECT data, expiration_time FROM session WHERE id=$1')
		row = load.first(self.__sid)
			
		# Check that it's not expired
		if row is not None and datetime.datetime.now() < row[1]:
			# Restore values
			self.__values = pickle.loads(binascii.unhexlify(row[0].encode()))
			
			# Unchanged so far
			self.__is_changed = False
		elif row is not None:
			self.__is_changed = True
				
	def save(self):
		"""
		Saves the current session to the database. Does so regardless of changes
		"""
		data = binascii.hexlify(pickle.dumps(self.__values)).decode()
		expire = self.__expiration.strftime('%Y-%m-%d %H:%M:%S')
		
		update = self.__db.prepare('UPDATE session SET data=$2, expiration_time=$3::text::timestamp WHERE id=$1')
		if not update.first(self.__sid,	data, expire):
			# Update failed, must not have already been in database
			insert = self.__db.prepare('INSERT INTO session (id, data, expiration_time) VALUES ($1, $2, $3::text::timestamp)')
			if not insert.first(self.__sid,	data, expire):
				# This failed too. How sad
				raise SessionError('Unable to save session')
		
		self.__is_changed = False
	
	def expire(self):
		"""
		Ends the current session
		"""
		# End cookie
		cherrypy.response.cookie[self.__cookie_name] = ''
		cherrypy.response.cookie[self.__cookie_name]['expires'] = datetime.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
		
		# Remove session from DB
		remove = self.__db.prepare('DELETE FROM session WHERE id=$1')
		remove(self.__sid)
		
		# And change ourselves
		self.__values = {}
		self.__is_changed = False
	
	def set(self, name, val):
		"""
		Set value for given name
		"""
		self.__values[name] = val
		self.__is_changed = True
		
	def get(self, name):
		"""
		Get value for given name
		"""
		return self.__values[name]
		
	def remove(self, name):
		"""
		Removes given name entirely
		"""
		del self.__values[name]
		self.__is_changed = True

def SessionError(Exception):
	"""
	Thrown when an execption occurs with the session
	"""
	def __init__(self, value):
		self.value = value
	
	def __str__(self):
		return self.value
	