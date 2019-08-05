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
    file.read(buffer, count);
    return file.good();
}

bool BinFile::readBytes(std::vector<char>& buffer, size_t count)
{
    size_t vecPos = buffer.size();
    buffer.insert(buffer.end(), count, 0);
    file.read(buffer.data() + vecPos, count);
    return file.good();
}

bool BinFile::writeBytes(const char*& buffer, size_t count)
{
    if (buffer == nullptr) return false;
    file.write(buffer, count);
    return file.good();
}

bool BinFile::writeBytes(const char*& buffer, size_t index, size_t count)
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
