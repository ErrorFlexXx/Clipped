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

#include <ClippedUtils/cOsDetect.h>

#ifdef LINUX //This implementation shall only be compiled on linux systems.

#include "cSerialPortLinux.h"
#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cTime.h>

#include <string.h> //strerror
#include <sys/ioctl.h>
#include <sys/signal.h>

using namespace Clipped;

SerialPort::SerialPort(const String& interfaceName, const Settings& settings)
    : ISerialPort(interfaceName, settings)
{
    isOpen = false;
}

SerialPort::~SerialPort()
{
    close();
}

bool SerialPort::open(const IODevice::OpenMode& mode)
{
    this->mode = mode;
    int flags = O_NOCTTY | O_NONBLOCK; //Unconfigurable settings.

    switch(mode) //Attach access mode
    {
        case IODevice::OpenMode::Read: flags |= O_RDONLY; break;
        case IODevice::OpenMode::Write: flags |= O_WRONLY; break;
        case IODevice::OpenMode::ReadWrite: flags |= O_RDWR; break;
    }

    handle = ::open(interfaceName.c_str(), flags);
    if(handle < 0)
    {
        LogError() << "Can't open port: " << interfaceName << " because: " << strerror(errno);
        return false;
    }
    isOpen = true;
    return config();
}

bool SerialPort::config()
{
    struct termios tty;
    memset (&tty, 0, sizeof tty);

    if(tcgetattr(handle, &tty ))
    {
        LogError() << "Configure failed (tcgetattr: " << strerror(errno) << ")";
        close();
        return false;
    }

    //Setup of Baudrate:
    switch(settings.baud)
    {
        case Baud50: cfsetospeed (&tty, B50); cfsetispeed (&tty, B50); break;
        case Baud75: cfsetospeed (&tty, B75); cfsetispeed (&tty, B75); break;
        case Baud110: cfsetospeed (&tty, B110); cfsetispeed (&tty, B110); break;
        case Baud134: cfsetospeed (&tty, B134); cfsetispeed (&tty, B134); break;
        case Baud150: cfsetospeed (&tty, B150); cfsetispeed (&tty, B150); break;
        case Baud200: cfsetospeed (&tty, B200); cfsetispeed (&tty, B200); break;
        case Baud300: cfsetospeed (&tty, B300); cfsetispeed (&tty, B300); break;
        case Baud600: cfsetospeed (&tty, B600); cfsetispeed (&tty, B600); break;
        case Baud1200: cfsetospeed (&tty, B1200); cfsetispeed (&tty, B1200); break;
        case Baud1800: cfsetospeed (&tty, B1800); cfsetispeed (&tty, B1800); break;
        case Baud2400: cfsetospeed (&tty, B2400); cfsetispeed (&tty, B2400); break;
        case Baud4800: cfsetospeed (&tty, B4800); cfsetispeed (&tty, B4800); break;
        case Baud9600: cfsetospeed (&tty, B9600); cfsetispeed (&tty, B9600); break;
        case Baud19200: cfsetospeed (&tty, B19200); cfsetispeed (&tty, B19200); break;
        case Baud38400: cfsetospeed (&tty, B38400); cfsetispeed (&tty, B38400); break;
        case Baud57600: cfsetospeed (&tty, B57600); cfsetispeed (&tty, B57600); break;
        case Baud115200: cfsetospeed (&tty, B115200); cfsetispeed (&tty, B115200); break;
        case Baud230400: cfsetospeed (&tty, B230400); cfsetispeed (&tty, B230400); break;
        default: LogError() << "Selected baud rate not supported!"; return false; break;
    }

    //Setup of DataBits:
    tty.c_cflag &=  ~CSIZE;
    switch(settings.dataBits)
    {
        case DataBits5:
        {
            tty.c_cflag |= CS5;
            break;
        }
        case DataBits6:
        {
            tty.c_cflag |= CS6;
            break;
        }
        case DataBits7:
        {
            tty.c_cflag |= CS7;
            break;
        }
        case DataBits8:
        {
            tty.c_cflag |= CS8;
            break;
        }
    }

    //Setup of StopBits:
    switch(settings.stopBits)
    {
        case StopBits1:
        {
            tty.c_cflag &= ~CSTOPB;
            break;
        }
        case StopBits2:
        {
            tty.c_cflag |= CSTOPB;
            break;
        }
    }

    switch(settings.parity)
    {
        case ParityNone: tty.c_cflag &= ~PARENB; break;
        case ParityEven: tty.c_cflag |= PARENB; tty.c_cflag &= ~PARODD; break;
        case ParityOdd: tty.c_cflag |= PARENB; tty.c_cflag |= PARODD; break;
    }

    switch(settings.flowControl)
    {
        case FlowControlNone: tty.c_cflag &= ~CRTSCTS; tty.c_iflag &= ~(IXON | IXOFF | IXANY); break;
    }

    /* Unconfigurable settings */
    tty.c_lflag &= ~(ECHO|ECHONL|ICANON|ISIG|IEXTEN);
    tty.c_cflag |=  CREAD | CLOCAL; // turn on READ & ignore ctrl lines

    tcflush(handle, TCIFLUSH);

    if (tcsetattr(handle, TCSANOW, &tty ))
    {
        LogError() << "Configure failed (tcsetattr: " << strerror(errno) << ")";
        close();
        return false;
    }
    tcflush(handle, TCIOFLUSH);
    LogDebug() << "Configuring serial port done!";
    return true;
}

bool SerialPort::config(const Settings& settings)
{
    this->settings = settings;
    return config();
}

bool SerialPort::availableBytes(size_t& availableBytes) const
{
    int nread;

    if(-1 == ::ioctl(handle, TIOCINQ, &nread))
    {
        LogError() << interfaceName << " Error: " << errno << ": " << strerror(errno);
        return false;
    }
    availableBytes = static_cast<size_t>(nread);
    return true;
}

bool SerialPort::write(const String& data)
{
    LogDebug() << "Serial write: " << String(data).replace("\n", "");
    int ret = ::write(handle, data.c_str(), data.length());
    tcflush(handle, TCOFLUSH);
    return 0 < ret;
}

bool SerialPort::write(const std::vector<char>& data)
{
    LogDebug() << "Serial write: " << String(data).replace("\n", "");
    int ret = ::write(handle, data.data(), data.size());
    tcflush(handle, TCOFLUSH);
    return 0 < ret;
}

bool SerialPort::readAll(std::vector<char>& data)
{
    size_t available;
    if(availableBytes(available) && 0 < available)
    {
        bool ret = read(data, available);
        LogDebug() << "Read " << String(data);
        return ret;
    }
    return false;
}

bool SerialPort::read(std::vector<char>& data, size_t count)
{
    char buff[1] = {};
    int result;

    for(size_t i = 0; i < count; i++)
    {
        result = ::read(handle, buff, 1);
        if(0 < result)
            data.push_back(buff[0]);
        else if(result < 0)
        {
            LogDebug() << "Serial Port read failed: " << errno << ": " << strerror(errno);
            return false;
        }
        else
            return false;
    }
    return true;
}

bool SerialPort::sendBreak(size_t milliseconds)
{
    if(-1 == tcsendbreak(handle, milliseconds))
    {
        LogError() << interfaceName << ": Error: " << errno << ": " << strerror(errno);
        return false;
    }
    return true;
}

bool SerialPort::close()
{
    if(handle > 0) //If there
        if(::close(handle))
        {
            LogDebug() << "Serial port close error " << errno << ": " << strerror(errno);
            return false;
        }
    handle = 0;
    isOpen = false;
    return true;
}

#endif //LINUX
