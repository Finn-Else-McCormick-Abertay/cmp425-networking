#include "system.h"

#include <system/system_manager.h>

ITickingSystem::ITickingSystem() { SystemManager::Registry::register_system(*this); }
ITickingSystem::~ITickingSystem() { SystemManager::Registry::unregister_system(*this); }