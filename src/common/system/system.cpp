#include "system.h"

#include <system/system_manager.h>

ITickingSystem::ITickingSystem() { SystemManager::Registry::__register(*this); }
ITickingSystem::~ITickingSystem() { SystemManager::Registry::__unregister(*this); }

IFixedTickingSystem::IFixedTickingSystem() { SystemManager::Registry::__register(*this); }
IFixedTickingSystem::~IFixedTickingSystem() { SystemManager::Registry::__unregister(*this); }