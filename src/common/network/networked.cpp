#include "networked.h"

#include <network/network_manager.h>

INetworked::INetworked(str network_id) : _network_id(network_id) { NetworkManager::Registry::__register(*this); }

INetworked::INetworked(INetworked&& other) : _network_id(other._network_id) {
    NetworkManager::Registry::__unregister(other);
    other._invalidated = true;
    NetworkManager::Registry::__register(*this);
}

INetworked::~INetworked() {
    if (!_invalidated) NetworkManager::Registry::__unregister(*this);
}

LogicalPacket::LogicalPacket(str packet_id, sf::Packet&& packet) : packet_id(packet_id), packet(move(packet)) {}

str INetworked::network_id() const { return _network_id; }

void INetworked::set_network_id(const str& id) {
    NetworkManager::Registry::__unregister(*this);
    _network_id = id;
    NetworkManager::Registry::__register(*this);
}