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
    static constexpr uint16 LISTENER_PORT = 5300;
    DECL_REGISTRY(INetworked);
    ~NetworkManager();

    static const str& username();
    static void set_username(const str&);

    static bool connect_listener();
    static bool connect(const sf::IpAddress&, sf::Time timeout = 0s);
    static bool connect(const sf::IpAddress&, sf::TcpSocket&&, sf::Time timeout = 0s);

    static opt_cref<sf::IpAddress> server_address();
    static void connect_to_server(const sf::IpAddress&, sf::Time timeout = 0s);

    static bool disconnect_listener();
    static bool disconnect(const sf::IpAddress&);
    static void disconnect_all();

    static void request(const network_id&, const packet_id&, const sf::IpAddress& = sf::IpAddress::Any);
    static void broadcast(const network_id&, const packet_id&, const sf::IpAddress& = sf::IpAddress::Any);

    static void network_tick(uint64 elapsed_ticks);

    static str debug_message();

private:
    uint64 _current_tick;

    str _username;

    opt<sf::TcpListener> _client_listener;
    opt<sf::IpAddress> _server_address; sf::TcpSocket* _server_socket;
    bstmap<sf::IpAddress, sf::TcpSocket> _sockets;

    set<INetworked*> _networked; hashmap<network_id, INetworked*> _networked_by_id;

    dyn_arr<tuple<network_id, packet_id, uint64, sf::IpAddress>> _outgoing_requests;
    dyn_arr<tuple<network_id, packet_id, uint64, sf::IpAddress>> _recieved_requests;

    bool send_packet(sf::Packet&, sf::TcpSocket&);
    bool send_packet(sf::Packet&&, const sf::IpAddress&);
    bool send_packet(sf::Packet&&);

    enum class MessageType { Default, Request, Lifecycle };
    static sf::Packet wrap(const network_id& owner, LogicalPacket&&, MessageType);
    static tuple<network_id, LogicalPacket, MessageType> unwrap(sf::Packet&&);

    void seek_server_connection(); void seek_client_connection();
    static void handle_disconnection(sf::IpAddress, bool connection_lost = false);

    void handle_incoming(const sf::IpAddress&, sf::TcpSocket&);
    void handle_outgoing(INetworked&);
};