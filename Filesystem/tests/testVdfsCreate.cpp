#include <ClippedFilesystem/Archives/cVdfsArchive.h>
#include <ClippedUtils/cLogger.h>

using namespace Clipped;

int main(void)
{
    VDFSArchive newArchive("testArchive.vdfs");

    if(newArchive.create())
    {
        LogDebug() << "Created new archiver instance for new file \"" << newArchive.getBasePath() << "\".";

    }
}
