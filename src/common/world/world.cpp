#include "world.h"

#include <alias/ranges.h>
#include <deque>

World::World() : _name(), _authority(false), INetworked(construct_noinit) {}

World::World(const str& name, bool authority) : _name(name), _authority(authority), INetworked(::network_id("world"_id, name)) {}

World::World(World&& rhs) : _name(move(rhs._name)), _authority(rhs._authority), _levels(move(rhs._levels)), INetworked(move(rhs)) {}
//World::World(const World& rhs) : _name(rhs._name), _authority(rhs._authority), _levels(rhs._levels), INetworked(construct_noinit) {}

const str& World::name() const { return _name; }
bool World::is_authority() const { return _authority; }

opt_cref<Level> World::level(const id& level_id) const {
    if (!_levels.contains(level_id)) return nullopt;
    return _levels.at(level_id);
}
opt_ref<Level> World::level(const id& level_id) {
    if (!_levels.contains(level_id)) return nullopt;
    return _levels.at(level_id);
}

bool World::make_level(const id& level_id) {
    auto [it, success] = _levels.try_emplace(level_id);
    if (success) print<::success, World>("Created level {}.", _name, level_id);
    return success;
}

void World::network_reregister() { set_network_id(::network_id("world"_id, _name)); }

LogicalPacket World::package_chunks(id level_id, dyn_arr<ivec2>&& chunks) const {
    dyn_arr<str> id_args = { level_id.to_str() };
    id_args.append_range(chunks | views::transform([](const ivec2& v){ return fmt::format("{}", fmt::join(v, ",")); }));

    auto id = packet_id("chunk", move(id_args));
    auto packet = LogicalPacket(id);

    if (auto level_opt = level(level_id)) {
        const Level& level = *level_opt;
        for (auto& pos : chunks) {
            str buffer = _authority ? "null" : "unknown";
            auto chunk_opt = level.chunk_at(pos);
            if (chunk_opt) {
                const Chunk& chunk = *chunk_opt;
                if (auto result = glz::write_json(chunk)) buffer = *result;
                else print<error, World>("Failed to serialise chunk at {}[{}].", level_id, fmt::join(chunk.pos(), ", "));
            }
            packet.packet << buffer;
        }
    }
    else {
        print<warning, World>("Attempted to package chunks [{}] for non-existent level {}.", fmt::join(chunks, ", "), level_id);
        for (auto& pos : chunks) packet.packet << "null";
    }
    return packet;
}

dyn_arr<LogicalPacket> World::get_outstanding_messages() {
    dyn_arr<LogicalPacket> messages;
    for (auto& [level_id, level] : _levels) {
        dyn_arr<ivec2> chunks;
        for (auto& chunk : level.chunks()) {
            if (chunk._updated_since_sync) {
                chunks.push_back(chunk.pos());
                chunk._updated_since_sync = false;
            }
        }
        if (chunks.size() > 0) messages.emplace_back(package_chunks(level_id, move(chunks)));
    }
    return messages;
}

result<LogicalPacket, str> World::get_requested_message(const packet_id& id) const {
    if (id.type() == "chunk") {
        if (id.args().size() == 0) return err("No level specified.");
        else if (id.args().size() <= 1) return err("No chunk specified.");

        ::id level_id = id.args()[0];
        dyn_arr<ivec2> chunks; chunks.reserve(id.args().size() - 1);
        for (auto& arg : id.args() | views::drop(1)) {
            ivec2 chunk_pos; auto numbers = arg | views::split(str(",")) | ranges::to<dyn_arr<str>>();
            try { chunk_pos = ivec2(str_to<int>(numbers[0]), str_to<int>(numbers[1])); }
            catch(const std::exception& e) { return err(fmt::format("Arg {} was not an ivec2 ({}).", arg, e.what())); }
            chunks.push_back(chunk_pos);
        }
        return package_chunks(level_id, move(chunks));
    }
    return err("");
}

result<success_t, str> World::read_message(LogicalPacket&& packet) {
    if (packet.id.type() == "chunk") {
        if (packet.id.args().size() == 0) return err("No level specified.");
        else if (packet.id.args().size() <= 1) return err("No chunk specified.");
        
        dyn_arr<ivec2> chunks; chunks.reserve(packet.id.args().size() - 1);
        for (auto& arg : packet.id.args() | views::drop(1)) {
            ivec2 chunk_pos; auto numbers = arg | views::split(str(",")) | ranges::to<dyn_arr<str>>();
            try { chunk_pos = ivec2(str_to<int>(numbers[0]), str_to<int>(numbers[1])); }
            catch(const std::exception& e) { return err(fmt::format("Arg {} was not an ivec2 ({}).", arg, e.what())); }
            chunks.push_back(chunk_pos);
        }
        
        id level_id = packet.id.args()[0];
        auto level_opt = level(level_id);
        if (!level_opt && !_authority) { make_level(level_id); level_opt = level(level_id); }
        else if (!level_opt && _authority) return err(fmt::format("Chunks recieved for invalid level '{}'.", level_id));

        Level& level = *level_opt;
        for (auto& pos : chunks) {
            str buffer; packet.packet >> buffer;
            if (buffer == "unknown") continue;

            auto result = glz::read_json<Chunk>(buffer);
            if (!result)
                return err(fmt::format("Chunk {}[{}] failed to parse ({},{}).",
                    level_id, fmt::join(pos, ", "),
                    std::to_underlying(result.error().ec), result.error().custom_error_message)
                );
            
            print<network_info, World>("Recieved chunk at {}[{}] on tick {}.", level_id, fmt::join(pos, ", "), packet.time);
            auto chunk_opt = level.set_chunk(pos, move(*result));
            // If acting as authority, rebroadcast to clients
            if (is_authority() && chunk_opt) {
                Chunk& chunk = *chunk_opt;
                chunk._updated_since_sync = true;
            }
        }
        return empty_success;
    }
    return err("");
}