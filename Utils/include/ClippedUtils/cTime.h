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

#pragma once

#include <stdint.h>
#include <time.h>
#include "cOsDetect.h"
#include "cString.h"

#ifdef WINDOWS
#include <windows.h>
#elif defined(LINUX)
#include <sys/time.h>
#endif

#ifdef _MSC_VER
#pragma warning(disable : 4996)  // Disable 4996 __timezone may be unsafe. Suggested alternative
                                 // _get_timezone is broken, thanks microsoft.
#define timezone _timezone
#endif

/** \file uTime.h contains implementations regarding date-times.
 * Working with and converting Time of different formats.
 *
 * Contents:
 * zDATE - ZenGin DateTime structure.
 * MSDOSTime32 - 32 Bit MSDOS DateTime structure (used in e.g. filesystem API functions).
 * Time - UTC Time based on time_t with constructors and converters in and from other time formats.
 * Stopwatch - Measuring times in us resolutions.
 */

namespace Clipped
{
    /**
     * @brief The zDATE is a custom date time structure used by Zengin.
     */
#pragma pack(push, 4)
    struct zDATE
    {
        uint32_t year;    //!< yyyy year
        uint16_t month;   //!< Month 1-12
        uint16_t day;     //!< Day
        uint16_t hour;    //!< Hour
        uint16_t minute;  //!< Minute
        uint16_t second;  //!< Second
    };
#pragma pack(pop)
    /**
     * @brief The MSDOSTime32 union data structure to represent a time on msdos with 32 bits.
     */
    union MSDOSTime32
    {
        uint32_t timestamp;

        /**
         * @brief The parts struct access to parts of informations in the timestamp.
         */
#pragma pack(push, 1)
        struct parts
        {
            unsigned second : 5;  //!< seconds / 2
            unsigned minute : 6;  //!< minutes
            unsigned hour : 5;    //!< hours
            unsigned day : 5;     //!< day
            unsigned month : 4;   //!< month (1 - 12)
            unsigned year : 7;    //!< year since 1980
        } parts;
#pragma pack(pop)
    };

    /**
     * @brief The Time class offers utils to work with and convert times.
     */
    class Time
    {
    public:
        /**
         * @brief Time constructs a time object, which contains the current time.
         */
        Time();

        /**
         * @brief Time constructs a time from a time_t
         * @param time time_t to create from.
         */
        Time(const time_t& time);

        /**
         * @brief Time constructs a time object, which contains the given zDATE datetime.
         * @param zDate zDATE input.
         */
        Time(const zDATE& zDate);

        /**
         * @brief Time constructs a time object, which contains the msdos given 32bit time.
         * @param msdosTime32Bit MSDos Timestamp
         */
        Time(const MSDOSTime32& msdosTime);

        /**
         * @brief operator zDATE casts this time object to a zDATE.
         */
        operator zDATE() const;

        /**
         * @brief operator MSDOSTime32 casts this time object to a MSDOSTime32.
         */
        operator MSDOSTime32() const;

        inline Time operator+(const Time& rhs) const
        {
            return Time(this->time + rhs.time);
        }  ///< Operator to add a time.

        inline Time operator+(const long seconds) const
        {
            return Time(time + seconds);
        }  ///< Operator to add seconds.

        inline Time operator-(const Time& rhs) const
        {
            return Time(this->time - rhs.time);
        }  ///< Operator to subtract a time.

        inline Time operator-(const long seconds) const
        {
            return Time(time - seconds);
        }  ///< Operator to subtract seconds.

        inline bool operator<(const Time& rhs) const
        {
            return (this->time < rhs.time);
        }  ///< Operator to compare times.

        inline bool operator<=(const Time& rhs) const
        {
            return (this->time <= rhs.time);
        }  ///< Operator to compare times.

        inline bool operator>(const Time& rhs) const
        {
            return (this->time > rhs.time);
        }  ///< Operator to compare times.

        inline bool operator>=(const Time& rhs) const
        {
            return (this->time >= rhs.time);
        }  ///< Operator to compare times.

        inline bool operator==(const Time& rhs) const
        {
            return (this->time == rhs.time);
        }  ///< Operator to compare times.

        inline bool operator!=(const Time& rhs) const
        {
            return (this->time != rhs.time);
        }  ///< Operator to compare times.

        /**
         * @brief toString creates a formatted string representing this time.
         * @param format strftime format string.
         * @return the string representation of this time.
         */
        String toString(const String& format = "%c", bool utc = false) const;

        /**
         * @brief seconds returns the complete time as an amount of seconds.
         * @return the amount of seconds this time represents.
         */
        inline long long seconds() const { return static_cast<long long>(time); }

        /**
         * @brief minutes returns the complete time as an amount of minutes.
         * @return the amount of minutes this time represents.
         */
        inline long long minutes() const { return static_cast<long long>(time / 60); }

        /**
         * @brief hours returns the complete time as an amount of hours.
         * @return  the amount of hours this time represents.
         */
        inline long long hours() const { return static_cast<long long>(time / (60 * 60)); }

    private:
        time_t time;  //!< The stored time

        void buildTimeInfo(const time_t* time, tm* timeinfo, bool utc) const;
    };

    /**
     * @brief The Stopwatch class may be used to measure a time in microsecond resultion.
     */
    class Stopwatch
    {
    public:
        /**
         * @brief Stopwatch creates a stopwatch
         * @param createStarted wether the stopwatch shall run instantly after creation.
         */
        Stopwatch(bool createStarted = false)
        {
            reset();
            if (createStarted) start();
        }

        /**
         * @brief start starts the time measurement.
         */
        void start() { before = currentMicros(); }

        /**
         * @brief stop stops the current measureing, adds the elapsed time to the counter.
         */
        void stop()
        {
            if (before != 0ULL)
            {
                microCounter += currentMicros() - before;
                before = 0ULL;
            }
        }

        /**
         * @brief reset resets the current counter.
         */
        void reset()
        {
            microCounter = 0ULL;
            before = 0ULL;
        }

        /**
         * @brief millis returns the counted time in milliseconds.
         * @return counted time in milliseconds.
         */
        unsigned long long millis() { if(isRunning()) { stop(); start(); } return microCounter / 1000ULL; }

        /**
         * @brief micros returns the counted time in microseconds.
         * @return counted time in microseconds.
         */
        unsigned long long micros() { if(isRunning()) { stop(); start(); } return microCounter; }

        /**
         * @brief toString creates a string representing the result of the current microCounter.
         * @return
         */
        String toString()
        {
            if (microCounter > 1000)
            {
                float millis = microCounter / 1000.f;
                return String(millis) + " ms";
            }
            return String(microCounter) + " us";
        }

        /**
         * @brief isRunning returns true if the stopwatch is currently running.
         */
        bool isRunning() const { return before != 0ULL; }

    private:
        unsigned long long microCounter;  //!< Counter of elapsed microseconds.
        unsigned long long before;        //!< Elapsed micros since start().

        /**
         * @brief currentMicros returns some sort of timestamp in microseconds (system dependent)
         *   Only reliable usable if used for delta calulations.
         * @return microsecond timestamp.
         */
        unsigned long long currentMicros()
        {
#ifdef LINUX
            struct timeval te;
            gettimeofday(&te, nullptr);
            return te.tv_sec * 1000000LL + te.tv_usec;
#elif defined(WINDOWS)
            static LARGE_INTEGER frequency;
            static BOOL inited = QueryPerformanceFrequency(&frequency);
            (void)inited;  // Assuming it wont fail.
            LARGE_INTEGER largeInt;
            QueryPerformanceCounter(&largeInt);
            return 1000000ULL * largeInt.QuadPart / frequency.QuadPart;
#endif
        }
    };

    /**
     * @brief The Scopewatch implements a performance watch to measure time in a scope.
     *   The result is given out with a name if the Scopewatch get's destroyed.
     */
    class Scopewatch : public Stopwatch
    {
    public:
        Scopewatch(const String& name) : Stopwatch(true), name(name) {}

        /**
         * @brief ~Scopewatch stops the time measurement and outputs the results.
         */
        ~Scopewatch();

    private:
        String name;  //!< Name of this scope watch.
    };
}  // namespace Clipped
