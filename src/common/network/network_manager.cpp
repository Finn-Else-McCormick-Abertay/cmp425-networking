#include "network_manager.h"
#include <prelude/containers.h>
#include <console.h>
#include <util/format/SFML/network.h>
#include <prelude/format.h>
#include <alias/ranges.h>

#include <actor/actor_manager.h>

DEFINE_SINGLETON(NetworkManager);

NetworkManager::~NetworkManager() {
    disconnect_all();
}

void NetworkManager::Registry::__register(INetworked& networked) {
    auto network_id = networked.network_id();
    if (inst()._networked_by_id.contains(network_id))
        print<error, NetworkManager::Registry>("Overwrote network id '{}'.");
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
        print<error, NetworkManager>("!UNHANDLED! Username changed while server connected.");
}

opt<str> NetworkManager::user_uid() {
    if (inst()._user_uid) return *inst()._user_uid;
    return nullopt;
}

opt_cref<str> NetworkManager::get_uid(const SocketAddress& address) {
    if (inst()._socket_uids.contains(address)) return inst()._socket_uids[address];
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
    print<success, NetworkManager>("Listening on port {}.", port);
}
void NetworkManager::disconnect_listener() {
    if (_client_listener) {
        print<success, NetworkManager>("Stopped listening on port {}.", _client_listener->getLocalPort());
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
    if (!_server_address) return;
    if (!_sockets.contains(*_server_address)) connect(*_server_address, 100ms, true);

    if (_sockets.contains(*_server_address) && !_user_uid && !_awaiting_user_uid) {
        sf::Packet packet; packet << _username;
        auto logical_packet = LogicalPacket("uid", _current_tick, move(packet));
        logical_packet.owner = network_id("lifecycle"_id, "request");
        logical_packet.type = LogicalPacket::MessageType::Lifecycle;
        send(logical_packet, *_server_address);
        _awaiting_user_uid = true;
        //auto wrapped = wrap(, , );
        //_awaiting_user_uid = send_packet(wrapped, _sockets.at(*_server_address));
    }
}

void NetworkManager::seek_client_connection() {
    if (!_client_listener) return;
    TcpSocket new_socket;
    auto status = _client_listener.value().accept(new_socket);
    if (status == Socket::Status::Done) register_socket(move(new_socket));
    else if (status == Socket::Status::Error) print<error, NetworkManager>("Failed to accept new connection.");
}

opt_cref<str> NetworkManager::try_set_uid(const SocketAddress& address, const str& requested, bool try_fallback) {
    if (requested.empty()) {
        if (try_fallback) return try_set_uid(address, fmt::format("guest[{}]", address), false);
        return nullopt;
    }
    for (auto& [existing_addr, existing_uid] : _socket_uids) {
        if (existing_uid == requested && existing_addr != address) {
            print<warning, NetworkManager>("{} requested username '{}', which is already in use by {}.", address, requested, existing_addr);
            if (try_fallback) return try_set_uid(address, fmt::format("{}[{}]", requested, address), false);
            return nullopt;
        }
    }
    clear_uid(address);
    auto [it, _] = _socket_uids.emplace(address, requested);
    const str& uid = it->second;
    print<network_info, NetworkManager>("Registered {} with uid {}.", address, uid);

    // As it stands this method should only ever run in server mode but yknow. Just in case.
    #ifdef SERVER
    // Create client's corresponding player
    ActorManager::register_player(uid, true);
    #endif

    return cref(uid);
}

void NetworkManager::clear_uid(const SocketAddress& address) {
    //print<debug, NetworkManager>("Attempted to clear uid for {}. ({})", address, _socket_uids.contains(address));
    //for (auto [socket, uid] : _socket_uids) print<debug, NetworkManager>(" - {} : {}", socket, uid);
    if (!_socket_uids.contains(address)) return;
    #ifdef SERVER
    ActorManager::unregister_player(_socket_uids[address], true);
    #endif
    _socket_uids.erase(address);
}

result<success_t, str> NetworkManager::handle_lifecycle(const SocketAddress& address, const network_id& owner, LogicalPacket&& packet) {
    if (owner.type() != "lifecycle"_id && !(owner.inst() == "request" || owner.inst() == "answer"))
        return err(fmt::format("Invalid network id '{}'.", owner));
    
    bool is_request = owner.inst() == "request";
    
    if (packet.id.type() == "uid") {
        if (is_request) {
            auto response_packet = LogicalPacket("uid", _current_tick);
            response_packet.owner = network_id("lifecycle"_id, "answer");
            response_packet.type = LogicalPacket::MessageType::Lifecycle;

            str requested_username; packet.packet >> requested_username;
            if (auto result = try_set_uid(address, requested_username)) {
                response_packet.id = packet_id("uid!success");
                response_packet.packet << *result;
            }
            else  {
                print<error, NetworkManager>("Failed to set uid for {}.", address);
                response_packet.id = packet_id("uid!failure;taken");
            }

            send(response_packet, address);

            // Update them on the already connected players
            for (auto& [_, uid] : inst()._socket_uids)
                broadcast(network_id("singleton"_id, "actor_manager"), packet_id("player", { "existing", uid }), address);

            return empty_success;
        }
        else {
            _awaiting_user_uid = false;
            // If operation failed
            if (auto success_arg = packet.id.get_arg(0); success_arg
                && *success_arg != "success"
            ) {
                if (auto reason_arg = packet.id.get_arg(1)) return err(fmt::format("{}: {}", *success_arg, *reason_arg));
                return err(*success_arg);
            }
            str validated_uid; packet.packet >> validated_uid;
            _user_uid = validated_uid;
            ActorManager::update_player_authority_states();
            print<network_info, NetworkManager>("Registered with server as {}.", *_user_uid);
            return empty_success;
        }
    }
    else return err(fmt::format("Unknown packet type '{}'.", packet.id.type()));

    return err("Unhandled.");
}

void NetworkManager::append_packet_into(sf::Packet& sum_packet, LogicalPacket&& logical_packet) {
    sum_packet << logical_packet.owner.to_str();
    sum_packet << logical_packet.id.as_str();
    sum_packet << (uint8)logical_packet.type;
    sum_packet << (uint64)logical_packet.time;
    sum_packet << (uint64)logical_packet.packet.getDataSize();
    sum_packet.append(logical_packet.packet.getData(), logical_packet.packet.getDataSize());
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
        _outgoing_broadcasts.emplace_back(logical_packet.owner, logical_packet.id, logical_packet.time, address);
    }
    else if (logical_packet.type == LogicalPacket::MessageType::Lifecycle) {
        auto result = handle_lifecycle(address, logical_packet.owner, move(logical_packet));
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
            str owner_str; packet >> owner_str;
            str id_str; packet >> id_str;
            uint8 type; packet >> type;
            uint64 time; packet >> time;
            uint64 data_size; packet >> data_size;
            uint8 data[data_size];
            for (int j = 0; j < data_size; ++j) packet >> data[j];

            LogicalPacket logical_packet = LogicalPacket(id_str, time);
            logical_packet.owner = network_id::from_str(owner_str);
            logical_packet.type = (LogicalPacket::MessageType)type;
            logical_packet.packet.append(data, data_size);

            handle_incoming_packet(address, move(logical_packet));
        }
    }
    else if (status == Socket::Status::Disconnected) {
        print<network_info>("Lost connection with {}.", address);
        clear_uid(address);
        socket.disconnect();
        inst()._sockets.erase(address);
    }
    else if (status != Socket::Status::NotReady)
        print<error, NetworkManager>("Failed to recieve packet from {} : {}", address, status);
}

void NetworkManager::handle_outgoing(INetworked& networked) {
    auto net_id = networked.network_id();
    for (auto& message : networked.outstanding()) {
        //print<network_info>("OUTGOING {} {}", net_id, message.id);
        message.time = _current_tick;
        message.owner = net_id;
        auto pack_id = message.id;

        send(move(message), nullopt);
    }
}

void NetworkManager::request(const network_id& owner, const packet_id& packet_id, const opt<SocketAddress>& target) {
    print<debug, NetworkManager>("REQUEST {} {} {}", owner, packet_id, target);
    inst()._outgoing_requests.emplace_back(owner, packet_id, inst()._current_tick, target);
}

void NetworkManager::broadcast(const network_id& owner, const packet_id& packet_id, const opt<SocketAddress>& target) {
    print<debug, NetworkManager>("BROADCAST {} {} {}", owner, packet_id, target);
    inst()._outgoing_broadcasts.emplace_back(owner, packet_id, inst()._current_tick, target);
}

void NetworkManager::network_tick(uint64 elapsed_ticks) {
    //uint64 tick_delta = elapsed_ticks - inst()._current_tick;
    inst()._current_tick = elapsed_ticks;

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

    // -- Handle outgoing messages --
    
    // Outgoing requests
    for (auto it = inst()._outgoing_requests.begin(), it_next = it; it != inst()._outgoing_requests.end(); it = it_next) {
        ++it_next; auto [netid, packid, time, target] = *it;
        auto logical_packet = LogicalPacket(packid, inst()._current_tick);
        logical_packet.owner = netid;
        logical_packet.type = LogicalPacket::MessageType::Request;
        inst().send(logical_packet, target);
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

        pack.owner = netid;

        inst().send(pack, address);
        it_next = inst()._outgoing_broadcasts.erase(it);
    }

    // Ticked messages
    for (auto networked : inst()._networked) inst().handle_outgoing(*networked);

    // -- Sum up messages sent this tick --

    // Sum together packets aimed at all sockets
    auto all_address = SocketAddress(sf::IpAddress::Any, Socket::AnyPort);
    size_t shared_message_count = 0;
    sf::Packet shared_sum_packet;
    if (inst()._accumulated_packets.contains(all_address)) {
        auto& packet_array = inst()._accumulated_packets.at(all_address);
        shared_message_count = packet_array.size();
        for (auto& logical_packet : packet_array) append_packet_into(shared_sum_packet, move(logical_packet));
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
            for (auto& specific_packet : *specific_arr) append_packet_into(sum_packet, move(specific_packet));
        
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

str NetworkManager::debug_message() {
    return fmt::format("{}", fmt::join(dyn_arr<str>{
        fmt::format("Username: {} | UID: {}", inst()._username, inst().user_uid().value_or("null")),
        fmt::format("Server: {}", inst()._server_address),
        fmt::format("Networked Object Ids: {}", fmt::join(views::keys(inst()._networked_by_id), ", "))
    }, "\n"));
}