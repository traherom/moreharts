#!/usr/bin/python 
import sys
import argparse
from gi.repository import Gtk

import gui

class SparseEdit:
	def __init__(self, argv):
		self.__editors = []
		self.__project = None
		
		# Are there any files given on the command line?
		parser = argparse.ArgumentParser(description='Simple and austere text editor')
		parser.add_argument('files', metavar='Files', nargs='*')
		res = parser.parse_args(argv[1:])
		files = res.files
		
		if len(files) > 0:
			for f in files:
				self.open_file(f)
		else:
			self.new_file()
	
	def start(self):
		"""Runs GTK's main loop, starting the event sytem"""
		Gtk.main()	
	
	##############################
	# File management functions
	def open_file(self, filename):
		"""Opens the given file"""
		newWin = gui.EditorWindow(self, filename)
		self.__editors.append(newWin)
		newWin.show()
	
	def new_file(self):
		"""Creates a new editor window that is not pointed to anything"""
		newWin = gui.EditorWindow(self)
		self.__editors.append(newWin)
		newWin.show()
	
	###############################
	# Window management functions
	def stop_managing_editor(self, editor):
		"""
		Removes the given editor from the list of editor windows we have
		open. If this was the last window we had, stop execution
		"""
		self.__editors.remove(editor)
		
		# Should we finish and close completely?
		if self.__editors == []:
			Gtk.main_quit()
	
	def switch_to_next_editor(self, curr):
		"""
		Switches to the editor "after" the current one
		"""
		nextIndex = self.__editors.index(curr) + 1
		if len(self.__editors) == nextIndex:
			nextIndex = 0
		
		self.__editors[nextIndex].present()
	
	#######################
	# Project management
	
	def is_project_open(self):
		"""Returns True if a project is currently open"""
		if self.__project is None:
			return False
		else:
			return True
	
	def project_name(self):
		"""
		Returns the name of the current project or None if there is no
		project.
		"""
		# TODO
		return "Blah"

#######################
# Main
def main(argv = None):
	"""Primary startup point for the editor"""
	# We have the default as None rather than sys.argv
	# just in case sys.argv were changed after startup
	if argv is None:
		argv = sys.argv
        
    # Run application
	app = SparseEdit(sys.argv)
	app.start()
	
if __name__ == '__main__':
	main(sys.argv)

