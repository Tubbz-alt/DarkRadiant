#include "LanguageManager.h"

namespace language
{

void LanguageManager::registerProvider(const ILocalisationProvider::Ptr& instance)
{
	assert(!_provider); // only one provider supported right now

	_provider = instance;
}

void LanguageManager::clearProvider()
{
	_provider.reset();
}

std::string LanguageManager::getLocalisedString(const char* stringToLocalise)
{
	return _provider ? _provider->getLocalisedString(stringToLocalise) : stringToLocalise;
}

} // namespace
