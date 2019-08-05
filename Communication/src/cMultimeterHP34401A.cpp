#include "cMultimeterHP34401A.h"
#include <ClippedUtils/cLogger.h>
#include <ClippedEnvironment/cSystem.h>

using namespace Clipped;

MultimeterHP34401A::MultimeterHP34401A(const String& serialInterface, const ISerialPort::Settings& settings)
    : device(new SerialPort(serialInterface, settings))
    , pendingCommand(nullptr)
    , comWorkerRunning(false)
{}

MultimeterHP34401A::~MultimeterHP34401A()
{
    if(device)
    {
        device->close();
        while(comWorkerRunning) System::mSleep(1);
        delete device;
    }
}

void MultimeterHP34401A::comWorkerRoutine()
{
    comWorkerRunning = true;
    while(device->getIsOpen())
    {
        std::vector<char> buffer;
        if(device->readAll(buffer))
        {
            this->buffer.append(String(buffer));
            processResponse();
        }
        if(device->getIsOpen())
            System::mSleep(100);
    }
    comWorkerRunning = false;
}

bool MultimeterHP34401A::connect()
{
    if(device->open(IODevice::ReadWrite))
    {
        comWorker = std::thread(&MultimeterHP34401A::comWorkerRoutine, this);
        comWorker.detach();
        return true;
    }
    return false;
}

bool MultimeterHP34401A::close()
{
    return device->close();
}

bool MultimeterHP34401A::sendSCPICommand(SCPI::Command* command)
{
    pendingCommand = command;
    return device->write(command->command);
}

bool MultimeterHP34401A::sendSCPICommand(SCPI::Command* command, const String& args)
{
    pendingCommand = command;
    String assembledCommand = command->command + args + "\n";
    return device->write(assembledCommand);
}

bool MultimeterHP34401A::readSCPIResponse(SCPI::Response& response)
{
    return queue.try_dequeue(response);
}

void MultimeterHP34401A::processResponse()
{
    auto it = buffer.find_first_of(String("\n"), 0);
    while(it != String::npos) //Process all responses.
    {
        String response = buffer.substr(0, it);
        if(response.length() != 0)
        {
            if(pendingCommand)
            {
                SCPI::Response resp;
                resp.id = pendingCommand->id;
                resp.response = response;
                if(!queue.try_enqueue(resp))
                    LogWarn() << "Queue out of memory! Keep sure to pull responses with readSCPIResponse ?";

                if(pendingCommand->callback)
                {
                    pendingCommand->callback(response);
                }
            }
        }
        buffer = buffer.erase(0, it + 1);
        it = buffer.find_first_of(String("\n"), 0); //Search next \n
    }
}
