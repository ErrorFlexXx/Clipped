#pragma once

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <ClippedUtils/cString.h>
#include "cSocketInterface.h"

namespace Clipped
{
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
        Socket(const SocketType sockType, const AddressFamily& addrFamily);

        /**
         * @brief listen sets the socket to listening mode, waiting for connections.
         * @param listeningAddress address activated for listening (0.0.0.0 for all interfaces).
         * @param listenPort port to listen on.
         * @return true, if the socket has been set to listening successfully.
         */
        bool listen(const Clipped::String& listeningAddress, const int listenPort);

        /**
         * @brief accept waits for an incoming connection.
         * @return tbd.
         */
        bool accept();

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
        AddressFamily addressFamily;//!< Address family (IPv4/IPv6).
        int port;                   //!< Port this socket acts on.
        //Unix specific:
        int sockfd;                 //!< Socket file descriptor.
        int unixAddressFamily;      //!< Unix value for the address family.
        int unixSocketType;         //!< Unix value for the socket type.
        sockaddr* myFamilyAddress;  //!< Current pointer to my address of general type sock_addr.
        sockaddr* myFamilyRemote;   //!< Current pointer to remote address of general type sock_addr.
        sockaddr_in myAddress;      //!< Address of this socket.
        sockaddr_in6 myAddress6;    //!< Address of this socket (IPv6).
        sockaddr_in remoteAddress;  //!< Address of remote end.
        sockaddr_in6 remoteAddress6;//!< Address of the remote end (IPv6).
        static Clipped::String IPv4Regex; //!< Regex to check for an IPv4 Address.

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
         * @return true, if created and configured successfully.
         */
        bool setupSocket();
    }; //class Socket

    class SocketPeer : public SocketPeerInterface
    {
    public:
        SocketPeer();

        virtual bool open(const IODevice::OpenMode& mode) override;

        virtual bool readAll(std::vector<char>& data) override;

        virtual bool read(std::vector<char>& data, size_t count) override;

        virtual bool write(const String& data) override;

        virtual bool write(const std::vector<char>& data) override;

        virtual bool close() override;

    private:
        //Allow Socket to setup privates of this SocketPeer.
        friend class Socket;

        int fd;                 //!< File descriptor identifying the socket.
        sockaddr_in address;    //!< Address of the socket peer.
    }; //class SocketPeer
} //namespace Clipped
