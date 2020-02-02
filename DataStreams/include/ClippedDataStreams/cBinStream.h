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
#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cString.h>

const Clipped::String UNEXPECTED_END_TEXT = "Unexpected end of data!";
#define UNEXPECTED_END() LogWarn() << UNEXPECTED_END_TEXT, false;

namespace Clipped
{
    /**
     * @brief The BinStream class implements a binary stream.
     */
    class BinStream
    {
    public:
        /**
         * @brief BinStream creates a stream from a data container..
         * @param data container to work with.
         */
        BinStream(std::vector<char>& data);

        /**
         * @brief read reads data into a variable.
         * @return true if read successfully, false if out of data.
         */
        template <typename T>
        bool read(T& value)
        {
            if (index + sizeof(T) <= data.size())  // Enaugh data available ?
            {
                value = reinterpret_cast<T*>(data.data() + index)[0];
                index += sizeof(T);
                return true;
            }
            LogError() << "Stream is empty! Nothing to read.";
            return false;
        }

        /**
         * @brief read reads data from specified positioninto a variable. Does not touch the read
         * index.
         * @return true if read successfully, false if out of data.
         */
        template <typename T>
        bool readAt(T& value, size_t pos)
        {
            if (pos + sizeof(T) <= data.size())  // Enaugh data available ?
            {
                value = reinterpret_cast<T*>(data.data() + pos)[0];
                return true;
            }
            LogError() << "Stream is empty! Nothing to read.";
            return false;
        }

        /**
         * @brief readString reads a zero/CR/LF terminated string from the stream.
         * @param value to read the string into.
         * @return true, if successfully read a string. false if out of data.
         */
        bool readString(String& value);

        /**
         * @brief readZString reads a string, which is zero terminated from the stream.
         * @param value to read the string into.
         * @return true if successfull, false otherwise (Out of data).
         */
        bool readZString(String& value);

        /**
         * @brief empty returns wether there still is data to be read.
         * @return true if data available, false otherwise.
         */
        inline bool empty() const { return !(index < data.size()); }

        /**
         * @brief seek moves the current index pointer offset positions forward.
         * @param offset to move the cursor.
         * @return true if data avail. or we hit the end, false if we would seek out of the data
         * container.
         */
        inline bool seek(const uint32_t offset)
        {
            if (index + offset > data.size())
            {
                LogError() << "Stream overrun! Out of data!";
                return false;
            }
            index += offset;
            return true;
        }

        /**
         * @brief position sets the current position in the stream.
         * @param pos to set.
         * @return true if successfully set, false otherwise.
         */
        inline bool position(const size_t& pos)
        {
            if (pos < data.size())
            {
                index = pos;
                return true;
            }
            return false;
        }

        /**
         * @brief position returns the current position inside the stream.
         * @return the position.
         */
        inline size_t position() const { return index; }

        /**
         * @brief getData returns a reference to the data container.
         * @return reference to data.
         */
        std::vector<char>& getData() { return data; }

    private:
        size_t index;             //!< Current index in data container.
        std::vector<char>& data;  //!< Data container used for this stream.
    };
}  // namespace Clipped
