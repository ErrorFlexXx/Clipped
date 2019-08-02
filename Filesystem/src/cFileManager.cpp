#include "cFileManager.h"

using namespace Clipped;

FileEntry::FileEntry(const Path& path, const MemorySize size, bool exists, bool override)
    : path(path)
    , size(size)
    , exists(exists)
    , override(override)
{}

const Path& FileEntry::GetPath() const
{
    return path;
}

const MemorySize FileEntry::GetSize() const
{
    return size;
}

bool FileEntry::Exists() const
{
    return exists;
}

bool FileEntry::Override() const
{
    return override;
}

IFileManager::IFileManager(const Path& basePath)
    : basePath(basePath)
{}

IFileManager::~IFileManager()
{}
