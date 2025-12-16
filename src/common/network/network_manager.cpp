#include "network_manager.h"
#include <prelude/containers.h>
#include <console.h>
#include <util/format/SFML/network.h>
#include <prelude/format.h>
#include <alias/ranges.h>

DEFINE_SINGLETON(NetworkManager);

NetworkManager::~NetworkManager() {
    disconnect_all();
}

void NetworkManager::Registry::__register(INetworked& networked) {
    auto network_id = networked.network_id();
    if (inst()._networked_by_id.contains(network_id)) print<error, NetworkManager::Registry>("Overwrote network id '{}'.");
    inst()._networked.insert(&networked);
    inst()._networked_by_id[network_id] = &networked;
    //print<debug, NetworkManager::Registry>("Registered {}", network_id);
}

void NetworkManager::Registry::__unregister(INetworked& networked) {
    inst()._networked.erase(&networked);
    inst()._networked_by_id.erase(networked.network_id());
    //print<debug, NetworkManager::Registry>("Unregistered {}", networked.network_id());
}

const str& NetworkManager::username() { return inst()._username; }
void NetworkManager::set_username(const str& username) {
    inst()._username = username;
    print<info, NetworkManager>("Set username to {}.", username);
    // If connected to server
    //if (inst()._server_address && inst()._sockets.contains(inst()._server_address.value()))
    //    print<warning, NetworkManager>("!UNHANDLED! Username changed while server connected.");
}

opt_cref<sf::IpAddress> NetworkManager::server_address() {
    if (inst()._server_address) return cref(*inst()._server_address);
    return nullopt;
}

void NetworkManager::connect_to_server(const sf::IpAddress& address, sf::Time timeout) {
    inst()._server_address = address;
    if (!connect(*inst()._server_address, SERVER_PORT, timeout))
        print<network_info>("Connecting to server {}:{}.", address, SERVER_PORT);
}

bool NetworkManager::connect_listener() {
    inst()._client_listener = make_opt<sf::TcpListener>();
    inst()._client_listener.value().setBlocking(false);
    if (inst()._client_listener.value().listen(SERVER_PORT) != sf::Socket::Status::Done) {
        print<error, NetworkManager>("Failed to begin listening on port {}.", SERVER_PORT);
        return false;
    }
    print<network_info, NetworkManager>("Listening on port {}.", SERVER_PORT);
    return true;
}
bool NetworkManager::disconnect_listener() {
    if (inst()._client_listener) {
        print<network_info, NetworkManager>("Stopped listening on port {}.", inst()._client_listener->getLocalPort());
        inst()._client_listener.value().close();
        inst()._client_listener = nullopt;
        return true;
    }
    return false;
}

bool NetworkManager::connect(const sf::IpAddress& address, uint32 port, sf::Time timeout) {
    auto socket = sf::TcpSocket();
    //if (timeout == 0s) socket.setBlocking(false);
    auto status = socket.connect(address, port, timeout);
    if (status != sf::Socket::Status::Done) return false;
    return register_socket(move(socket));
}
bool NetworkManager::register_socket(sf::TcpSocket&& socket) {
    auto address = socket.getRemoteAddress();
    auto port = socket.getRemotePort();

    if (!address || port == 0) {
        print<warning, NetworkManager>("Attempted to register unconnected socket ({}:{}).", address, port);
        return false;
    }
    socket.setBlocking(false);
    inst()._sockets[make_pair(*address, port)] = move(socket);
    print<network_info, NetworkManager>("Connected to {}:{}.", address, port);

    if (port == SERVER_PORT && inst()._server_address && *inst()._server_address == *address) inst()._server_socket = &inst()._sockets[make_pair(*address, port)];
    return true;
}

bool NetworkManager::disconnect(const sf::IpAddress& address, uint32 port) {
    if (!inst()._sockets.contains(make_pair(address, port))) {
        print<error, NetworkManager>("Could not disconnect from non-existent socket {}:{}.", address, port);
        return false;
    }
    deregister_socket(address, port, false);
    return true;
}
void NetworkManager::deregister_socket(sf::IpAddress address, uint32 port, bool connection_lost) {
    auto key = make_pair(address, port);
    if (inst()._sockets.contains(key)) { inst()._sockets.at(key).disconnect(); inst()._sockets.erase(key); }
    if (port == SERVER_PORT && inst()._server_address && *inst()._server_address == address) inst()._server_socket = nullptr;

    if (!connection_lost) print<success, NetworkManager>("Disconnected from {}:{}.", address, port);
    else print<network_info, NetworkManager>("Lost connection with {}:{}.", address, port);
}

void NetworkManager::disconnect_all() {
    disconnect_listener();
    for (auto it = inst()._sockets.begin(), it_next = it; it != inst()._sockets.end(); it = it_next) {
        ++it_next; disconnect(it->first.first, it->first.second);
    }
}

void NetworkManager::seek_server_connection() {
    if (_server_address && !_server_socket) connect(*_server_address, SERVER_PORT);
}

void NetworkManager::seek_client_connection() {
    if (!_client_listener) return;
    sf::TcpSocket new_socket;
    auto status = _client_listener.value().accept(new_socket);
    if (status == sf::Socket::Status::Done) register_socket(move(new_socket));
    else if (status == sf::Socket::Status::Error) print<error, NetworkManager>("Failed to accept new connection.");
}

void NetworkManager::handle_incoming(const sf::IpAddress& address, uint32 port, sf::TcpSocket& socket) {
    sf::Packet packet; auto status = socket.receive(packet);
    if (status == sf::Socket::Status::Done) {
        auto [owner, logical_packet, type] = unwrap(move(packet));
        if (type == MessageType::Default) {
            if (_networked_by_id.contains(owner)) _networked_by_id.at(owner)->read(move(logical_packet));
            else print<error, NetworkManager>("Recieved packet directed to non-existent network id '{}'.", owner);
        }
        else if (type == MessageType::Request) {
            _recieved_requests.emplace_back(owner, logical_packet.id, logical_packet.time, make_pair(address, port));
        }
        else {
            print<warning, NetworkManager>("Unhandled lifecycle request.");
        }
    }
    else if (status == sf::Socket::Status::Disconnected) deregister_socket(address, port, true);
    else if (status != sf::Socket::Status::NotReady) print<error, NetworkManager>("Failed to recieve packet from {} : {}", address, status);
}

void NetworkManager::handle_outgoing(INetworked& networked) {
    for (auto& message : networked.outstanding()) {
        message.time = _current_tick;
        send_packet(wrap(networked.network_id(), move(message), MessageType::Default));
    }
}

void NetworkManager::request(const network_id& owner, const packet_id& packet_id, const opt<pair<sf::IpAddress, uint32>>& target) {
    inst()._outgoing_requests.emplace_back(owner, packet_id, inst()._current_tick, target);
}

void NetworkManager::broadcast(const network_id& owner, const packet_id& packet_id, const opt<pair<sf::IpAddress, uint32>>& target) {
    inst()._recieved_requests.emplace_back(owner, packet_id, inst()._current_tick, target);
}

void NetworkManager::network_tick(uint64 elapsed_ticks) {
    uint64 tick_delta = elapsed_ticks - inst()._current_tick;
    inst()._current_tick = elapsed_ticks;

    // -- Seek connections --
    inst().seek_server_connection();
    inst().seek_client_connection();

    // -- Handle incoming messages --
    for (auto it = inst()._sockets.begin(), it_next = it; it != inst()._sockets.end(); it = it_next) {
        ++it_next; inst().handle_incoming(it->first.first, it->first.second, it->second);
    }

    // -- Handle outgoing messages --
    
    // Outgoing message requests
    for (auto it = inst()._outgoing_requests.begin(), it_next = it; it != inst()._outgoing_requests.end(); it = it_next) {
        ++it_next; auto [netid, packid, time, target] = *it;
        if (inst().send_packet(wrap(netid, LogicalPacket(packid, inst()._current_tick), MessageType::Request), target))
            it_next = inst()._outgoing_requests.erase(it);
    }

    // Outgoing answers
    for (auto it = inst()._recieved_requests.begin(), it_next = it; it != inst()._recieved_requests.end(); it = it_next) {
        ++it_next; auto [netid, packid, time, address] = *it;

        opt<LogicalPacket> answer = nullopt;
        if (inst()._networked_by_id.contains(netid)) answer = inst()._networked_by_id.at(netid)->request(packid);

        LogicalPacket pack = answer.or_else([&packid](){
            auto packet = LogicalPacket(packid);
            packet.packet << "unhandled";
            return make_opt(move(packet));
        }).value();
        answer->time = inst()._current_tick;

        if (inst().send_packet(wrap(netid, move(pack), MessageType::Default), address))
            it_next = inst()._recieved_requests.erase(it);
    }

    // Ticked messages
    for (auto networked : inst()._networked) inst().handle_outgoing(*networked);
}

sf::Packet NetworkManager::wrap(const network_id& owner, LogicalPacket&& message, MessageType type) {
    sf::Packet wrapped;
    wrapped << message.time;
    wrapped << (int)type;
    wrapped << owner.to_str();
    wrapped << message.id.as_str();
    wrapped.append(message.packet.getData(), message.packet.getDataSize());
    return wrapped;
}

tuple<network_id, LogicalPacket, NetworkManager::MessageType> NetworkManager::unwrap(sf::Packet&& packet) {
    uint64 time; packet >> time;
    int type; packet >> type;
    str owner_str, msg_id; packet >> owner_str; packet >> msg_id;
    return make_tuple(network_id::from_str(owner_str), LogicalPacket(msg_id, time, move(packet)), (MessageType)type);
}

bool NetworkManager::send_packet(sf::Packet& packet, sf::TcpSocket& socket) {
    sf::Socket::Status status;
    do status = socket.send(packet); while (status == sf::Socket::Status::Partial);
    return status == sf::Socket::Status::Done;
}

bool NetworkManager::send_packet(sf::Packet&& packet, const opt<pair<sf::IpAddress, uint32>>& target) {
    if (!target) return send_packet(move(packet));
    if (!_sockets.contains(*target)) return false;
    return send_packet(packet, _sockets.at(*target));
}

bool NetworkManager::send_packet(sf::Packet&& packet) {
    bool failed = false;
    for (auto& [address, socket] : _sockets) failed |= !send_packet(packet, socket);
    return !failed;
}

str NetworkManager::debug_message() {
    return fmt::format("{}", fmt::join(dyn_arr<str>{
        fmt::format("Username: {}", inst()._username),
        fmt::format("Server: {}", inst()._server_address
            .transform([](const sf::IpAddress& x) { return fmt::format("{}", x); }).value_or("null")),
        fmt::format("Networked Object Ids: {}", fmt::join(views::keys(inst()._networked_by_id), ", "))
    }, "\n"));
}