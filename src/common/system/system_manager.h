#pragma once

#include <system/system.h>
#include <set>
#include <util/helper/singleton.h>

class SystemManager { DECL_SINGLETON(SystemManager);
public:
    DECL_REGISTRY(ITickingSystem);

    static void tick(float dt);

private:
    std::set<ITickingSystem*> _ticking;
};