#pragma once

#include <prelude.h>
#include <prelude/containers.h>
#include <util/helper/singleton.h>
#include <alias/chrono.h>

#include <network/networked.h>

#include <SFML/Network.hpp>

class NetworkManager { DECL_SINGLETON(NetworkManager);
public:
    DECL_REGISTRY(INetworked);

    static bool connect(uint16 port);
    static void disconnect();

    static void set_server_address(const sf::IpAddress&);

    static void network_tick(uint64 elapsed_ticks);
    
    static constexpr uint16 CLIENT_PORT = 5300;
    static constexpr uint16 SERVER_PORT = 5301;

private:
    set<INetworked*> _networked;

    sf::UdpSocket _socket;
    opt<sf::IpAddress> _server_address; bool _has_server_acknowledged = false;
    set<sf::IpAddress> _send_addresses;
    set<sf::IpAddress> _addresses_to_acknowledge;

    //dyn_arr<pair<str, LogicalPacket>> _remaining_packets;

    static bool is_authority();

    bool send_packet(sf::Packet&);
    bool send_packet(sf::Packet&, const sf::IpAddress&);
};