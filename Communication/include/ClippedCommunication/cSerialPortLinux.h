/* Copyright 2019 Christian Löpke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
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

        virtual ~SerialPort();

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
