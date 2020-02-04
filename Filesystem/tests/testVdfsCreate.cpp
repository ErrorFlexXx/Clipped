#include <ClippedFilesystem/Archives/cVdfsArchive.h>
#include <ClippedUtils/cLogger.h>

using namespace Clipped;

bool createEmpty();
bool createWithContent();

int main(void)
{
    Logger() << Logger::MessageType::Debug;
    int result = 0;

    result |= !createEmpty();
    result |= !createWithContent();

    return result;
}

bool createEmpty()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    bool result = false;
    VDFSArchive newArchive("testArchive.vdfs");

    if(newArchive.create())
    {
        LogDebug() << "Created new archiver instance for new file \"" << newArchive.getBasePath() << "\".";
        newArchive.getHeader().comment = "VDFS Archive created by Clipped.";
        newArchive.getHeader().signature = "PSVDSC_V2.00\n\r\n\r";
        LogDebug() << newArchive.getHeader().toString();
        result = newArchive.close();
    }

    return result;
}

bool createWithContent()
{
    LogInfo() << "Testcase: " << __FUNCTION__;
    bool result = true;
    VDFSArchive newArchive("testArchiveWithContent.vdfs");
    String content = "This is the text file content!";

    result = newArchive.create();
    if(result)
    {
        LogDebug() << "Created new archiver instance for new file \"" << newArchive.getBasePath() << "\".";
        newArchive.getHeader().comment = "VDFS Archive created by Clipped.";
        newArchive.getHeader().signature = "PSVDSC_V2.00\n\r\n\r";
        auto* entry = newArchive.createFile("Base/Second/textfile.txt");
        result &= (entry != nullptr);
        if(nullptr == entry)
            LogError() << "CreateFile failed!";
        if(!newArchive.writeFile(entry, content.data(), content.size()))
        {
            result = false;
            LogError() << "newArchive.writeFile failed!";
        }
        LogDebug() << newArchive.getHeader().toString();
        result &= newArchive.close();
    }

    return result;
}
