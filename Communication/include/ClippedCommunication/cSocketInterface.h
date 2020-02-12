#pragma once

#include "cIODevice.h"

namespace Clipped
{
    /**
     * @brief The SocketInterface class parent class of sockets.
     */
    class SocketInterface
    {
    public:
        virtual ~SocketInterface() {}

        /**
         * @brief The SocketType enum contains available types of sockets.
         */
        enum SocketType
        {
            UDP = 0,    //!< An UDP Socket.
            TCP         //!< An TCP Socket.
        };

        /**
         * @brief The AddressFamily enum contains availble address families.
         */
        enum AddressFamily
        {
            IPv4 = 0,   //!< IPv4 connection.
            IPv6        //!< IPv6 connection.
        };
    }; //class SocketInterface

    /**
     * @brief The SocketPeerInterface class has to be implemented from
     *  platform SocketPeer classes to get a working Peer communication.
     */
    class SocketPeerInterface : public IODevice
    {
    }; //class SocketPeer
} //namespace Clipped
