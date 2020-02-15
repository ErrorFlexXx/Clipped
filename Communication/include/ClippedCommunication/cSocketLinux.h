#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ClippedUtils/cString.h>

namespace Clipped
{
    class SocketAddress
    {
    public:
        union
        {
            sockaddr_in ipv4Addr;   //!< IPv4 address type.
            sockaddr_in6 ipv6Addr;  //!< IPv6 address type.
        };
    };

    /**
     * @brief The Socket class implements network sockets UDP/TCP.
     */
    class Socket : public SocketInterface
    {
    public:
        /**
         * @brief Socket creates a socket object with given socket type and address family.
         * @param sockType to create the socket for (TCP/UDP).
         * @param addrFamily to create the socket for (IPv4/IPv6).
         */
        Socket(const SocketType sockType);

        /**
         * @brief listen sets the socket to listening mode, waiting for connections.
         * @param listeningAddress address activated for listening (0.0.0.0 for all interfaces).
         * @param listenPort port to listen on.
         * @return true, if the socket has been set to listening successfully.
         */
        bool listen(const Clipped::String& listeningAddress, const int listenPort);

        /**
         * @brief connect connects the socket to a server.
         * @param connectAddress address of the server to connect to.
         * @param connectPort the server port to connect to.
         * @return true, if the connection has been established, false otherwise.
         */
        bool connect(const Clipped::String& connectAddress, const int connectPort);

        /**
         * @brief close stops listening / closes the connection.
         * @return true, if the socket has been stopped successfully.
         */
        bool close();

        /**
         * @brief isConnected returns wether the socket is currently listening/connected.
         * @return true if listening/conected, false otherwise.
         */
        bool isConnected() const;

    private:
        Clipped::String address;    //!< Address this socket works with (either listening or connecting to).
        SocketType socketType;      //!< Type of this socket (UDP/TCP).
        int port;                   //!< Port this socket acts on.
        SocketAddress remoteAddress;//!< The address of the remote.
        SocketAddress myAddress;    //!< The address of this socket.
        //Unix specific:
        int sockfd;                 //!< Socket file descriptor.
        int unixSocketType;         //!< Unix value for the socket type.

        /**
         * @brief getLinuxAddressFamily converts Clipped AddressFamily identifier to unix int.
         * @param family out int representing the address family.
         * @return true, if the value has been converted successfully.
         */
        bool getLinuxAddressFamily(int& family) const;

        /**
         * @brief getLinuxSocketType converts the Clipped SocketType identifier to an unix socket int.
         * @param type out int representing the socket type.
         * @return true, if the value has been converted successfully.
         */
        bool getLinuxSocketType(int& type) const;

        /**
         * @brief toIPv4 parses an IPv4 address string to an unix specific address type.
         * @param addressString to parse (xxx.xxx.xxx.xxx reange 0-255).
         * @param outIp the converted address, if successfull.
         * @return true, if the address string has been converted successfully.
         */
        bool toIPv4(const Clipped::String& addressString, in_addr& outIp) const;

        /**
         * @brief toIPv6 parses an IPv6 address string to an unix specific address type.
         * @param addressString to parse.
         * @param outIp the converted address, if successfull.
         * @return true, if the address string has been converted successfully.
         */
        bool toIPv6(const Clipped::String& addressString, in6_addr& outIp) const;

        /**
         * @brief setupSocket creates and configures the protocol/type of the socket.
         * @param addressFamily address family type to use (IPv4: AF_INET or IPv6: AF_INET6)
         * @return true, if created and configured successfully.
         */
        bool setupSocket(int addressFamily);
    }; //class Socket

} //namespace Clipped
