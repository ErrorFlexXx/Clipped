#pragma once

#include "cSerialPort.h"
#include <readerwriterqueue/readerwriterqueue.h>
#include <thread>
#include <functional>

namespace Clipped
{
    namespace SCPI
    {
        enum ID
        {
            RemoteId,
            LocalId,
            ErrorId,
            MeasVoltDCId,
            IdentId,
            DispTextId
        };

        struct Command
        {
        public:
            ID id;
            String command;
            std::function<void(const String& response)> callback;
        };

        /**
         * @brief The Response struct shipped via readSCPIResponse method.
         */
        struct Response
        {
        public:
            ID id;  //!< Enum ID value to identify the response type.
            String response; //!< Response text
        };

        struct Remote : public Command
        {
        public:
            Remote() { id = RemoteId; command = "SYST:REM\n"; }
        } Remote;

        struct Local : public Command
        {
        public:
            Local() { id = LocalId; command = "SYST:LOC\n"; }
        } Local;

        struct Error : public Command
        {
        public:
            Error() { id = ErrorId; command = "SYST:ERR?\n"; }
        } Error;

        struct MeasVoltDC : public Command
        {
        public:
            MeasVoltDC() { id = MeasVoltDCId; command = "MEAS:VOLT:DC?\n"; }
        } MeasVoltDC;

        struct Ident : public Command
        {
        public:
            Ident() { id = IdentId; command = "*IDN?\n"; }
        } Ident;

        struct DispText : public Command
        {
        public:
            DispText() { id = DispTextId; command = "DISP:TEXT "; }
        } DispText;
    }

    class MultimeterHP34401A
    {
    public:
        /**
         * @brief Multimeter constructor for a serial port connected multimeter.
         * @param serialInterface serial interface path (e.g. COM0 or /dev/ttyUSB0)
         * @param settings Serial port interface settings.
         */
        MultimeterHP34401A(const String& serialInterface, const ISerialPort::Settings& settings);

        ~MultimeterHP34401A();

        bool connect();
        bool sendSCPICommand(SCPI::Command* command);
        bool sendSCPICommand(SCPI::Command* command, const String& args);
        bool readSCPIResponse(SCPI::Response& response);
        bool close();

    private:
        moodycamel::ReaderWriterQueue<SCPI::Response> queue;
        SCPI::Command* pendingCommand;
        std::atomic<bool> comWorkerRunning;
        std::thread comWorker;
        IODevice* device;
        String buffer;

        void comWorkerRoutine();
        void processResponse();
    };
}
