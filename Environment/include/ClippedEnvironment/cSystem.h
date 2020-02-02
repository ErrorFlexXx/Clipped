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

#pragma once

#include <ClippedUtils/cMemory.h>
#include <ClippedUtils/cPath.h>

namespace Clipped
{
    /**
     * @brief The System class delivers features to get informations about the current system environment.
     */
    class System
    {
    public:
        /**
         * @brief executableFilePath returns the full path to the executable file.
         * @return full path to the executable including the executable file.
         */
        static Path getExecutableFilePath();

        /**
         * @brief executablePath returns the full path to the executable file.
         * @return full path to the executable excluding the executable file.
         */
        static Path getExecutablePath();

        /**
         * @brief getEnvVar reads an environment variable.
         * @param name name of the environment variable.
         * @return the value of the environment variable.
         */
        static String getEnvVar(const String& name);

        /**
         * @brief setEnvironmentVariable sets an environment variable.
         * @param name of the environment variable.
         * @param value of the environment variable.
         */
        static void setEnvVar(const String& name, const String& value);

        /**
         * @brief appendEnvironmentVariable appends a value to an existing environment variable.
         * @param name of the environment variable.
         * @param value of the environment variable.
         */
        static void appendEnvVar(const String& name, const String& value);

        /**
         * @brief getTotalSystemMemory returns the total amount of ram in this system.
         * @return the memory size.
         */
        static MemorySize getTotalSystemMemory();

        /**
         * @brief getCurrentProcessSystemMemory returns the current reserved ram of this process.
         * @return the current allocated process memory size.
         */
        static MemorySize getCurrentProcessSystemMemory();

        /**
         * @brief getTotalCpus returns the number of installed cpu cores in this system.
         * @return number of cpu cores.
         */
        static int getTotalCpus();

        /**
         * @brief msleep let's the current thread sleep for some time.
         * @param milliseconds to sleep.
         */
        static void mSleep(size_t milliseconds);

        /**
         * @brief GetSystemErrorText returns the current error text of the os api.
         * @return an error string.
         */
        static String getSystemErrorText();
    };

}  // namespace Clipped
