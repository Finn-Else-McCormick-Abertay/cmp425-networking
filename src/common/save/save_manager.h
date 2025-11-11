#pragma once

#include <util/helper/singleton.h>
#include <util/std_aliases.h>

#include <terrain/world.h>

class SaveManager { DECL_SINGLETON(SaveManager);
public:
    static void save(const World&);
    static opt<World> load();

private:
};