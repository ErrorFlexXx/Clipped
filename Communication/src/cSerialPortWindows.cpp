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

#ifdef WINDOWS //This implementation shall only be compiled on windows systems.

#include "cSerialPortWindows.h"
#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cTime.h>
#include <ClippedEnvironment/cSystem.h>

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
           FILE_FLAG_OVERLAPPED,
           nullptr
           );
    isOpen = true;
    if(handle == INVALID_HANDLE_VALUE)
    {
        LogError() << "Can't open port: " << interfaceName << " because: " << System::getSystemErrorText();
    }
    return config();
}

bool SerialPort::config()
{
    // Do some basic settings
    DCB serialParams = {};
    serialParams.DCBlength = sizeof(serialParams);

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
        break;
    }


    if(!SetCommState(handle, &serialParams))
    {
        LogError() << "Serial port setup failed!";
        return false;
    }
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
    DWORD errors;
    COMSTAT stat;
    if(!ClearCommError(handle, &errors, &stat))
    {
        LogError() << "Cannot get port status.";
        return false;
    }
    availableBytes = static_cast<size_t>(stat.cbInQue);
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
    if(!WriteFile(handle, data.data(), static_cast<DWORD>(data.size()), &bytesWritten, nullptr))
        return false;
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
        if(!ReadFile(handle, &buffer, 1, &bytesRead, nullptr))
            return false;
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
    isOpen = false;
    return true;
}

#endif //WINDOWS
