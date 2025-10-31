#include "system.h"

#include <system/system_manager.h>

ITickingSystem::ITickingSystem() { SystemManager::Registry::__register(*this); }
ITickingSystem::~ITickingSystem() { SystemManager::Registry::__unregister(*this); }