#pragma once

#include<ClippedUtils/cString.h>
#include <ClippedUtils/cOsDetect.h>
#include "cSerialPortInterface.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

namespace Clipped
{
    class SerialPort : public SerialPortInterface
    {
    public:
        SerialPort(const String& interface, const Settings& settings);

        /**
         * @brief open opens the serial interface and configures it.
         * @return true if successfull, false otherwise.
         */
        bool open();

        /**
         * @brief config configures the interface.
         * @return true if successfull, false otherwise.
         */
        bool config();

        /**
         * @brief availableBytes returns the amount of current bytes that can be read.
         * @return available byte amount.
         */
        int availableBytes() const;

        /**
         * @brief writeLine writes a line over the serial interface.
         * @param data the data to be sent.
         * @param lineEnd to use.
         * @return the amoutn of characters being sent.
         */
        int writeLine(const String& data, const String& lineEnd = "\n");

        /**
         * @brief write writes the given ASCII data over the serial interface.
         * @param data String to send.
         * @return the amount of characters being sent.
         */
        int write(const String& data);

        /**
         * @brief readLine reads a line form the serial interface.
         * @param lineEnd line separator to check for.
         * @param timeoutMS timeout in milliseconds to wait before this call returns.
         * @return the read content.
         */
        String readLine(const String& lineEnd = "\n", size_t timeoutMS = 1000);

        /**
         * @brief read reads all current available data from the port.
         * @return the read content.
         */
        String read();

        /**
         * @brief sendBreak sends a line break.
         * @param milliseconds duration of the break in milliseconds.
         * @return true if successfully breaked, false otherwise.
         */
        bool sendBreak(size_t milliseconds);

        /**
         * @brief close closes the serial interface resource.
         */
        void close();

    private:
        int handle; //!< Unix file handle to serial port.
    };
} //namespace Clipped
