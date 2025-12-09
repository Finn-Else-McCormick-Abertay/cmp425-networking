#include "network_manager.h"
#include <prelude/containers.h>
#include <console.h>
#include <util/format/SFML/network.h>
#include <prelude/format.h>
#include <ranges>

DEFINE_SINGLETON(NetworkManager);

void NetworkManager::Registry::__register(INetworked& networked) {
    auto network_id = networked.network_id();
    if (inst()._networked_by_id.contains(network_id)) print<error, NetworkManager::Registry>("Overwrote network id '{}'.");
    inst()._networked.insert(&networked);
    inst()._networked_by_id[network_id] = &networked;
    print<debug, NetworkManager::Registry>("Registered {}", network_id);
}

void NetworkManager::Registry::__unregister(INetworked& networked) {
    inst()._networked.erase(&networked);
    inst()._networked_by_id.erase(networked.network_id());
    print<debug, NetworkManager::Registry>("Unregistered {}", networked.network_id());
}

NetworkManager::~NetworkManager() {
    disconnect_all();
}

bool NetworkManager::connect_listener() {
    inst()._listener = make_opt<sf::TcpListener>();
    inst()._listener.value().setBlocking(false);
    if (inst()._listener.value().listen(PORT) != sf::Socket::Status::Done) {
        print<error, NetworkManager>("Failed to begin listening on port {}.", PORT);
        return false;
    }
    print<success, NetworkManager>("Listening on port {}.", PORT);
    return true;
}
bool NetworkManager::disconnect_listener() {
    if (inst()._listener) {
        print<success, NetworkManager>("Stopped listening on port {}.", inst()._listener.value().getLocalPort());
        inst()._listener.value().close();
        inst()._listener = nullopt;
        return true;
    }
    return false;
}

bool NetworkManager::connect(const sf::IpAddress& address, sf::Time timeout) {
    sf::TcpSocket socket;
    auto status = socket.connect(address, PORT, timeout);
    if (status != sf::Socket::Status::Done) {
        print<error, NetworkManager>("Failed to connect to {} : {}.", address, status);
        return false;
    }
    socket.setBlocking(false);
    inst()._sockets[address] = move(socket);
    print<success, NetworkManager>("Connected to {}.", address);
    return true;
}

bool NetworkManager::disconnect(const sf::IpAddress& address) {
    if (!inst()._sockets.contains(address)) {
        print<error, NetworkManager>("Could not disconnect from {} : address is not connected.", address);
        return false;
    }
    auto& socket = inst()._sockets.at(address);
    socket.disconnect();
    print<success, NetworkManager>("Disconnected from {}.", address);
    inst()._sockets.erase(address);
    return true;
}

void NetworkManager::disconnect_all() {
    disconnect_listener();
    for (auto it = inst()._sockets.begin(), it_next = it; it != inst()._sockets.end(); it = it_next) {
        ++it_next; disconnect(it->first);
    }
}

void NetworkManager::network_tick(uint64 elapsed_ticks) {
    // Seek new connections
    if (inst()._listener) {
        sf::TcpSocket new_socket; new_socket.setBlocking(false);
        auto accept_status = inst()._listener.value().accept(new_socket);
        if (accept_status == sf::Socket::Status::Done) {
            auto address = new_socket.getRemoteAddress().value();
            inst()._sockets[address] = move(new_socket);
            print<info, NetworkManager>("Connected to {}.", address);
        }
        else if (accept_status == sf::Socket::Status::Error) print<error, NetworkManager>("Failed to accept new connection.");
    }

    // Handle incoming messages
    for (auto it = inst()._sockets.begin(), it_next = it; it != inst()._sockets.end(); it = it_next) {
        ++it_next;
        auto& [address, socket] = *it;

        sf::Packet received_packet;
        auto status = socket.receive(received_packet);
        if (status == sf::Socket::Status::Done) {
            str owner_id, packet_id;
            received_packet >> owner_id; received_packet >> packet_id;
            if (inst()._networked_by_id.contains(owner_id)) {
                auto networked = inst()._networked_by_id.at(owner_id);
                networked->read_message({ packet_id, move(received_packet) });
            }
            else print<error, NetworkManager>("Recieved packet directed to non-existent network id '{}'.", owner_id);
        }
        else if (status == sf::Socket::Status::Disconnected) {
            print<info, NetworkManager>("{} has disconnected.", address);
            inst()._sockets.erase(address);
        }
        else if (status != sf::Socket::Status::NotReady) print<error, NetworkManager>("Failed to recieve packet from {} : {}", address, status);
    }

    // Handle outgoing messages
    for (auto networked : inst()._networked) {
        auto owner_id = networked->network_id();
        auto messages = networked->write_messages();
        for (auto& message : messages) {
            sf::Packet wrapped_packet;
            wrapped_packet << owner_id; wrapped_packet << message.packet_id;
            wrapped_packet.append(message.packet.getData(), message.packet.getDataSize());
            inst().send_packet(wrapped_packet);
        }
    }
}

bool NetworkManager::send_packet(sf::Packet& packet, sf::TcpSocket& socket) {
    sf::Socket::Status status;
    do status = socket.send(packet); while (status == sf::Socket::Status::Partial);
    return status == sf::Socket::Status::Done;
}

bool NetworkManager::send_packet(sf::Packet& packet, const sf::IpAddress& address) {
    if (!_sockets.contains(address)) return false;
    return send_packet(packet, _sockets.at(address));
}

bool NetworkManager::send_packet(sf::Packet& packet) {
    bool failed = false;
    for (auto& [address, socket] : _sockets) failed |= !send_packet(packet, socket);
    return !failed;
}

str NetworkManager::debug_message() {
    auto keys_view = std::views::keys(inst()._networked_by_id);
    return fmt::format("Networked Object Ids: {}", fmt::join(keys_view, ", "));
}