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
		
		return '{:>4}, {:>3}, {}'.format(d, v, c) 

class Node:
	def __init__(self, id):
		#threading.Thread.__init__(self)
		
		self.__id = id
		
		# Init with just a route to ourselves
		# Direct routes is only used for sending updates
		self.__route_lock = threading.RLock()
		self.__routes = []
		self.__direct_routes = []
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
		#print('Update from', from_node.get_id(), 'to', self.get_id())
		#print(self.__str_routes(table_update))
		
		#print('Current table')
		#print(self.__str_routes(self.__routes))
		
		with self.__route_lock:
			# How for is it to this node?
			route_to_from = self.__find_route(self.__routes, from_node)
		
			# Copy old table and add routes to it that table_update holds
			all_routes = list(self.__routes)
			all_routes.extend(table_update)
			#new_routes.extend([Route(route.dest, route_to_from.via, route_to_from.cost + route.cost) for route in table_update])

			# Create list of known nodes and edges
			new_routes = list(self.__routes)
			links = []
			for route in all_routes:
				if self.__find_route(new_routes, route.dest) is None:
					new_routes.append(Route(route.dest, None, float('inf')))
				links.append(route)

			# Run Bellman-Ford
			for route in new_routes:
				for link in links:
					# Get the existing route (if there is one) to where this link goes
					curr_route = self.__find_route(new_routes, link.dest)
					
					# Get the route to the beginning point of this link (if we know of one)
					via_route = self.__find_route(new_routes, link.via)

					# New via route faster than already known one?
					if via_route.cost + link.cost < curr_route.cost:
						# Faster, save this one
						curr_route.via = link.via
						curr_route.cost = via_route.cost + link.cost
			
			# Did we change anything?
			changed = False
			if len(new_routes) != len(self.__routes):
				# Whelp, something must have changed
				changed = True
			else:
				# Have to do more detailed check
				for new_route in new_routes:
					old_route = self.__find_route(self.__routes, route.dest)
					if not (old_route.via is new_route.via or old_route.cost != new_route.cost):
						changed = True
						break
			
			# Update everyone else if we changed
			if changed:
				# Save new table
				self.__routes = new_routes
				self.save_history()
				
				print('Change detected in {} routing table because of update from {}'.format(self.get_id(), from_node.get_id()))
				print('New table:')
				print(self.__str_routes(self.__routes))
				
				# Update our neighbors
				self.send_routes()
			
			return changed

	def send_routes(self):
		"""
		Sends routing info to all direct-connected nodes, rewriting our tables to 
		show the routes through ourselves
		"""
		for route in self.__direct_routes:
			if route.dest is not self:
				route.dest.update_routes(self, [Route(route.dest, self, route.cost) for route in self.__routes])
	
	def add_connection(self, neighbor, cost):
		"""
		Adds a direct connection to our table
		"""
		with self.__route_lock:
			self.__routes.append(Route(neighbor, neighbor, cost))
			self.__direct_routes.append(Route(neighbor, neighbor, cost))
	
	def __find_route(self, route_table, dest):
		"""
		Locates the route to the given destination
		"""
		# Ensure it works with both string ids and actual nodes
		if type(dest) is not str:
			dest = dest.get_id()
		
		with self.__route_lock:
			for route in route_table:
				if route.dest.get_id() == dest:
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
	print(u)
	print(v)
	print(x)
	print(y)
	print(z)
	
	# Have everybody transmit, once they all have it should have propagated everywhere
	print('u sending routes...')
	u.send_routes()
	print('y sending routes...')
	y.send_routes()
	print('x sending routes...')
	x.send_routes()
	print('y sending routes...')
	y.send_routes()
	print('z sending routes...')
	z.send_routes()
	
	# Final z
	node = z
	print('=== FINAL {} ==='.format(node.get_id()))
	print(node)
	
	# Show full history of one node
	print('=== HISTORY FOR {} ==='.format(node.get_id()))
	hist = node.get_history()
	for i in range(5):
		for state in hist:
			if len(state) > i:
				print(state[i], '|', end='')
			else:
				print(' '*14, end='')
		print()
	
	return 0
	
if __name__ == '__main__':
	sys.exit(main(sys.argv))
	
