#include "components.h"

#include <network/network_manager.h>
#include <data/data_manager.h>
#include <save/save_manager.h>
#include <world/world_manager.h>
#include <actor/actor_manager.h>
#include <alias/ranges.h>
#include <cctype>

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

lyra::cli cli::interpolation() {
    return lyra::cli()
        | lyra::opt(
            [](str arg){
                // This is fucked but it's the best way I have to deserialise this. Don't ask me why I have to add quotes, I do not know.
                str normalized_arg = arg | views::transform(toupper) | ranges::to<str>();
                auto result = glz::read_json<actor::InterpolationMode>(fmt::format("\"{}\"", normalized_arg));
                if (result) {
                    actor::InterpolationMode mode = *result;
                    ActorManager::set_interpolation_mode(mode);
                }
                else print<warning>("{} is not a valid interpolation mode.", normalized_arg);
            }, "interpolation")
            ["-i"]["--interpolation"]
            ("Actor interpolation mode.")
            .optional();
}

lyra::cli cli::client() {
    return lyra::cli()
        | lyra::opt(
            [](str ip){
                if (auto opt = SocketAddress::resolve(ip)) NetworkManager::set_server_address(*opt);
                else print<warning>("{} is not a valid address.", ip);
            }, "server")
            ["-s"]["--server"]["--connect"]
            ("Server address.")
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