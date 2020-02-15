#include <ClippedCommunication/cSocket.h>
#include <ClippedUtils/cLogger.h>

using namespace Clipped;

bool listenSocket();


int main(void)
{
    Logger() << Logger::MessageType::Debug;
    int result = 0;
    //TODO: Currently only linux is ready to test.
    #ifdef LINUX
    result |= !listenSocket();
    #endif
    return result;

}

//TODO: Currently only linux is ready to test.
#ifdef LINUX
Socket server(SocketType::UDP);
bool listenSocket()
{
    LogInfo() << "Testcase: " << __FUNCTION__;

    if(!server.listen("::1", 4000))
    {
        LogError() << "Listen failed!";
        return false;
    }
    unsigned long long i;
    for (i = 0; i < 10000000000; i++);
    return true;
}
#endif
