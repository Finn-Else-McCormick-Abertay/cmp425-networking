#pragma once

#include <system/system.h>
#include <util/helper/singleton.h>

#include <prelude.h>
#include <prelude/containers.h>

class SystemManager { DECL_SINGLETON(SystemManager);
public:
    DECL_MULTI_REGISTRY(ITickingSystem, IFixedTickingSystem);

    static void tick(float dt);

    static void fixed_tick(uint64 elapsed_ticks);

private:
    set<ITickingSystem*> _ticking;
    set<IFixedTickingSystem*> _fixed_ticking;
};