#!/usr/bin/env python3
import sys
import argparse
import socket

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
		if self.__ip is None:
			self.__ip = self.locate_roku()
			
			# Found?
			if self.__ip is None:
				print("Unable to locate Roku on local network, please specify IP or host name on the command line")
				quit()

		# Connect
		self.__port = 8080
		try:
			self.__conn = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
			self.__conn.connect((self.__ip, self.__port))
		except socket.error as e:
			print("Unable to connect to Roku: " + str(e))
			quit()
		
		# Read in the Roku ID information
		self.__roku_id = self.read_line()
		self.read_to_prompt()

	def __del__(self):
		"""
		Disconnect promperly (if that ever mattered)
		"""
		self.disconnect()
	
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

		print(line)
		return line

	def read_to_prompt(self):
		"""
		Reads and returns all data until the Roku prompt (does not include 
		the prompt itself). Includes the terminating \r\n
		"""
		self.__NO_PROMPT_STATE = 0
		self.__PROMPT_START_STATE = 1
		self.__PROMPT_COMPLETE_STATE = 2

		data = ''
		state = self.__NO_PROMPT_STATE
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
		
		print(data)
		return data

	def send(self, cmd):
		"""
		Sends the given command to the Roku. Adds terminating \n if needed
		"""
		if cmd[-1] != '\n':
			cmd = ''.join([cmd, '\n'])

		print('> ', cmd)
		self.__conn.sendall(cmd.encode())

	def locate_roku(self):
		"""
		Static function that returns the first IP address of a system on the local network
		that has port 8080 open
		"""
		return "192.168.1.130"

	def disconnect(self):
		"""
		Disconnects from the Roku
		"""
		self.__conn.shutdown()
		self.__conn.close()

	def start(self):
		print('Done')

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
        
 	# Run application
	app = RokuRemote(sys.argv)
	app.start()
	app.send('press home')
        
if __name__ == '__main__':
	main(sys.argv)

