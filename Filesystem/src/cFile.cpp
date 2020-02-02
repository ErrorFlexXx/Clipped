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
