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
	def savetorrent(self, torrent_file, email=None):
		"""
		Saves the uploaded torrent file and associated email address
		"""
		# Drop the torrent file into "to be downloaded" directory
		handle, torrent_path = tempfile.mkstemp('.torrent', 'fmm-', os.path.join(self.__download_root, 'torrents'))
		new_file = os.fdopen(handle, 'w')
		
		# Copy uploaded data in
		for line in torrent_file.file:
			new_file.write(str(line))
			
		# Ensure the download portion can actually read the files
		new_file.close()
		os.chmod(torrent_path, stat.S_IROTH | stat.S_IRUSR | stat.S_IRGRP)
		
		# Make a corresponding .email file as a hacky thing to know who to email once a torrent completes
		if email is not None:
			email_path = new_path + '.email'
			email_file = open(email_path, 'w')
			email_file.write(email)
			email_file.close()
			os.chmod(email_path, stat.S_IROTH | stat.S_IRUSR | stat.S_IRGRP)
		
		# Success
		template = self.__lookup.get_template('saved.html')
		return template.render()
	
	@cherrypy.expose
	def feed(self, dir=None, sort=None, limit=None):
		"""
		Interface that MyMedia mostly talks on
		"""
		root = etree.Element('rss', version='2.0')

		if dir is None:
			channel = etree.SubElement(root, 'channel')
			title = etree.SubElement(channel, 'title').text = 'FakeMyMedia Feed'
			
			# List "shortcut" folders
			item = etree.SubElement(channel, 'item')
			etree.SubElement(item, 'title').text = 'Newest 10'
			etree.SubElement(item, 'image').text = 'http://{}:{}/images/videos_square.jpg'.format(self.__ip, self.__port)
			etree.SubElement(item, 'link').text = 'http://{}:{}/feed?dir={}&sort={}&limit={}'.format(self.__ip, self.__port, '.', 'newtoold', 10)
			
			item = etree.SubElement(channel, 'item')
			etree.SubElement(item, 'title').text = 'Recently Watched'
			etree.SubElement(item, 'image').text = 'http://{}:{}/images/videos_square.jpg'.format(self.__ip, self.__port)
			etree.SubElement(item, 'link').text = 'http://{}:{}/feed?dir={}&sort={}&limit={}'.format(self.__ip, self.__port, '.', 'watchedtonot', 10)
			
			item = etree.SubElement(channel, 'item')
			etree.SubElement(item, 'title').text = 'Not Recently Watched'
			etree.SubElement(item, 'image').text = 'http://{}:{}/images/videos_square.jpg'.format(self.__ip, self.__port)
			etree.SubElement(item, 'link').text = 'http://{}:{}/feed?dir={}&sort={}&limit={}'.format(self.__ip, self.__port, '.', 'nottowatched', 10)
			
			# Complete movie listing
			item = etree.SubElement(channel, 'item')
			etree.SubElement(item, 'title').text = 'All Movies'
			etree.SubElement(item, 'image').text = 'http://{}:{}/images/videos_square.jpg'.format(self.__ip, self.__port)
			etree.SubElement(item, 'link').text = 'http://{}:{}/feed?dir={}'.format(self.__ip, self.__port, '.')
		else:
			channel = etree.SubElement(root, 'channel')
			etree.SubElement(channel, 'title').text = 'Video Feed'
			etree.SubElement(channel, 'link').text = 'http://{}:{}/feed?dir={}'.format(self.__ip, self.__port, dir)
			etree.SubElement(channel, 'description').text = 'My Videos'
			etree.SubElement(channel, 'theme').text = 'video'
			etree.SubElement(channel, 'lastBuildDate').text = time.strftime('%a, %d %b %Y %H:%M:%S GMT')
			
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
				# Well it's an item of some kind
				item = etree.SubElement(channel, 'item')
				
				# Ignore anything but directories and mp4 files
				if f.is_dir:
					etree.SubElement(item, 'title').text = f.name
					link = etree.SubElement(item, 'link')
					link.text = 'http://{}:{}/feed?dir={}'.format(self.__ip, self.__port, quote(f.short_path))
					etree.SubElement(item, 'description').text = 'Folder'
					guid = etree.SubElement(item, 'guid')
					guid.text = link.text
					guid.set('isPermaLink', 'false')
					etree.SubElement(item, 'pubDate').text = time.strftime('%a, %d %b %Y %H:%M:%S GMT', time.gmtime(f.mtime))
				elif f.ext == '.mp4':
					etree.SubElement(item, 'title').text = f.name
					link = etree.SubElement(item, 'link')
					link.text = 'http://{}/video/{}'.format(self.__ip, quote(f.short_path))
					etree.SubElement(item, 'filetype').text = 'mp4'
					etree.SubElement(item, 'ContentType').text = 'movie'
					etree.SubElement(item, 'StreamFormat').text = 'mp4'
					etree.SubElement(item, 'description').text = 'Video'
					enc = etree.SubElement(item, 'enclosure')
					enc.set('url', link.text)
					enc.set('length', str(f.size))
					enc.set('type', 'video/mp4')
					guid = etree.SubElement(item, 'guid')
					guid.text = link.text
					guid.set('isPermaLink', 'false')
					etree.SubElement(item, 'pubDate').text = time.strftime('%a, %d %b %Y %H:%M:%S GMT', time.gmtime(f.mtime))
		
			# Maybe we'd want to add a "show all" link here if we limited the display?
			if limited:
				item = etree.SubElement(channel, 'item')
				etree.SubElement(item, 'title').text = 'Show All'
				link = etree.SubElement(item, 'link')
				link.text = 'http://{}:{}/feed?dir={}&sort={}'.format(self.__ip, self.__port, quote(dir), quote(sort))
				etree.SubElement(item, 'description').text = 'Show More Videos'
				guid = etree.SubElement(item, 'guid')
				guid.text = link.text
				guid.set('isPermaLink', 'false')
				etree.SubElement(item, 'pubDate').text = time.strftime('%a, %d %b %Y %H:%M:%S GMT')
		
		# Output whatever XML we produced
		return etree.tostring(root, pretty_print=True)
	
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
