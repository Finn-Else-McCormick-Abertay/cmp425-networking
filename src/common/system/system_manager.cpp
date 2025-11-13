#include "system_manager.h"


DEFINE_SINGLETON(SystemManager);

void SystemManager::tick(float dt) { for (auto system : inst()._ticking) system->tick(dt); }


#define DEF_SYSTEM_REGISTRY_FUNCS(map, type)\
    void SystemManager::Registry::__register(type& system) { SystemManager::inst().map.insert(&system); }\
    void SystemManager::Registry::__unregister(type& system) { SystemManager::inst().map.erase(&system); }

DEF_SYSTEM_REGISTRY_FUNCS(_ticking, ITickingSystem)