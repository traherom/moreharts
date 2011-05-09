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
		self.__filename = filename
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
		self.__text.set_show_line_numbers(True)
		self.__text.set_auto_indent(True)
		self.__text.set_smart_home_end(True)
		
		scroll = gtk.ScrolledWindow()
		scroll.set_policy(gtk.POLICY_AUTOMATIC, gtk.POLICY_AUTOMATIC)
		scroll.add(self.__text)
		self.__text.show()
		self.__mainBox.pack_start(scroll, True, True, 0)
		scroll.show()
		
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
		self.__replaceLabel = gtk.Label("Replace:")
		self.__replaceLabel.set_justify(gtk.JUSTIFY_RIGHT)
		self.__searchAreaTable.attach(self.__replaceLabel, 0, 1, 1, 2, 0, gtk.FILL)
		self.__replaceLabel.show()
		
		self.__replaceEntry = gtk.Entry()
		self.__searchAreaTable.attach(self.__replaceEntry, 1, 2, 1, 2, gtk.EXPAND | gtk.FILL)
		self.__replaceEntry.show()
		
		# Basically ready to go, start accepting keys
		self.connect('key_press_event', self.key_pressed)
		
		# Do we have a file to open?
		if self.__filename is not None:
			self.open_file(filename)
		
	def destroy(self, widget, data = None):
		"""
		Kills the window and lets the editor manager know we're dead
		"""
		self.__editor.stop_managing_editor(self)
	
	def key_pressed(self, widget, data = None):
		"""
		Know when we change and when a shortcut is entered
		"""
		self.__isChanged = True
		self.__update_title()
		
		print("Key:", data.keyval)
		
		if data.state & gtk.gdk.CONTROL_MASK != 0:
			# Ctrl+...
			print("Control+" + str(data.keyval))
			
			if data.keyval == 115: # s
				self.save_file()
			elif data.keyval == 114: # r
				self.open_replace()
			elif data.keyval == 102: # f
				self.open_search()
				
		elif data.state & gtk.gdk.SHIFT_MASK != 0:
			# Shift+...
			print("Shift+" + str(data.keyval))
		elif data.state & gtk.gdk.MOD1_MASK != 0:
			# Alt+...
			print("Alt+" + str(data.keyval))
		elif data.keyval == 65307:
			# Esc, close anything that's open other than the main editing window
			self.hide_search()
	
	def open_file(self, filename):
		"""
		Opens the given file. Prompts the user if the current file is unsaved
		"""
		if self.__isChanged:
			pass
		
		# Read in new file
		self.__filename = filename
		buf = self.__text.get_buffer()
		
		f = open(self.__filename, 'r')
		buf.set_text(f.read())
		f.close()
		
		self.__isChanged = False
		
		# Highlighting mode
		self.set_language(self.detect_language())
		
		# Ensure our title is correct
		self.__update_title()
	
	def save_file(self):
		"""
		Saves the current file to disk. If there is no filename currently set,
		prompts the user for one
		"""
		if self.__filename is None:
			chooser = gtk.FileChooserDialog("Save As...", self,
				gtk.FILE_CHOOSER_ACTION_SAVE,
				('Save', 0, 'Cancel', 1))
			chooser.set_do_overwrite_confirmation(True)
			#chooser.connect('', )
			chooser.show()
			return
		
		buf = self.__text.get_buffer()
		
		f = open(self.__filename, 'w')
		f.write(buf.get_text(buf.get_start_iter(), buf.get_end_iter()))
		f.close()
		
		self.__isChanged = False
		self.__update_title()
	
	def open_search(self):
		"""
		Opens the search box, hiding the replacement components
		"""
		# Hide the replace stuff
		self.__replaceLabel.hide()
		self.__replaceEntry.hide()
		
		# Show the search area and focus the keyboard
		self.__searchAreaTable.show()
		self.__searchEntry.grab_focus()
		
	def hide_search(self):
		"""
		Opens the search box
		"""
		self.__searchAreaTable.hide()
		
		# Return focus to editor
		self.__text.grab_focus()
	
	def open_replace(self):
		"""
		Open the search and replace box
		"""
		# Hide the replace stuff
		self.__replaceLabel.show()
		self.__replaceEntry.show()
		
		# Show the search area and focus the keyboard
		self.__searchAreaTable.show()
		self.__searchEntry.grab_focus()
	
	def set_language(self, lang):
		"""
		Changes the language being highligted
		"""
		pass
		
	def detect_language(self):
		"""
		Guesses the language that the current file is in. If no file is open,
		it guesses "None"
		"""
		if self.__filename is None:
			return None
		else:
			return 'C'
	
	def __update_title(self):
		"""
		Ensures that the window title is correct based on the current project
		and open file name
		"""
		if self.__isChanged:
			title = '* '
		else:
			title = ''
		
		if self.__editor.is_project_open():
			title += self.__editor.project_name() + ' - '
	
		if self.__filename is None:
			title += "New File"
		else:
			title += self.__filename
		
		title += " - SparseEdit"
		
		# Actually set the title
		self.set_title(title)

