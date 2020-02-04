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

#include "cSystem.h"
#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cOsDetect.h>
#include <thread>
#include <vector>

#ifdef LINUX
#include <stdlib.h>
#include <unistd.h>
#include <cstring> //strerror
#elif defined(WINDOWS)
#include <windows.h>
#include <psapi.h>
#endif

using namespace Clipped;

#ifdef LINUX
Path System::getExecutableFilePath()
{
    std::vector<char> buff;
    ssize_t result;
    do
    {
        buff.resize(buff.size() + 100, 0);
        result = ::readlink("/proc/self/exe", buff.data(), buff.size());
    } while (result < 0 || static_cast<size_t>(result) == buff.size());
    return buff.data();
}
#elif defined(WINDOWS)
Path System::getExecutableFilePath()
{
    std::vector<char> buff;
    DWORD result;
    do
    {
        buff.resize(buff.size() + 100);
        result = GetModuleFileNameA(nullptr, buff.data(), static_cast<DWORD>(buff.size()));
    } while (result == 0 || result == buff.size());
    return buff.data();
}
#endif

Path System::getExecutablePath()
{
    Path filepath = getExecutableFilePath();
    return Path(filepath.getDirectory());
}

#ifdef LINUX
String System::getEnvVar(const String &name) { return secure_getenv(name.c_str()); }
#elif defined(WINDOWS)
String System::getEnvVar(const String& name)
{
    char* buffer = nullptr;
    size_t length = 0;
    errno_t err = _dupenv_s(&buffer, &length, name.c_str());
    if (err) return "";
    String result = buffer;
    free(buffer);
    return result;
}
#endif

#ifdef LINUX
void System::setEnvVar(const String &name, const String &value)
{
    setenv(name.c_str(), value.c_str(), true);
}
#elif defined(WINDOWS)
void System::setEnvVar(const String& name, const String& value)
{
    _putenv_s(name.c_str(), value.c_str());
}
#endif

void System::appendEnvVar(const String &name, const String &value)
{
    String append = getEnvVar(name) + String(";") + value;
    setEnvVar(name, append);
}

#ifdef LINUX
MemorySize System::getTotalSystemMemory()
{
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
}
#elif defined(WINDOWS)
MemorySize System::getTotalSystemMemory()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    return status.ullTotalPhys;
}
#endif

#ifdef LINUX
MemorySize System::getCurrentProcessSystemMemory()
{
    long rss = 0L;
    FILE* fp = NULL;
    if ( (fp = fopen( "/proc/self/statm", "r" )) == NULL )
        return (size_t)0L;      /* Can't open? */
    if ( fscanf( fp, "%*s%ld", &rss ) != 1 )
    {
        fclose( fp );
        return (size_t)0L;      /* Can't read? */
    }
    fclose( fp );
    return (size_t)rss * (size_t)sysconf( _SC_PAGESIZE);
}
#elif defined(WINDOWS)
MemorySize System::getCurrentProcessSystemMemory()
{
    PROCESS_MEMORY_COUNTERS info;
    GetProcessMemoryInfo( GetCurrentProcess(), &info, sizeof(info) );
    return (size_t)info.WorkingSetSize;
}
#endif

int System::getTotalCpus() { return std::thread::hardware_concurrency(); }

void System::mSleep(size_t milliseconds)
{
#ifdef LINUX
    usleep(milliseconds * 1000);
#elif defined(WINDOWS)
    Sleep(static_cast<DWORD>(milliseconds));
#endif
}

String System::getSystemErrorText()
{
#ifdef LINUX
    return strerror(errno);
#elif defined(WINDOWS)
        DWORD errorMessageID = ::GetLastError();
        if(errorMessageID == 0)
            return ""; //No error
        LPSTR messageBuffer = nullptr; //Let Format message allocate memory.
        size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                     nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&messageBuffer), 0, nullptr);
        String message(messageBuffer, size);
        LocalFree(messageBuffer); //Free the buffer.
        return message;
#endif
}
