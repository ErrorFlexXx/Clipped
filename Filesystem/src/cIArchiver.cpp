#include "cIArchiver.h"

using namespace Clipped;

FileEntry::FileEntry()
    : path("")
    , size(0)
    , exists(false)
    , override(false)
{}

FileEntry::FileEntry(const Path& path, const MemorySize size, bool exists, bool override)
    : path(path)
    , size(size)
    , exists(exists)
    , override(override)
{}

const Path& FileEntry::getPath() const
{
    return path;
}

const MemorySize FileEntry::getSize() const
{
    return size;
}

bool FileEntry::getExists() const
{
    return exists;
}

bool FileEntry::getOverride() const
{
    return override;
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
