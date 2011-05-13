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
		self.__isFullscreen = False
		
		# Basic window stuff
		self.set_default_size(500, 500)
		self.connect('window-state-event', self.__window_state_change)
		self.__mainBox = Gtk.VBox(homogeneous = False, spacing = 3)
		self.add(self.__mainBox)
		self.__mainBox.show()
		
		# Main events
		self.connect("destroy", self.close)
		
		# Menubar (hidden by default)
		self.__menubar = Gtk.MenuBar()
		self.__mainBox.pack_start(self.__menubar, False, False, 0)
		
		# File menu
		fileMenuTop = Gtk.MenuItem(label = 'File')
		self.__menubar.append(fileMenuTop)
		fileMenuTop.show()
		
		fileMenu = Gtk.Menu()
		fileMenuTop.set_submenu(fileMenu)
		item = Gtk.MenuItem(label = 'New')
		fileMenu.append(item)
		item.show()
		item.connect('activate', self.save_file)
		
		item = Gtk.MenuItem(label = 'Open')
		fileMenu.append(item)
		item.show()
		item.connect('activate', self.save_file)
		
		item = Gtk.SeparatorMenuItem()
		fileMenu.append(item)
		item.show()
		
		item = Gtk.MenuItem(label = 'Save')
		fileMenu.append(item)
		item.show()
		item.connect('activate', self.save_file)
		
		item = Gtk.MenuItem(label = 'Save As...')
		fileMenu.append(item)
		item.show()
		item.connect('activate', self.save_file_as)
		
		item = Gtk.SeparatorMenuItem()
		fileMenu.append(item)
		item.show()
		
		item = Gtk.MenuItem(label = 'Close')
		fileMenu.append(item)
		item.show()
		item.connect('activate', self.close)
		
		# Edit menu
		editMenuTop = Gtk.MenuItem(label = 'Edit')
		self.__menubar.append(editMenuTop)
		editMenuTop.show()
		
		editMenu = Gtk.Menu()
		editMenuTop.set_submenu(editMenu)
		item = Gtk.MenuItem(label = 'Undo')
		editMenu.append(item)
		item.show()
		item.connect('activate', self.undo)
		
		item = Gtk.MenuItem(label = 'Redo')
		editMenu.append(item)
		item.show()
		item.connect('activate', self.redo)
		
		item = Gtk.SeparatorMenuItem()
		editMenu.append(item)
		item.show()
		
		item = Gtk.MenuItem(label = 'Cut')
		editMenu.append(item)
		item.show()
		item.connect('activate', self.cut)
		
		item = Gtk.MenuItem(label = 'Copy')
		editMenu.append(item)
		item.show()
		item.connect('activate', self.copy)
		
		item = Gtk.MenuItem(label = 'Paste')
		editMenu.append(item)
		item.show()
		item.connect('activate', self.paste)
		
		# Project
		projectMenuTop = Gtk.MenuItem(label = 'Project')
		self.__menubar.append(projectMenuTop)
		projectMenuTop.show()
		
		# Create editor area and attach the main handler we'll be relying on
		self.__text = GtkSource.SourceView(buffer = GtkSource.SourceBuffer())
		self.__text.set_show_line_numbers(True)
		self.__text.set_auto_indent(True)
		self.__text.set_smart_home_end(GtkSource.SourceSmartHomeEndType.ALWAYS)
		
		self.__buffer = self.__text.get_buffer()
		self.__buffer.set_highlight_matching_brackets(True)
		self.__buffer.set_max_undo_levels(200)
		self.__buffer.connect('modified-changed', self.__buffer_changed)
		self.__buffer.connect('mark-set', self.__cursor_moved)
		
		#self.__clipboard = Gtk.Clipboard.get(Gdk.Atom.intern('PRIMARY'))
		#print(self.__clipboard)
		
		scroll = Gtk.ScrolledWindow()
		scroll.set_policy(Gtk.PolicyType.AUTOMATIC, Gtk.PolicyType.AUTOMATIC)
		scroll.add(self.__text)
		self.__text.show()
		self.__mainBox.pack_start(scroll, True, True, 0)
		scroll.show()
		
		# Create search area (do not show, only activates on shortcut key)
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
		self.__searchEntry.connect('activate', self.search_key_pressed)
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
		self.__replaceEntry.connect('activate', self.replace_key_pressed)
		self.__searchAreaTable.attach(self.__replaceEntry, 1, 2, 1, 2,
			Gtk.AttachOptions.EXPAND | Gtk.AttachOptions.FILL, Gtk.AttachOptions.FILL, 0, 0)
		self.__replaceEntry.show()
		
		# Status bar
		self.__status = Gtk.Statusbar()
		self.__mainBox.pack_start(self.__status, False, False, 0)
		self.__status.show()
		
		self.__locationLbl = Gtk.Label()
		self.__status.pack_start(self.__locationLbl, False, False, 0)
		self.__locationLbl.show()
		
		self.set_status('Welcome to SparseEdit')
		
		# Basically ready to go, start accepting keys
		self.connect('key_press_event', self.key_pressed)
		
		# Do we have a file to open?
		if self.__filename is not None:
			self.open_file(filename)
			
		# Ensure the title is correct
		self.__update_title()
	
	#######################
	# Main events
	
	def close(self, widget = None, data = None):
		"""
		Kills the window and lets the editor manager know we're dead
		"""
		self.__editor.stop_managing_editor(self)
	
	def __window_state_change(self, widget, event):
		"""
		Keeps track of whether the window is full screen or not
		"""
		if event.window_state.new_window_state & Gdk.WindowState.FULLSCREEN != 0:
			self.__isFullscreen = True
		else:
			self.__isFullscreen = False
	
	def __buffer_changed(self, widget):
		"""
		Catches when the buffer is changed
		"""
		self.__update_title()
	
	def __cursor_moved(self, widget, newIter, newMark):
		"""
		Ensures that the current line/column display is correct
		"""
		cursorIter = self.__buffer.get_iter_at_mark(self.__buffer.get_insert())
		self.__locationLbl.set_text('Ln ' + str(cursorIter.get_line() + 1)
			+ ', Col ' + str(cursorIter.get_line_offset() + 1))
	
	def cut(self, ignored = None):
		"""
		Cuts the current selection to the clipboard
		"""
		self.__buffer.cut_clipboard(self.__clipboard)
		
	def copy(self, ignored = None):
		"""
		Copies the current selection to the clipboard
		"""
		self.__buffer.cut_clipboard(self.__clipboard)
		
	def paste(self, ignored = None):
		"""
		Pastes the clipboard to the cursor
		"""
		self.__buffer.cut_clipboard(self.__clipboard, None, True)
	
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
			elif data.keyval == Gdk.KEY_w:
				self.close()
			elif data.keyval == Gdk.KEY_n:
				self.__editor.new_file()
			elif data.keyval == Gdk.KEY_g:
				self.__searchEntry.activate()
			elif data.keyval == Gdk.KEY_Return:
				if self.__isFullscreen:
					self.unfullscreen()
				else:
					self.fullscreen()
			elif data.keyval == Gdk.KEY_Tab:
				self.__editor.switch_to_next_editor(self)
				
		elif data.state & Gdk.ModifierType.MOD1_MASK != 0:
			# Alt+...
			print("Alt+" + str(data.keyval))
		
		elif data.keyval == Gdk.KEY_Alt_L or data.keyval == Gdk.KEY_Alt_R:
			# Alt by itself
			self.show_menubar()
		
		elif data.keyval == Gdk.KEY_Escape:
			# Esc, close anything that's open other than the main editing window
			self.hide_search()
			self.hide_menubar()
			
		elif data.keyval == Gdk.KEY_F3:
			# Re-search, just as if the user hit 'enter' on the search box
			self.__searchEntry.activate()
	
	#############################
	# File utilities
	
	def open_file(self, filename):
		"""
		Opens the given file. Prompts the user if the current file is unsaved
		"""
		if self.__buffer.get_modified():
			self.set_status('Save current file first')
			return
		
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
	
	def save_file_as(self, ignored = None):
		"""
		Prompts the user for where to save the current file 
		"""
		chooser = Gtk.FileChooserDialog("Save As...", self,
			Gtk.FileChooserAction.SAVE,
			('Save', 0, 'Cancel', 1))
		chooser.set_do_overwrite_confirmation(True)
		#chooser.connect('', )
		chooser.show()
		
	def save_file(self, ignored = None):
		"""
		Saves the current file to disk. If there is no filename currently set,
		prompts the user for one
		"""
		if self.__filename is None:
			self.save_file_as()
			return
		
		f = open(self.__filename, 'w')
		f.write(self.__buffer.get_text(self.__buffer.get_start_iter(), self.__buffer.get_end_iter(), True))
		f.close()
		
		self.set_status('Saved to ' + self.__filename)
			
		self.__buffer.set_modified(False)
	
	def undo(self):
		"""Undoes a single step"""
		if self.__buffer.can_undo():
			self.__buffer.undo()
			self.set_status('Undo')
		else:
			self.set_status('Nothing to undo')
		
	def redo(self):
		"""Redoes a single step"""
		if self.__buffer.can_redo():
			self.__buffer.redo()
			self.set_status('Redo')
		else:
			self.set_status('Nothing to redo')
	
	###########################
	# Find and replace
	
	def open_search(self):
		"""
		Opens the search box, hiding the replacement components
		"""
		# Is there a current selection? If so, use that as the literal search term
		if self.__buffer.get_has_selection():
			ignored, startIter, endIter = self.__buffer.get_selection_bounds()
			searchText = self.__buffer.get_text(startIter, endIter, True)
			
			# Do some escaping to make it be interpreted literally, not as a regex
			searchText = searchText.replace('\\', '\\\\')
			searchText = searchText.replace('.', '\\.')
			searchText = searchText.replace('*', '\\*')
			searchText = searchText.replace('?', '\\?')
			searchText = searchText.replace('+', '\\+')
			searchText = searchText.replace('(', '\\(')
			searchText = searchText.replace(')', '\\)')
			searchText = searchText.replace('[', '\\[')
			searchText = searchText.replace('^', '\\^')
			searchText = searchText.replace('|', '\\|')
			
			# And use it
			self.__searchEntry.set_text(searchText)
		
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
		self.open_search()
		
		# Show the replace stuff
		self.__replaceLabel.show()
		self.__replaceEntry.show()
		self.__replaceEntry.grab_focus()
	
	def search_key_pressed(self, widget):
		"""
		Search when the user hits enter
		"""
		self.do_search(self.__searchEntry.get_text())
			
	def replace_key_pressed(self, widget):
		"""
		Search and replace when the user hits enter
		"""
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
	# Menu
	
	def show_menubar(self):
		"""
		Displays the menu bar to the user
		"""
		self.__menubar.show()
		
	def hide_menubar(self):
		"""
		Hides the menubar from the user
		"""
		self.__menubar.hide()
	
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
		
