#include "networked.h"

#include <network/network_manager.h>
#include <console.h>

INetworked::INetworked(str network_id) : _network_id(network_id) { NetworkManager::Registry::__register(*this); }

INetworked::INetworked(INetworked&& other) : _network_id(other._network_id) {
    NetworkManager::Registry::__unregister(other);
    other._invalidated = true;
    NetworkManager::Registry::__register(*this);
}

INetworked::~INetworked() {
    if (!_invalidated) NetworkManager::Registry::__unregister(*this);
}

str INetworked::network_id() const { return _network_id; }

void INetworked::set_network_id(const str& id) {
    NetworkManager::Registry::__unregister(*this);
    _network_id = id;
    NetworkManager::Registry::__register(*this);
}

dyn_arr<LogicalPacket> INetworked::get_outstanding_messages() const { return {}; }
result<LogicalPacket, str> INetworked::get_requested_message(const PacketId& id) const { return err(""); }

dyn_arr<LogicalPacket> INetworked::outstanding() const { return get_outstanding_messages(); }
opt<LogicalPacket> INetworked::request(const PacketId& id) const {
    auto result = get_requested_message(id);
    if (result) return move(result.value());

    if (result.error().empty()) print<warning, INetworked>("[{}] Packet request for '{}' was not handled.", _network_id, id.as_str());
    else print<error, INetworked>("[{}] Packet request for '{}' failed: {}", _network_id, id.as_str(), result.error());
    return nullopt;
}

void INetworked::read(LogicalPacket&& packet) {
    PacketId id = packet.id;
    auto result = read_message(move(packet));
    if (result) return;
    if (result.error().empty()) print<warning, INetworked>("[{}] Packet read for '{}' was not handled.", _network_id, id.as_str());
    else print<error, INetworked>("[{}] Packet read for '{}' failed: {}", _network_id, id.as_str(), result.error());
}