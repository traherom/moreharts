#!/usr/bin/env python3
import sys
import threading
import time

class Route:
	def __init__(self, dest, via, cost):
		self.dest = dest
		self.via = via
		self.cost = cost
		
	def __str__(self):
		if self.dest is not None:
			d = self.dest.get_id()
		else:
			d = '-'
		if self.via is not None:
			v = self.via.get_id()
		else:
			v = '-'
		c = self.cost
		
		return '{:>4}, {:>3}, {:<3}'.format(d, v, c) 

class Node:
	def __init__(self, id):
		#threading.Thread.__init__(self)
		
		self.__id = id
		
		# Init with just a route to ourselves
		# Direct routes is only used for sending updates
		self.__route_lock = threading.RLock()
		self.__routes = []
		self.__direct_routes = []
		self.__is_updated = True
		self.add_connection(self, 0)
		
		# Keep a history of old routing tables
		self.__history = []
	
	def save_history(self):
		"""
		Saves the current routing table
		"""
		self.__history.append(self.__routes)
	
	def get_history(self):
		"""
		Returns the history of this nodes routing table
		"""
		return self.__history
	
	def get_routes(self):
		"""
		Returns the current route setup
		"""
		return self.__routes
	
	def update_routes(self, from_node, table_update):
		"""
		Receives an update from another node
		"""
		print('Update from', from_node.get_id(), 'to', self.get_id())
		print(self.__str_routes(table_update))
		
		print('Current table')
		print(self.__str_routes(self.__routes))
		
		with self.__route_lock:
			# How for is it to this node?
			route_to_from = self.__find_route(self.__routes, from_node)
		
			# Combine old table and the update to get the list of all links we know about
			links = list(self.__routes)
			links.extend(table_update)

			# Create non-duplicate list of known nodes, all infinity away except for ourselves
			new_routes = [Route(self, self, 0)]
			for route in links:
				if self.__find_route(new_routes, route.dest) is None:
					new_routes.append(Route(route.dest, None, 100000))

			# Run Bellman-Ford
			for i in range(len(new_routes)):
				#print('Route debug update')
				#print(self.__str_routes(new_routes), self.__str_routes(links))
				#print('Done update')
			
				for link in links:
					# Get the existing route to where this link goes
					curr_route = self.__find_route(new_routes, link.dest)
					
					# Get the route to the beginning point of this link (if we know of one)
					via_route = self.__find_route(new_routes, link.via)

					# New via route faster than already known one?
					if via_route.cost + link.cost < curr_route.cost:
						# Faster, save this one
						curr_route.via = link.via
						curr_route.cost = via_route.cost + link.cost
						
			# Did we change anything?
			self.__is_updated = False
			if len(new_routes) != len(self.__routes):
				# Whelp, something must have changed
				self.__is_updated = True
			else:
				# Have to do more detailed check
				for new_route in new_routes:
					old_route = self.__find_route(self.__routes, new_route.dest)
					if old_route.via is not new_route.via or old_route.cost != new_route.cost:
						self.__is_updated = True
						break
			
			# Update everyone else if we changed
			if self.__is_updated:
				# Save new table
				self.__routes = new_routes
				self.save_history()
				
				print('Change detected in {} routing table because of update from {}'.format(self.get_id(), from_node.get_id()))
				print('New table:')
				print(self.__str_routes(self.__routes))
				
				# Update our neighbors in the future
				#self.send_routes()
			
			return self.__is_updated

	def send_routes(self, force=False):
		"""
		Sends routing info to all direct-connected nodes, rewriting our tables to 
		show the routes through ourselves. Only done if Node has actually been updated
		since the last time it sent
		"""
		# Skip if not needed
		if not force and not self.__is_updated:
			return False
		
		changed = False
		for route in self.__direct_routes:
			if route.dest is not self:
				if route.dest.update_routes(self, [Route(route.dest, self, route.cost) for route in self.__routes]):
					changed = True
					
		# Changes have been sent
		self.__is_updated = False
		
		return changed
	
	def add_connection(self, neighbor, cost):
		"""
		Adds a direct connection to our table
		"""
		with self.__route_lock:
			self.__routes.append(Route(neighbor, self, cost))
			self.__direct_routes.append(Route(neighbor, self, cost))
	
	def __find_route(self, route_table, dest):
		"""
		Locates the route to the given destination
		"""
		# Never mind "no route"
		if dest is None:
			return None
			
		# Ensure it works with both string ids and actual nodes
		if type(dest) is not str:
			dest = dest.get_id()
		
		with self.__route_lock:
			for route in route_table:
				if route.dest is not None and route.dest.get_id() == dest:
					return route
				
		# Not found
		return None
	
	def get_id(self):
		"""
		Returns the nice human-readable name for this node
		"""
		return self.__id
		
	def __str_routes(self, route_table):
		"""
		Returns the routes this node currently has in memory
		"""
		# Sort for consistency
		t = sorted(route_table, key=lambda r : r.dest.get_id())
		
		s = 'Dest, Via, Cost\n'
		for x in t:
			s += str(x) + '\n'
			
		return s
		
	def __str__(self):
		with self.__route_lock:
			return 'Routes for {}:\n{}'.format(self.__id, self.__str_routes(self.__routes))

def main(argv=None):
	if argv is None:
		argv = sys.argv
	
	u = Node('u')
	v = Node('v')
	x = Node('x')
	y = Node('y')
	z = Node('z')
	nodes = {
		'u' : u,
		'v' : v,
		'x' : x,
		'y' : y,
		'z' : z
		}
	
	# Initial Routes
	u.add_connection(v, 4)
	u.add_connection(x, 12)
	u.save_history()
	
	v.add_connection(u, 4)
	v.add_connection(z, 5)
	v.add_connection(y, 7)
	v.save_history()
	
	x.add_connection(u, 12)
	x.add_connection(z, 2)
	x.add_connection(y, 1)
	x.save_history()
	
	y.add_connection(x, 1)
	y.add_connection(z, 10)
	y.add_connection(v, 7)
	y.save_history()
	
	z.add_connection(v, 5)
	z.add_connection(y, 10)
	z.add_connection(x, 2)
	z.save_history()
	
	print('=== Initial routes (direct connections) ===')
	for key in nodes:
		print(nodes[key])
	
	# Have everybody transmit until no more changes are detected
	changed = True
	while changed:
		changed = False
		for key in nodes:
			n = nodes[key]
			print('{} sending routes...'.format(n.get_id()))
			if n.send_routes():
				changed = True
	
	# Show full history of one node
	print('=== HISTORY ===')
	for key in nodes:
		node = nodes[key]
		
		print('History for {}:'.format(node.get_id()))
		hist = node.get_history()
		for i in range(len(nodes)):
			for state in hist:
				if len(state) > i:
					print(state[i], '|', end='')
				else:
					print(' '*16, end='')
			print()
	
	# Final for one node
	node = nodes['z']
	print('=== FINAL {} ==='.format(node.get_id()))
	print(node)
	
	return 0
	
if __name__ == '__main__':
	sys.exit(main(sys.argv))
	
