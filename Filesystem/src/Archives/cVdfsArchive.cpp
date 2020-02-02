/* Copyright 2019 Christian Löpke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "Archives/cVdfsArchive.h"
#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cPath.h>

using namespace Clipped;

const String VDFSArchive::CommentFillChar = "\x1A";
const size_t VDFSArchive::CommentLength = 256;
const size_t VDFSArchive::SignatureLength = 16;
const size_t VDFSArchive::EntryNameLength = 64;

VDFSArchive::VDFSArchive(const Path& filepath)
    : IArchiver(filepath)
    , file(basePath)
    , directoryOffsetCount(0)
    , modified(false)
{
}

VDFSArchive::~VDFSArchive()
{
    finalize();
}

bool VDFSArchive::open()
{
    bool result = file.open(FileAccessMode::READ_WRITE);
    if (!result)
    {
        LogError() << "File cannot be opened!";
        return result;
    }
    result = readHeader(header);
    if (!result)
    {
        LogError() << "VDFS Header corrupt!";
        return result;
    }
    result = readVDFSIndex();
    if (!result)
    {
        LogError() << "VDFS Index corrupt!";
        return result;
    }
    return result;
}

bool VDFSArchive::close()
{
    return finalize();
}

bool VDFSArchive::finalize()
{
    bool success = true;
    if(file.isOpen())
    {
        if(modified)
        {
            success = writeHeader(header);
            if(!success) LogError() << "Can't write the VDFS header!";
            if(success) success = writeVDFSIndex();
            if(!success)
                LogError() << "Can't write the VDFS index!";
            else
                modified = false; //Index updated. No modifications left.
        }
        file.close();
    }
    else if(modified)
    {
        //Should never happen.
        LogError() << "Can't update vdfs index -- File handle is closed to early.";
        success = false;
    }
    return success;
}

bool VDFSArchive::readVDFSIndex()
{
    file.setPosition(header.rootOffset);
    vdfsIndex.currentStoredSize = 0; //Reset length. Gets updated in recursive createIndexTree calls.
    size_t entriesRead = readIndexTree(vdfsIndex.indexTree);
    return entriesRead == header.entryCount;
}

bool VDFSArchive::writeVDFSIndex()
{
    vdfsIndex.indexTree.removeEmptyChilds(); //Cleanup of empty directories - Unsupported by vdfs!

    if(!allocIndexMemory()) return false;
    if(!file.setPosition(header.rootOffset)) return false;
    directoryOffsetCount = 0;
    bool writeResult = writeIndexTree(vdfsIndex.indexTree);

    return writeResult;
}

bool VDFSArchive::allocIndexMemory()
{
    size_t entriesToBeWritten = vdfsIndex.indexTree.countChildsAndElements();
    size_t newIndexByteSize = header.entrySize * entriesToBeWritten;
    size_t availableIndexMemory = 0;

    do
    {
        VdfsEntry* firstEntry = nullptr;
        if(!getFirstStoredEntry(firstEntry))
        {
            LogError() << "File corrupt or index is empty!";
            return false;
        }
        availableIndexMemory = firstEntry->vdfs_offset - header.rootOffset;
        if(availableIndexMemory < newIndexByteSize) //Not enaugh place ?
        {
            if(!moveEntryDataToTheEnd(firstEntry)) return false; //Move first file to the end.
        }
    } while(availableIndexMemory < newIndexByteSize); //A file has been moved, recheck or not, then done.

    return true;
}

bool VDFSArchive::moveEntryDataToTheEnd(VdfsEntry*& entry)
{
    bool success = true;
    char* tmpData = new char[entry->vdfs_size];
    if(tmpData == nullptr)
    {
        LogError() << "Out of memory!";
        return false;
    }
    success = file.setPosition(entry->vdfs_offset);
    if(success) success = file.readBytes(tmpData, entry->vdfs_size);
    if(success) success = file.setPosition(file.getSize());
    size_t newOffset = file.getPosition();
    if(success) success = file.writeBytes(tmpData, entry->vdfs_size);
    if(success)
    {
        entry->vdfs_offset = static_cast<uint32_t>(newOffset);
    }
    delete[] tmpData;
    return success;
}

bool VDFSArchive::getFirstStoredEntry(VdfsEntry*& entry)
{
    entry = nullptr;
    for(auto& treeElement : vdfsIndex.indexTree)
    {
        if(entry && treeElement.second.vdfs_offset < entry->vdfs_offset)
        {
            entry = &treeElement.second;
        }
        else if(entry == nullptr)
            entry = &treeElement.second;
    }
    return entry != nullptr;
}

size_t VDFSArchive::readIndexTree(Tree<String, VdfsEntry>& tree)
{
    for (size_t i = 1; i <= header.entryCount; i++) //We count the processed entries with i, so
                                                    //it's important to start counting from 1.
    {
        VdfsEntry entry;
        bool interpretResult = true;

        size_t beforeEntryRead = file.getPosition();
        if (interpretResult) interpretResult = file.readString(entry.vdfs_name, EntryNameLength);
        if (interpretResult) interpretResult = file.read(entry.vdfs_offset);
        if (interpretResult) interpretResult = file.read(entry.vdfs_size);
        if (interpretResult) interpretResult = file.read(entry.vdfs_type);
        if (interpretResult) interpretResult = file.read(entry.vdfs_attribute);

        if(interpretResult) //If all entry data has been successfully read from file
        {
            vdfsIndex.currentStoredSize += file.getPosition() - beforeEntryRead;
            entry.vdfs_name = entry.vdfs_name.trim(); //Remove whitespaces (Fill char in the archive)

            if (entry.vdfs_type & EntryType::DIRECTORY) //Ordering in VDFS -> first enumerate existing directories
            {
                //Recursivly add next directory stage.
                tree.addSubtree(entry.vdfs_name);
            }
            else //Ordering in VDFS -> secondly list files in this directory stage.
            {
                entry.path = entry.vdfs_name;
                entry.size = entry.vdfs_size;
                tree.addElement(entry.path, entry);
            }

            if (entry.vdfs_type & EntryType::LAST) //Ordering in VDFS -> third, after local dirs and files join the directory contents.
            {
                for( auto& subtree : tree.getSubtrees())
                {
                    i += readIndexTree(subtree.second); //Add subentry processed
                }
                return i;
            }
        }
        else // Read failed
        {
            return i - 1; //Return processed entries (-1 because the current had failed and wasn't interpreted).
        }
    }
    return header.entryCount; //Never reached in general, cause all stages are terminated with LAST entries.
}

bool VDFSArchive::writeIndexTree(Tree<String, VdfsEntry>& tree)
{
    bool writeSuccess = true;

    directoryOffsetCount += tree.countLocalElements();  //Count all files of this stage.
    directoryOffsetCount += tree.countLocalSubtrees();  //Count directory entries.

    size_t subdirectoryOffsetCount = directoryOffsetCount;
    for(auto& child : tree.childs) //Write directories.
    {
        if(writeSuccess) writeSuccess = file.writeString(child.first.fill(" ", EntryNameLength));
        if(writeSuccess) writeSuccess = file.write(subdirectoryOffsetCount);
        if(writeSuccess) writeSuccess = file.write((uint32_t) 0); //Size
        if(writeSuccess) writeSuccess = file.write(EntryType::DIRECTORY);
        if(writeSuccess) writeSuccess = file.write((uint32_t) 0); //Attribute

        subdirectoryOffsetCount += child.second.countChildsAndElements();
    }

    size_t i = 1;
    size_t elementCount = tree.countLocalElements();
    for(auto& element : tree.elements) //Write local elements.
    {
        EntryType entryType = EntryType::BLANK;
        if(i++ >= elementCount) //Last element ?
            entryType = EntryType::LAST;

        if(writeSuccess) writeSuccess = file.writeString(element.first.fill(" ", EntryNameLength)); //EntryName
        if(writeSuccess) writeSuccess = file.write(element.second.vdfs_offset);     //Offset
        if(writeSuccess) writeSuccess = file.write(element.second.vdfs_size);       //Size
        if(writeSuccess) writeSuccess = file.write(entryType);                      //EntryType
        if(writeSuccess) writeSuccess = file.write(element.second.vdfs_attribute);  //Attribute
    }

    //Join directories.
    for(auto& child : tree.childs) //Write subdirectory contents.
    {
        writeIndexTree(child.second);
    }

    return writeSuccess;
}

FileEntry* VDFSArchive::getVdfsFile(const Path& filepath, bool createIfNotFound)
{
    Path dir = filepath.getDirectory();
    Path file = filepath.getFilenameWithExt();

    auto* searchIndex = &vdfsIndex.indexTree;

    for (String stage : dir.split(Path("/")))
    {
        bool subtreeExists = searchIndex->subtreeExist(stage);
        if ( createIfNotFound || subtreeExists)
        {
            if(!subtreeExists)
            {
                header.entryCount++;
            }
            searchIndex = &searchIndex->getSubtree(stage);
        }
        else
        {
            return nullptr;
        }
    }
    bool fileExists = searchIndex->elementExist(file);
    if (createIfNotFound || searchIndex->elementExist(file))
    {
        if(!fileExists)
        {
            header.entryCount++;
            header.fileCount++;
        }
        return &searchIndex->getElement(file);
    }
    return nullptr;
}

FileEntry* VDFSArchive::searchVdfsFile(const Path& filename, Tree<String, VdfsEntry>& tree)
{
    if(tree.elementExist(filename.getFilenameWithExt()))
    {
        return &tree.getElement(filename.getFilenameWithExt());
    }
    else
    {
        for(auto & leaf : tree.childs)
        {
            FileEntry *result = searchVdfsFile(filename, leaf.second);
            if(result) return result; //Found in subtrees.
        }
    }
    return nullptr;
}

FileEntry* VDFSArchive::getFile(const Path& filepath)
{
    return getVdfsFile(filepath, false);
}

FileEntry* VDFSArchive::searchFile(const Path& filename)
{
    return searchVdfsFile(filename, vdfsIndex.indexTree);
}

FileEntry* VDFSArchive::createFile(const Path& filepath)
{
    return getVdfsFile(filepath, true);
}

bool VDFSArchive::readFile(const FileEntry* fileEntry, char* dest)
{
    const VdfsEntry* vdfsEntry = dynamic_cast<const VdfsEntry*>(fileEntry);
    if(!vdfsEntry)
    {
        LogError() << "fileEntry given that wasn't constructed by a vdfsArchive instance!";
        return false;
    }

    file.setPosition(vdfsEntry->vdfs_offset); //Set filepointer to fileEntry inside the vdfs.

    if (!file.readBytes(dest, vdfsEntry->vdfs_size))
    {
        LogError() << "Error while reading from file.";
        return false;
    }
    return true; //Successfully read the file data.
}

bool VDFSArchive::readFile(const FileEntry* fileEntry, std::vector<char>& dest)
{
    const VdfsEntry* vdfsEntry = dynamic_cast<const VdfsEntry*>(fileEntry);
    if(!vdfsEntry)
    {
        LogError() << "fileEntry given that wasn't constructed by a vdfsArchive instance!";
        return false;
    }

    file.setPosition(vdfsEntry->vdfs_offset); //Set filepointer to fileEntry inside the vdfs.

    if (!file.readBytes(dest, vdfsEntry->vdfs_size))
    {
        LogError() << "Error while reading from file.";
        return false;
    }
    return true; //Successfully read the file data.
}

bool VDFSArchive::writeFile(FileEntry* fileEntry, const char* src, const size_t length)
{
    VdfsEntry* vdfsEntry;
    if(!checkFileEntryIsVdfsEntry(fileEntry, vdfsEntry))
    {
        LogError() << "Handle given, that wasn't created by an VDFSArchive instance!";
        return false;
    }
    vdfsEntry->vdfs_size = static_cast<uint32_t>(length); //Enter size.
    size_t writeOffset = getFreeMemoryOffset(length);
    if(!file.setPosition(writeOffset)) return false;
    if(!file.writeBytes(src, length)) return false;
    vdfsEntry->vdfs_offset = static_cast<uint32_t>(writeOffset);
    vdfsEntry->vdfs_attribute = EntryAttribute::ARCHIVE;
    header.contentSize += static_cast<uint32_t>(length);

    modified = true; //Update index on disk, if archive gets closed.
    return true;
}

bool VDFSArchive::writeFile(FileEntry* fileEntry, const std::vector<char>& src)
{
    return writeFile(fileEntry, src.data(), src.size());
}

bool VDFSArchive::removeFile(FileEntry* fileEntry)
{
    VdfsEntry* vdfsEntry;
    if(!checkFileEntryIsVdfsEntry(fileEntry, vdfsEntry))
    {
        LogError() << "Handle given, that wasn't created by an VDFSArchive instance!";
        return false;
    }
    size_t sizeOfFile = vdfsEntry->vdfs_size;
    bool removed = vdfsIndex.indexTree.removeElement(fileEntry->getPath(), vdfsEntry);
    if(removed)
    {
        //Update header:
        modified = true; //Update index on disk, if archive gets closed.
        header.fileCount--;
        header.contentSize -= static_cast<uint32_t>(sizeOfFile);
        header.entryCount--;
    }
    return removed;
}

bool VDFSArchive::readHeader(VDFSArchive::VDFSHeader& header)
{
    bool result = true;

    result = file.setPosition(0); //Set file pointer to the header location.
    if (result) result = file.readString(header.comment, VDFSArchive::CommentLength);
    if (result) result = file.readString(header.signature, VDFSArchive::SignatureLength);
    if (result) result = file.read(header.entryCount);
    if (result) result = file.read(header.fileCount);
    if (result) result = file.read(header.creationTime);
    if (result) result = file.read(header.contentSize);
    if (result) result = file.read(header.rootOffset);
    if (result) result = file.read(header.entrySize);

    header.comment = header.comment.trim(CommentFillChar);

    return result;
}

bool VDFSArchive::writeHeader(const VDFSArchive::VDFSHeader& header)
{
    bool result = true;

    file.setPosition(0); //Set file pointer to the header location.

    if (header.comment.length() > CommentLength)
    {
        LogWarn() << "Header comment too large (" << header.comment.length()
                  << ")! Cutted to max length (" << CommentLength << ").";
        if (result) result = file.writeString(header.comment.substr(0, CommentLength));
    }
    else  // Comment size ok
        if (result) result = file.writeString(header.comment.fill(CommentFillChar, CommentLength));

    if (header.signature.length() > SignatureLength)
    {
        LogWarn() << "Header signature too large (" << header.signature.length()
                  << ")! Cutted to max length (" << SignatureLength << ").";
        if (result) result = file.writeString(header.signature.substr(0, SignatureLength));
    }
    else  // Signature size ok
        if (result) result = file.writeString(header.signature.fill(" ", SignatureLength));

    if (result) result = file.write(header.entryCount);
    if (result) result = file.write(header.fileCount);
    if (result) result = file.write((MSDOSTime32)Time());
    if (result) result = file.write(header.contentSize);
    if (result) result = file.write(header.rootOffset);
    if (result) result = file.write(header.entrySize);

    return result;
}

bool VDFSArchive::checkFileEntryIsVdfsEntry(FileEntry* check, VdfsEntry*& target) const
{
    bool success = true;
    target = dynamic_cast<VdfsEntry*>(check);
    if(!target)
    {
        LogError() << "fileEntry given that wasn't constructed by a vdfsArchive instance!";
        success = false;
    }
    return success;
}

size_t VDFSArchive::getFreeMemoryOffset(const size_t requiredBytes) const
{
    return file.getSize(); //To do: Implement Gap Management to minimize file fragmentation.
}
