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

using namespace Clipped;

String getLastErrorAsString()
{
    DWORD errorMessageId = ::GetLastError();
    if(errorMessageId == 0)
        return "";

    LPSTR messageBuffer = nullptr;
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 NULL, errorMessageId, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
    String message(messageBuffer, size);
    LocalFree(messageBuffer);
    return message;
}

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
    DWORD flags = 0;

    switch(mode) //Attach access mode
    {
        case IODevice::OpenMode::Read: flags |= GENERIC_READ; break;
        case IODevice::OpenMode::Write: flags |= GENERIC_WRITE; break;
        case IODevice::OpenMode::ReadWrite: flags |= GENERIC_READ | GENERIC_WRITE; break;
    }

    handle = CreateFile(interfaceName.c_str(),
                        flags,
                        0,
                        0,
                        OPEN_EXISTING,
                        FILE_FLAG_OVERLAPPED,
                        0);
    if(handle == INVALID_HANDLE_VALUE)
    {
        LogError() << "Can't open port: " << interfaceName << " because: " << getLastErrorAsString();
        return false;
    }
    isOpen = true;
    return config();
}

bool SerialPort::config()
{
    //Baud rate setup:
    DCB dcb;

    FillMemory(&dcb, sizeof(dcb), 0);
    if (!GetCommState(handle, &dcb)) //Get current DCB
        return false; // Error in GetCommState

    //Setup of Baudrate:
    dcb.BaudRate = (DWORD) settings.baud;
    dcb.ByteSize = settings.dataBits;

    switch(settings.parity)
    {
        case ParityNone: dcb.Parity = NOPARITY; break;
        case ParityEven: dcb.Parity = EVENPARITY; break;
        case ParityOdd: dcb.Parity = ODDPARITY; break;
    }

    switch(settings.stopBits)
    {
        case StopBits1: dcb.StopBits = ONESTOPBIT; break;
        case StopBits2: dcb.StopBits = TWOSTOPBITS; break;
    }

    switch(settings.flowControl)
    {
        case FlowControlNone:
        {
            dcb.fOutX = false;
            dcb.fInX = false;
            dcb.fDtrControl = DTR_CONTROL_DISABLE;
            break;
        }
    }

    if (!SetCommState(handle, &dcb))
    {
       LogError() << "Com port setup failed: " << getLastErrorAsString();
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
        LogError() << "Failed: " << getLastErrorAsString();
        return false;
    }

    if(0 <= stat.cbInQue)
    {
        availableBytes = static_cast<size_t>(stat.cbInQue);
        return true;
    }
    return false;
}

bool SerialPort::write(const String& data)
{
    LogDebug() << "Serial write: " << String(data).replace("\n", "");
    DWORD bytesWritten;

    return WriteFile(handle, data.c_str(), data.length(), &bytesWritten, NULL);
}

bool SerialPort::write(const std::vector<char>& data)
{
    LogDebug() << "Serial write: " << String(data).replace("\n", "");
    DWORD bytesWritten;

    return WriteFile(handle, data.data(), data.size(), &bytesWritten, NULL);
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
        DWORD bytesRead;
        result = ReadFile(handle, buff, 1, &bytesRead, NULL);
        if(result && bytesRead)
            data.push_back(buff[0]);
        else if(!result)
        {
            LogDebug() << "Serial Port read failed: " << getLastErrorAsString();
            return false;
        }
        else
            return false;
    }
    return true;
}

bool SerialPort::sendBreak(size_t milliseconds)
{
    if(!SetCommBreak(handle))
    {
        LogError() << "Break not supported: " << getLastErrorAsString();
        return false;
    }
    Sleep(milliseconds);
    ClearCommBreak(handle);
    return true;
}

bool SerialPort::close()
{
    if(handle != INVALID_HANDLE_VALUE) //If there
        CloseHandle(handle);

    handle = 0;
    isOpen = false;
    return true;
}

#endif //WINDOWS
