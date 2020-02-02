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

#include "cString.h"

#define KILO (1024.f)
#define MEGA (1024.f * 1024.f)
#define GIGA (1024.f * 1024.f * 1024.f)
#define TERA (1024.f * 1024.f * 1024.f * 1024.f)

namespace Clipped
{
    /**
     * @brief The MemorySize struct represents a size of a file.
     */
    struct MemorySize
    {
        MemorySize(unsigned long long value) : bytes(value) {}

        unsigned long long bytes;  //!< Bytes this struct represents.

        operator size_t() const { return static_cast<size_t>(bytes); }

        MemorySize& operator+=(const size_t rhs)
        {
            bytes += rhs;
            return *this;
        }

        /**
         * @brief toString creates a String representation of this memory size type.
         *   Sizes are being casted for better human readability: Bytes, kB, MB, GB, TB.
         * @return a String.
         */
        String toString(const size_t precision = 2) const
        {
            using namespace std;

            String output;  // Output String representing this MemorySize.

            if (bytes > TERA)
            {
                return String(static_cast<float>(bytes) / TERA, precision) + String(" TB");
            }
            else if (bytes > GIGA)
            {
                return String(static_cast<float>(bytes) / GIGA, precision) + String(" GB");
            }
            else if (bytes > MEGA)
            {
                return String(static_cast<float>(bytes) / MEGA, precision) + String(" MB");
            }
            else if (bytes > KILO)
            {
                return String(static_cast<float>(bytes) / KILO, precision) + String(" kB");
            }
            else
            {
                return String(bytes) + String(" Bytes);");
            }
        }
    };

}  // namespace Clipped
