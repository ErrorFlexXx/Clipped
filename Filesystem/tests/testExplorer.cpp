#include <ClippedFilesystem/cExplorer.h>
#include <ClippedFilesystem/cTextFile.h>
#include <ClippedUtils/cLogger.h>

using namespace Clipped;

Explorer explorer;

bool checkSpaceAvailable();
bool createDirectories();
bool switchToDirectory();
bool createFiles();
bool listFiles();
bool listDirectories();
bool copyFile();

int main(void)
{
    Logger() << Logger::MessageType::Debug;
    int result = 0;

    Path cleanup = explorer.getCurrentPath() + "/testDirA";
    std::list<Path> directories = explorer.searchDirs("testDirA");
    if(1 == directories.size())
    {
        explorer.Remove(directories.front(), true);
    }

    result |= !checkSpaceAvailable();
    result |= !createDirectories();
    result |= !switchToDirectory();
    result |= !createFiles();
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

bool createDirectories()
{
    bool result = true;
    Path path = explorer.getCurrentPath();
    LogDebug() << "Current path is: " << path;
    result &= explorer.CreateDir(path + "/testDirA");
    result &= explorer.CreateDir(path + "/testDirA/testDirAA");
    result &= explorer.CreateDir(path + "/testDirA/testDirAB");

    return result;
}

bool switchToDirectory()
{
    explorer.changeDirectory(explorer.getCurrentPath() + "/testDirA");
    return true;
}

bool createFiles()
{
    bool result = true;

    TextFile testFile("testFile1.txt");
    if(testFile.touch(true))
    {
        if(testFile.open(FileAccessMode::READ_WRITE))
        {
            if(!testFile.writeLine("This is the test file content."))
            {
                result = false;
                LogError() << "Can't write to test File!";
            }

            testFile.close();
        }
        else
        {
            result = false;
            LogError() << "Can't write to test file: " << testFile.getFilepath();
        }
    }
    else
    {
        result = false;
        LogError() << "Can't create test file!";
    }
    return result;
}

bool listFiles()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    bool result = true;

    std::list<Path> files = explorer.searchFiles("*");
    size_t totalFileCount = files.size();

    if(1 == totalFileCount)
    {
        LogDebug() << "File list of current directory: " << files.size() << " elements.";
    }
    else
    {
        LogError() << "File list shall contain 1 file! But files seen are:";
        for(const Path& file : files)
            LogDebug() << file.toString();
        result = false;
    }
    return result;
}

bool listDirectories()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    bool result = true;

    std::list<Path> directories = explorer.searchDirs("*");

    if(2 != directories.size())
    {
        result = false;
        String output = "List directories has unplausible results! Found more directories for this testcase than expected! Expected 2! Got: " + String((int)directories.size()) + ":\r\n";

        for(const Path& dir : directories)
        {
            output += dir.toString() + "\r\n";
        }
        LogError() << output;
    }

    return result;
}

bool copyFile()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    bool result = true;

    std::list<Path> files = explorer.searchFiles("testFile1.txt");

    if(1 != files.size())
    {
        result = false;
        LogError() << "Found less files than expected! Expected 1! But got: " << files.size() << ": ";
        for(const Path& file : files)
            LogDebug() << file.toString();
        return result;
    }

    Path exeFile = files.front();
    Path exeCopyFile = exeFile;
    exeCopyFile = exeCopyFile.setFilename("testFile2");
    explorer.Copy(exeFile, exeCopyFile, false);

    if(1 != explorer.searchFiles("testFile2.txt", false).size())
    {
        result = false;
        LogError() << "Copy failed!" << exeFile.toString() << " copy to " << exeCopyFile.toString();
    }

    return result;
}
