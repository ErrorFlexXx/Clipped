#include <ClippedFilesystem/cExplorer.h>
#include <ClippedUtils/cLogger.h>

using namespace Clipped;

Explorer explorer;

bool checkSpaceAvailable();
bool listFiles();
bool listDirectories();
bool copyFile();

int main(void)
{
    Logger() << Logger::MessageType::Debug;
    int result = 0;

    result |= !checkSpaceAvailable();
    result |= !listFiles();
    result |= !listDirectories();
    result |= !copyFile();

    return result;
}

bool checkSpaceAvailable()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    bool result = false;

    MemorySize memAvail = explorer.getDriveAvailableMemory();
    MemorySize memCapa = explorer.getDriveCapacity();

    if(memAvail <= memCapa)
    {
        result = true;
        LogDebug() << memAvail.toString() + " available from " + memCapa.toString() + " capacity.";
    }
    else
    {
        LogError() << "Space available unplausible! More space available than capacity of the drive!";
    }

    return result;
}

bool listFiles()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    bool result = true;

    std::list<Path> files = explorer.searchFiles("*");
    size_t totalFileCount = files.size();

    if(0 < totalFileCount)
    {
        LogDebug() << "File list of current directory: " << files.size() << " elements";
    }
    else
    {
        LogError() << "File list is empty!";
        result = false;
    }

    std::list<Path> testExplorerFiles = explorer.searchFiles("testExplorer*");
    if(testExplorerFiles.size() <= totalFileCount)
    {
        LogDebug() << "List of explorer files: ";
        for(const Path& file : testExplorerFiles)
            LogDebug() << file.toString();
    }
    else
    {
        LogError() << "File list unplausible! more testExplorer files found, than total files in directory!";
        result = false;
    }

    return result;
}

bool listDirectories()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    bool result = true;

    std::list<Path> directories = explorer.searchDirs("testExplorer*");

    if(1 != directories.size())
    {
        result = false;
        LogError() << "List directories has unplausible results! Found more directories for this testcase than expected!";
        for(const Path& dir : directories)
        {
            LogDebug() << dir.toString();
        }
    }

    return result;
}

bool copyFile()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    bool result = true;

    std::list<Path> files = explorer.searchFiles("testExplorer", false);
    if(0 == files.size())
        files = explorer.searchFiles("testExplorer.exe", false);

    if(files.size() <= 0)
    {
        result = false;
        LogError() << "Found less files than expected!";
        return result;
    }

    Path exeFile = files.front();
    Path exeCopyFile = exeFile;
    exeCopyFile = exeCopyFile.setFilename("testExplorerCopyFile");
    explorer.Copy(exeFile, exeCopyFile, false);

    if(1 != explorer.searchFiles("testExplorerCopyFile", false).size())
    {
        if(1 != explorer.searchFiles("testExplorerCopyFile.exe", false).size())
        {
            result = false;
            LogError() << "Copy failed!" << exeFile.toString() << " copy to " << exeCopyFile.toString();
        }
    }

    return result;
}
