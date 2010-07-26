#include "DifficultyDialog.h"

#include "i18n.h"
#include "iregistry.h"
#include "iundo.h"
#include "imainframe.h"
#include "iscenegraph.h"
#include "string/string.h"
#include "gtkutil/RightAlignment.h"
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

#include <iostream>

namespace ui {

namespace {
	const char* const WINDOW_TITLE = N_("Difficulty Editor");
	
	const std::string RKEY_ROOT = "user/ui/difficultyDialog/";
	const std::string RKEY_WINDOW_STATE = RKEY_ROOT + "window";
}

DifficultyDialog::DifficultyDialog() :
	gtkutil::BlockingTransientWindow(_(WINDOW_TITLE), GlobalMainFrame().getTopLevelWindow())
{
	// Load the settings
	_settingsManager.loadSettings();

	// Set the default border width in accordance to the HIG
	gtk_container_set_border_width(GTK_CONTAINER(getWindow()), 12);
	gtk_window_set_type_hint(GTK_WINDOW(getWindow()), GDK_WINDOW_TYPE_HINT_DIALOG);
	gtk_window_set_modal(GTK_WINDOW(getWindow()), TRUE);
	
	g_signal_connect(G_OBJECT(getWindow()), "key-press-event", 
					 G_CALLBACK(onWindowKeyPress), this);
	
	// Create the widgets
	populateWindow();

	// Connect the window position tracker
	_windowPosition.loadFromPath(RKEY_WINDOW_STATE);
	
	_windowPosition.connect(this);
	_windowPosition.applyPosition();

	// Show the dialog, this enters the gtk main loop
	show();
}

void DifficultyDialog::_preHide() {
	// Tell the position tracker to save the information
	_windowPosition.saveToPath(RKEY_WINDOW_STATE);
}

void DifficultyDialog::_preShow() {
	// Restore the position
	_windowPosition.applyPosition();
}

void DifficultyDialog::createDifficultyEditors() {
	int numLevels = GlobalRegistry().getInt(RKEY_DIFFICULTY_LEVELS);
	for (int i = 0; i < numLevels; i++) {
		// Acquire the settings object
		difficulty::DifficultySettingsPtr settings = _settingsManager.getSettings(i);

		if (settings != NULL) {
			_editors.push_back(
				DifficultyEditorPtr(new DifficultyEditor(
					_settingsManager.getDifficultyName(i), settings)
				)
			);
		}
	}

	for (std::size_t i = 0; i < _editors.size(); i++) {
		DifficultyEditor& editor = *_editors[i];

		GtkWidget* label = editor.getNotebookLabel();
		// Show the widgets before using them as label, they won't appear otherwise	
		gtk_widget_show_all(label);

		gtk_notebook_append_page(_notebook, editor.getEditor(), label);
	}
}

void DifficultyDialog::populateWindow() {
	// Create the overall vbox
	_dialogVBox = gtk_vbox_new(FALSE, 12);
	gtk_container_add(GTK_CONTAINER(getWindow()), _dialogVBox);
	
	// Create the notebook and add it to the vbox
	_notebook = GTK_NOTEBOOK(gtk_notebook_new());
	gtk_box_pack_start(GTK_BOX(_dialogVBox), GTK_WIDGET(_notebook), TRUE, TRUE, 0);
	
	// Create and pack the editors
	createDifficultyEditors();

	// Pack in dialog buttons
	gtk_box_pack_start(GTK_BOX(_dialogVBox), createButtons(), FALSE, FALSE, 0);
}

// Lower dialog buttons
GtkWidget* DifficultyDialog::createButtons() {

	GtkWidget* buttonHBox = gtk_hbox_new(TRUE, 12);
	
	// Save button
	GtkWidget* okButton = gtk_button_new_from_stock(GTK_STOCK_OK);
	g_signal_connect(G_OBJECT(okButton), "clicked", G_CALLBACK(onSave), this);
	gtk_box_pack_end(GTK_BOX(buttonHBox), okButton, TRUE, TRUE, 0);
	
	// Close Button
	_closeButton = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
	g_signal_connect(
		G_OBJECT(_closeButton), "clicked", G_CALLBACK(onClose), this);
	gtk_box_pack_end(GTK_BOX(buttonHBox), _closeButton, TRUE, TRUE, 0);
	
	return gtkutil::RightAlignment(buttonHBox);	
}

void DifficultyDialog::save() {
	// Consistency check can go here
	
	// Scoped undo object
	UndoableCommand command("editDifficulty");
	
	// Save the working set to the entity
	_settingsManager.saveSettings();
}

void DifficultyDialog::onSave(GtkWidget* button, DifficultyDialog* self) {
	self->save();
	self->destroy();
}

void DifficultyDialog::onClose(GtkWidget* button, DifficultyDialog* self) {
	self->destroy();
}

gboolean DifficultyDialog::onWindowKeyPress(
	GtkWidget* dialog, GdkEventKey* event, DifficultyDialog* self)
{
	if (event->keyval == GDK_Escape) {
		self->destroy();
		// Catch this keyevent, don't propagate
		return TRUE;
	}
	
	// Propagate further
	return FALSE;
}

// Static command target
void DifficultyDialog::showDialog(const cmd::ArgumentList& args) {
	// Construct a new instance, this enters the main loop
	DifficultyDialog _editor;
}

} // namespace ui
