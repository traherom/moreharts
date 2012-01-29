#!/usr/bin/env python3
import sys
import os
import argparse
import hashlib
import pymysql

class HashChecker:
	def __init__(self, sys_str_id=None, host='localhost', port=3306, 
			user='root', password=None, database='hashes'):
		# Currently we have nothing to check and no results
		self.__include = []
		self.__exclude = []
		self.__results = {}

		# Save info
		self.__sys_str_id = sys_str_id

		self.__host = host
		self.__port = port
		self.__user = user
		self.__password = password
		self.__database = database
		
		self.__do_nist_check = False
		self.__do_save_state = False
		self.__do_audit_state = False
		self.__correct_show_hashes()
		
		# Haven't gotten our ID yet and haven't connected to DB
		self.__conn = None
		self.__id = None
	
	def __connect(self):
		"""
		Connects to the database if we aren't already
		"""
		if self.__conn is None:
			try:
				self.__conn = pymysql.connect(host=self.__host, port=self.__port,
											  user=self.__user, passwd=self.__password,
											  db=self.__database)
			except AttributeError as e:
				raise Exception('Unable to connect to database (did you supply login information?)')
	
	def __get_id(self):
		"""
		Gets this system's numeric ID, if we haven't already
		
		Return: True if an ID is found, False otherwise
		"""
		# Ensure we have a database connection
		self.__connect()
		
		if self.__sys_str_id is not None:
			cur = self.__conn.cursor()
			if cur.execute('SELECT comp_id FROM systems WHERE name=%s', self.__sys_str_id) == 1:
				self.__id = int(cur.fetchone()[0])
			cur.close()
			
		# Did we succeed?
		return (self.__id is not None)
	
	def enable_nist_check(self, enable=True):
		"""
		Enables checking files against NIST database
		"""
		# We'll need a database for this
		if enable:
			self.__connect()
		
		self.__do_nist_check = enable
		self.__correct_show_hashes()
		
	def enable_save_state(self, enable=True):
		"""
		Saves the current state of system files to database
		"""
		# Only allow one state-mode at once
		if self.__do_audit_state and enable:
			raise Exception('The system can only check or save a system state, not both at the same time')
			
		# We'll need a database for this
		if enable:
			self.__connect()
		
		# Init?
		if enable:
			# Do we need to create a system id?
			if not self.__get_id():
				# They did give us a description, right?
				if self.__sys_str_id is None:
					raise Exception('A system ID must be set for state checking to work')
				
				# Now create
				cur = self.__conn.cursor()
				cur.execute('INSERT INTO systems (name) VALUES (%s);', self.__sys_str_id)
				cur.execute('SELECT LAST_INSERT_ID();')
				self.__get_id = int(cur.fetchone()[0])
				cur.close()
			else:
				# Remove old state? Or move to a new ID? TBD. For now remove the old state
				cur = self.__conn.cursor()
				cur.execute('DELETE FROM current_files WHERE comp_id=%s', self.__id)
				cur.close()
				
			# And actually turn on
			self.__do_save_state = enable
		
		self.__correct_show_hashes()
	
	def enable_audit_state(self, enable=True):
		"""
		Checks the current state of system files against the database
		"""
		# Only allow one state-mode at once
		if self.__do_save_state and enable:
			raise Exception('The system can only check or save a system state, not both at the same time')
	
		# We'll need a database for this
		if enable:
			self.__connect()
		
		# Init?
		if enable:
			# We do have a system ID, right?
			if not self.__get_id():
				raise Exception('A system ID must be set for state checking to work')
				
			# Clear state flags in database for this system
			cur = self.__conn.cursor()
			cur.execute('UPDATE current_files SET found=0 WHERE comp_id=%s', self.__id)
			cur.close()
			
			# And actually turn on
			self.__do_audit_state = enable
			
		self.__correct_show_hashes()
			
	def __correct_show_hashes(self):
		"""
		Correctly enables or disables showing hashes based on current settings
		for the other checks. Basically, if any of them are on, we don't show
		hashes
		"""
		if self.__do_audit_state or self.__do_save_state or self.__do_nist_check:
			self.__do_show_hashes = False
		else:
			self.__do_show_hashes = True
		
	def add_include(self, path):
		"""
		Adds the given path (dir or file) to the list of files to check.
		All children of the path will also be checked
		"""
		self.__include.append(os.path.abspath(path))
		
	def add_exclude(self, path):
		"""
		Excludes the given path (dir or file) from being checked. All
		children will also be excluded
		"""
		self.__exclude.append(os.path.abspath(path))
	
	def run(self):
		"""
		Start actual checking
		"""
		# We want to walk through all our include path bottom-up, but
		# os.walk() makes it a little inconvenient to obey our exclusions
		# Hence, we do it ourselves
		for p in self.__include:
			self.__walk(p)
			
		# Finish out the status line, if we were just showing our current location
		if not self.__do_show_hashes:
			print('\r{}\r'.format(' '*80), end='', file=sys.stderr)
			sys.stdout.flush()
			
		# Commit whatever we did
		cur = self.__conn.cursor()
		cur.execute('COMMIT')
		cur.close()
			
	def __walk(self, path):
		"""
		Walks the give path in bottom-up order.
		Returns True if any child has something to report (what that means
			depends on enabled modes), False otherwise
		"""
		# Status, if needed
		if not self.__do_show_hashes:
			print('\r{}\rChecking {}'.format(' '*80, path[-71:]), end='', file=sys.stderr)
			sys.stdout.flush()
		
		# Go through each directory first, descending in if
		# they are not excluded, we have access, and they
		# are not a link (to avoid circular directories)
		has_report = False
		for child in os.listdir(path):
			full = os.path.join(path, child)
			
			# Only work with dirs here
			if not os.path.isdir(full):
				continue
				
			if os.path.islink(full):
				self.__results[full] = 'Link to directory, not following'
				has_report = True
			elif not os.access(full, os.R_OK):
				self.__results[full] = 'Permission denied (unable to check children as well)'
				has_report = True
			elif full in self.__exclude:
				self.__results[full] = 'Excluded from check'
				has_report = True
			else:
				if self.__walk(full):
					has_report = True
		
		# Now go through all the files, checking appropriately
		for child in os.listdir(path):
			full = os.path.join(path, child)
			
			# Only work with files here
			if not os.path.isfile(full):
				continue
				
			# Hash it
			try:
				hash = self.__hash_file(full)
			except IOError as e:
				self.__results[full] = str(e)
				continue
						
			# Now what should we do with this?
			if self.__do_show_hashes:
				print('{}  {}'.format(hash, full))
			
			if self.__do_nist_check:
				score = self.__check_nist(hash, full)
				if score == 0:
					self.__results[full] = 'Failed NSRL check (score {})'.format(score)
					has_report = True
					
			if self.__do_save_state:
				self.__save_hash(hash, full)
				
			if self.__do_audit_state:
				result = self.__audit_hash(hash, full)
				if result is not None:
					self.__results[full] = result
					has_report = True
		
		# If we were auditing, we have to see if we _didn't_ find some files
		if self.__do_audit_state:
			self.__audit_finish()
		
		# Did we find anything?
		return has_report
	
	def display_results(self):
		"""
		Dumps out the results of the check(s). For now just to stdout, maybe
		email or save to the database in the future
		"""
		for path, status in self.__results.items():
			print('{}: {}'.format(path, status))
	
	def __hash_file(self, path):
		"""
		Computes the SHA1 hash the given file
		Returns the hex string of hash
		"""
		m = hashlib.sha1()
		with open(path, 'rb') as f:
			while True:
				sec = f.read(5120)
				if not sec:
					break
				m.update(sec)

		return m.hexdigest()
			
	def __check_nist(self, hash, path):
		"""
		Checks hash of file against database
		"""
		cur = self.__conn.cursor()
		cur.execute('''SELECT SHA1, FileName, ProductCode,
										   OpSystemCode, SpecialCode
										FROM NSRLFile
										WHERE SHA1=%s''', hash)
		
		# There _may_ be more than one file with the same hash... probably not
		good = 0
		if cur.rowcount > 0:
			for row in cur:
				if row[1] == path:
					good = 1.0
				else:
					nistBase = os.path.basename(row[1])
					ourBase = os.path.basename(path)
					
					# If NIST didn't have a full path and we match their base name,
					# we're as good as we can tell. If they did have a path and
					# we didn't match that, then things don't look so good
					if nistBase == row[1] and nistBase == ourBase:
						good = 0.9
					elif nistBase == ourBase:
						good = 0.5
		
		# All done
		cur.close()
		return good

	def __save_hash(self, hash, path):
		"""
		Saves the given hash to the database, associated with the current system
		"""
		cur = self.__conn.cursor()
		cur.execute('''INSERT INTO current_files (comp_id, SHA1, path)
										  VALUES (%s, %s, %s);''',
										  (self.__id, hash, path))
		cur.close()
		
	def __audit_hash(self, hash, path):
		"""
		Checks if the given hash/path match the saved system configuration information
		"""
		cur = self.__conn.cursor()
		cur.execute('''SELECT SHA1, path FROM current_files
										 WHERE comp_id=%s AND (SHA1=%s OR path=%s)''',
										 (self.__id, hash, path))
		
		# How'd it match up?
		if cur.rowcount == 0:
			# No match at all, it's new
			return 'New file'
		else:
			# One or more matches found, just check against one to see the cause
			# The only real reason for 2 or more matches would be that the same 
			# file is in multiple locations (links/copies), which doesn't really
			# matter that much for our purposes
			foundHash, foundPath = cur.fetchone()
			if foundPath == path and foundHash == hash:
				# Exact match, perfect
				# Note that we found this file though, so we can detect deleted files
				cur.execute('UPDATE current_files SET found=1 WHERE path=%s', path)
				return None
			elif foundPath == path and foundHash != hash:
				# We'd seen this file before, it just has a different hash now
				return 'Changed'
			else:
				# There was a file with the same hash on the system previously
				# Possibilities are:
				# 1. Old moved to here
				# 2. Old copied to here
				# 3. Old copied/moved to here and then changed
				# For now old check move vs. copy. We could use this info to suppress
				# that the old file was deleted in the future?
				if not os.path.exists(foundPath):
					return 'Moved from {}'.format(foundPath)
				else:
					return 'Copied from {}'.format(foundPath)
		
	def __audit_finish(self):
		"""
		Adds notes about any deleted files to report
		"""
		cur = self.__conn.cursor()
		cur.execute('SELECT path FROM current_files WHERE comp_id=%s AND found=0', self.__id)
		
		for row in cur:
			self.__results[row[0]] = 'Deleted'
		
		cur.close()
		
def main(argv):
    # Parse command line
    parser = argparse.ArgumentParser(description='Compares hashes of files on drive to hash database')
    parser.add_argument('--host', default='localhost', help='Host with the hash database')
    parser.add_argument('--port', type=int, default=3306, help='Port for database')
    parser.add_argument('-s', '-db', '--database', '--schema', default='hashes', help='Schema (database) on the server to use')
    parser.add_argument('-u', '--user', help='Username for database')
    parser.add_argument('-p', '--password', help='Password for database')
    parser.add_argument('-o', '--output', help='File to output results to')
    parser.add_argument('--enable-nist', action='store_true', help='Do not check files against NIST NSRL hashes (known-good files)')
    parser.add_argument('--enable-save', action='store_true', help='Save the current system state to the database')
    parser.add_argument('--enable-audit', action='store_true', help='Checks the current system state against the database')
    parser.add_argument('-i', '--identifier', help='Identifier for this computer')
    parser.add_argument('-e', '--exclude', default=[], action='append', help='Exclude the given directory from hashing')
    parser.add_argument('include', metavar='Root', nargs='+', help='Directory to start recursive hashing of files')
    args = parser.parse_args()
    
    # Create checker
    checker = HashChecker(sys_str_id=args.identifier,
    					  host=args.host, port=args.port,
    					  user=args.user, password=args.password,
    					  database=args.database)
    					  
    # Enable the correct checks
    if args.enable_nist:
	    checker.enable_nist_check()
    if args.enable_audit:
    	checker.enable_audit_state()
    if args.enable_save:
    	checker.enable_save_state()
    
    # Where should we check (and not)?
    for p in args.include:
    	checker.add_include(p)
    
    for p in args.exclude:
    	checker.add_exclude(p)
    
    # Go!
    checker.run()
    
    # Show results
    checker.display_results()
    
    return 0
    
if __name__ == '__main__':
    sys.exit(main(sys.argv))
    
