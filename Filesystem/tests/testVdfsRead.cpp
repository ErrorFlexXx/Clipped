#include <ClippedUtils/cLogger.h>
#include <ClippedFilesystem/Archives/cVdfsArchive.h>

using namespace Clipped;

bool checkFilesExist(VDFSArchive& archive);
bool checkFilesDoesntExist(VDFSArchive& archive);
bool addAFile(VDFSArchive& archive);

int main(void)
{
    bool status = true;
    Logger() << Logger::MessageType::Debug;
    LogInfo() << "Create Archive handle.";
    VDFSArchive vdfsArchiver("extractTest.vdfs");

    LogInfo() << "Call vdfsArchiver.open";
    status &= vdfsArchiver.open();
    LogInfo() << "Check file contents.";
    status &= checkFilesExist(vdfsArchiver);
    LogInfo() << "Check unstored file contents.";
    status &= checkFilesDoesntExist(vdfsArchiver);
    LogInfo() << "Check addAFile.";
    status &= addAFile(vdfsArchiver);

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

bool addAFile(VDFSArchive& archive)
{
    bool success = true;
    File orig(archive.getBasePath());
    String testfile = "addFileTest.vdfs";
    String newFilename = "NewFile.txt";
    String data = "This is the text!\r\n";
    if(!orig.copy(testfile)) success = false;
    if(success)
    {
        VDFSArchive addAFileArchive(testfile);
        if(!addAFileArchive.open())
        {
            LogError() << "Can't open file!";
            return false;
        }
        auto newFile = addAFileArchive.createFile(newFilename);
        addAFileArchive.writeFile(newFile, data.data(), data.size());
    }
    if(success)
    {
        VDFSArchive addAFileArchive(testfile);
        if(!addAFileArchive.open())
        {
            LogError() << "Can't open file!" << testfile;
            return false;
        }

        auto addedFileHandle = addAFileArchive.getFile(newFilename);
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
    }

    return success;
}
