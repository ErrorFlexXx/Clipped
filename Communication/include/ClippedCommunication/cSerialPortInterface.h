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
#include "cIODevice.h"

namespace Clipped
{
    /**
     * @brief The SerialPortInterface parent class of serial ports. (OS independent).
     */
    class ISerialPort : public IODevice
    {
    public:
        /**
         * @brief The Baudrate enum contains possible baud rate settings.
         */
        enum Baudrate
        {
            Baud50,
            Baud75,
            Baud110,
            Baud134,
            Baud150,
            Baud200,
            Baud300,
            Baud600,
            Baud1200,
            Baud1800,
            Baud2400,
            Baud4800,
            Baud9600,
            Baud19200,
            Baud38400,
            Baud57600,
            Baud115200,
            Baud230400
        };

        /**
         * @brief The DataBits enum represents how many bits represent one byte on the transmission.
         */
        enum DataBits
        {
            DataBits5,
            DataBits6,
            DataBits7,
            DataBits8
        };

        /**
         * @brief The StartBits enum contains possible numbers of startbits
         */
        enum StartBits
        {
            StartBits1 //!< One startbit per byte.
        };

        /**
         * @brief The StopBits enum contains the number of stopbits per byte.
         */
        enum StopBits
        {
            StopBits1, //!< One Stopbit per byte.
            StopBits2  //!< Two Stopbits per byte.
        };

        /**
         * @brief The Parity enum contains available options regarding the parity setting.
         */
        enum Parity
        {
            ParityNone, //!< No parity
            ParityEven, //!< Even parity
            ParityOdd,  //!< Odd parity
        };

        /**
         * @brief Available options reagarding the flow control.
         */
        enum FlowControl
        {
            FlowControlNone
        };

        /**
         * @brief The Settings struct clusters all necessary settings to power up a serial port.
         */
        struct Settings
        {
            /**
             * @brief Settings constructor for default settings.
             */
            Settings() : Settings(Baud9600, DataBits8, StartBits1, StopBits1, ParityNone, FlowControlNone) {}

            /**
             * @brief Settings constructor containing all attributes.
             */
            Settings(Baudrate baud, DataBits dataBits, StartBits startBits, StopBits stopBits, Parity parity, FlowControl flowControl)
                : baud(baud)
                , dataBits(dataBits)
                , startBits(startBits)
                , stopBits(stopBits)
                , parity(parity)
                , flowControl(flowControl)
            {}

            Baudrate baud;          //!< The baud rate setting.
            DataBits dataBits;      //!< How many bits a byte contains.
            StartBits startBits;    //!< How many start bits are flagging a byte.
            StopBits stopBits;      //!< How many stop bits are at the end of a byte.
            Parity parity;          //!< Used parity for the transmission.
            FlowControl flowControl;//!< Setupped flow control.
        };

        ISerialPort(const String& interfaceName, const Settings& settings)
            : interfaceName(interfaceName)
            , settings(settings)
        {}

        virtual ~ISerialPort() {}

    protected:
        String interfaceName; //!< Name of the serial interface (e.g. /dev/ttyUSB0 on linux or COM0 on windows)
        Settings settings;//!< The serial port settings (baudrate, databits, parity, etc.)
    };

} //namespace Clipped
