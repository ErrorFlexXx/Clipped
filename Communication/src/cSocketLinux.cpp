/*
** Clipped -- a Multipurpose C++ Library.
**
** Copyright (C) 2019-2020 Christian Löpke. All rights reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
*/

#include <ClippedUtils/cOsDetect.h>

#ifdef LINUX //This implementation shall only be compiled on linux systems.

#include "cSocket.h"
#include <ClippedUtils/cLogger.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <regex>

using namespace Clipped;

//=====   Socket   =====//

Socket::Socket(const SocketType sockType)
    : address("")
    , socketType(sockType)
    , port(0)
    , sockfd(0)
    , myAddress{0}
    , remoteAddress{0}
{}

bool Socket::listen(const String& listeningAddress, const int listenPort)
{
    address = listeningAddress;
    port = listenPort;

    //Detect if an IPv4 or IPv6 address is requested.
    if(std::regex_match(listeningAddress, std::regex(IPv4Regex)))
    {
        LogDebug() << "Socket set to IPv4 mode.";
        isIPv4 = true;
        if(!toIPv4(address, myAddress.ipv4Addr.sin_addr))
        {
            LogError() << "Invalid IPv4 address detected: " << address;
            return false;
        }

        myAddress.ipv4Addr.sin_family = AF_INET;
        myAddress.ipv4Addr.sin_port = htons(port);

        if(!setupSocket(AF_INET)) //Setup IPv4 socket.
        {
            LogError() << "Socket setup failed!";
            return false;
        }
        if(0 != ::bind(sockfd, reinterpret_cast<sockaddr*>(&myAddress.ipv4Addr), sizeof(myAddress.ipv4Addr)))
        {
            LogError() << "Socket bind failed: " << strerror(errno);
            return false;
        }
    } //IPv4 end.
    else //IPv6 mode.
    {
        LogDebug() << "Socket set to IPv6 mode.";
        this->isIPv4 = false;
        if(!toIPv6(address, myAddress.ipv6Addr.sin6_addr))
        {
            LogError() << "Invalid IPv6 address detected: " << address;
            return false;
        }
        myAddress.ipv6Addr.sin6_port = htons(port);
        myAddress.ipv6Addr.sin6_family = AF_INET6;
        if(!setupSocket(AF_INET6)) //Setup IPv6 socket.
        {
            LogError() << "Socket setup failed!";
            return false;
        }
        if(0 != ::bind(sockfd, reinterpret_cast<sockaddr*>(&myAddress.ipv6Addr), sizeof(myAddress.ipv6Addr)))
        {
            LogError() << "Socket bind failed!" << strerror(errno);
            return false;
        }
    } //IPv6 end.

    //All modes:
    if(SocketType::UDP != socketType)
    {
        if(-1 == ::listen(sockfd, 5)) //5 , the maximum value on most systems.
        {
            LogError() << "Socket listen failed: " << strerror(errno);
            return false;
        }
    }
    //else: UDP doesn't need/supports the listen function.
    sockaddr_in6 addr;
    socklen_t socklen;
    ::accept(sockfd, (sockaddr*) &addr, &socklen);
    return true;
}

//Internals - private:
bool Socket::getLinuxSocketType(int& type) const
{
    if(socketType == SocketType::TCP)
    {
        type = SOCK_STREAM;
    }
    else if(socketType == SocketType::UDP)
    {
        type = SOCK_DGRAM;
    }
    else
    {
        LogError() << "Unknown socket type!";
        return false;
    }
    return true;
}

bool Socket::toIPv4(const Clipped::String& addressString, in_addr& outIp) const
{
    return (1 == inet_pton(AF_INET, addressString.c_str(), &outIp));
}

bool Socket::toIPv6(const Clipped::String& addressString, in6_addr& outIp) const
{
    return (1 == inet_pton(AF_INET6, addressString.c_str(), &outIp));
}

bool Socket::setupSocket(int addressFamily)
{
    if(0 > sockfd)
    {
        LogError() << "The socket is already configured!";
        return false;
    }

    bool result = true;
    result &= getLinuxSocketType(unixSocketType);

    if(result)
    {
        sockfd = socket(addressFamily, unixSocketType, 0);
        if(0 > sockfd) //sockfd returns -1 on fail condition.
        {
            LogError() << "Can't create socket!";
            return false;
        }
//        int sockoptval = 1; //Possibly means: enable.
//        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));
//        int flags = fcntl(sockfd, F_GETFL); //Get current socket flags.
//        if(-1 == fcntl(sockfd, F_SETFL, flags | O_NONBLOCK)) //Set socket to be non blocking.
//        {
//            LogError() << "Set socket non blocking failed!";
//            return false;
//        }
    }
    return result;
}

#endif //ifdef LINUX
