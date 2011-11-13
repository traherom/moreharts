#!/usr/bin/env python3
import sys
import cherrypy
import http.client
import socket
import os
import time
import tempfile
import stat
from urllib.parse import quote
from lxml import etree
from mako.template import Template
from mako.lookup import TemplateLookup

class FakeMyMedia:
	def __init__(self, media_root, download_root, ip, port=8001):
		"""
		Setup fake server
		"""
		if not os.path.isdir(media_root):
			raise ValueError('Media root must be a directory')
		if not os.path.isdir(download_root):
			raise ValueError('Download root must be a directory')
		
		self.__media_root = os.path.abspath(media_root)
		self.__download_root = os.path.abspath(download_root)
		self.__ip = ip
		self.__port = str(port)
		
		# Register with the rendevous server
		conn = http.client.HTTPConnection('rokumm.appspot.com', timeout=5)
		conn.set_debuglevel(0)
		conn.request('POST', '/register',
			'code=ttan&type=server&server=http://{}%3A{}'.format(self.__ip, self.__port),
			headers={'Content-Type' : 'application/x-www-form-urlencoded'})
		resp = conn.getresponse()
		body = resp.read()
		conn.close()
		
		if resp.status != 302:
			print('Error registering with rendezvous server:', resp.status)
			
		# Lookup engine for the user-facing pages (RSS stuff just built by hand)
		self.__lookup = TemplateLookup(directories=[os.path.join(os.path.dirname(__file__), 'html')])
	
	@cherrypy.expose
	def index(self):
		"""
		Allows the user to upload a torrent file to download
		"""
		template = self.__lookup.get_template('main.html')
		return template.render()
		
	@cherrypy.expose
	def savetorrent(self, torrent_file):
		"""
		Saves the uploaded torrent file and associated email address
		"""
		# Drop the torrent file into "to be downloaded" directory
		handle, torrent_path = tempfile.mkstemp('.torrent', 'fmm-', os.path.join(self.__download_root, 'torrents'))

		# Copy by reading/writing files
		with os.fdopen(handle, 'wb') as new_file:
			torrent_file.file.seek(0)
			while True:
				b = torrent_file.file.read(8192)
				if not b:
					break
				new_file.write(b)
			
		# Ensure the download portion can actually read the files
		os.chmod(torrent_path, stat.S_IROTH | stat.S_IRUSR | stat.S_IRGRP)
		
		# Success
		template = self.__lookup.get_template('saved.html')
		return template.render()

	@cherrypy.expose
	def play(self, video):
		"""
		Plays the given video in an HTML5 player
		"""
		template = self.__lookup.get_template('player.html')
		return template.render(video_title=os.path.basename(video), video_url=os.path.join('video', video))

	@cherrypy.expose
	def feed(self, dir=None, sort=None, limit=None):
		"""
		Interface for browsing videos
		"""
		class MainInfo:
			baseurl = None
			title = None
			url = None
			
		class Item:
			name = None
			image = None
			url = None
			date = None
			size = None
		
		# Info for templates
		main = MainInfo()
		main.baseurl = 'http://{}:{}'.format(self.__ip, self.__port)	
		
		folders = []
		videos = []
			
		if dir is None:
			main.url = '/feed'
			main.title = 'Videos'
			
			folders = []
			
			x = Item()
			x.name = 'Newest 10'	
			x.image = '/images/videos_square.jpg'
			x.url = '/feed?dir={}&sort={}&limit={}'.format('.', 'newtoold', 10)
			folders.append(x)
			
			x = Item()
			x.name = 'Recently Watched'
			x.image = '/images/videos_square.jpg'
			x.url = '/feed?dir={}&sort={}&limit={}'.format('.', 'watchedornot', 10)
			folders.append(x)
			
			x = Item()
			x.name = 'Not Recently Watched'
			x.image = '/images/videos_square.jpg'
			x.url = '/feed?dir={}&sort={}&limit={}'.format('.', 'nottowatched', 10)
			folders.append(x)
		
			x = Item()
			x.name = 'All Movies'
			x.image = '/images/videos_square.jpg'
			x.url = '/feed?dir={}'.format('.')
			folders.append(x)
		else:
			main.title = 'Video Feed'
			main.url = '/feed?dir={}'.format(dir)
			
			# Ensure they aren't having us enumerate a directory they shouldn't
			curr_path = os.path.abspath(os.path.join(self.__media_root, dir))
			if not self.__isChildOf(self.__media_root, curr_path):
				raise ValueError('Invalid directory specified')
						
			# Get list of files, pull out their info, and process for order and what-not
			files = [self.FileInfo(curr_path, f) for f in os.listdir(curr_path)]
			
			if sort == 'newtoold':
				files.sort(key=lambda f: f.mtime, reverse=True)
			elif sort == 'oldtonew':
				files.sort(key=lambda f: f.mtime)
			elif sort == 'watchedtonot':
				files.sort(key=lambda f: f.atime, reverse=True)
			elif sort == 'nottowatched':
				files.sort(key=lambda f: f.atime)
			else:
				files.sort(key=lambda f: f.name)
			
			# Restrict number shown?
			limited = False
			if limit is not None:
				limit = int(limit)
				if limit < len(files) and limit > 0:
					files = files[:limit]
					limited = True
			
			# Add each item to list
			for f in files:
				# Ignore anything but directories and mp4 files
				if f.is_dir:
					item = Item()
					item.name = f.name
					item.url = '/feed?dir={}'.format(quote(f.short_path))
					item.date = time.strftime('%a, %d %b %Y %H:%M:%S GMT', time.gmtime(f.mtime))
					folders.append(item)
				elif f.ext == '.mp4':
					item = Item()
					item.name = f.name
					item.url = '/video/{}'.format(quote(f.short_path))
					item.size = f.size
					item.date = time.strftime('%a, %d %b %Y %H:%M:%S GMT', time.gmtime(f.mtime))
					videos.append(item)
		
		# Choose correct template
		if cherrypy.request.headers.get('User-Agent', '').find('Roku') != -1:
			template = self.__lookup.get_template('main_feed.xml')
		else:
			template = self.__lookup.get_template('listing.html')
			
		# And render
		return template.render(main=main, folders=folders, videos=videos)
	
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
		
	class FileInfo:
		def __init__(self, base, name):
			"""
			Stores all file info for quick reference
			"""
			# Common to both types
			self.short_path = name
			self.path = os.path.join(base, name)
			self.name = os.path.split(self.path)[1]
			self.mtime = os.path.getmtime(self.path)
			self.atime = os.path.getatime(self.path)
			self.ctime = os.path.getctime(self.path)
			
			if os.path.isdir(self.path):
				# Dir info
				self.is_dir = True
			else:
				# File info
				self.is_dir = False
				self.size = os.path.getsize(self.path)
				self.ext = os.path.splitext(self.path)[1]
		
		def __str__(self):
			return self.path
		
# WSGI compatibility
def application(environ, start_response):
	"""
	Run as WSGI application
	"""
	cherrypy.config.update(environ['configuration'])
	cherrypy.tree.mount(FakeMyMedia('/var/samba/media/', '/home/traherom/downloads/',
		environ['SERVER_NAME'], environ['SERVER_PORT']),
		script_name=environ['SCRIPT_NAME'], config=environ['configuration'])
	return cherrypy.tree(environ, start_response)

# Callable from command line
def main(argv=None):
	"""
	Run cherrypy when called on the command line
	"""
	cherrypy.config.update('prod.conf')
	cherrypy.tree.mount(FakeMyMedia('/var/samba/media/', os.path.expanduser('~/downloads/'), '192.168.1.50', cherrypy.config['server.socket_port']), '/', 'prod.conf')
	cherrypy.engine.start()
	cherrypy.engine.block()

if __name__ == '__main__':
	main(sys.argv)
