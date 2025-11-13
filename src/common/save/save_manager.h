#pragma once

#include <util/helper/singleton.h>

#include <prelude.h>
#include <prelude/opt.h>

#include <terrain/world.h>

class SaveManager { DECL_SINGLETON(SaveManager);
public:
    static void save(const World&);
    static opt<World> load();

private:
};