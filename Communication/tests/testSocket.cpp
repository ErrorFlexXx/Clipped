#include <ClippedCommunication/cSocket.h>
#include <ClippedUtils/cLogger.h>

using namespace Clipped;

bool listenSocket();
bool connect();

int main(void)
{
    Logger() << Logger::MessageType::Debug;
    int result = 0;

    result |= !listenSocket();
    result |= !connect();

    return result;
}

Socket server(Socket::SocketType::UDP, Socket::AddressFamily::IPv4);
bool listenSocket()
{
    LogInfo() << "Testcase: " << __FUNCTION__;

    if(!server.listen("0.0.0.0", 4000))
    {
        LogError() << "Listen failed!";
        return false;
    }
    return true;
}

Socket client(Socket::SocketType::UDP, Socket::AddressFamily::IPv4);
bool connect()
{
    LogInfo() << "Testcase: " << __FUNCTION__;

    if(!client.connect("127.0.0.1", 4000))
    {
        LogError() << "Can't connect to previously created socket!";
        return false;
    }
    return true;
}
