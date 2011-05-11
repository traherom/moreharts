import pygtk
pygtk.require('2.0')
from gi.repository import Gtk, GtkSource, Gdk
import mimetypes
import re

# There are big dereferencing problems with the language manager,
# it helps to just have the one instance out here
lang_manager = GtkSource.SourceLanguageManager.get_default()

class EditorWindow(Gtk.Window):
	"""
	Editor window that handles majority of interaction with user. Currently
	only handles a single file at a time. Multiple windows will be used for
	editing	multiple files
	"""
	
	def __init__(self, editor, filename = None):
		"""
		Creates new editor window, but does not automatically show itself.
		"""
		super(EditorWindow, self).__init__(type=Gtk.WindowType.TOPLEVEL)
		
		# Save off reference to the running instance we're a part of
		self.__editor = editor
		self.__filename = filename
		
		# Basic window stuff
		self.set_default_size(500, 500)
		self.__mainBox = Gtk.VBox(homogeneous = False, spacing = 0)
		self.add(self.__mainBox)
		self.__mainBox.show()
		
		# Main events
		self.connect("destroy", self.destroy)
		
		# Create editor area and attach the main handler we'll be relying on
		self.__text = GtkSource.SourceView(buffer = GtkSource.SourceBuffer())
		self.__text.set_show_line_numbers(True)
		self.__text.set_auto_indent(True)
		self.__text.set_smart_home_end(GtkSource.SourceSmartHomeEndType.ALWAYS)
		self.__buffer = self.__text.get_buffer()
		self.__buffer.set_highlight_matching_brackets(True)
		self.__buffer.set_max_undo_levels(200)
		self.__buffer.connect('modified-changed', self.__buffer_changed)
		
		scroll = Gtk.ScrolledWindow()
		scroll.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
		scroll.add(self.__text)
		self.__text.show()
		self.__mainBox.pack_start(scroll, True, True, 0)
		scroll.show()
		
		# Create search area (do not show, only activates on shortcut key)
		#self.__searchAreaTable = Gtk.Table(rows = 2, columns = 2, homogeneous = False)
		self.__searchAreaTable = Gtk.Table()
		self.__searchAreaTable.resize(2, 2)
		self.__mainBox.pack_start(self.__searchAreaTable, False, False, 0)
		
		# Search area
		searchLabel = Gtk.Label(label="Find:")
		searchLabel.set_justify(Gtk.Justification.RIGHT)
		self.__searchAreaTable.attach(searchLabel, 0, 1, 0, 1,
			Gtk.AttachOptions.FILL, Gtk.AttachOptions.FILL, 0, 0)
		searchLabel.show()
		
		self.__searchEntry = Gtk.Entry()
		self.__searchEntry.connect('key-press-event', self.search_key_pressed)
		self.__searchAreaTable.attach(self.__searchEntry, 1, 2, 0, 1,
			Gtk.AttachOptions.EXPAND | Gtk.AttachOptions.FILL, Gtk.AttachOptions.FILL, 0, 0)
		self.__searchEntry.show()
		
		# Replace area
		self.__replaceLabel = Gtk.Label(label="Replace:")
		self.__replaceLabel.set_justify(Gtk.Justification.RIGHT)
		self.__searchAreaTable.attach(self.__replaceLabel, 0, 1, 1, 2,
			Gtk.AttachOptions.FILL, Gtk.AttachOptions.FILL, 0, 0)
		self.__replaceLabel.show()
		
		self.__replaceEntry = Gtk.Entry()
		self.__replaceEntry.connect('key-press-event', self.replace_key_pressed)
		self.__searchAreaTable.attach(self.__replaceEntry, 1, 2, 1, 2,
			Gtk.AttachOptions.EXPAND | Gtk.AttachOptions.FILL, Gtk.AttachOptions.FILL, 0, 0)
		self.__replaceEntry.show()
		
		# Status bar
		self.__status = Gtk.Statusbar()
		self.__mainBox.pack_start(self.__status, False, False, 0)
		self.__status.show()
		self.set_status('Welcome to SparseEdit')
		
		# Basically ready to go, start accepting keys
		self.connect('key_press_event', self.key_pressed)
		
		# Do we have a file to open?
		if self.__filename is not None:
			self.open_file(filename)
			
		# Ensure the title is correct
		self.__update_title()
	
	#######################
	# Events
	
	def destroy(self, widget, data = None):
		"""
		Kills the window and lets the editor manager know we're dead
		"""
		self.__editor.stop_managing_editor(self)
	
	def __buffer_changed(self, widget):
		""" Catches when the buffer is changed """
		self.__update_title()
	
	def key_pressed(self, widget, event):
		"""
		Know when we change and when a shortcut is entered
		"""
		data = event.key
		if data.state & Gdk.ModifierType.CONTROL_MASK != 0:
			# Ctrl+...
			print("Control+" + str(data.keyval))
			
			if data.keyval == Gdk.KEY_u:
				self.undo()
			elif data.keyval == Gdk.KEY_y:
				self.redo()
			elif data.keyval == Gdk.KEY_s:
				self.save_file()
			elif data.keyval == Gdk.KEY_r:
				self.open_replace()
			elif data.keyval == Gdk.KEY_f:
				self.open_search()
				
		elif data.state & Gdk.ModifierType.SHIFT_MASK != 0:
			# Shift+...
			print("Shift+" + str(data.keyval))
		elif data.state & Gdk.ModifierType.MOD1_MASK != 0:
			# Alt+...
			print("Alt+" + str(data.keyval))
		elif data.keyval == Gdk.KEY_Escape:
			# Esc, close anything that's open other than the main editing window
			self.hide_search()
	
	#############################
	# File utilities
	
	def open_file(self, filename):
		"""
		Opens the given file. Prompts the user if the current file is unsaved
		"""
		if self.__buffer.get_modified():
			pass
		
		# Read in new file
		self.__filename = filename
		self.__buffer.begin_not_undoable_action()
		
		f = open(self.__filename, 'r')
		contents = f.read()
		self.__buffer.set_text(contents, len(contents))
		f.close()
		
		self.__buffer.end_not_undoable_action()
		self.__buffer.set_modified(False)
		
		# Move cursor to start of file
		self.__buffer.place_cursor(self.__buffer.get_start_iter())
		
		# Highlighting mode based on file opened
		self.set_language(self.detect_language())
		
		# Ensure our title is correct
		self.__update_title()
	
	def save_file(self):
		"""
		Saves the current file to disk. If there is no filename currently set,
		prompts the user for one
		"""
		if self.__filename is None:
			chooser = Gtk.FileChooserDialog("Save As...", self,
				Gtk.FileChooserAction.SAVE,
				('Save', 0, 'Cancel', 1))
			chooser.set_do_overwrite_confirmation(True)
			#chooser.connect('', )
			chooser.show()
			return
		
		f = open(self.__filename, 'w')
		f.write(self.__buffer.get_text(self.__buffer.get_start_iter(), self.__buffer.get_end_iter(), True))
		f.close()
		
		self.__buffer.set_modified(False)
	
	def undo(self):
		"""Undoes a single step"""
		self.__buffer.undo()
		
	def redo(self):
		"""Redoes a single step"""
		self.__buffer.redo()
	
	###########################
	# Find and replace
	
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
		# Show the replace stuff
		self.__replaceLabel.show()
		self.__replaceEntry.show()
		self.__replaceEntry.grab_focus()
		
		# Show the search area
		self.__searchAreaTable.show()
	
	def search_key_pressed(self, widget, event):
		"""
		Search when the user hits enter
		"""
		data = event.key
		if data.keyval == Gdk.KEY_Return:
			self.do_search(self.__searchEntry.get_text())
			
	def replace_key_pressed(self, widget, event):
		"""
		Search and replace when the user hits enter
		"""
		data = event.key
		if data.keyval == Gdk.KEY_Return:
			self.do_replace(self.__searchEntry.get_text(), self.__replaceEntry.get_text())
			
	def do_search(self, find_regex = None, loop_end = None):
		"""
		Find next instance of given regex in source buffer, starting from 
		where the cursor is currently located. If no regex is given, 
		the current match object is used (assuming there is one)
		"""
		if find_regex is not None:
			try:
				self.__currRegex = re.compile(find_regex)
			except re.error as e:
				self.set_status('Search error: ' + str(e))
				return None
		
		# Start next match from the current cursor location
		# If there is a selection (possibly from our previous a search),
		# start one beyond the first part of it
		start = self.__buffer.get_iter_at_mark(self.__buffer.get_insert()).get_offset()
		selPoint = self.__buffer.get_iter_at_mark(self.__buffer.get_selection_bound()).get_offset()
		if start < selPoint:
			start += 1
		elif selPoint < start:
			start = selPoint + 1
		
		while True:
			match = self.__currRegex.search(self.__buffer.get_text(
					self.__buffer.get_start_iter(), self.__buffer.get_end_iter(), True),
					start)
		
			if match is not None:
				# Select from the start to the end of match, if we found one
				startIter = self.__buffer.get_iter_at_offset(match.start())
				self.__buffer.select_range(
					startIter,
					self.__buffer.get_iter_at_offset(match.end()))
					
				self.set_status('Match found on line ' + str(startIter.get_line()))
				return match.start()
				
			elif start > 0:
				# No match found, start search from beginning
				self.set_status('No match found, looping from beginning')
				start = 0
			
			else:
				# No match found, even from beginning
				self.set_status('No match found')
				return None
	
	def do_replace(self, find_regex = None, replace_regex = None, loop_end = None):
		"""
		Find next instance of given regex in source buffer and replaces it with
		the replacement regex, starting from where the cursor is currently
		located. If no regex is given, the current match object is used
		(assuming there is one)
		"""
		startIndex = self.do_search(find_regex)
		if startIndex is not None:
			# Found a match, replace selection with replacement text
			endIndex = self.__buffer.get_iter_at_mark(self.__buffer.get_selection_bound()).get_offset()
			
			# This would be ideal, but it seems buggy:
			# self.__buffer.delete_selection(False, True)
			# self.__buffer.insert_at_cursor('test')
			
			text = self.__buffer.get_text(self.__buffer.get_start_iter(),
				self.__buffer.get_end_iter(), True)
			newText = text[:startIndex] + replace_regex + text[endIndex:]
			
			# Send back to buffer and select insertion
			self.__buffer.set_text(newText, len(newText))
			self.__buffer.select_range(
				self.__buffer.get_iter_at_offset(startIndex),
				self.__buffer.get_iter_at_offset(startIndex + len(replace_regex)))
			return True
			
		else:
			self.set_status('No matches to replace')
			return False
	
	###########################
	# Highlighting
	
	def set_language(self, lang):
		"""
		Changes the language being highligted. If lang is none, turns off
		highlighting
		"""
		# Disable highlighting until the new one is set, for performance
		self.__buffer.set_highlight_syntax(False)
		
		if lang is not None:
			self.__buffer.set_language(lang)
			self.__buffer.set_highlight_syntax(True)
		
	def detect_language(self):
		"""
		Guesses the language that the current file is in. If no file is open,
		it guesses "None"
		"""
		if self.__filename is None:
			return None
		else:
			mime = mimetypes.guess_type(self.__filename)[0]
			lang = lang_manager.guess_language(self.__filename, mime)
			return lang
	
	####################
	# UI updates
	
	def __update_title(self):
		"""
		Ensures that the window title is correct based on the current project
		and open file name
		"""
		if self.__buffer.get_modified():
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

	def set_status(self, msg):
		"""
		Sets the status to the given message
		"""
		cid = self.__status.get_context_id('main')
		self.__status.push(cid, msg)
		
