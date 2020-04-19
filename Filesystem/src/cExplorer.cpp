#include "cExplorer.h"
#include <experimental/filesystem>

using namespace std;
using namespace Clipped;
namespace fs = std::experimental::filesystem;

Explorer::Explorer()
    : currentDir(fs::current_path().c_str())
{}

Explorer::Explorer(const Path &cwd)
{
    fs::current_path(cwd.c_str());
    currentDir = fs::current_path().c_str();
}

MemorySize Explorer::getDriveAvailableMemory() const
{
    return fs::space(currentDir.c_str()).available;
}

MemorySize Explorer::getDriveCapacity() const
{
    return fs::space(currentDir.c_str()).capacity;
}

void Explorer::changeDirectory(const Path& gotoDirectory)
{
    fs::current_path(gotoDirectory.c_str());
    currentDir = fs::current_path().c_str();
}

list<Path> Explorer::searchFiles(const String& searchString, bool recursive)
{
    list<Path> matches;

    if(recursive)
    {
        for(auto &p : fs::recursive_directory_iterator(currentDir.c_str()))
        {
            if(!fs::is_directory(p.path()))
            {
                Path entry = p.path().c_str();
                if(entry.wildcardMatch(searchString))
                    matches.push_back(entry);
            } //else, it's a file and no dir.
        }
    }
    else
    {
        for(auto &p : fs::directory_iterator(currentDir.c_str()))
        {
            if(!fs::is_directory(p.path()))
            {
                Path entry = p.path().c_str();
                if(entry.wildcardMatch(searchString))
                    matches.push_back(entry);
            } //else, it's a file and no dir.
        }
    }

    return matches;
}

list<Path> Explorer::searchDirs(const String &searchString, bool recursive)
{
    list<Path> matches;

    if(recursive)
    {
        for(auto &p : fs::recursive_directory_iterator(currentDir.c_str()))
        {
            if(fs::is_directory(p.path()))
            {
                Path entry = p.path().c_str();
                if(entry.wildcardMatch(searchString))
                    matches.push_back(entry);
            } //else, it's a file and no dir.
        }
    }
    else
    {
        for(auto &p : fs::directory_iterator(currentDir.c_str()))
        {
            if(fs::is_directory(p.path()))
            {
                Path entry = p.path().c_str();
                if(entry.wildcardMatch(searchString))
                    matches.push_back(entry);
            } //else, it's a file and no dir.
        }
    }

    return matches;
}

bool Explorer::CreateDir(const Path &directoryName)
{
    return fs::create_directory(directoryName.c_str());
}

bool Explorer::Remove(const Path& directoryName, bool recursive)
{
    bool deleted = false;
    if(recursive)
        deleted = fs::remove_all(directoryName.c_str());
    else
        deleted = fs::remove(directoryName.c_str());
    return deleted;
}

void Explorer::Rename(const Path &from, const Path &to)
{
    fs::rename(from.c_str(), to.c_str());
}

bool Explorer::Exists(const Path &path)
{
    return fs::exists(path.c_str());
}

void Explorer::Copy(const Path &from, const Path &to, bool recursive)
{
    if(recursive)
    {
        fs::copy(from.c_str(), to.c_str(), fs::copy_options::recursive);
    }
    else
    {
        fs::copy(from.c_str(), to.c_str(), fs::copy_options::none);
    }
}
