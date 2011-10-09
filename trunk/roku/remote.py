#!/usr/bin/env python3
import sys
import argparse
import socket
import time
import locale
import http.client

# Get the best XML lib available
try:
	from lxml import etree
except ImportError:
	print('Please ensure python3-lxml is installed.')
	exit(1)

# Interface imports
try:
	import curses
	import curses.wrapper

	curses_available = False
except ImportError:
	curses_available = False

class RokuRemoteUI:
	"""
	All Roku UI's should implement this
	"""
	def start(self):
		raise NotImplementedError("Implement me")

class RokuRemoteCurses(RokuRemoteUI):
	"""
	Terminal-based UI
	"""
	def __init__(self, remote):
		"""
		Save off the given remote
		"""
		self.__remote = remote

	def start(self):
		"""
		Initialize curses
		"""
		locale.setlocale(locale.LC_ALL, '')
		self.__locale_code = locale.getpreferredencoding()

		curses.wrapper(self.__input_monitor)

	def __input_monitor(self, stdscr):
		"""
		Draw display and watch for keys from curses
		"""
		max_row, max_col = stdscr.getmaxyx()
		mid_row = max_row / 2
		mid_col = max_col / 2
		
		ui =	(('Connected to ' + self.__remote.get_roku_id()),
				(()),
				(('Up')),
				(('Left', 'OK', 'Right')),
				(('Down')))
		line_num = 0
		for line in ui:
			# Combine and print it out
			s = '  '.join(line)
			stdscr.addstr(line_num, round(mid_col + len(s) / 2), s)
			
			# New line
			line_num = line_num + 1
		
		while True:
			k = stdscr.getkey()
			stdscr.addstr(str(k))

class RokuRemotePlainTerminal(RokuRemoteUI):
	"""
	Terminal-based UI
	"""
	def __init__(self, remote):
		"""
		Save off the given remote
		"""
		self.__remote = remote

	def start(self):
		"""
		Show prompt, accept single-letter commands
		"""
		while True:
			cmd = input('> ')
			if cmd == 'l':
				self.__remote.send_left()
			elif cmd == 'r':
				self.__remote.send_right()
			elif cmd == 'u':
				self.__remote.send_up()
			elif cmd == 'd':
				self.__remote.send_down()
			elif cmd == 'ff':
				self.__remote.send_ff()
			elif cmd == 'rwd':
				self.__remote.send_rwd()
			elif cmd == 'h':
				self.__remote.send_home()
			elif cmd == 'i':
				self.__remote.send_info()
			elif cmd == 'p':
				self.__remote.send_pause()
			elif cmd == 'search':
				phrase = input('Search for: ')
				self.__remote.send_search(phrase)
			elif cmd == 'secret':
				self.__remote.open_secret_screen()
			elif cmd == 'bitrate':
				self.__remote.open_bitrate_screen()
			else:
				# Send raw command
				self.__remote.send(cmd)
				print('Response:', self.__remote.read_to_prompt())

class RokuRemote:
	"""
	Ties together the UI and connection to the Roku
	"""
	__ROKU_PORT = 8060

	def __init__(self, argv):
		"""
		Creates the remote GUI and connection, unless the user specified just a few commands
		to run on the command line
		"""
		# Parse command line
		parser = argparse.ArgumentParser(description='Remote for the Roku media player')
		parser.add_argument('ip', metavar='IP Address', nargs='*')
		res = parser.parse_args(argv[1:])

		if len(res.ip) != 0:
			self.__ip = res.ip[0]
		else:
			self.__ip = None

		# IP given?
		if self.__ip is not None:
			# Ensure it's valid and connect
			model = self.__get_roku_info()

			if model is not None:
				self.__roku_model = model
			else:
				print('The given IP address (', self.__ip, ') does not appear to be a valid device', sep='')
				quit()
		else:
			# Attempt to locate
			if not self.locate_roku():
				print('Unable to locate Roku on local network, please specify IP or host name on the command line')
				quit()

	def __get_roku_info(self):
		"""
		Retrieves the player information from the Roku. Returns None if it is
		not a valid Roku device
		"""
		home_page = self.send('/', request_type='GET')

		# Should be an XML file
		tree = etree.fromstring(home_page)
		m = self.__find_in_response(tree, '/a:root/a:device/a:modelName')
		if len(m) == 1:
			return m[0].text
		else:
			return None

	def __find_in_response(self, tree, xpath):
		"""
		Finds the element at the given path from the given element.
		Useful as it defines the stupid namespaces that roku uses
		"""
		return tree.xpath(xpath, namespaces = {
			'a' : 'urn:schemas-upnp-org:device-1-0'
			})

	def locate_roku(self):
		"""
		Static function that returns the first IP address of a system on the local network
		that has port 8080 open
		"""
		print('Searching for Roku on local network (assuming 192.168.1.0/24)... Trying .   ', end='')
		sys.stdout.flush()

		for i in range(1, 254):
			try:
				# Status message
				print('\b\b\b' + format(i, '03d'), end='')
				sys.stdout.flush()

				# Attempt to connect and query
				self.__ip = '192.168.1.' + str(i)
				self.__roku_model = self.__get_roku_info()

				# Work?
				if self.__roku_model is not None:
					print('\nFound a Roku at', self.__ip)
					return True
			except socket.error as e:
				# Well clearly that one didn't work
				pass

		# Didn't find a Roku, but make sure we bump down to the next line
		print()
		return False

	def disconnect(self):
		"""
		Disconnects from the Roku
		"""
		self.__ip = None
			
	def __del__(self):
		"""
		Disconnect promperly (if that ever mattered)
		"""
		try:
			self.disconnect()
		except socket.error as e:
			# Ignore, probably we weren't connected anyway
			pass
	
	def get_model(self):
		"""
		Returns the model/description of the Roku
		"""
		return self.__roku_model

	def get_roku_ip(self):
		"""
		Returns the IP of the Roku we're connected to
		"""
		return self.__ip

	def send(self, url, request_type='POST', ip=None):
		"""
		Sends the given URL request to the Roku. Uses POST by default,
		but may be set to GET. Resonse body (if any)
		is returned
		"""
		# Allow a different IP to be specified (useful for init, for example)
		if ip is None:
			ip = self.__ip
		
		# Run request
		conn = http.client.HTTPConnection(ip, port=self.__ROKU_PORT, timeout=1)
		conn.set_debuglevel(1)
		conn.request(request_type, url)
		resp = conn.getresponse()
		body = resp.read()
		conn.close()

		return body

	def send_keypress(self, keyname):
		"""
		Sends the given command to the Roku. Adds terminating \n if needed
		"""
		# Send the keypress
		self.send('/keypress/' + keyname)

	def send_home(self):
		self.send_keypress('home')
	
	def send_left(self):
		self.send_keypress('left')

	def send_right(self):
		self.send_keypress('right')

	def send_up(self):
		self.send_keypress('up')

	def send_down(self):
		self.send_keypress('down')

	def send_rwd(self):
		self.send_keypress('back')

	def send_ff(self):
		self.send_keypress('fwd')

	def send_pause(self):
		self.send_keypress('pause')

	def send_select(self):
		self.send_keypress('select')

	def send_search(self, phrase):
		self.send_keypress('search')
		print('searching for')
		for c in phrase:
			print(c)
			self.send_keypress('lit_' + c)
		self.send_keypress('enter')

	def open_bitrate_screen(self):
		"""
		Sends the sequence of commands to open the bitrate-selection screen
		"""
		for x in range(5):
			self.send_home()
			time.sleep(.5)
		for x in range(3):
			self.send_rwd()
			time.sleep(.5)
		for x in range(2):
			self.send_ff()
			time.sleep(.5)	

	def open_secret_screen(self):
		"""
		Sends the sequence of commands to open the 'secret' Roku screen
		"""
		for x in range(5):
			self.send_home()
			time.sleep(.5)
		for x in range(3):
			self.send_ff()
			time.sleep(.5)
		for x in range(2):
			self.send_rwd()
			time.sleep(.5)

	def __str__(self):
		"""
		Gives the state of the connection
		"""
		return 'Connected to ' + self.__roku_id

#######################
# Main
def main(argv = None):
	"""
	Primary startup point for the editor
	"""
	# We have the default as None rather than sys.argv
 	# just in case sys.argv were changed after startup
	if argv is None:
		argv = sys.argv
        
	# Connect
	try:
		remote = RokuRemote(sys.argv)
		print('Connected to', remote.get_model())
	except socket.timeout as e:
		print('Connection to Roku timedout, likely not a valid device')
		quit(1)

 	# Run application with the best available UI
	if curses_available:
		ui = RokuRemoteCurses(remote)
	else:
		ui = RokuRemotePlainTerminal(remote)

	ui.start()
        
if __name__ == '__main__':
	main(sys.argv)

