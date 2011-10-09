#!/usr/bin/env python3
import sys
import argparse
import socket
import time
import locale

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

		# Search for Roku if not specified
		if self.__ip is not None:
			try:
				self.__connect(self.__ip)
			except socket.error as e:
				print('Unable to connect to Roku at ' +  self.__ip + ':', e)
		else:
			self.locate_roku()
			
			# Found?
			if self.__conn is None:
				print('Unable to locate Roku on local network, please specify IP or host name on the command line')
				quit()

	def __connect(self, ip):
		"""
		Attempts to connect to the given ip on port 8080 and checks that
		it responds like a Roku
		"""
		# Connect
		self.__conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
		self.__conn.connect((ip, 8080))	
			
		# Read in the Roku ID information
		self.__roku_id = self.read_line()
		self.read_to_prompt()
		
		return self.__conn

	def locate_roku(self):
		"""
		Static function that returns the first IP address of a system on the local network
		that has port 8080 open
		"""
		print('Searching for Roku on local network (assuming 192.168.1.0/24)... Trying .   ', end='')
		sys.stdout.flush()
		
		# Don't waste too much time trying to connect
		socket.setdefaulttimeout(.1)

		self.__conn = None
		for i in range(1, 254):
			try:
				print('\b\b\b' + format(i, '03d'), end='')
				sys.stdout.flush()

				self.__ip = '192.168.1.' + str(i)
				self.__connect(self.__ip)
				
				if self.__conn is not None:
					print('\nFound a Roku at', self.__ip)
					return
			except socket.error as e:
				# Well clearly that one didn't work
				pass

		# Didn't find a Roku, but make sure we bump down to the next line
		print()

	def disconnect(self):
		"""
		Disconnects from the Roku
		"""
		if self.__conn is not None:
			self.__conn.shutdown(socket.SHUT_RDWR)
			self.__conn.close()
			self.__conn = None
			
	def __del__(self):
		"""
		Disconnect promperly (if that ever mattered)
		"""
		try:
			self.disconnect()
		except socket.error as e:
			# Ignore, probably we weren't connected anyway
			pass
	
	def read_line(self):
		"""
		Reads and returns a single line on the connection
		"""
		line = ''
		b = self.__conn.recv(1)
		while b != b'\n':
			c = bytes.decode(b)
			line = ''.join([line, c])
			b = self.__conn.recv(1)

		# And remove the '\r' from the line
		if line[-1] == '\r':
			line[:-1]

		return line

	def read_to_prompt(self):
		"""
		Reads and returns all data until the Roku prompt (does not include 
		the prompt itself). Includes the terminating \r\n
		"""
		self.__NO_PROMPT_STATE = 0
		self.__PROMPT_START_STATE = 1
		self.__PROMPT_COMPLETE_STATE = 2

		# A prompt could start imediately at the beginning of the line
		# and we wouldn't see the \n, so assume we're beginning now
		state = self.__PROMPT_START_STATE

		data = ''
		while state != self.__PROMPT_COMPLETE_STATE:
			b = self.__conn.recv(1)

			# Check for prompt state change
			if state == self.__NO_PROMPT_STATE and b == b'\n':
				state = self.__PROMPT_START_STATE
			elif state == self.__PROMPT_START_STATE:
				if b == b'>':
					state = self.__PROMPT_COMPLETE_STATE
				else:
					state = self.__NO_PROMPT_STATE
			
			# Add to data if needed
			if state != self.__PROMPT_COMPLETE_STATE:
				# Add to string
				c = bytes.decode(b)
				data = ''.join([data, c])
		
		return data

	def get_roku_id(self):
		"""
		Returns the ID of the Roku we're connected to
		"""
		return self.__roku_id

	def get_roku_ip(self):
		"""
		Returns the IP of the Roku we're connected to
		"""
		return self.__ip

	def send(self, cmd):
		"""
		Sends the given command to the Roku. Adds terminating \n if needed
		"""
		if cmd[-1] != '\n':
			cmd = ''.join([cmd, '\n'])

		self.__conn.sendall(cmd.encode())

	def send_home(self):
		self.send('press home')
		self.read_to_prompt()
	
	def send_left(self):
		self.send('press left')
		self.read_to_prompt()

	def send_right(self):
		self.send('press right')
		self.read_to_prompt()

	def send_up(self):
		self.send('press up')
		self.read_to_prompt()

	def send_down(self):
		self.send('press down')
		self.read_to_prompt()

	def send_rwd(self):
		self.send('press back')
		self.read_to_prompt()

	def send_ff(self):
		self.send('press fwd')
		self.read_to_prompt()

	def send_pause(self):
		self.send('press pause')
		self.read_to_prompt()

	def send_select(self):
		self.send('press select')
		self.read_to_prompt()

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
        
 	# Run application with the best available UI
	remote = RokuRemote(sys.argv)
	if curses_available:
		ui = RokuRemoteCurses(remote)
	else:
		ui = RokuRemotePlainTerminal(remote)

	ui.start()
        
if __name__ == '__main__':
	main(sys.argv)

