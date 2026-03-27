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

    /** Initialise connections. Should be called after command line arguments have been handled. */
    static void init();

    /** Recieve messages over network, tick all registered INetworked objects, send accumulated packets. */
    static void perform_network_tick();
    
    /** Get messages to be displayed in the debug hud this frame. */
    static dyn_arr<str> debug_message();

    
    /* --------------------------------- */
    /* --  Network State Information  -- */
    /* --------------------------------- */

    // Putting the ifdefs here allows us to use if constexpr everywhere else, making everything much cleaner

    /** Is the application in server mode? */
    static constexpr bool is_server() {
        #ifdef SERVER
        return true;
        #endif
        return false;
    }
    
    /** Is the application in client mode? */
    static constexpr bool is_client() {
        #ifdef CLIENT
        return true;
        #endif
        return false;
    }


    /* --------------------------------- */
    /* -- Packet Broadcasting Methods -- */
    /* --------------------------------- */
    
    /** Request packet from given network id at given address.
     * If no address is provided, request will be sent to all active sockets. */
    static void request(const network_id&, const packet_id&, const opt<SocketAddress>& = nullopt);

    /** Get packet from given network id with INetworked::get_requested_message(const packet_id&) const and send it to given address.
     *  If packet could not be successfully obtained, a packet with the contents 'unhandled' will be sent. If no address is provided, packet will be sent to all active sockets.
     * @retval Was packet successfully obtained from network id? */
    static bool broadcast(const network_id&, const packet_id&, const opt<SocketAddress>& = nullopt);

    /** Send packet to given address. If no address is provided, packet will be sent to all active sockets. */
    static void broadcast(const LogicalPacket&, const opt<SocketAddress>& = nullopt);

    /** Send packet to given address. If no address is provided, packet will be sent to all active sockets. */
    static void broadcast(LogicalPacket&&, const opt<SocketAddress>& = nullopt);


    /* ---------------------------------- */
    /* --   Client Mode Information    -- */
    /* ---------------------------------- */

    class Client;
    /** Object representing local client, if we are a client. Will be nullopt in server mode. */
    static opt<Client>& local_client();
    
    /** Address of remote server we are connected to or trying to connect to. */
    static const opt<SocketAddress>& remote_server_address();
    /** Set address of remote server to connect to.
     * Note that this method does not trigger disconnection from the old address or connection to the new address. */
    static void set_remote_server_address(const SocketAddress&);
    
    /** Are we connected to the currently set remote server address?
     * \related static opt_cref<SocketAddress> remote_server_address() */
    static bool is_remote_server_connected();

    
    /* ---------------------------------- */
    /* --   Server Mode Information    -- */
    /* ---------------------------------- */
    
    /** Get client corresponding to given socket address. Only works in server mode. */
    static opt_cref<Client> get_remote_client(const SocketAddress&);
    
    /** Get client corresponding to given UID. In client mode, it can only find the local client. */
    static opt_cref<Client> get_client_by_uid(const str& uid);


    /* ------------------------------------------------ */
    /* -- Class Representing Local or Remote Client  -- */
    /* ----------------------------------------------- */
    
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
private:
    hashmap<network_id, INetworked*> _networked;
    

    opt<Client> _local_client;
    opt<SocketAddress> _remote_server_address;


    static constexpr Port SERVER_PORT = 5300;
    opt<TcpListener> _remote_client_listener;
    void connect_remote_client_listener(Port port = SERVER_PORT);
    void disconnect_remote_client_listener();


    bstmap<SocketAddress, TcpSocket> _sockets;
    bstmap<SocketAddress, Client> _socket_clients;
    void register_socket(TcpSocket&&);
    void connect(const SocketAddress&, sf::Time timeout = -1s, bool blocking = false);
    void disconnect(const SocketAddress&);
    void disconnect_all();
    
    Client& set_remote_client(const SocketAddress&, Client&&);
    void clear_remote_client(const SocketAddress&);
    
    
    void handle_incoming(const SocketAddress&, TcpSocket&);
    void handle_incoming_packet(const SocketAddress&, LogicalPacket&&);
    void handle_outgoing(INetworked&);
    result<success_t, str> handle_lifecycle(const SocketAddress&, LogicalPacket&&);

    
    bstmap<SocketAddress, dyn_arr<LogicalPacket>> _accumulated_packets;
    void send(const LogicalPacket&, const opt<SocketAddress>&);
    void send(LogicalPacket&&, const opt<SocketAddress>&);
};