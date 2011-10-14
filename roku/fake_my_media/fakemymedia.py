#!/usr/bin/env python3
import sys
import cherrypy
import http.client
import socket
import os
import time
from urllib.parse import quote
from lxml import etree

class FakeMyMedia:
	def __init__(self, media_root, ip, port=8001):
		"""
		Register ourselves with the MyMedia rendevous server
		"""
		if not os.path.isdir(media_root):
			raise ValueError('Media root must be a directory')
		
		self.__media_root = os.path.abspath(media_root)
		self.__ip = ip
		self.__port = str(port)
		
		conn = http.client.HTTPConnection('rokumm.appstop.com', timeout=5)
		conn.request('POST', '/register', 'code=ttan&type=server&server=http://{}%3A{}'.format(self.__ip, self.__port))
		resp = conn.getresponse()
		body = resp.read()
		conn.close()
		
		if resp.status != 302:
			print('Error registering with rendezvous server:', resp.status)
	
	@cherrypy.expose
	def index(self):
		raise cherrypy.HTTPRedirect('/feed')
	
	@cherrypy.expose
	def feed(self, key=None, dir=None):
		"""
		Interface that MyMedia mostly talks on
		"""
		root = etree.Element('rss', version='2.0')

		if key is None:
			channel = etree.SubElement(root, 'channel')
			title = etree.SubElement(channel, 'title').text = 'FakeMyMedia Feed'
			
			# Only list videos folder
			item = etree.SubElement(channel, 'item')
			etree.SubElement(item, 'title').text = 'My Videos'
			etree.SubElement(item, 'image').text = 'http://{}:{}/images/video_square.jpg'.format(self.__ip, self.__port)
			etree.SubElement(item, 'link').text = 'http://{}:{}/feed?key={}&dir={}'.format(self.__ip, self.__port, 'video', '.')
		elif key == 'video':
			channel = etree.SubElement(root, 'channel')
			etree.SubElement(channel, 'title').text = 'Video Feed'
			etree.SubElement(channel, 'link').text = 'http://{}:{}/feed?key={}&dir={}'.format(self.__ip, self.__port, key, dir)
			etree.SubElement(channel, 'description').text = 'My Videos'
			etree.SubElement(channel, 'theme').text = 'video'
			etree.SubElement(channel, 'lastBuildDate').text = time.strftime('%a, %d %b %Y %H:%M:%S GMT')
			
			# Add each item in current media folder
			curr_path = os.path.join(self.__media_root, dir)
			for f in os.listdir(curr_path):
				# The "pub date" of the item
				full_path = os.path.join(curr_path, f)
				mod_time = time.strftime('%a, %d %b %Y %H:%M:%S GMT', time.gmtime(os.path.getmtime(full_path)))
				
				# Well it's an item of some kind
				item = etree.SubElement(channel, 'item')
				
				# Ignore anything but directories and mp4 files
				if os.path.isdir(f):
					etree.SubElement(item, 'title').text = f
					link = etree.SubElement(item, 'link')
					link.text = 'http://{}:{}/feed?key={}&dir={}'.format(self.__ip, self.__port, key, quote(full_path))
					etree.SubElement(item, 'description').text = 'Folder'
					guid = etree.SubElement(item, 'guid')
					guid.text = link.text
					guid.set('isPermaLink', 'false')
					etree.SubElement(item, 'pubDate').text = mod_time
				elif os.path.basename(f)[-3:] == 'mp4':
					etree.SubElement(item, 'title').text = f[:-4]
					link = etree.SubElement(item, 'link')
					link.text = 'http://{}/video/{}'.format(self.__ip, quote(f))
					etree.SubElement(item, 'filetype').text = 'mp4'
					etree.SubElement(item, 'ContentType').text = 'movie'
					etree.SubElement(item, 'StreamFormat').text = 'mp4'
					etree.SubElement(item, 'description').text = 'Video'
					enc = etree.SubElement(item, 'enclosure')
					enc.set('url', link.text)
					enc.set('length', str(os.path.getsize(full_path)))
					enc.set('type', 'video/mp4')
					guid = etree.SubElement(item, 'guid')
					guid.text = link.text
					guid.set('isPermaLink', 'false')
					etree.SubElement(item, 'pubDate').text = mod_time
			
		# Output whatever XML we produced
		return etree.tostring(root, pretty_print=True)
			
	@cherrypy.expose	
	def mytest(self, param_1=None, param_2=None, *args, **kw):
		return repr(dict(param_1=param_1,
			param_2=param_2,
			args=args,
			kw=kw))

# WSGI compatibility
def application(environ, start_response):
	"""
	Run as WSGI application
	"""
    cherrypy.config.update(environ['configuration'])
    cherrypy.tree.mount(FakeMyMedia('/var/samba/media/', environ['SERVER_NAME'], environ['SERVER_PORT']), script_name=environ['SCRIPT_NAME'], config=environ['configuration'])
    return cherrypy.tree(environ, start_response)

# Callable from command line
def main(argv=None):
	"""
	Run cherrypy when called on the command line
	"""
	cherrypy.config.update('prod.conf')
	cherrypy.tree.mount(FakeMyMedia('/var/samba/media/', cherrypy.config['server.socket_port']), '/', 'prod.conf')
	cherrypy.engine.start()
	cherrypy.engine.block()

if __name__ == '__main__':
	main(sys.argv)
