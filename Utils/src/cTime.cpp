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

#include "cTime.h"
#include "cLogger.h"
#include <ClippedUtils/cOsDetect.h>
#ifdef WINDOWS
#include <iomanip>
#endif

using namespace Clipped;

Time::Time()
{
    ::time(&time);  // Current time
}

Time::Time(const time_t &time) : time(time) {}

Time::Time(const zDATE &zDate)
{
    tm timeinfo = {};
    timeinfo.tm_year = static_cast<int>(zDate.year - 1900);
    timeinfo.tm_mon = zDate.month - 1;
    timeinfo.tm_mday = zDate.day;
    timeinfo.tm_hour = zDate.hour;
    timeinfo.tm_min = zDate.minute;
    timeinfo.tm_sec = zDate.second;
    time = mktime(&timeinfo) - getUTCOffsetSeconds();  // Make time_t as utc.
}

Time::Time(const MSDOSTime32 &msdosTime)
{
    tm timeinfo = {};
    timeinfo.tm_year = msdosTime.parts.year + 1980 - 1900;
    timeinfo.tm_mon = msdosTime.parts.month - 1;
    timeinfo.tm_mday = msdosTime.parts.day;
    timeinfo.tm_hour = msdosTime.parts.hour;
    timeinfo.tm_min = msdosTime.parts.minute;
    timeinfo.tm_sec = msdosTime.parts.second << 1;
    time = mktime(&timeinfo) - timezone;  // Make time_t as utc.
}

Time::operator zDATE() const
{
    zDATE zDate;
    tm timeinfo = {};
    time_t localTime = time + getUTCOffsetSeconds();  // ctime expects time_t to be localtime...
    buildTimeInfo(&localTime, &timeinfo, false);

    zDate.year = static_cast<uint32_t>(timeinfo.tm_year + 1900);
    zDate.month = static_cast<uint16_t>(timeinfo.tm_mon + 1);
    zDate.day = static_cast<uint16_t>(timeinfo.tm_mday);
    zDate.hour = static_cast<uint16_t>(timeinfo.tm_hour);
    zDate.minute = static_cast<uint16_t>(timeinfo.tm_min);
    zDate.second = static_cast<uint16_t>(timeinfo.tm_sec);
    return zDate;
}

Time::operator MSDOSTime32() const
{
    MSDOSTime32 msdosTime;
    tm timeinfo = {};
    time_t localTime = time + getUTCOffsetSeconds();  // ctime expects time_t to be localtime...
    buildTimeInfo(&localTime, &timeinfo, false);

    msdosTime.parts.year = static_cast<unsigned>(timeinfo.tm_year - 1980 + 1900);
    msdosTime.parts.month = static_cast<unsigned>(timeinfo.tm_mon + 1);
    msdosTime.parts.day = static_cast<unsigned>(timeinfo.tm_mday);
    msdosTime.parts.hour = static_cast<unsigned>(timeinfo.tm_hour);
    msdosTime.parts.minute = static_cast<unsigned>(timeinfo.tm_min);
    msdosTime.parts.second = static_cast<unsigned>(timeinfo.tm_sec >> 1);
    return msdosTime;
}

String Time::toString(const String &format, bool utc) const
{
    char result[256] = {};
    tm timeinfo;
    buildTimeInfo(&time, &timeinfo, utc);
    if (strftime(result, sizeof(result), format.c_str(), &timeinfo) != 0)
    {
        return String(result);
    }
    else
    {
        LogWarn() << "strftime returned with an error. Conversion failed!";
    }
    return String("");
}

time_t Time::getUTCOffsetSeconds() const
{
#ifdef LINUX
    time_t t = ::time(nullptr);
    struct tm lt = {0};
    localtime_r(&t, &lt);
    return -lt.tm_gmtoff;
#else
    time_t t = std::time(nullptr);
    auto const tm = std::localtime(&t);
    StringStream os;
    os << std::put_time(tm, "%z");
    String s = os.str();
    int h = String(s.substr(0, 3)).toInt(0, 10);
    int m = String(s[0] + s.substr(3)).toInt(0, 10);
    return -(h * 3600 + m * 60);
#endif
}

void Time::buildTimeInfo(const time_t *time, tm *timeinfo, bool utc) const
{
    if (utc)
    {
#ifdef _MSC_VER
        gmtime_s(timeinfo, time);
#else
        gmtime_r(time, timeinfo);
#endif
    }
    else
    {
#ifdef _MSC_VER
        localtime_s(timeinfo, time);
#else
        localtime_r(time, timeinfo);
#endif
    }
}

Scopewatch::~Scopewatch()
{
    stop();
    LogDebug() << "Scopewatch \"" << name << "\" took: " << toString();
}
