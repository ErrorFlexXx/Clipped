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
#include <vector>

namespace Clipped
{
    template <class T>
    class CRC
    {
    public:
        CRC(const T generator, const T init = 0, const T finalXOR = 0, const bool reflectInput = false, const bool reflectResult = false);

        T encrypt(const uint8_t* data, size_t length) const;

        T encrypt(const std::vector<uint8_t>& data) const;

    private:
        T init;                     //!< Init value for calculation.
        bool reflectInput;          //!< Flag stating, if the input bits shall be "bitmirrored"
        bool reflectResult;         //!< Flag stating, if the result shall be "bitmirrored"
        T finalXOR;                 //!< Final XOR operation.
        T generator;                //!< Generator polynom used for calculation.
        std::vector<T> lookupTable; //!< Lookup table with precalculations.

        /**
         * @brief calculateTable fills the lookup table with precalculated data as performance optimization.
         */
        void calculateTable();

    }; //class CRC

    using CRC8 = CRC<uint8_t>;
    using CRC16 = CRC<uint16_t>;
    using CRC32 = CRC<uint32_t>;
} //namespace Clipped
