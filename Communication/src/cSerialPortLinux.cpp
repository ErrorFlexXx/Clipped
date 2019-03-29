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

#include <ClippedUtils/cOsDetect.h>

#ifdef LINUX //This implementation shall only be compiled on linux systems.

#include "cSerialPortLinux.h"
#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cTime.h>

#include <string.h> //strerror
#include <sys/ioctl.h>
#include <sys/signal.h>

using namespace Clipped;

SerialPort::SerialPort(const String& interface, const Settings& settings)
    : ISerialPort(interface, settings)
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

    handle = ::open(interface.c_str(), flags);
    if(handle < 0)
    {
        LogError() << "Can't open port: " << interface << " because: " << strerror(errno);
        return false;
    }
    isOpen = true;
    config();
    //LogDebug() << "Serial port: " << interface.c_str() << " opened!";
    return true;
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
        LogError() << interface << " Error: " << errno << ": " << strerror(errno);
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
        LogError() << interface << ": Error: " << errno << ": " << strerror(errno);
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
