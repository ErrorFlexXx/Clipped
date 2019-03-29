#include "cMultimeterHP34401A.h"
#include <ClippedUtils/cLogger.h>
#include <unistd.h>

using namespace Clipped;

Multimeter::Multimeter(const String& serialInterface, const ISerialPort::Settings& settings)
    : device(new SerialPort(serialInterface, settings))
    , pendingCommand(nullptr)
    , comWorkerRunning(false)
{}

Multimeter::~Multimeter()
{
    if(device)
    {
        device->close();
        while(comWorkerRunning) usleep(1000);
        delete device;
    }
}

void Multimeter::comWorkerRoutine()
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
            usleep(100000);
    }
    comWorkerRunning = false;
}

bool Multimeter::connect()
{
    if(device->open(IODevice::ReadWrite))
    {
        comWorker = std::thread(&Multimeter::comWorkerRoutine, this);
        comWorker.detach();
        return true;
    }
    return false;
}

bool Multimeter::close()
{
    return device->close();
}

bool Multimeter::sendSCPICommand(SCPI::Command* command)
{
    pendingCommand = command;
    return device->write(command->command);
}

bool Multimeter::sendSCPICommand(SCPI::Command* command, const String& args)
{
    pendingCommand = command;
    String assembledCommand = command->command + args + "\n";
    return device->write(assembledCommand);
}

bool Multimeter::readSCPIResponse(SCPI::Response& response)
{
    return queue.try_dequeue(response);
}

void Multimeter::processResponse()
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
