#pragma once

#include <prelude.h>
#include <prelude/containers.h>
#include <glaze/glaze.hpp>

#include <network/networked.h>
#include <data/namespaced_id.h>
#include <world/level.h>
#include <alias/ranges.h>

class World : INetworked {
public:
    World(); World(World&&); //World(const World&);
    World(const str& name, bool authority = true);

    const str& name() const;
    bool is_authority() const;

    opt_ref<Level> level(const id& level_id);
    opt_cref<Level> level(const id& level_id) const;

    inline auto levels() const { return _levels | views::keys; }

private:
    str _name; bool _authority;
    hashmap<id, Level> _levels;
    friend class glz::meta<World>;
    friend class WorldManager;

    void network_reregister();

    bool make_level(const id& level_id);
    
    virtual dyn_arr<LogicalPacket> get_outstanding_messages() override;
    virtual result<LogicalPacket, str> get_requested_message(const packet_id& id) const override;
    virtual result<success_t, str> read_message(LogicalPacket&&) override;

    LogicalPacket package_chunks(id level, dyn_arr<ivec2>&& chunks) const;
};

template<> struct glz::meta<World> {
    static constexpr auto value = object(
        "name", &World::_name,
        "levels", &World::_levels
    );
};