"""
Includes majority of GUI code for SparseEdit
"""

import pygtk
pygtk.require('2.0')
import gtk
import gtksourceview2 as gtksourceview

class EditorWindow(gtk.Window):
	"""
	Editor window that handles majority of interaction with user. Currently
	only handles a single file at a time. Multiple windows will be used for
	editing	multiple files
	"""
	
	def __init__(self, editor, filename = None):
		"""
		Creates new editor window, but does not automatically show itself.
		"""
		gtk.Window.__init__(self, gtk.WINDOW_TOPLEVEL)
		
		# Save off reference to the running instance we're a part of
		self.__editor = editor
		self.__filename = None
		self.__isChanged = False
		
		# Basic window stuff
		self.set_default_size(500, 500)
		self.__mainBox = gtk.VBox(False, 0)
		self.add(self.__mainBox)
		self.__mainBox.show()
		self.__update_title()
		
		# Main events
		self.connect("destroy", self.destroy)
		
		# Create editor area and attach the main handler we'll be relying on
		self.__text = gtksourceview.View()
		self.__mainBox.pack_start(self.__text, True, True, 0)
		self.__text.show()
		
		self.__text.connect('key_press_event', self.keyPressed)
		
		# Create search area (do not show, only activates on shortcut key)
		self.__searchAreaTable = gtk.Table(2, 2, False)
		self.__mainBox.pack_start(self.__searchAreaTable, False, False, )
		
		# Search area
		searchLabel = gtk.Label("Find:")
		searchLabel.set_justify(gtk.JUSTIFY_RIGHT)
		self.__searchAreaTable.attach(searchLabel, 0, 1, 0, 1, gtk.FILL)
		searchLabel.show()
		
		self.__searchEntry = gtk.Entry()
		self.__searchAreaTable.attach(self.__searchEntry, 1, 2, 0, 1, gtk.EXPAND | gtk.FILL)
		self.__searchEntry.show()
		
		# Replace area
		replaceLabel = gtk.Label("Replace:")
		replaceLabel.set_justify(gtk.JUSTIFY_RIGHT)
		self.__searchAreaTable.attach(replaceLabel, 0, 1, 1, 2, 0, gtk.FILL)
		replaceLabel.show()
		
		self.__replaceEntry = gtk.Entry()
		self.__searchAreaTable.attach(self.__replaceEntry, 1, 2, 1, 2, gtk.EXPAND | gtk.FILL)
		self.__replaceEntry.show()
		
		# Do we have a file to open?
		if self.__filename is not None:
			self.openFile(filename)
		
	def destroy(self, widget, data = None):
		"""
		Kills the window and lets the editor manager know we're dead
		"""
		self.__editor.stopManagingEditor(self)
	
	def keyPressed(self, widget, data = None):
		"""
		Watches for all the cool
		"""
		print(widget)
		pass
	
	def openFile(filename):
		"""
		Opens the given file. Prompts the user if the current file is unsaved
		"""
		if self.__isChanged:
			pass
		
		print("Opening " + filename)
		
		# Read in new file
		self.__filename = filename
		f = open(self.__filename, 'r')
		buf = self.__text.get_buffer()
		buf.set_text(f.read())
		f.close()
		
		self.__isChanged = False
		
		# Ensure our title is correct
		self.__update_title()
	
	def __update_title(self):
		"""
		Ensures that the window title is correct based on the current project
		and open file name
		"""
		if self.__isChanged:
			title = '* '
		else:
			title = ''
		
		if self.__editor.isProjectOpen():
			title += self.__editor.projectName() + ' - '
	
		if self.__filename is None:
			title += "New File"
		else:
			title += self.__file_name
			
		# Actually set the title
		self.set_title(title)
			
