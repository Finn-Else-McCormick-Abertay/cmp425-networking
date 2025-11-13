#pragma once

#include <system/system.h>
#include <util/helper/singleton.h>

#include <prelude/containers.h>

class SystemManager { DECL_SINGLETON(SystemManager);
public:
    DECL_REGISTRY(ITickingSystem);

    static void tick(float dt);

private:
    set<ITickingSystem*> _ticking;
};