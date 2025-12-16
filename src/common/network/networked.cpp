#include "networked.h"

#include <network/network_manager.h>
#include <console.h>

INetworked::INetworked(construct_noinit_t _) : _network_id(nullid, ""), _invalidated(true) {}
INetworked::INetworked(const ::network_id& network_id) : _network_id(network_id) { NetworkManager::Registry::__register(*this); }
INetworked::~INetworked() { if (!_invalidated) NetworkManager::Registry::__unregister(*this); }

INetworked::INetworked(INetworked&& other) : _network_id(other._network_id) {
    NetworkManager::Registry::__unregister(other);
    other._invalidated = true;
    NetworkManager::Registry::__register(*this);
}

const network_id& INetworked::network_id() const { return _network_id; }

void INetworked::set_network_id(const ::network_id& id) {
    if (!_invalidated) NetworkManager::Registry::__unregister(*this);
    _network_id = id; _invalidated = false;
    NetworkManager::Registry::__register(*this);
}

dyn_arr<LogicalPacket> INetworked::get_outstanding_messages() { return {}; }
result<LogicalPacket, str> INetworked::get_requested_message(const packet_id& id) const { return err(""); }
result<success_t, str> INetworked::read_message(LogicalPacket&& packet) { return err(""); }

dyn_arr<LogicalPacket> INetworked::outstanding() { return get_outstanding_messages(); }
opt<LogicalPacket> INetworked::request(const packet_id& id) const {
    auto result = get_requested_message(id);
    if (result) return move(result.value());

    if (result.error().empty()) print<warning, INetworked>("[{}] Packet request for '{}' was not handled.", _network_id, id.as_str());
    else print<error, INetworked>("[{}] Packet request for '{}' failed: {}", _network_id, id.as_str(), result.error());
    return nullopt;
}

void INetworked::read(LogicalPacket&& packet) {
    packet_id id = packet.id;
    auto result = read_message(move(packet));
    if (result) return;
    if (result.error().empty()) print<warning, INetworked>("[{}] Packet read for '{}' at {} was not handled.", _network_id, id.as_str(), packet.time);
    else print<error, INetworked>("[{}] Packet read for '{}' at {} failed: {}", _network_id, id.as_str(), packet.time, result.error());
}