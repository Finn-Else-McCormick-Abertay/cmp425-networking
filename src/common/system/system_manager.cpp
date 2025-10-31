#include "system_manager.h"

using namespace std;

SINGLETON_INST_DEF(SystemManager)

void SystemManager::tick(float dt) { for (auto system : inst()._ticking) system->tick(dt); }


#define DEF_SYSTEM_REGISTRY_FUNCS(map, type)\
    void SystemManager::Registry::register_system(type& system) { SystemManager::inst().map.insert(&system); }\
    void SystemManager::Registry::unregister_system(type& system) { SystemManager::inst().map.erase(&system); }

DEF_SYSTEM_REGISTRY_FUNCS(_ticking, ITickingSystem)