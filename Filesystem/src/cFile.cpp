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

#include "cFile.h"
#include "cBinFile.h"
#include <stdio.h>
#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cOsDetect.h>

using namespace std;
using namespace Clipped;

File::File(const Path& filepath) : filepath(filepath) {}

File::~File()
{
    if (file.is_open()) file.close();
}

bool File::open(const FileAccessMode& accessMode, const FileDataMode& dataMode)
{
    this->accessMode = accessMode;
    this->dataMode = dataMode;

    ios_base::openmode mode = ios::in;

    switch (accessMode)
    {
        case FileAccessMode::READ_ONLY:
        {
            break;
        }
        case FileAccessMode::READ_WRITE:
        {
            mode |= ios::out;
            break;
        }
        case FileAccessMode::TRUNC:
        {
            mode |= ios::out | ios::trunc;
            break;
        }
    }

    switch (dataMode)
    {
        case FileDataMode::TEXT:
            break;
        case FileDataMode::BINARY:
        {
            mode |= ios::binary;
            break;
        }
    }
    file.open(filepath, mode);
    return file.is_open();
}

void File::close()
{
    if (file.is_open()) file.close();
}

bool File::exists() const
{
    fstream test;
    bool exist;

    test.open(filepath, ios::in);
    exist = test.is_open();
    test.close();

    return exist;
}

MemorySize File::getSize() const
{
    std::fstream query;
    query.open(filepath, ios::in | ios::binary | ios::ate);
    if (query.is_open())
    {
        MemorySize size = static_cast<unsigned long long>(query.tellg());
        query.close();
        return size;
    }
    else  // Cannot open file for read.
    {
        LogError() << "Cannot open file: " << filepath;
    }
    return 0;
}

bool File::remove() { return std::remove(filepath.c_str()) == 0; }

bool File::touch(const bool override)
{
    if (exists())
    {
        close();
        if (override)
        {
            if (!remove())  // File exists, override set, file not removable -> fail
                return false;
        }
        else  // File exsits, but override not set -> fail
            return false;
    }
    LogDebug() << "Create now.";
    // File doesn't exist, or is deleted now, so go create a new one.
    return open(FileAccessMode::TRUNC, dataMode);  // And return the result.
}

bool File::copy(const Path& destination)
{
    bool closeAfterwards = false;
    bool success = true;
    if(!isOpen())
    {
        closeAfterwards = true;
        if(!open(FileAccessMode::READ_ONLY, FileDataMode::BINARY))
            return false;
    }
    BinFile copy(destination);
    if(!copy.open(FileAccessMode::TRUNC))
    {
        LogError() << "Can't create file: " << destination;
        success = false;
    }
    if(success)
    {
        char* buffer = new char[1024];
        MemorySize i = 0;
        MemorySize size = this->getSize();
        for(i = 0; i + 1024 < size; i += 1024)
        {
            file.read(buffer, 1024);
            copy.writeBytes(buffer, 1024);
        }
        size = size - (i); //Calculate left over size.
        file.read(buffer, size); //Read rest of content.
        copy.writeBytes(buffer, size); //Write remaining bytes.

        delete[] buffer;
        copy.close();
    }

    if(closeAfterwards)
        file.close();
    return success;
}

bool File::isOpen() const { return file.is_open(); }

bool File::setPosition(size_t pos)
{
    file.seekg(pos);
    return file.good();
}

bool File::setPostionToFileEnd()
{
    file.seekp(0, ios::end);
    return file.good();
}

size_t File::getPosition()
{
    return static_cast<size_t>(file.tellg());
}

bool File::seek(long delta)
{
    file.seekg(delta, ios::cur);
    return ((file.failbit | file.badbit) == 0);
}

const Path& File::getFilepath() const { return filepath; }
