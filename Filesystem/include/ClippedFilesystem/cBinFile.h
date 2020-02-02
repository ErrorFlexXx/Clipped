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
         * @brief read template for types. Reads any specified type from the file.
         * @param value reference to value to read in.
         */
        template <typename T>
        bool read(T& value)
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
         * @brief write template for types. Reads any specified type from the file.
         * @param value reference to value to read in.
         */
        template <typename T>
        bool write(const T& value)
        {
            file.write(reinterpret_cast<const char*>(&value), sizeof(T));
            return file.good();
        }

        /**
         * @brief readBytes reads count bytes to buffer.
         * @param buffer to store the bytes in..
         * @param count amount of bytes to read.
         * @return true, if successfully read, false otherwise.
         */
        bool readBytes(char*& buffer, size_t count);

        /**
         * @brief readBytes read count bytes to vector
         * @param buffer vector to store bytes in.
         * @param count amount of bytes to read.
         * @return true, if successfully read, false otherwise.
         */
        bool readBytes(std::vector<char>& buffer, size_t count);

        /**
         * @brief writeBytes writes count bytes to file.
         * @param buffer that contains the bytes to write.
         * @param count amount of bytes to be written.
         * @return true, if written successfully, false otherwise.
         */
        bool writeBytes(const char* buffer, size_t count);

        /**
         * @brief writeBytes write count bytes from buffer + offset to file.
         * @param buffer that contains the data to be written.
         * @param index offset from buffer start to start write from.
         * @param count amount of bytes to be written.
         * @return true, if successfully written, false otherwise.
         */
        bool writeBytes(const char* buffer, size_t index, size_t count);

        /**
         * @brief writeBytes writes a whole vector to file.
         * @param buffer vector data to write.
         * @return true on success, false otherwise.
         */
        bool writeBytes(const std::vector<char>& buffer);

        /**
         * @brief readString reads a string from the file.
         * @param str to store the string in.
         * @param count amount of bytes to be casted to a string.
         * @return true, if successfully read, false otherwise.
         */
        bool readString(String& str, size_t count);

        /**
         * @brief writeString writes a string to the file without terminaton.
         * @param str to write to the file.
         * @return true, if successfully written, false otherwise.
         */
        bool writeString(const String& str);

        /**
         * @brief writeStringTerminated writes a string with 0 termination to the file.
         * @param str to write to the file.
         * @return true, if successfully written, false otherwise.
         */
        bool writeStringTerminated(const String& str);
    };
}  // namespace Clipped
