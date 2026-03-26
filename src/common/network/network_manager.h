#pragma once

#include <prelude.h>
#include <prelude/opt.h>
#include <prelude/containers.h>
#include <util/helper/singleton.h>
#include <alias/chrono.h>

#include <network/networked.h>
#include <network/address.h>

#include <SFML/Network.hpp>

class NetworkManager { DECL_SINGLETON_WITH_CONSTRUCTOR(NetworkManager);
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

    class Client {
    public:
        Client(str username = "", bool has_player = true); Client(Client&&) = default;

        const str& username() const; void set_username(const str&);
        const opt<str>& uid() const;

        bool has_player() const; void set_has_player(bool);

    private:
        bool _has_player = true;
        str _username; opt<str> _uid; bool _awaiting_uid_validation;
        friend class NetworkManager;
    };
    static opt<Client>& local_client();
    static opt_cref<Client> get_remote_client(const SocketAddress&);
    static opt_cref<Client> get_client_by_uid(const str& uid);
    
    static opt_cref<SocketAddress> remote_server_address();
    static void set_remote_server_address(const SocketAddress&);
    
    /// Messages to be displayed in the debug hud
    static dyn_arr<str> debug_message();
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
    
    set<INetworked*> _networked;
    hashmap<network_id, INetworked*> _networked_by_id;
    
    opt<SocketAddress> _remote_server_address;
    opt<Client> _local_client;

    opt<TcpListener> _remote_client_listener;

    bstmap<SocketAddress, TcpSocket> _sockets; bstmap<SocketAddress, Client> _socket_clients;

    Client& set_remote_client(const SocketAddress&, Client&&);
    void clear_remote_client(const SocketAddress&);

    bstmap<SocketAddress, dyn_arr<LogicalPacket>> _accumulated_packets;
    void send(const LogicalPacket&, const opt<SocketAddress>&);
    void send(LogicalPacket&&, const opt<SocketAddress>&);
};