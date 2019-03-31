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
         * @brief getTotalCpus returns the number of installed cpu cores in this system.
         * @return number of cpu cores.
         */
        static size_t getTotalCpus();

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
