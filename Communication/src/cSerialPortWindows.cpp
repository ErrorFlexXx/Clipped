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

#ifdef WINDOWS //This implementation shall only be compiled on windows systems.

#include "cSerialPortWindows.h"
#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cTime.h>
#include <ClippedEnvironment/cSystem.h>

using namespace Clipped;

SerialPort::SerialPort(const String& interfaceName, const Settings& settings)
    : ISerialPort(interfaceName, settings)
{
    Logger() << Logger::MessageType::Debug;
    isOpen = false;
}

SerialPort::~SerialPort()
{
    close();
}

bool SerialPort::open(const IODevice::OpenMode& mode)
{
    DWORD access = 0;
    switch(mode)
    {
    case IODevice::OpenMode::Read:
    {
        access |= GENERIC_READ;
        break;
    }
    case IODevice::OpenMode::Write:
    {
        access |= GENERIC_WRITE;
        break;
    }
    case IODevice::OpenMode::ReadWrite:
    {
        access |= GENERIC_READ | GENERIC_WRITE;
        break;
    }
    }

    handle = ::CreateFile( interfaceName.c_str(),
           access,
           0,
           nullptr,
           OPEN_EXISTING,
           FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
           nullptr);

    if(handle == INVALID_HANDLE_VALUE)
    {
        LogError() << "Can't open port: " << interfaceName << " because: " << System::getSystemErrorText();
        return false;
    }
    isOpen = true;
    return config();
}

bool SerialPort::config()
{
    // Do some basic settings
    DCB serialParams = {};
    serialParams.DCBlength = sizeof(serialParams);

    memset( &overlappedRead, 0, sizeof( OVERLAPPED ) );
    memset( &overlappedWrite, 0, sizeof( OVERLAPPED ) );

    overlappedRead.hEvent = CreateEvent( nullptr, TRUE, FALSE, nullptr);
    overlappedWrite.hEvent = CreateEvent( nullptr, TRUE, FALSE, nullptr );

    GetCommState(handle, &serialParams);

    switch(settings.baud)
    {
        case Baud50: serialParams.BaudRate = 50; break;
        case Baud75: serialParams.BaudRate = 75; break;
        case Baud110: serialParams.BaudRate = CBR_110; break;
        case Baud134: serialParams.BaudRate = 134; break;
        case Baud150: serialParams.BaudRate = 150; break;
        case Baud200: serialParams.BaudRate = 200; break;
        case Baud300: serialParams.BaudRate = CBR_300; break;
        case Baud600: serialParams.BaudRate = CBR_600; break;
        case Baud1200: serialParams.BaudRate = CBR_1200; break;
        case Baud1800: serialParams.BaudRate = 1800; break;
        case Baud2400: serialParams.BaudRate = CBR_2400; break;
        case Baud4800: serialParams.BaudRate = CBR_4800; break;
        case Baud9600: serialParams.BaudRate = CBR_9600; break;
        case Baud19200: serialParams.BaudRate = CBR_19200; break;
        case Baud38400: serialParams.BaudRate = CBR_38400; break;
        case Baud57600: serialParams.BaudRate = CBR_57600; break;
        case Baud115200: serialParams.BaudRate = CBR_115200; break;
        case Baud230400: serialParams.BaudRate = 230400; break;
    }

    switch (settings.dataBits)
    {
        case DataBits5: serialParams.ByteSize = 5; break;
        case DataBits6: serialParams.ByteSize = 6; break;
        case DataBits7: serialParams.ByteSize = 7; break;
        case DataBits8: serialParams.ByteSize = 8; break;
    }

    switch (settings.stopBits)
    {
        case StopBits1: serialParams.StopBits = ONESTOPBIT; break;
        case StopBits2: serialParams.StopBits = TWOSTOPBITS; break;
    }

    switch (settings.parity)
    {
        case ParityNone: serialParams.Parity = NOPARITY; break;
        case ParityEven: serialParams.Parity = EVENPARITY; break;
        case ParityOdd: serialParams.Parity = ODDPARITY; break;
    }

    switch(settings.flowControl)
    {
    case FlowControlNone:
        serialParams.fDtrControl = DTR_CONTROL_DISABLE;
        serialParams.fRtsControl = RTS_CONTROL_DISABLE;
        serialParams.fInX = 0;
        serialParams.fOutX = 0;
        break;
    case FlowControlHardwareHandshake:
        serialParams.fDtrControl = DTR_CONTROL_ENABLE;
        serialParams.fRtsControl = RTS_CONTROL_ENABLE;
        serialParams.fInX = 0;
        serialParams.fOutX = 0;
        break;
    }

    if (!::SetCommMask(handle, EV_RXCHAR))
    {
        LogError() << "Faild enable read: " << System::getSystemErrorText();
        return false;
    }

    COMMTIMEOUTS timeouts;
    timeouts.ReadIntervalTimeout            = MAXDWORD;
    timeouts.ReadTotalTimeoutMultiplier		= 0;
    timeouts.ReadTotalTimeoutConstant       = 0;
    timeouts.WriteTotalTimeoutMultiplier    = 0;
    timeouts.WriteTotalTimeoutConstant      = 0;

    if (!::SetCommTimeouts(handle, &timeouts))
    {
        LogError() << "Faild config timeouts: " << System::getSystemErrorText();
        return false;
    }

    if(!SetCommState(handle, &serialParams))
    {
        LogError() << "Problem: " << System::getSystemErrorText();
        return false;
    }

    if(!SetupComm(handle, 10000, 10000))
    {
        LogError() << "Problem: " << System::getSystemErrorText();
        return false;
    }
    return true;
}

bool SerialPort::config(const Settings& settings)
{
    this->settings = settings;
    return config();
}

bool SerialPort::availableBytes(size_t& availableBytes) const
{
    DWORD errors;
    COMSTAT stat;
    if(!ClearCommError(handle, &errors, &stat))
    {
        LogError() << "Cannot get port status.";
        return false;
    }
    availableBytes = static_cast<size_t>(stat.cbInQue);
    if(availableBytes > 0) LogDebug() << "Some data available on port!";
    return true;
}

bool SerialPort::write(const String& data)
{
    return write(data.toVector());
}

bool SerialPort::write(const std::vector<char>& data)
{
    DWORD bytesWritten;
    LogDebug() << "Serial write: " << String(data).replace("\n", "");
    BOOL status;
    if((status = !WriteFile(handle, data.data(), static_cast<DWORD>(data.size()), &bytesWritten, &overlappedWrite)))
    {
        if(status == ERROR_IO_PENDING)
            WaitForSingleObject(overlappedWrite.hEvent, 100);
        return false;
    }
    return bytesWritten == data.size();
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
    char buffer;
    DWORD bytesRead;
    for(size_t i = 0; i < count; i++)
    {
        BOOL status;
        if(!(status = ReadFile(handle, &buffer, 1, &bytesRead, &overlappedRead)))
        {
            if(status == ERROR_IO_PENDING)
            {
                WaitForSingleObject(overlappedRead.hEvent, 100);
                return false;
            }
        }
        data.push_back(buffer);
    }
    return true;
}

bool SerialPort::sendBreak(size_t milliseconds)
{
    //Note: Seems to be the only possibility to get a break on windows.
    SetCommBreak(handle);
    System::mSleep(milliseconds);
    ClearCommBreak(handle);
    return true;
}

bool SerialPort::close()
{
    if(handle != INVALID_HANDLE_VALUE)
        if(!CloseHandle(handle))
        {
            LogDebug() << "Serial port close error: " << GetLastError << ": " << System::getSystemErrorText();
            return false;
        }
    if(overlappedRead.hEvent != nullptr) CloseHandle(overlappedRead.hEvent);
    if(overlappedWrite.hEvent != nullptr) CloseHandle(overlappedWrite.hEvent);
    isOpen = false;
    handle = INVALID_HANDLE_VALUE;
    return true;
}

#endif //WINDOWS
