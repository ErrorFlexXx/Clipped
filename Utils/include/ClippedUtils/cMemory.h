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
