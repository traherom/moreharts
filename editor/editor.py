#!/usr/bin/python -3
import sys
import gtk
from gui import EditorWindow

class SparseEdit:
	def __init__(self, argv):
		self.__editors = []
		self.__project = None
		
		# Are there any files given on the command line?
		
	
	def start(self):
		"""Runs GTK's main loop, starting the event sytem"""
		gtk.main()	
	
	##############################
	# File management functions
	def openFile(self, filename):
		"""Opens the given file"""
		newWin = EditorWindow(self, filename)
		self.__editors.append(newWin)
		newWin.show()
	
	def newFile(self):
		"""Creates a new editor window that is not pointed to anything"""
		newWin = EditorWindow(self)
		self.__editors.append(newWin)
		newWin.show()
	
	###############################
	# Window management functions
	def stopManagingEditor(self, editor):
		"""
		Removes the given editor from the list of editor windows we have
		open. If this was the last window we had, stop execution
		"""
		self.__editors.remove(editor)
		
		# Should we finish and close completely?
		if self.__editors == []:
			gtk.main_quit()
	
	def isProjectOpen(self):
		"""Returns True if a project is currently open"""
		if self.__project is None:
			return False
		else:
			return True
	
	def projectName(self):
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

