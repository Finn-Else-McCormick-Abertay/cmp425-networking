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

void NetworkManager::init() {
    #ifdef CLIENT
    if (!server_address()) set_server_address(IpAddress::LocalHost);
    print<network_info>("Connecting to server {}...", *server_address());
    inst().connect(*server_address());
    #elifdef SERVER
    inst().connect_listener(SERVER_PORT);
    #endif
}

const str& NetworkManager::username() { return inst()._username; }
void NetworkManager::set_username(const str& username) {
    inst()._username = username;
    print<info, NetworkManager>("Set username to {}.", username);
    // If connected to server
    if (inst()._server_address && inst()._sockets.contains(*inst()._server_address))
        print<warning, NetworkManager>("!UNHANDLED! Username changed while server connected.");
}

opt_cref<str> NetworkManager::user_uid() {
    if (inst()._user_uid) return *inst()._user_uid;
    return nullopt;
}

opt_cref<SocketAddress> NetworkManager::server_address() {
    if (inst()._server_address) return *inst()._server_address;
    return nullopt;
}

void NetworkManager::set_server_address(const SocketAddress& address) {
    inst()._server_address = address;
    if (inst()._server_address->port == Socket::AnyPort) inst()._server_address->port = SERVER_PORT;
}

void NetworkManager::connect_listener(Port port) {
    disconnect_listener();
    _client_listener = make_opt<TcpListener>();
    _client_listener->setBlocking(false);
    if (_client_listener->listen(port) != Socket::Status::Done)
        return print<error, NetworkManager>("Failed to begin listening on port {}.", port);
    print<network_info>("Listening on port {}.", port);
}
void NetworkManager::disconnect_listener() {
    if (_client_listener) {
        print<network_info>("Stopped listening on port {}.", _client_listener->getLocalPort());
        _client_listener->close();
        _client_listener = nullopt;
    }
}

void NetworkManager::connect(const SocketAddress& address, sf::Time timeout, bool blocking) {
    auto socket = TcpSocket(); socket.setBlocking(blocking);
    auto status = socket.connect(address.ip, address.port, timeout);
    if (status == Socket::Status::Done) register_socket(move(socket));
    //else print<error, NetworkManager>("Socket {} could not connect : {}", address, status);
}
void NetworkManager::register_socket(sf::TcpSocket&& socket) {
    auto ip_address = socket.getRemoteAddress(); auto port = socket.getRemotePort();
    if (!ip_address || port == 0) return print<warning, NetworkManager>("Attempted to register unconnected socket ({}, {}).", ip_address, port);
    SocketAddress address { *ip_address, port };
    socket.setBlocking(false); inst()._sockets[address] = move(socket);
    print<network_info>("Connected to {}.", address);
}

void NetworkManager::disconnect(const SocketAddress& address) {
    if (!_sockets.contains(address)) return print<error, NetworkManager>("Could not disconnect from non-existent socket {}.", address);
    _sockets[address].disconnect(); inst()._sockets.erase(address);
    print<success, NetworkManager>("Disconnected from {}.", address);
}

void NetworkManager::disconnect_all() {
    disconnect_listener();
    for (auto it = _sockets.begin(), it_next = it; it != _sockets.end(); it = it_next) {
        ++it_next;
        disconnect(it->first);
    }
}

void NetworkManager::seek_server_connection() {
    if (!_server_address) return;
    if (!_sockets.contains(*_server_address)) connect(*_server_address, 100ms, true);

    if (_sockets.contains(*_server_address) && !_user_uid && !_awaiting_user_uid) {
        auto wrapped = wrap(network_id(nullid, "lifecycle"), LogicalPacket(packet_id("uid", { _username })), MessageType::Lifecycle);
        _awaiting_user_uid = send_packet(wrapped, _sockets.at(*_server_address));
    }
}

void NetworkManager::seek_client_connection() {
    if (!_client_listener) return;
    TcpSocket new_socket;
    auto status = _client_listener.value().accept(new_socket);
    if (status == Socket::Status::Done) register_socket(move(new_socket));
    else if (status == Socket::Status::Error) print<error, NetworkManager>("Failed to accept new connection.");
}

void NetworkManager::handle_incoming(const SocketAddress& address, TcpSocket& socket) {
    sf::Packet packet; auto status = socket.receive(packet);
    if (status == Socket::Status::Done) {
        auto [owner, logical_packet, type] = unwrap(move(packet));
        if (type == MessageType::Default) {
            if (_networked_by_id.contains(owner)) _networked_by_id.at(owner)->read(move(logical_packet));
            else print<error, NetworkManager>("Recieved packet directed to non-existent network id '{}'.", owner);
        }
        else if (type == MessageType::Request) {
            _outgoing_broadcasts.emplace_back(owner, logical_packet.id, logical_packet.time, address);
        }
        else if (type == MessageType::Lifecycle) {


            print<warning, NetworkManager>("Unhandled lifecycle request.");
        }
        else print<warning, NetworkManager>("Unhandled message type {}.", std::to_underlying(type));
    }
    else if (status == Socket::Status::Disconnected) {
        print<network_info>("Lost connection with {}.", address);
        inst()._sockets.erase(address);
    }
    else if (status != Socket::Status::NotReady)
        print<error, NetworkManager>("Failed to recieve packet from {} : {}", address, status);
}

void NetworkManager::handle_outgoing(INetworked& networked) {
    for (auto& message : networked.outstanding()) {
        message.time = _current_tick;
        send_packet(wrap(networked.network_id(), move(message), MessageType::Default));
    }
}

void NetworkManager::request(const network_id& owner, const packet_id& packet_id, const opt<SocketAddress>& target) {
    inst()._outgoing_requests.emplace_back(owner, packet_id, inst()._current_tick, target);
}

void NetworkManager::broadcast(const network_id& owner, const packet_id& packet_id, const opt<SocketAddress>& target) {
    inst()._outgoing_broadcasts.emplace_back(owner, packet_id, inst()._current_tick, target);
}

void NetworkManager::network_tick(uint64 elapsed_ticks) {
    uint64 tick_delta = elapsed_ticks - inst()._current_tick;
    inst()._current_tick = elapsed_ticks;

    // -- Seek connections --
    #ifdef CLIENT
    inst().seek_server_connection();
    #elifdef SERVER
    inst().seek_client_connection();
    #endif

    // -- Handle incoming messages --
    for (auto it = inst()._sockets.begin(), it_next = it; it != inst()._sockets.end(); it = it_next) {
        ++it_next; inst().handle_incoming(it->first, it->second);
    }

    // -- Handle outgoing messages --
    
    // Outgoing requests
    for (auto it = inst()._outgoing_requests.begin(), it_next = it; it != inst()._outgoing_requests.end(); it = it_next) {
        ++it_next; auto [netid, packid, time, target] = *it;
        if (inst().send_packet(wrap(netid, LogicalPacket(packid, inst()._current_tick), MessageType::Request), target))
            it_next = inst()._outgoing_requests.erase(it);
    }

    // Outgoing broadcasts / request answers
    for (auto it = inst()._outgoing_broadcasts.begin(), it_next = it; it != inst()._outgoing_broadcasts.end(); it = it_next) {
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
            it_next = inst()._outgoing_broadcasts.erase(it);
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

bool NetworkManager::send_packet(sf::Packet& packet, TcpSocket& socket) {
    Socket::Status status;
    do status = socket.send(packet); while (status == Socket::Status::Partial);
    return status == Socket::Status::Done;
}

bool NetworkManager::send_packet(sf::Packet&& packet, const opt<SocketAddress>& target) {
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
        fmt::format("Server: {}", inst()._server_address),
        fmt::format("Networked Object Ids: {}", fmt::join(views::keys(inst()._networked_by_id), ", "))
    }, "\n"));
}