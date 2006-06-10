#ifndef TEXTPROPERTYEDITOR_H_
#define TEXTPROPERTYEDITOR_H_

#include "PropertyEditor.h"

namespace ui
{

/* TextPropertyEditor
 * 
 * PropertyEditor that displays and edits a simple text string (all Entity key
 * values are in fact text strings).
 */

class TextPropertyEditor:
    public PropertyEditor
{
private:

	// The text entry field
	GtkWidget* _textEntry;

public:

	// Construct a TextPropertyEditor with an entity and key to edit
	TextPropertyEditor(Entity* entity, const std::string& name);
	
	// Construct a blank TextPropertyEditor for use in the PropertyEditorFactory
	TextPropertyEditor();

	// Create a new TextPropertyEditor
    virtual PropertyEditor* createNew(Entity* entity, const std::string& name) {
    	return new TextPropertyEditor(entity, name);
    }
    
    virtual void setValue(const std::string&);
    virtual const std::string getValue();
    
};

}

#endif /*TEXTPROPERTYEDITOR_H_*/
