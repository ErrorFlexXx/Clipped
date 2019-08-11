#include <ClippedUtils/cLogger.h>
#include <ClippedFilesystem/Archives/cVdfsArchive.h>

using namespace Clipped;

bool checkFilesExist(VDFSArchive& archive);
bool checkFilesDoesntExist(VDFSArchive& archive);
bool addAFile(VDFSArchive& archive);
bool removeAFile(VDFSArchive& archive);
bool checkEmptyDirGetsRemoved(VDFSArchive& archive);

int main(void)
{
    bool status = true;
    Logger() << Logger::MessageType::Debug;
    LogInfo() << "Create Archive handle.";
    VDFSArchive vdfsArchiver("extractTest.vdfs");

    LogInfo() << "Call vdfsArchiver.open";
    status &= vdfsArchiver.open();

    status &= checkFilesExist(vdfsArchiver);
    status &= checkFilesDoesntExist(vdfsArchiver);
    status &= addAFile(vdfsArchiver);
    status &= removeAFile(vdfsArchiver);
    status &= checkEmptyDirGetsRemoved(vdfsArchiver);

    status &= vdfsArchiver.close();

    if(status)
        LogInfo() << "All tests passed!";
    else
        LogError() << "At least one test failed!";
    return !status; //success => true => Return code 0
}

/**
 * @brief checkFilesExist checks, if all files can be found in the archive.
 * @param archive handle to archiver instance.
 * @return true if all files has been found, false otherwise.
 */
bool checkFilesExist(VDFSArchive& archive)
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    bool result = true;

    Path filesToCheck[] = {"testfile0.txt",
                           "Level1/testfile1.txt",
                           "Level1/testfile1-2.txt",
                           "Level1/ALevel1.1/textfile1.1.txt",
                           "Level1/Level1.0/testfile1.0.txt",
                           "Level1/Level1.2/testfile1.2.txt",
                           "Level1/Level2/testfile2.txt"};

    for(Path& file : filesToCheck)
    {
        FileEntry* entry = archive.getFile(file.toUpper());
        if(nullptr == entry)
        {
            LogError() << "File not found in test vdfs: " << file.toUpper();
            result = false;
        }
    }

    return result;
}

bool checkFilesDoesntExist(VDFSArchive& archive)
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    bool result = true;

    Path filesDoesntExist[] = {"NotFound.txt",
                              "DoesntExist.log",
                              "Even/In/Subdirectories/Not.txt"};

    for(Path& file : filesDoesntExist)
    {
        FileEntry* entry = archive.getFile(file.toUpper());
        if(nullptr != entry)
        {
            LogError() << "File found in vdfs, that shouldn't exist: " << file.toUpper();
            result = false;
        }
    }

    return result;
}

String addTestFilename = "NewFile.txt";

bool addAFile(VDFSArchive& archive)
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    Path origFilepath = archive.getBasePath();
    Path addTestFilepath = origFilepath;
    addTestFilepath.setFilename(origFilepath.getFilename().append("-addTest"));
    File orig(origFilepath);

    String data = "This is the text!\r\n";
    if(!orig.copy(addTestFilepath))
    {
        LogError() << "Copy file " << orig.getFilepath() << " to " << addTestFilepath << " failed!";
        return false;
    }

    {   //Archive close scope.
        VDFSArchive addAFileArchive(addTestFilepath);
        if(!addAFileArchive.open())
        {
            LogError() << "Can't open file: " << addAFileArchive.getBasePath();
            return false;
        }
        auto newFile = addAFileArchive.createFile(addTestFilename);
        addAFileArchive.writeFile(newFile, data.data(), data.size());
        if(!addAFileArchive.close())
        {
            LogError() << "VDFS Closing failed!";
            return false;
        }
    }

    VDFSArchive addAFileArchive(addTestFilepath);
    if(!addAFileArchive.open())
    {
        LogError() << "Can't open file!" << addAFileArchive.getBasePath();
        return false;
    }

    auto addedFileHandle = addAFileArchive.getFile(addTestFilename);
    if(!addedFileHandle)
    {
        LogError() << "File add test failed! File not found after adding it!";
        return false;
    }

    std::vector<char> checkData;
    if(!addAFileArchive.readFile(addedFileHandle, checkData))
    {
        LogError() << "Read file from vdfs failed!";
        return false;
    }

    MemorySize gotSize = addedFileHandle->getSize();
    MemorySize expSize = data.size();
    if(gotSize != expSize)
    {
        LogError() << "Check added file filesize failed! " << "Expected size: " << expSize.toString() << " but got size: " << gotSize.toString();
        return false;
    }

    String checkContent = checkData;
    if(!checkContent.equals(data))
    {
        LogError() << "Content written != readback. Expected: \"" << data << "\" but got: \"" << checkContent << "\"";
        return false;
    }
    if(!addAFileArchive.close())
    {
        LogError() << "VDFS Closing failed!";
        return false;
    }

    return true;
}

bool removeAFile(VDFSArchive& archive)
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    Path origFilepath = archive.getBasePath();
    Path addTestFilepath = origFilepath;
    Path removeTestFilepath = origFilepath;
    addTestFilepath.setFilename(origFilepath.getFilename().append("-addTest"));
    removeTestFilepath.setFilename(origFilepath.getFilename().append("-removeTest"));

    File addTestFile(addTestFilepath);
    if(!addTestFile.copy(removeTestFilepath))
    {
        LogError() << "Copy file " << addTestFile.getFilepath() << " to " << removeTestFilepath << " failed!";
        return false;
    }

    //Remove a file:
    {
        VDFSArchive removeFileArchive(removeTestFilepath);
        if(!removeFileArchive.open())
        {
            LogError() << "Can't open archive: " << removeFileArchive.getBasePath();
            return false;
        }
        auto entry = removeFileArchive.getFile(addTestFilename);
        if(!entry)
        {
            LogError() << "Element to remove: " << addTestFilename << " not found in archive!";
            return false;
        }
        if(!removeFileArchive.removeFile(entry))
        {
            LogError() << "Vdfs removeFile failed!";
            return false;
        }
        if(!removeFileArchive.close())
        {
            LogError() << "VDFS Closing failed!";
            return false;
        }
    }

    //Check if file is gone:
    {
        VDFSArchive removeFileArchive(removeTestFilepath);
        if(!removeFileArchive.open())
        {
            LogError() << "Can't open archive: " << removeFileArchive.getBasePath();
            return false;
        }
        auto entry = removeFileArchive.getFile(addTestFilename);
        if(nullptr != entry)
        {
            LogError() << "Removed file is still stored in the archive!";
            return false;
        }
        if(!removeFileArchive.close())
        {
            LogError() << "VDFS Closing failed!";
            return false;
        }
    }
    return true;
}

bool checkEmptyDirGetsRemoved(VDFSArchive& archive)
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    Path origFilepath = archive.getBasePath();
    Path emptyDirTestFilepath = origFilepath;
    emptyDirTestFilepath.setFilename(origFilepath.getFilename().append("-emptyDirTest"));

    File origFile(origFilepath);
    if(!origFile.copy(emptyDirTestFilepath))
    {
        LogError() << "Copy file " << origFile.getFilepath() << " to " << emptyDirTestFilepath << " failed!";
        return false;
    }

    return true;
}
