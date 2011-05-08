#!/usr/bin/python -3
import sys
from PyQt4 import QtGui
from gui import EditorWindow

class SparseEdit(QtGui.QApplication):
	def __init__(self, argv):
		super(SparseEdit, self).__init__(argv)
		
		self.__windows = []
		self.__project = None
		
		# Are there any files given on the command line?
		# TODO
		self.new_file()
		
	def open_file(self, filename):
		"""Opens the given file"""
		new_win = EditorWindow(self, filename)
		self.__windows.append(new_win)
		new_win.show()
	
	def new_file(self):
		"""Creates a new editor window that is not pointed to anything"""
		new_win = EditorWindow(self)
		self.__windows.append(new_win)
		new_win.show()
	
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
	
def main(argv = None):
	"""Primary startup point for the editor"""
	# We have the default as None rather than sys.argv
	# just in case sys.argv were changed after startup
	if argv is None:
		argv = sys.argv
        
    # Run application
	app = SparseEdit(sys.argv)
	return app.exec_()
	
if __name__ == '__main__':
	sys.exit(main(sys.argv))

