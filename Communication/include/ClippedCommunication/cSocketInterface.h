#pragma once

#include "cIODevice.h"
#include <ClippedUtils/cString.h>

namespace Clipped
{
    /**
     * @brief The SocketType enum contains available types of sockets.
     */
    enum SocketType
    {
        UDP = 0,    //!< An UDP Socket.
        TCP         //!< An TCP Socket.
    };

    const String IPv4Regex("^((25[0-5]|(2[0-4]|1[0-9]|[1-9]|)[0-9])(\\.(?!$)|$)){4}$"); //!< Regex to check for an IPv4 Address.

    class SocketInterface
    {
    public:
        virtual ~SocketInterface() {}

    protected:
        bool isIPv4; //!< Flag stating, if the socket is in IPv4 mode.
    };

} //namespace Clipped
