#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <util/helper/singleton.h>
#include <alias/chrono.h>

#include <network/networked.h>

#include <SFML/Network.hpp>

class NetworkManager { DECL_SINGLETON(NetworkManager);
public:
    static constexpr uint16 SERVER_PORT = 5300;
    DECL_REGISTRY(INetworked);
    ~NetworkManager();

    static const str& username();
    static void set_username(const str&);

    static bool connect_listener();
    static bool connect(const sf::IpAddress&, uint32 port, sf::Time timeout = 0s);

    static opt_cref<sf::IpAddress> server_address();
    static void connect_to_server(const sf::IpAddress&, sf::Time timeout = 0s);

    static bool disconnect_listener();
    static bool disconnect(const sf::IpAddress&, uint32 port);
    static void disconnect_all();

    static void request(const network_id&, const packet_id&, const opt<pair<sf::IpAddress, uint32>>& = nullopt);
    static void broadcast(const network_id&, const packet_id&, const opt<pair<sf::IpAddress, uint32>>& = nullopt);

    static void network_tick(uint64 elapsed_ticks);

    static str debug_message();

private:
    static bool register_socket(sf::TcpSocket&&);
    static void deregister_socket(sf::IpAddress, uint32, bool connection_lost = false);

    uint64 _current_tick;

    str _username;

    opt<sf::TcpListener> _client_listener;
    opt<sf::IpAddress> _server_address; sf::TcpSocket* _server_socket;
    bstmap<pair<sf::IpAddress, uint32>, sf::TcpSocket> _sockets;

    set<INetworked*> _networked; hashmap<network_id, INetworked*> _networked_by_id;

    dyn_arr<tuple<network_id, packet_id, uint64, opt<pair<sf::IpAddress, uint32>>>> _outgoing_requests;
    dyn_arr<tuple<network_id, packet_id, uint64, opt<pair<sf::IpAddress, uint32>>>> _recieved_requests;

    bool send_packet(sf::Packet&, sf::TcpSocket&);
    bool send_packet(sf::Packet&&, const opt<pair<sf::IpAddress, uint32>>&);
    bool send_packet(sf::Packet&&);

    enum class MessageType { Default, Request, Lifecycle };
    static sf::Packet wrap(const network_id& owner, LogicalPacket&&, MessageType);
    static tuple<network_id, LogicalPacket, MessageType> unwrap(sf::Packet&&);

    void seek_server_connection(); void seek_client_connection();

    void handle_incoming(const sf::IpAddress&, uint32 port, sf::TcpSocket&);
    void handle_outgoing(INetworked&);
};