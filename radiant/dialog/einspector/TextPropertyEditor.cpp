#include "TextPropertyEditor.h"
#include "EntityKeyValueVisitor.h"

#include "exception/InvalidKeyException.h"

#include <gtk/gtk.h>

namespace ui
{

// Blank ctor

TextPropertyEditor::TextPropertyEditor() {}

// Constructor. Create the GTK widgets here

TextPropertyEditor::TextPropertyEditor(Entity* entity, const std::string& name):
	PropertyEditor(entity, name, "text") 
{
	GtkWidget* editBox = gtk_hbox_new(FALSE, 3);
	gtk_container_set_border_width(GTK_CONTAINER(editBox), 3);
	_textEntry = gtk_entry_new();
	
	std::string caption = getKey();
	caption.append(": ");
	gtk_box_pack_start(GTK_BOX(editBox), gtk_label_new(caption.c_str()), FALSE, FALSE, 0);
	gtk_box_pack_end(GTK_BOX(editBox), _textEntry, TRUE, TRUE, 0);
	
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(getEditWindow()),
										  editBox);
    refresh();
}

// Refresh and commit

void TextPropertyEditor::setValue(const std::string& val) {
	gtk_entry_set_text(GTK_ENTRY(_textEntry), val.c_str());
}

const std::string TextPropertyEditor::getValue() {
	return std::string(gtk_entry_get_text(GTK_ENTRY(_textEntry)));
}

}
