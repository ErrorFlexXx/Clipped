#pragma once

#include <ClippedUtils/cString.h>

namespace Clipped
{
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

    enum DataBits
    {
        DataBits5,
        DataBits6,
        DataBits7,
        DataBits8
    };

    enum StartBits
    {
        StartBits1
    };

    enum StopBits
    {
        StopBits1,
        StopBits2
    };

    enum Parity
    {
        ParityNone,
        ParityEven,
        ParityOdd,
    };

    enum FlowControl
    {
        FlowControlNone
    };

    /**
     * @brief The SerialPortInterface parent class of serial ports. (OS independent).
     */
    class SerialPortInterface
    {
    public:
        struct Settings
        {
            Settings(Baudrate baud, DataBits dataBits, StartBits startBits, StopBits stopBits, Parity parity, FlowControl flowControl)
                : baud(baud)
                , dataBits(dataBits)
                , startBits(startBits)
                , stopBits(stopBits)
                , parity(parity)
                , flowControl(flowControl)
            {}

            Baudrate baud;
            DataBits dataBits;
            StartBits startBits;
            StopBits stopBits;
            Parity parity;
            FlowControl flowControl;
        };

        SerialPortInterface(const String& interface, const Settings& settings)
            : interface(interface)
            , settings(settings)
        {}

    protected:
        String interface; //!< Name of the serial interface (e.g. /dev/ttyUSB0 on linux or COM0 on windows)
        Settings settings;//!< The serial port settings (baudrate, databits, parity, etc.)
    };
}
