#pragma once

#include <system/system.h>
#include <set>
#include <util/singleton.h>

class SystemManager {
    SINGLETON_DECL(SystemManager);
public:
    SINGLETON_REGISTRY(ITickingSystem);

    static void tick(float dt);

private:
    std::set<ITickingSystem*> _ticking;
};