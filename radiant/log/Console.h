#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include "icommandsystem.h"

#include <gtkmm/box.h>

#include "ui/common/CommandEntry.h"
#include "LogDevice.h"

namespace gtkutil { class ConsoleView; }

namespace ui
{

class Console;
typedef boost::shared_ptr<Console> ConsolePtr;

/**
 * greebo: The Console class encapsulates a GtkTextView and represents
 *         the "device", which the LogWriter is writing its output to.
 *
 *         The Console is a singleton which needs to be constructed and packed
 *         during mainframe construction.
 */
class Console :
	public Gtk::VBox,
	public applog::LogDevice
{
private:
	gtkutil::ConsoleView* _view;

	// The entry box for console commands
	CommandEntry* _commandEntry;

	// Private constructor, creates the Gtk structures
	Console();

public:
	/**
	 * greebo: Static command target for toggling the console.
	 */
	static void toggle(const cmd::ArgumentList& args);

	// Command target to clear the console
	void clearCmd(const cmd::ArgumentList& args);

	/**
	 * greebo: Writes the given output string to the Console.
	 * The log level indicates which tag is used for colouring the output.
	 * (Note: this gets called by the LogWriter automatically).
	 */
	void writeLog(const std::string& outputStr, applog::ELogLevel level);

	/**
	 * greebo: Detaches itself from the LogWriter
	 */
	static void destroy();

	// Accessor to the static singleton instance.
	static Console& Instance();

private:
	void shutdown();

	// Static shared pointer
	static ConsolePtr& InstancePtr();
};

} // namespace ui

#endif /* _CONSOLE_H_ */
