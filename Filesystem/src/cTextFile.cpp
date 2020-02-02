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
