"""
Includes majority of GUI code for SparseEdit
"""

from PyQt4 import QtGui
from PyQt4.Qsci import QsciScintilla

class EditorWindow(QtGui.QWidget):
	"""
	Editor window that handles majority of interaction with user. Currently
	only handles a single file at a time. Multiple windows will be used for
	editing	multiple files
	"""
	
	def __init__(self, editor_instance, filename = None):
		"""
		Creates new editor window, but does not automatically show itself.
		"""
		super(EditorWindow, self).__init__(None)
		
		# Save off reference to the running instance we're a part of
		self.__editor = editor_instance
		self.__file_name = filename
		
		# Basic window stuff
		self.__update_title()
			
		# Create editor area
		
		
	def __update_title(self):
		"""
		Ensures that the window title is correct based on the current project
		and open file name
		"""
		if self.__editor.is_project_open():
			title = self.__editor.project_name() + ' - '
		else:
			title = ''
	
		if self.__file_name is None:
			title += "New File"
		else:
			title += self.__file_name
			
		# Actually set the title
		self.setWindowTitle(title)
			
			
