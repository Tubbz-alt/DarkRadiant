#include "RadiantModule.h"

#include <iostream>
#include <ctime>

#include "iregistry.h"
#include "icommandsystem.h"
#include "itextstream.h"
#include "iuimanager.h"
#include "ieventmanager.h"
#include "i18n.h"
#include "imainframe.h"

#include "scene/Node.h"

#include "module/StaticModule.h"

namespace radiant
{

sigc::signal<void> RadiantModule::signal_radiantStarted() const
{
    return _radiantStarted;
}

sigc::signal<void> RadiantModule::signal_radiantShutdown() const
{
    return _radiantShutdown;
}

void RadiantModule::broadcastShutdownEvent()
{
    _radiantShutdown.emit();
    _radiantShutdown.clear();
}

// Broadcasts a "startup" event to all the listeners
void RadiantModule::broadcastStartupEvent()
{
    _radiantStarted.emit();
}

// RegisterableModule implementation
const std::string& RadiantModule::getName() const
{
	static std::string _name(MODULE_RADIANT_APP);
	return _name;
}

const StringSet& RadiantModule::getDependencies() const
{
	static StringSet _dependencies;
	return _dependencies;
}

void RadiantModule::initialiseModule(const IApplicationContext& ctx)
{
	rMessage() << getName() << "::initialiseModule called." << std::endl;

	// Reset the node id count
  	scene::Node::resetIds();

    // Subscribe for the post-module init event
    module::GlobalModuleRegistry().signal_allModulesInitialised().connect(
        sigc::mem_fun(this, &RadiantModule::postModuleInitialisation));
}

void RadiantModule::shutdownModule()
{
	rMessage() << getName() << "::shutdownModule called." << std::endl;

	_radiantStarted.clear();
    _radiantShutdown.clear();
}

void RadiantModule::postModuleInitialisation()
{
    // Initialise the mainframe
    GlobalMainFrame().construct();

	// Broadcast the startup event
    broadcastStartupEvent();

    // Load the shortcuts from the registry
    GlobalEventManager().loadAccelerators();

	// Show the top level window as late as possible
	GlobalMainFrame().getWxTopLevelWindow()->Show();

    time_t localtime;
    time(&localtime);
    rMessage() << "Startup complete at " << ctime(&localtime) << std::endl;
}

// Define the static Radiant module
module::StaticModule<RadiantModule> radiantModule;

// Return the static Radiant module to other code within the main binary
RadiantModulePtr getGlobalRadiant()
{
	return std::static_pointer_cast<RadiantModule>(
		module::GlobalModuleRegistry().getModule(MODULE_RADIANT_APP)
	);
}

} // namespace radiant

