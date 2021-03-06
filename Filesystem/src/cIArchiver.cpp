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

#include "cIArchiver.h"

using namespace Clipped;

FileEntry::FileEntry()
    : path("")
    , size(0)
{}

FileEntry::FileEntry(const Path& path, const MemorySize size)
    : path(path)
    , size(size)
{}

const Path& FileEntry::getPath() const
{
    return path;
}

const MemorySize FileEntry::getSize() const
{
    return size;
}

IArchiver::IArchiver(const Path& basePath)
    : basePath(basePath)
{}

IArchiver::~IArchiver()
{}

bool IArchiver::finalize()
{
    return true;    //By default no action and success.
                    //To be overriden, if required by archiver type.
}

const Path& IArchiver::getBasePath() const
{
    return basePath;
}
