#include "networked.h"

#include <network/network_manager.h>

INetworked::INetworked() { NetworkManager::Registry::__register(*this); }
INetworked::~INetworked() { NetworkManager::Registry::__unregister(*this); }

LogicalPacket::LogicalPacket(str packet_id, PacketPriority priority, sf::Packet&& packet)
: packet_id(packet_id), priority(priority), packet(move(packet)) {}

opt<LogicalPacket> INetworked::resend_message(str, uint64) const { return nullopt; }