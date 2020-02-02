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

#include "cBinStream.h"

using namespace Clipped;

BinStream::BinStream(std::vector<char>& data) : index(0), data(data) {}

bool BinStream::readString(String& value)
{
    if (index < data.size())
    {
        value.clear();
        while (index < data.size() &&           // Stop if data empty
               *(data.data() + index) != 0 &&   // or null termination seen
               *(data.data() + index) != 10 &&  // or LF seen
               *(data.data() + index) != 16)    // or CR seen
        {
            value.append(data.data() + index, 1);
            index++;
        }
        index++;  // 0 not inserted to string, but we increment the index.
        return true;
    }
    LogError() << "Stream is empty! Nothing to read!";
    return false;
}

bool BinStream::readZString(String& value)
{
    if (index < data.size())
    {
        value.clear();
        while (index < data.size() &&        // Stop if data empty
               *(data.data() + index) != 0)  // or null termination seen
        {
            value.append(data.data() + index, 1);
            index++;
        }
        index++;  // 0 not inserted to string, but we increment the index.
        return true;
    }
    LogError() << "Stream is empty! Nothing to read!";
    return false;
}
