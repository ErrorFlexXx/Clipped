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

#include <vector>
#include "cFile.h"
#include <ClippedUtils/cString.h>

namespace Clipped
{
    /**
     * @brief The BinFile class implements reading and writing from binary files.
     */
    class BinFile : public File
    {
    public:
        BinFile(const Path& filepath);

        /**
         * @brief open opens the file in specifies access mode and binary data mode.
         * @param accessMode requested access
         * @return true if successfull, false otherwise.
         */
        bool open(const FileAccessMode& accessMode);

        /**
         * @brief Read template for types. Reads any specified type from the file.
         * @param value reference to value to read in.
         */
        template <typename T>
        bool Read(T& value)
        {
            char buffer[sizeof(T)];
            file.read(buffer, sizeof(T));
            if (file.good())
            {
                value = (reinterpret_cast<T*>(buffer))[0];
                return true;
            }
            return false;
        }

        /**
         * @brief Read template for types. Reads any specified type from the file.
         * @param value reference to value to read in.
         */
        template <typename T>
        bool Write(const T& value)
        {
            file.write(reinterpret_cast<const char*>(&value), sizeof(T));
            return file.good();
        }

        /**
         * @brief ReadBytes reads count bytes to buffer.
         * @param buffer to store the bytes in..
         * @param count amount of bytes to read.
         * @return true, if successfully read, false otherwise.
         */
        bool ReadBytes(char*& buffer, size_t count);

        /**
         * @brief ReadBytes read count bytes to vector
         * @param buffer vector to store bytes in.
         * @param count amount of bytes to read.
         * @return true, if successfully read, false otherwise.
         */
        bool ReadBytes(std::vector<char>& buffer, size_t count);

        /**
         * @brief WriteBytes writes count bytes to file.
         * @param buffer that contains the bytes to write.
         * @param count amount of bytes to be written.
         * @return true, if written successfully, false otherwise.
         */
        bool WriteBytes(const char*& buffer, size_t count);

        /**
         * @brief WriteBytes write count bytes from buffer + offset to file.
         * @param buffer that contains the data to be written.
         * @param index offset from buffer start to start write from.
         * @param count amount of bytes to be written.
         * @return true, if successfully written, false otherwise.
         */
        bool WriteBytes(const char*& buffer, size_t index, size_t count);

        /**
         * @brief WriteBytes writes a whole vector to file.
         * @param buffer vector data to write.
         * @return true on success, false otherwise.
         */
        bool WriteBytes(const std::vector<char>& buffer);

        /**
         * @brief ReadString reads a string from the file.
         * @param str to store the string in.
         * @param count amount of bytes to be casted to a string.
         * @return true, if successfully read, false otherwise.
         */
        bool ReadString(String& str, size_t count);

        /**
         * @brief WriteString writes a string to the file without terminaton.
         * @param str to write to the file.
         * @return true, if successfully written, false otherwise.
         */
        bool WriteString(const String& str);

        /**
         * @brief WriteStringTerminated writes a string with 0 termination to the file.
         * @param str to write to the file.
         * @return true, if successfully written, false otherwise.
         */
        bool WriteStringTerminated(const String& str);
    };
}  // namespace Clipped
