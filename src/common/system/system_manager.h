#pragma once

#include <system/system.h>
#include <util/helper/singleton.h>

#include <prelude.h>
#include <prelude/containers.h>
#include <alias/chrono.h>

class SystemManager { DECL_SINGLETON(SystemManager);
public:
    DECL_MULTI_REGISTRY(ITickingSystem, IFixedTickingSystem);

    static void perform_tick(float dt);
    static void perform_fixed_tick();

    static uint64 get_fixed_tick();
    static void jump_fixed_tick(uint64);
    
    // 60 fixed-rate steps per second
    static constexpr chrono::microseconds FIXED_TIMESTEP = 16ms;
private:
    set<ITickingSystem*> _ticking;
    set<IFixedTickingSystem*> _fixed_ticking;

    uint64 _elapsed_fixed_ticks;
};