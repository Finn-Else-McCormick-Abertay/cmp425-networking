#include "network_manager.h"
#include <prelude/containers.h>
#include <console.h>

DEFINE_SINGLETON(NetworkManager);

void NetworkManager::Registry::__register(INetworked& networked) { inst()._networked.insert(&networked); }
void NetworkManager::Registry::__unregister(INetworked& networked) { inst()._networked.erase(&networked); }

bool NetworkManager::connect(uint16 port) {
    inst()._socket.setBlocking(false);
    if (inst()._socket.bind(port) != sf::Socket::Status::Done) {
        print<error, NetworkManager>("Failed to connect on port {}.", port);
        return false;
    }
    print<success, NetworkManager>("Connected on port {}.", port);
    return true;
}
void NetworkManager::disconnect() {
    if (!is_authority()) {
        sf::Packet packet; packet << "notify_disconnect";
        inst().send_packet(packet);
    }

    print<success, NetworkManager>("Disconnected from port {}.", inst()._socket.getLocalPort());
    inst()._socket.unbind();
}

bool NetworkManager::is_authority() {
#ifdef CLIENT
    return false;
#endif
#ifdef SERVER
    return true;
#endif
}

void NetworkManager::set_server_address(const sf::IpAddress& address) {
    if (inst()._server_address) inst()._send_addresses.erase(inst()._server_address.value());
    inst()._server_address = make_opt(address);
    inst()._send_addresses.insert(address);
    inst()._has_server_acknowledged = false;
}

void NetworkManager::network_tick(uint64 elapsed_ticks) {
    // Handle incoming messages
    sf::Packet recieve_packet;
    opt<sf::IpAddress> sender; uint16 port;
    auto recieve_status = inst()._socket.receive(recieve_packet, sender, port);
    if (recieve_status == sf::Socket::Status::Done) {
        str message_type; recieve_packet >> message_type;
        if (is_authority() && port == CLIENT_PORT) {
            if (message_type == "request_connect" && sender) {
                inst()._addresses_to_acknowledge.insert(sender.value());
                inst()._send_addresses.insert(sender.value());
                print<info, NetworkManager>("Connection requested from {}.", sender.value().toString());
            }
            else if (message_type == "notify_disconnect" && sender) {
                inst()._send_addresses.erase(sender.value());
                print<info, NetworkManager>("Disconnected from {}.", sender.value().toString());
            }
        }
        else if (!is_authority() && port == SERVER_PORT && sender == inst()._server_address) {
            if (message_type == "acknowledge_connect") {
                inst()._has_server_acknowledged = true;
                print<info, NetworkManager>("Connection acknowledged.");
            }
        }
    }
    else if (recieve_status != sf::Socket::Status::NotReady) print<error, NetworkManager>("Network error : {}.", std::to_underlying(recieve_status));


    if (!is_authority() && !inst()._has_server_acknowledged) {
        sf::Packet packet; packet << "request_connect";
        inst().send_packet(packet);
    }
    if (is_authority()) {
        set<sf::IpAddress> msg_success;
        for (auto& address : inst()._addresses_to_acknowledge) {
            sf::Packet packet; packet << "acknowledge_connect";
            if (inst().send_packet(packet, address)) msg_success.insert(address);
        }
        for (auto& address : msg_success) inst()._addresses_to_acknowledge.erase(address);
    }

    // Handle outgoing messages
    dyn_arr<pair<str, LogicalPacket>> outgoing_packets;
    //outgoing_packets.append_range(inst()._remaining_packets);
    //inst()._remaining_packets.clear();

    for (auto networked : inst()._networked) {
        if (auto opt = networked->write_message()) {
            outgoing_packets.emplace_back(make_pair(networked->network_id(), move(opt.value())));
        }
    }

    // If any outgoing packets
    if (outgoing_packets.size() > 0) {
        print<info, NetworkManager>("{} outgoing packets.", outgoing_packets.size());
        sf::Packet combined_packet;
        combined_packet << "combined";
        combined_packet << outgoing_packets.size();

        for (auto& [network_id, packet] : outgoing_packets) {
            combined_packet << network_id;
            combined_packet << packet.packet_id;
            combined_packet.append(packet.packet.getData(), packet.packet.getDataSize());
        }

        if (!inst().send_packet(combined_packet)) {
            //inst()._remaining_packets.append_range(outgoing_packets);
            print<error, NetworkManager>("Send error.");
        }
    }
}

bool NetworkManager::send_packet(sf::Packet& packet) {
    bool failed = false;
    for (auto& address : inst()._send_addresses) failed |= !send_packet(packet, address);
    return !failed;
}

bool NetworkManager::send_packet(sf::Packet& packet, const sf::IpAddress& address) {
    sf::Socket::Status status;
    do status = inst()._socket.send(packet, address, is_authority() ? CLIENT_PORT : SERVER_PORT);
    while (status == sf::Socket::Status::Partial);
    return status == sf::Socket::Status::Done;
}