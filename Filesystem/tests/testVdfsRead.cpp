#include <ClippedUtils/cLogger.h>
#include <ClippedFilesystem/Archives/cVdfsArchive.h>

using namespace Clipped;

int checkFilesExist(VDFSArchive& archive);

int main(void)
{
    bool status = true;
    Logger() << Logger::MessageType::Info;
    VDFSArchive vdfsArchiver("extractTest.vdfs");

    status &= vdfsArchiver.open();
    status &= checkFilesExist(vdfsArchiver);

    return !status; //success => true => Return code 0
}

/**
 * @brief checkFilesExist checks, if all files can be found in the archive.
 * @param archive handle to archiver instance.
 * @return true if all files has been found, false otherwise.
 */
int checkFilesExist(VDFSArchive& archive)
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
        auto entry = archive.getFile(file.toUpper());
        result &= entry->getExists();
        if(!entry->getExists())
            LogError() << "File not found in test vdfs: " << file.toUpper();
    }
    return result;
}
