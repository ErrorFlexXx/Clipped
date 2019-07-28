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
     * @brief The TextFile class implements reading and writing from text based files.
     */
    class TextFile : public File
    {
    public:
        TextFile(const Path& filepath);

        /**
         * @brief open opens the file in specifies access mode and text data mode.
         * @param accessMode requested access
         * @return true if successfull, false otherwise.
         */
        bool open(const FileAccessMode& accessMode);

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
         * @brief ReadLine reads a line from the file with given termination ('\n' is default).
         * @param lineOut String to read the line to.
         * @param terminationChar to detect a line end with.
         * @return true, if a line has been read, false otherwise.
         */
        bool ReadLine(String &lineOut, const char terminationChar = '\n');

        /**
         * @brief WriteLine writes a line to the file with given termination ('\n' is default).
         * @param lineIn line to write to the file.
         * @param terminationChar to use.
         * @return true, if the line has been written successfully, false otherwise.
         */
        bool WriteLine(const String &lineIn, const char terminationChar = '\n');

    }; //class TextFile
}  // namespace Clipped
