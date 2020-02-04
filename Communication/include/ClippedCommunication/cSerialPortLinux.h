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

#pragma once

#include <ClippedUtils/cString.h>
#include <ClippedUtils/cOsDetect.h>
#include "cSerialPortInterface.h"

#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

namespace Clipped
{
    /**
     * @brief The SerialPort class is the implementation of a serial interface.
     */
    class SerialPort : public ISerialPort
    {
    public:
        SerialPort(const String& interfaceName, const Settings& settings);

        virtual ~SerialPort() override;

        /**
         * @brief open opens the serial interface and configures it.
         * @return true if successfull, false otherwise.
         */
        virtual bool open(const IODevice::OpenMode& mode) override;

        /**
         * @brief config configures the interface.
         * @return true if successfull, false otherwise.
         */
        bool config();

        /**
         * @brief config setup of the port with given settings.
         * @param settings to use for the port setup.
         * @return true if successfull, false otherwise.
         */
        bool config(const Settings& settings);

        /**
         * @brief availableBytes returns the amount of current bytes that can be read.
         * @return true if query succeeded.
         */
        bool availableBytes(size_t& availableBytes) const;

        /**
         * @brief writeLine writes a line over the serial interface.
         * @param data the data to be sent.
         * @param lineEnd to use.
         * @return the amount of characters being sent.
         */
        int writeLine(const String& data, const String& lineEnd = "\n");

        /**
         * @brief write writes the given ASCII data over the serial interface.
         * @param data String to send.
         * @return the amount of characters being sent.
         */
        virtual bool write(const String& data) override;

        virtual bool write(const std::vector<char>& data) override;

        /**
         * @brief readLine reads a line form the serial interface.
         * @param lineEnd line separator to check for.
         * @param timeoutMS timeout in milliseconds to wait before this call returns.
         * @return the read content.
         */
        String readLine(const String& lineEnd = "\n", size_t timeoutMS = 1000);

        /**
         * @brief readAll reads all data currently available from serial port.
         * @param data to put the data in.
         * @return true if successfully, false if device error or something.
         */
        virtual bool readAll(std::vector<char>& data) override;

        /**
         * @brief read reads count bytes from the port.
         * @param data to store the read data in.
         * @param count bytes shall be read.
         * @return true if successfully read, false if something has gone wrong.
         */
        virtual bool read(std::vector<char>& data, size_t count) override;

        /**
         * @brief sendBreak sends a line break.
         * @param milliseconds duration of the break in milliseconds.
         * @return true if successfully breaked, false otherwise.
         */
        bool sendBreak(size_t milliseconds);

        /**
         * @brief close closes the serial interface resource.
         */
        virtual bool close() override;

    private:
        int handle; //!< Unix file handle to serial port.
    };
} //namespace Clipped
