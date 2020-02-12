#include "cSocketLinux.h"
#include <ClippedUtils/cLogger.h>
#include <arpa/inet.h>
#include <regex>

using namespace Clipped;

String Socket::IPv4Regex = "^((25[0-5]|(2[0-4]|1[0-9]|[1-9]|)[0-9])(\\.(?!$)|$)){4}$";

//=====   Socket   =====//

Socket::Socket(const SocketType sockType, const AddressFamily& addrFamily)
    : address("")
    , socketType(sockType)
    , addressFamily(addrFamily)
    , port(0)
    , sockfd(0)
    , myAddress{0}
    , remoteAddress{0}
{}

bool Socket::listen(const String& listeningAddress, const int listenPort)
{
    address = listeningAddress;
    port = listenPort;
    bool result = setupSocket();

    if(result)
    {
        if(AddressFamily::IPv4 == addressFamily)
        {
            myAddress = {0};
            myAddress.sin_family = unixAddressFamily;
            myAddress.sin_port = htons(listenPort);
            if(!toIPv4(address, myAddress.sin_addr))
            {
                LogError() << "Invalid IPv4 address: " << address;
                return false;
            }
        }
        else if(AddressFamily::IPv6 == addressFamily)
        {
            myAddress6 = {0};
            myAddress6.sin6_family = unixAddressFamily;
            myAddress6.sin6_port = htons(listenPort);
            if(!toIPv6(address, myAddress6.sin6_addr))
            {
                LogError() << "Invalid IPv6 address: " << address;
                return false;
            }
        }
        else
        {
            LogError() << "Unsupported address family!";
            return false;
        }
        //Address setup complete, if we get here.
        if(0 > bind(sockfd, (struct sockaddr*) &myAddress, sizeof(sockaddr)))
        {
            LogError() << "Can't bind socket!";
            return false;
        }
        ::listen(sockfd, 5); //Queue size 5: Maximum value on most systems.
    }
    else
    {
        LogError() << "Setup failed due to bad parameters!";
        return result;
    }
    return result;
}

bool Socket::connect(const String& connectAddress, const int connectPort)
{
    address = connectAddress;
    port = connectPort;
    bool result = setupSocket();

    if(result)
    {

    }
    else
    {
        LogError() << "Setup failed due to bad parameters!";
        return result;
    }
    return result;
}

//Internals - private:

bool Socket::getLinuxAddressFamily(int& family) const
{
    if(addressFamily == AddressFamily::IPv4)
    {
        family = AF_INET;
    }
    else if(addressFamily == AddressFamily::IPv6)
    {
        family = AF_INET6;
    }
    else //Should never happen.
    {
        LogError() << "Unknown protocol type!";
        return false;
    }
    return true;
}

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

bool Socket::setupSocket()
{
    if(0 > sockfd)
    {
        LogError() << "The socket is already configured!";
        return false;
    }

    bool result = true;
    result &= getLinuxSocketType(unixSocketType);
    result &= getLinuxAddressFamily(unixAddressFamily);

    if(result)
    {
        sockfd = socket(unixAddressFamily, unixSocketType, 0);
        if(0 > sockfd) //sockfd returns -1 on fail condition.
        {
            LogError() << "Can't create socket!";
            return false;
        }
        int sockoptval = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockoptval, sizeof(int));
    }
    return result;
}

//===== SocketPeer =====//

SocketPeer::SocketPeer()
{
    this->isOpen = true;
}

bool SocketPeer::open(const IODevice::OpenMode& mode)
{
    return isOpen; //SocketPeer will be active, if created. Therefore nothing to do here.
}

bool SocketPeer::write(const String& data)
{

}

bool SocketPeer::close()
{
    return (0 == shutdown(fd, SHUT_RDWR));
}
