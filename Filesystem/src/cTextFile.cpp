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

#include "cTextFile.h"
#include <iostream>

using namespace Clipped;

TextFile::TextFile(const Path& filepath) : File(filepath) {}

bool TextFile::open(const FileAccessMode& accessMode)
{
    return File::open(accessMode, FileDataMode::TEXT);
}

bool TextFile::readString(String& str, size_t count)
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

bool TextFile::writeString(const String& str)
{
    file.write(str.c_str(),
               static_cast<long>(str.length()));  // Write string incl. 0 termination.
    return file.good();
}

bool TextFile::readLine(String &lineOut, const char terminationChar)
{
    lineOut.clear();

    std::getline(file, lineOut, terminationChar);
    return file.good();
}

bool TextFile::writeLine(const String &lineIn, const char terminationChar)
{
    file.write(lineIn.c_str(), lineIn.length());
    file.write(&terminationChar, 1);
    return file.good();
}
