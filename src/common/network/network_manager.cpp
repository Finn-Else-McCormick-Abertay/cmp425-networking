#include "network_manager.h"
#include <prelude/containers.h>
#include <console.h>
#include <util/format/SFML/network.h>
#include <prelude/format.h>
#include <alias/ranges.h>

#include <actor/actor_manager.h>
#include <system/system_manager.h>

DEFINE_SINGLETON(NetworkManager);

NetworkManager::NetworkManager() {
    #ifdef CLIENT
    _local_client.emplace(Client("guest"));
    #endif
}

NetworkManager::~NetworkManager() {
    disconnect_all();
}

void NetworkManager::Registry::__register(INetworked& networked) {
    auto network_id = networked.netid();
    if (inst()._networked_by_id.contains(network_id))
        print<error, NetworkManager::Registry>("Overwrote network id '{}'.");
    inst()._networked.insert(&networked);
    inst()._networked_by_id[network_id] = &networked;
    //print<debug, NetworkManager::Registry>("Registered {}", network_id);
}

void NetworkManager::Registry::__unregister(INetworked& networked) {
    inst()._networked.erase(&networked);
    inst()._networked_by_id.erase(networked.netid());
    //print<debug, NetworkManager::Registry>("Unregistered {}", networked.netid());
}

void NetworkManager::init() {
    #ifdef CLIENT
    if (!remote_server_address()) set_remote_server_address(IpAddress::LocalHost);
    print<network_info>("Connecting to server {}...", *remote_server_address());
    inst().connect(*remote_server_address());
    #elifdef SERVER
    inst().connect_listener(SERVER_PORT);
    #endif
}

NetworkManager::Client::Client(str username, bool has_player) : _username(username), _uid(nullopt), _has_player(has_player) {}

const str& NetworkManager::Client::username() const { return _username; }
const opt<str>& NetworkManager::Client::uid() const { return _uid; }

void NetworkManager::Client::set_username(const str& username) {
    _username = username;
    if (inst()._remote_server_address && inst()._sockets.contains(*inst()._remote_server_address))
        print<error, NetworkManager>("!UNHANDLED! Username changed while server connected.");
}

bool NetworkManager::Client::has_player() const { return _has_player; }
void NetworkManager::Client::set_has_player(bool val) {
    _has_player = val;
    if (inst()._remote_server_address && inst()._sockets.contains(*inst()._remote_server_address))
        print<error, NetworkManager>("!UNHANDLED! Client player mode changed while server connected.");
}

opt<NetworkManager::Client>& NetworkManager::local_client() { return inst()._local_client; }

opt_cref<SocketAddress> NetworkManager::remote_server_address() {
    if (inst()._remote_server_address) return *inst()._remote_server_address;
    return nullopt;
}

void NetworkManager::set_remote_server_address(const SocketAddress& address) {
    inst()._remote_server_address = address;
    if (inst()._remote_server_address->port == Socket::AnyPort) inst()._remote_server_address->port = SERVER_PORT;
}

void NetworkManager::connect_listener(Port port) {
    disconnect_listener();
    _remote_client_listener = make_opt<TcpListener>();
    _remote_client_listener->setBlocking(false);
    if (_remote_client_listener->listen(port) != Socket::Status::Done)
        return print<error, NetworkManager>("Failed to begin listening on port {}.", port);
    print<success, NetworkManager>("Listening on port {}.", port);
}
void NetworkManager::disconnect_listener() {
    if (_remote_client_listener) {
        print<success, NetworkManager>("Stopped listening on port {}.", _remote_client_listener->getLocalPort());
        _remote_client_listener->close();
        _remote_client_listener = nullopt;
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
    socket.setBlocking(false);
    inst()._sockets[address] = move(socket);
    print<network_info>("Connected to {}.", address);
}

void NetworkManager::disconnect(const SocketAddress& address) {
    if (!_sockets.contains(address)) return print<error, NetworkManager>("Could not disconnect from non-existent socket {}.", address);
    auto address_str = address.to_str();
    _sockets[address].disconnect(); inst()._sockets.erase(address);
    print<success, NetworkManager>("Disconnected from {}.", address_str); // The SocketAddress object is invalidated upon closing
}

void NetworkManager::disconnect_all() {
    disconnect_listener();
    for (auto it = _sockets.begin(), it_next = it; it != _sockets.end(); it = it_next) {
        ++it_next;
        disconnect(it->first);
    }
}

void NetworkManager::seek_server_connection() {
    if (!_remote_server_address) return;
    if (!_sockets.contains(*_remote_server_address)) connect(*_remote_server_address, 100ms, true);

    if (!local_client()) return print<warning, NetworkManager>("seek_server_connection() called while no local client exists");
    if (_sockets.contains(*_remote_server_address) && !local_client()->_uid && !local_client()->_awaiting_uid_validation) {
        sf::Packet packet; packet << local_client()->username() << local_client()->has_player();
        broadcast(LogicalPacket("lifecycle#request"_netid, "uid"_packid, move(packet), nullopt, LogicalPacket::MessageType::Lifecycle), *_remote_server_address);
        local_client()->_awaiting_uid_validation = true;
    }
}

void NetworkManager::seek_client_connection() {
    if (!_remote_client_listener) return;
    TcpSocket new_socket;
    auto status = _remote_client_listener.value().accept(new_socket);
    if (status == Socket::Status::Done) register_socket(move(new_socket));
    else if (status == Socket::Status::Error) print<error, NetworkManager>("Failed to accept new connection.");
}

opt_cref<NetworkManager::Client> NetworkManager::get_remote_client(const SocketAddress& address) {
    if (inst()._socket_clients.contains(address)) return cref(inst()._socket_clients.at(address));
    return nullopt;
}
opt_cref<NetworkManager::Client> NetworkManager::get_client_by_uid(const str& uid) {
    if (local_client() && local_client()->uid() && local_client()->uid().value() == uid) return cref(local_client().value());
    for (auto& [_, client] : inst()._socket_clients) { if (client.uid() && client.uid().value() == uid) return cref(client); }
    return nullopt;
}

NetworkManager::Client& NetworkManager::set_remote_client(const SocketAddress& address, Client&& client) {
    clear_remote_client(address);
    client._uid = client._username;
    if (client._username.empty()) { client._username = "guest"; client._uid = fmt::format("guest[{}]", address); }
    for (auto& [existing_addr, existing_client] : _socket_clients) {
        if (existing_addr != address && existing_client._uid.value() == client._uid.value()) {
            print<warning, NetworkManager>("{} requested username '{}', which is already the uid of {}.", address, client._username, existing_addr);
            client._uid = fmt::format("{}[{}]", client._username, address);
        }
    }

    auto [it, _] = _socket_clients.emplace(address, move(client));
    auto& in_place_client = it->second;
    print<network_info, NetworkManager>("Registered {} with username {} and uid {}.", address, in_place_client.username(), in_place_client.uid().value_or("null"));

    // Create client's corresponding player
    if (in_place_client.has_player()) ActorManager::register_player(in_place_client.uid().value(), in_place_client.username(), true);

    return in_place_client;
}

void NetworkManager::clear_remote_client(const SocketAddress& address) {
    if (!_socket_clients.contains(address)) return;
    #ifdef SERVER
    if (_socket_clients[address].has_player()) ActorManager::unregister_player(_socket_clients[address].uid().value(), true);
    #endif
    _socket_clients.erase(address);
}

result<success_t, str> NetworkManager::handle_lifecycle(const SocketAddress& address, LogicalPacket&& packet) {
    if (packet.owner.type() != "lifecycle"_id && !(packet.owner.inst() == "request" || packet.owner.inst() == "answer"))
        return err(fmt::format("Invalid network id '{}'.", packet.owner));
    
    bool is_request = packet.owner.inst() == "request";
    
    if (packet.id.type() == "uid") {
        if (is_request) {
            auto response_packet = LogicalPacket("lifecycle#answer"_netid, "uid"_packid);
            response_packet.type = LogicalPacket::MessageType::Lifecycle;

            str username; packet.contents >> username;
            bool player_state; packet.contents >> player_state;
            auto& client = set_remote_client(address, Client(username, player_state));

            if (client.uid()) {
                response_packet.id = "uid!success"_packid;
                response_packet.contents << client.uid().value();
            }
            else {
                print<error, NetworkManager>("Failed to set uid for {}.", address);
                response_packet.id = "uid!failure;taken"_packid;
            }

            send(response_packet, address);

            // Update them on the already connected players
            for (auto& [_, client] : inst()._socket_clients) {
                if (client.has_player()) broadcast("singleton#actor_manager"_netid, packet_id("player", { "existing", client.uid().value() }), address);
            }

            return empty_success;
        }
        else {
            // If operation failed
            if (auto success_arg = packet.id.get_arg(0); success_arg
                && *success_arg != "success"
            ) {
                if (auto reason_arg = packet.id.get_arg(1)) return err(fmt::format("{}: {}", *success_arg, *reason_arg));
                return err(*success_arg);
            }
            str validated_uid; packet.contents >> validated_uid;
            local_client()->_uid = validated_uid;
            local_client()->_awaiting_uid_validation = false;
            ActorManager::update_player_authority_states();
            print<network_info, NetworkManager>("Registered with server as {}.", local_client()->uid().value_or("null"));
            return empty_success;
        }
    }
    else if (packet.id.type() == "sync") {
        uint64 sync_fixed_tick = packet.time;
        SystemManager::jump_fixed_tick(sync_fixed_tick);
        return empty_success;
    }
    else return err(fmt::format("Unknown packet type '{}'.", packet.id.type()));

    return err("Unhandled.");
}

void NetworkManager::handle_incoming_packet(const SocketAddress& address, LogicalPacket&& logical_packet) {
    //print<network_info>("RECIEVED {} {} from {}", logical_packet.owner, logical_packet.id, address);
    if (logical_packet.type == LogicalPacket::MessageType::Default) {
        //print<debug, NetworkManager>("INCOMING {} {}", owner, logical_packet.id);
        if (_networked_by_id.contains(logical_packet.owner))
            _networked_by_id.at(logical_packet.owner)->read(move(logical_packet));
        else print<error, NetworkManager>("Recieved packet directed to non-existent network id '{}'.", logical_packet.owner);
    }
    else if (logical_packet.type == LogicalPacket::MessageType::Request) {
        print<debug, NetworkManager>("ANSWER REQUEST {} {}", logical_packet.owner, logical_packet.id);
        broadcast(logical_packet.owner, logical_packet.id, address);
    }
    else if (logical_packet.type == LogicalPacket::MessageType::Lifecycle) {
        auto result = handle_lifecycle(address, move(logical_packet));
        if (!result) print<error, NetworkManager>("Failed lifecycle request ({},{}) for {}. {}", logical_packet.owner, logical_packet.id, address, result.error());
    }
    else print<warning, NetworkManager>("Unhandled message type {}.", std::to_underlying(logical_packet.type));
}

void NetworkManager::handle_incoming(const SocketAddress& address, TcpSocket& socket) {
    //print<debug, NetworkManager>("HANDLE INCOMING from {}", address);

    sf::Packet packet; auto status = socket.receive(packet);
    if (status == Socket::Status::Done) {
        uint16 packet_count; packet >> packet_count;
        for (int i = 0; i < packet_count; ++i) {
            auto logical_packet = LogicalPacket::unpack(packet);
            handle_incoming_packet(address, move(logical_packet));
        }
    }
    else if (status == Socket::Status::Disconnected) {
        print<network_info>("Lost connection with {}.", address);
        clear_remote_client(address);
        disconnect(address);
    }
    else if (status != Socket::Status::NotReady)
        print<error, NetworkManager>("Failed to recieve packet from {} : {}", address, status);
}

void NetworkManager::handle_outgoing(INetworked& networked) {
    auto net_id = networked.netid();
    for (auto& message : networked.outstanding()) {
        //print<network_info>("OUTGOING {} {}", net_id, message.id);
        message.owner = net_id;
        send(move(message), nullopt);
    }
}

void NetworkManager::request(const network_id& owner, const packet_id& packet_id, const opt<SocketAddress>& target) {
    print<debug, NetworkManager>("REQUEST {} {} {}", owner, packet_id, target);
    auto logical_packet = LogicalPacket(owner, packet_id);
    logical_packet.type = LogicalPacket::MessageType::Request;
    inst().send(logical_packet, target);
}

void NetworkManager::broadcast(const LogicalPacket& packet, const opt<SocketAddress>& target) {
    //print<debug, NetworkManager>("BROADCAST {} {} {}", packet.owner, packet.id, target);
    inst().send(packet, target);
}

void NetworkManager::broadcast(LogicalPacket&& packet, const opt<SocketAddress>& target) {
    //print<debug, NetworkManager>("BROADCAST {} {} {}", packet.owner, packet.id, target);
    inst().send(move(packet), target);
}

bool NetworkManager::broadcast(const network_id& owner, const packet_id& packet_id, const opt<SocketAddress>& target) {
    //print<debug, NetworkManager>("BROADCAST {} {} {}", owner, packet_id, target);
    opt<LogicalPacket> answer_opt = nullopt;
    if (inst()._networked_by_id.contains(owner)) answer_opt = inst()._networked_by_id.at(owner)->request(packet_id);

    LogicalPacket packet = answer_opt.or_else([&owner, &packet_id](){
        auto packet = LogicalPacket(owner, packet_id);
        packet.contents << "unhandled";
        return make_opt(move(packet));
    }).value();

    inst().send(packet, target);
    return answer_opt.has_value();
}

void NetworkManager::perform_network_tick() {
    #ifdef SERVER
    if (SystemManager::get_fixed_tick() % 20 == 0) inst().send(LogicalPacket("lifecycle#answer"_netid, "sync"_packid, nullopt, LogicalPacket::MessageType::Lifecycle), nullopt);
    #endif

    // -- Seek connections --
    #ifdef CLIENT
    inst().seek_server_connection();
    #elifdef SERVER
    inst().seek_client_connection();
    #endif

    // -- Handle incoming messages --
    // (These more verbose loops are used when calling methods that can remove items from the container we're looping over)
    for (auto it = inst()._sockets.begin(), it_next = it; it != inst()._sockets.end(); it = it_next) {
        ++it_next; inst().handle_incoming(it->first, it->second);
    }

    // -- Handle ticked messages --
    for (auto networked : inst()._networked) inst().handle_outgoing(*networked);

    // -- Sum up messages sent this tick --

    // Sum together packets aimed at all sockets
    auto all_address = SocketAddress(sf::IpAddress::Any, Socket::AnyPort);
    size_t shared_message_count = 0;
    sf::Packet shared_sum_packet;
    if (inst()._accumulated_packets.contains(all_address)) {
        auto& packet_array = inst()._accumulated_packets.at(all_address);
        shared_message_count = packet_array.size();
        for (auto& logical_packet : packet_array) logical_packet.pack_into(shared_sum_packet);
    }

    // Send all summed packets
    for (auto& [address, socket] : inst()._sockets) {
        uint16 packet_count = shared_message_count;
        dyn_arr<LogicalPacket>* specific_arr = nullptr;
        if (inst()._accumulated_packets.contains(address)) {
            specific_arr = &inst()._accumulated_packets.at(address);
            packet_count += specific_arr->size();
        }
        // Skip if no messages this tick
        if (packet_count == 0) continue;

        sf::Packet sum_packet;
        sum_packet << packet_count;

        // Append shared messages
        if (shared_sum_packet.getData() != nullptr)
            sum_packet.append(shared_sum_packet.getData(), shared_sum_packet.getDataSize());
        
        // Append specific messages
        if (specific_arr != nullptr)
            for (auto& specific_packet : *specific_arr) specific_packet.pack_into(sum_packet);
        
        Socket::Status status;
        do status = socket.send(sum_packet); while (status == Socket::Status::Partial);
        
        if (status != Socket::Status::Done) print<error, NetworkManager>("Failed to send summed packet to {} : {}", address, status);
        //else print<debug, NetworkManager>("Sent summed packet to {}.", address);
    }
    // Clear accumulated packets
    inst()._accumulated_packets.clear();
}

void NetworkManager::send(const LogicalPacket& packet, const opt<SocketAddress>& address_opt) {
    auto address = address_opt.value_or(SocketAddress(sf::IpAddress::Any, Socket::AnyPort));
    //print<debug, NetworkManager>("SEND {} {} to {}", packet.owner, packet.id, address);

    auto [it, is_new] = _accumulated_packets.try_emplace(address);
    it->second.emplace_back(packet);
}

void NetworkManager::send(LogicalPacket&& packet, const opt<SocketAddress>& address_opt) {
    auto address = address_opt.value_or(SocketAddress(sf::IpAddress::Any, Socket::AnyPort));
    //print<debug, NetworkManager>("SEND {} {} to {}", packet.owner, packet.id, address);

    auto [it, is_new] = _accumulated_packets.try_emplace(address);
    it->second.emplace_back(move(packet));
}

dyn_arr<str> NetworkManager::debug_message() {
    return {
        fmt::format("Username: {} | UID: {}", inst().local_client().transform([](auto& x) { return x.username(); }).value_or("null"), inst().local_client().and_then([](auto& x) { return x.uid(); }).value_or("null")),
        fmt::format("Server: {}", inst()._remote_server_address.transform([](auto& value) { return fmt::format("{}", value); }).value_or("null")),
        fmt::format("Networked Object Ids: {}", fmt::join(views::keys(inst()._networked_by_id), ", ")),
    };
}