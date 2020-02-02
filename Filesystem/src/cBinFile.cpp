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

#include "cBinFile.h"
#include <iostream>

using namespace Clipped;

BinFile::BinFile(const Path& filepath) : File(filepath) {}

bool BinFile::open(const FileAccessMode& accessMode)
{
    return File::open(accessMode, FileDataMode::BINARY);
}

bool BinFile::readBytes(char*& buffer, size_t count)
{
    if (buffer == nullptr) return false;
    file.read(reinterpret_cast<char*>(buffer), count);
    return file.good();
}

bool BinFile::readBytes(std::vector<char>& buffer, size_t count)
{
    size_t vecPos = buffer.size();
    buffer.insert(buffer.end(), count, 0);
    file.read(reinterpret_cast<char*>(buffer.data() + vecPos), count);
    return file.good();
}

bool BinFile::writeBytes(const char* buffer, size_t count)
{
    if (buffer == nullptr) return false;
    file.write(reinterpret_cast<const char*>(buffer), count);
    return file.good();
}

bool BinFile::writeBytes(const char* buffer, size_t index, size_t count)
{
    const char* tmpBuffer = buffer;
    tmpBuffer += index;
    return writeBytes(tmpBuffer, count);
}

bool BinFile::writeBytes(const std::vector<char>& buffer)
{
    const char* charBuff = buffer.data();
    return writeBytes(charBuff, buffer.size());
}

bool BinFile::readString(String& str, size_t count)
{
    char* buffer = new char[count + 1];
    if (buffer)
    {
        buffer[count] = 0;
        file.read(buffer, count);
        if (file.good())
        {
            str = buffer;
            delete[] buffer;
            return true;
        }
        str.clear();
        delete[] buffer;
    }
    return false;
}

bool BinFile::writeString(const String& str)
{
    file.write(str.c_str(),
               static_cast<long>(str.length()));  // Write string incl. 0 termination.
    return file.good();
}

bool BinFile::writeStringTerminated(const String& str)
{
    char nullTermination = 0;
    if (writeString(str)) return write<char>(nullTermination);
    return false;
}
