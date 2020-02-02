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
            FlowControlNone,
            FlowControlHardwareHandshake
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
    }; //class ISerialPort

} //namespace Clipped
