#include "cSerialPortLinux.h"
#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cTime.h>

#include <string.h> //strerror
#include <sys/ioctl.h>

#ifdef LINUX

using namespace Clipped;

SerialPort::SerialPort(const String& interface, const Settings& settings)
    : SerialPortInterface(interface, settings)
{}

bool SerialPort::open()
{
    handle = ::open(interface.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK);
    if(handle < 0)
    {
        LogError() << "Can't open port: " << interface << " because: " << strerror(errno);
        return false;
    }
    LogDebug() << "Serial port: " << interface.c_str() << " opened!";
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

int SerialPort::availableBytes() const
{
    int nread;

    if(-1 == ::ioctl(handle, TIOCINQ, &nread))
    {
        LogError() << interface << " Error: " << errno << ": " << strerror(errno);
        return -1;
    }

    return nread;
}

int SerialPort::writeLine(const String& data, const String& lineEnd)
{
    return write(data + lineEnd);
}

int SerialPort::write(const String& data)
{
    LogDebug() << "Serial write: " << data;
    int ret = ::write(handle, data.c_str(), data.length());
    tcflush(handle, TCOFLUSH);
    return ret;
}

String SerialPort::readLine(const String& lineEnd, size_t timeoutMS)
{
    String content;
    Stopwatch timeout(true);
    char buffer[1] = {};
    int readCount = 0;

    while(!content.endsWith(lineEnd))
    {
        if(0 < ::read(handle, buffer, sizeof(buffer)))
            content.append(buffer, 1);
        else //If no data available.
        {
            usleep(1000); //Wait a bit for incoming data.
            if(timeout.millis() > timeoutMS)
                return content; //Return what you have and stop here.
        }
    }
    return content;
}

String SerialPort::read()
{
    String content;
    char buffer[128] = {};
    int readCount = 0;
    while(0 < (readCount = ::read(handle, buffer, sizeof(buffer))))
        content.append(buffer, readCount);
    return content;
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

void SerialPort::close()
{
    if(handle >= 0)
        ::close(handle);
}

#endif
