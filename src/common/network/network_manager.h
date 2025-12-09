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
    static constexpr uint16 PORT = 5300;

    ~NetworkManager();
    DECL_REGISTRY(INetworked);

    static bool connect_listener();
    static bool connect(const sf::IpAddress&, sf::Time timeout = 1s);

    static bool disconnect_listener();
    static bool disconnect(const sf::IpAddress&);
    static void disconnect_all();

    static void network_tick(uint64 elapsed_ticks);

    static str debug_message();

private:
    set<INetworked*> _networked;
    hashmap<str, INetworked*> _networked_by_id;

    opt<sf::TcpListener> _listener;
    bstmap<sf::IpAddress, sf::TcpSocket> _sockets;

    bool send_packet(sf::Packet&, sf::TcpSocket&);
    bool send_packet(sf::Packet&, const sf::IpAddress&);
    bool send_packet(sf::Packet&);
};