import pygtk
pygtk.require('2.0')
from gi.repository import Gtk, GtkSource, Gdk
import mimetypes

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
		self.__searchAreaTable.attach(self.__replaceEntry, 1, 2, 1, 2,
			Gtk.AttachOptions.EXPAND | Gtk.AttachOptions.FILL, Gtk.AttachOptions.FILL, 0, 0)
		self.__replaceEntry.show()
		
		# Basically ready to go, start accepting keys
		self.connect('key_press_event', self.key_pressed)
		
		# Do we have a file to open?
		if self.__filename is not None:
			self.open_file(filename)
			
		# Ensure the title is correct
		self.__update_title()
		
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
		Changes the language being highligted. If lang is none, turns off
		highlighting
		"""
		# Disable highlighting until the new one is set, for performance
		#self.__buffer.set_highlight_syntax(False)
		
		#if lang is not None:
			#self.__buffer.set_language(lang)
			#self.__buffer.set_highlight_syntax(True)
		
	def detect_language(self):
		"""
		Guesses the language that the current file is in. If no file is open,
		it guesses "None"
		"""
		manager = GtkSource.SourceLanguageManager.get_default()
		
		if self.__filename is None:
			return None
		else:
			mime = mimetypes.guess_type(self.__filename)[0]
			lang = manager.guess_language(self.__filename, mime)
			self.__buffer.set_language(lang)
			#return lang
	
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

