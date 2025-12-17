#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <util/helper/singleton.h>

#include <world/world.h>
#include <network/networked.h>

class WorldManager : INetworked { DECL_SINGLETON_WITH_CONSTRUCTOR(WorldManager);
public:
    static void init();

    static opt_ref<World> world();
    static opt_ref<Level> level(const id&);

    static bool try_save();
    static bool load_from_file(const str& world_name);
    static bool create(const str& world_name, bool replace = true);

private:
    opt<World> _world;

    static void internal_load(World&&, bool authority);
    static void internal_unload();
    static opt<str> validate_name(const str&);

    virtual result<LogicalPacket, str> get_requested_message(const packet_id& id) const override;
    virtual result<success_t, str> read_message(LogicalPacket&&) override;
};