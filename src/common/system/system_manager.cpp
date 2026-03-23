#include "system_manager.h"

DEFINE_SINGLETON(SystemManager);

void SystemManager::perform_tick(float dt) {
    for (auto system : inst()._ticking) system->tick(dt);
}

void SystemManager::perform_fixed_tick() {
    for (auto system : inst()._fixed_ticking) system->fixed_tick();
    ++inst()._elapsed_fixed_ticks;
}

uint64 SystemManager::get_fixed_tick() { return inst()._elapsed_fixed_ticks; }
void SystemManager::jump_fixed_tick(uint64 new_tick) {
    //int64 tick_diff = (int64)new_tick - (int64)inst()._elapsed_fixed_ticks;
    //print<debug, SystemManager>("Jumped {} ticks", tick_diff);

    inst()._elapsed_fixed_ticks = new_tick;
}


#define DEF_SYSTEM_REGISTRY_FUNCS(map, type)\
    void SystemManager::Registry::__register(type& system) { SystemManager::inst().map.insert(&system); }\
    void SystemManager::Registry::__unregister(type& system) { SystemManager::inst().map.erase(&system); }\
    static_assert(true)

DEF_SYSTEM_REGISTRY_FUNCS(_ticking, ITickingSystem);
DEF_SYSTEM_REGISTRY_FUNCS(_fixed_ticking, IFixedTickingSystem);