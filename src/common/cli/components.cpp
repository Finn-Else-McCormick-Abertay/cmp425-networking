#include "components.h"

#include <network/network_manager.h>
#include <data/data_manager.h>
#include <save/save_manager.h>
#include <world/world_manager.h>

lyra::cli cli::dirs() {
    return lyra::cli()
        | lyra::opt(
            [](str dir) {
                DataManager::set_resources_folder(dir);
            }, "resource_dir")
            ["--resource_dir"]
            ("Path to the resources folder.")
            .optional()
        | lyra::opt(
            [](str dir) {
                SaveManager::set_user_folder(dir);
            }, "user_dir")
            ["--user_dir"]
            ("Path to user data folder.")
            .optional();
}

lyra::cli cli::client() {
    return lyra::cli()
        | lyra::opt(
            [](str ip){
                if (auto opt = sf::IpAddress::resolve(ip)) NetworkManager::connect_to_server(*opt);
                else print<warning>("{} is not a valid ip.", ip);
            }, "server")
            ["-s"]["--server"]["--connect"]
            ("Server ip address.")
            .optional()
        | lyra::opt([](str username) {
                NetworkManager::set_username(username);
            }, "username")
            ["-u"]["--user"]["--username"]
            ("Username to connect under.")
            .optional();
}

lyra::cli cli::world() {
    return lyra::cli()
        | lyra::opt(
            [](str world){
                if (!WorldManager::load_from_file(world)) WorldManager::create(world);
            }, "world")
            ["-w"]["--world"]
            ("Name of world to load.")
            .optional();
}