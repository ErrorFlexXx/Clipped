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

#include "cSystem.h"
#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cOsDetect.h>
#include <thread>
#include <vector>

#ifdef LINUX
#include <stdlib.h>
#include <unistd.h>
#elif defined(WINDOWS)
#include <Windows.h>
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

size_t System::getTotalCpus() { return std::thread::hardware_concurrency(); }

void System::mSleep(size_t milliseconds)
{
#ifdef LINUX
    usleep(milliseconds * 1000);
#elif defined(WINDOWS)
    Sleep(static_cast<DWORD>(milliseconds));
#endif
}
