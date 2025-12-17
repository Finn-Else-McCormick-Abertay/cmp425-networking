#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <util/helper/singleton.h>
#include <alias/chrono.h>

#include <network/networked.h>
#include <network/address.h>

#include <SFML/Network.hpp>

class NetworkManager { DECL_SINGLETON(NetworkManager);
public:
    DECL_REGISTRY(INetworked);
    ~NetworkManager();

    static void init();
    
    static void network_tick(uint64 elapsed_ticks);
    
    static void request(const network_id&, const packet_id&, const opt<SocketAddress>& = nullopt);
    static void broadcast(const network_id&, const packet_id&, const opt<SocketAddress>& = nullopt);

    static opt_cref<str> user_uid();
    static const str& username();
    static void set_username(const str&);
    
    static opt_cref<SocketAddress> server_address();
    static void set_server_address(const SocketAddress&);
    
    static str debug_message();

private:
    static constexpr Port SERVER_PORT = 5300;
    void connect_listener(Port port = SERVER_PORT);
    void disconnect_listener();

    void register_socket(TcpSocket&&);
    void connect(const SocketAddress&, sf::Time timeout = -1s, bool blocking = false);
    void disconnect(const SocketAddress&);

    void disconnect_all();
    
    void seek_server_connection();
    void seek_client_connection();
    
    void handle_incoming(const SocketAddress&, TcpSocket&);
    void handle_outgoing(INetworked&);
    
    bool send_packet(sf::Packet&, TcpSocket&);
    bool send_packet(sf::Packet&&, const opt<SocketAddress>&);
    bool send_packet(sf::Packet&&);
    
    enum class MessageType { Default, Request, Lifecycle };
    static sf::Packet wrap(const network_id& owner, LogicalPacket&&, MessageType);
    static tuple<network_id, LogicalPacket, MessageType> unwrap(sf::Packet&&);

    str _username;
    opt<str> _user_uid; bool _awaiting_user_uid;
    
    uint64 _current_tick;
    set<INetworked*> _networked; hashmap<network_id, INetworked*> _networked_by_id;

    dyn_arr<tuple<network_id, packet_id, uint64, opt<SocketAddress>>> _outgoing_requests, _outgoing_broadcasts;
    //dyn_arr<pair<network_id, LogicalPacket>> _failed_messages;

    bstmap<SocketAddress, TcpSocket> _sockets;
    opt<TcpListener> _client_listener; opt<SocketAddress> _server_address;
};