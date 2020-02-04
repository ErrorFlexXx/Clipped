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
         * @brief readLine reads a line from the file with given termination ('\n' is default).
         * @param lineOut String to read the line to.
         * @param terminationChar to detect a line end with.
         * @return true, if a line has been read, false otherwise.
         */
        bool readLine(String &lineOut, const char terminationChar = '\n');

        /**
         * @brief writeLine writes a line to the file with given termination ('\n' is default).
         * @param lineIn line to write to the file.
         * @param terminationChar to use.
         * @return true, if the line has been written successfully, false otherwise.
         */
        bool writeLine(const String &lineIn, const char terminationChar = '\n');

    }; //class TextFile
}  // namespace Clipped
