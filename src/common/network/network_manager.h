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
    
    static void perform_network_tick();
    
    /** Request packet from given network id at given address.
     * If no address is provided, request will be sent to all active sockets. */
    static void request(const network_id&, const packet_id&, const opt<SocketAddress>& = nullopt);

    /** Get packet from given network id with INetworked::get_requested_message(const packet_id&) const and send it to given address.
     *  If packet could not be successfully obtained, a packet with the contents 'unhandled' will be sent.
     *  If no address is provided, packet will be sent to all active sockets.
     * @retval Was packet successfully obtained from network id?
     */
    static bool broadcast(const network_id&, const packet_id&, const opt<SocketAddress>& = nullopt);

    /** Send packet to given address. If no address is provided, packet will be sent to all active sockets. */
    static void broadcast(const LogicalPacket&, const opt<SocketAddress>& = nullopt);

    /** Send packet to given address. If no address is provided, packet will be sent to all active sockets. */
    static void broadcast(LogicalPacket&&, const opt<SocketAddress>& = nullopt);

    static opt<str> user_uid();
    static const str& username();
    static void set_username(const str&);

    static opt_cref<str> get_uid(const SocketAddress&);
    
    static opt_cref<SocketAddress> server_address();
    static void set_server_address(const SocketAddress&);
    
    static str debug_message();
    
    // Set a minimum time between network ticks to avoid the sockets getting overwhelmed
    static constexpr chrono::microseconds NETWORK_TICK_MIN_TIMESTEP = 4ms;
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
    void handle_incoming_packet(const SocketAddress&, LogicalPacket&&);
    void handle_outgoing(INetworked&);

    result<success_t, str> handle_lifecycle(const SocketAddress&, LogicalPacket&&);
    
    set<INetworked*> _networked; hashmap<network_id, INetworked*> _networked_by_id;
    
    str _username;
    opt<str> _user_uid; bool _awaiting_user_uid;

    //sf::SocketSelector _selector;
    bstmap<SocketAddress, TcpSocket> _sockets; bstmap<SocketAddress, str> _socket_uids;
    opt<TcpListener> _client_listener; opt<SocketAddress> _server_address;

    bstmap<SocketAddress, dyn_arr<LogicalPacket>> _accumulated_packets;
    void send(const LogicalPacket&, const opt<SocketAddress>&);
    void send(LogicalPacket&&, const opt<SocketAddress>&);

    opt_cref<str> try_set_uid(const SocketAddress&, const str& requested = "", bool try_fallback = true);
    void clear_uid(const SocketAddress&);
};