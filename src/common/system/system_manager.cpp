#include "system_manager.h"

DEFINE_SINGLETON(SystemManager);

void SystemManager::tick(float dt) { for (auto system : inst()._ticking) system->tick(dt); }
void SystemManager::fixed_tick(uint64 elapsed_ticks) { for (auto system : inst()._fixed_ticking) system->fixed_tick(elapsed_ticks); }


#define DEF_SYSTEM_REGISTRY_FUNCS(map, type)\
    void SystemManager::Registry::__register(type& system) { SystemManager::inst().map.insert(&system); }\
    void SystemManager::Registry::__unregister(type& system) { SystemManager::inst().map.erase(&system); }\
    static_assert(true)

DEF_SYSTEM_REGISTRY_FUNCS(_ticking, ITickingSystem);
DEF_SYSTEM_REGISTRY_FUNCS(_fixed_ticking, IFixedTickingSystem);