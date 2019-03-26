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
