#pragma once

#include <util/helper/singleton.h>

#include <prelude.h>
#include <prelude/opt.h>

#include <world/world.h>

#include <alias/filesystem.h>

class SaveManager { DECL_SINGLETON(SaveManager);
public:
    static const fs::path& user_folder();
    static void set_user_folder(const fs::path&);
    
    static result<success_t, str> save_world(const World&);
    static result<World, str> load_world(const str& name);

private:
    fs::path _user_folder = "user";
};