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
