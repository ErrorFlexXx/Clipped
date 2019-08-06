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
{
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
    //writeVDFSIndex(); //Testing...
    return result;
}

bool VDFSArchive::readVDFSIndex()
{
    file.setPosition(header.rootOffset);
    archiveIndex.currentStoredSize = 0; //Reset length. Gets updated in recursive createIndexTree calls.
    size_t entriesRead = readIndexTree(archiveIndex.index);
    LogDebug() << "Index Length: " << MemorySize(archiveIndex.currentStoredSize).toString();
    return entriesRead == header.entryCount;
}

bool VDFSArchive::writeVDFSIndex()
{
    size_t entriesToBeWritten = archiveIndex.index.countChildsAndElements();
    size_t currentIndexBytes = archiveIndex.currentStoredSize;

    LogDebug() << "Entries to be written: " << entriesToBeWritten;
    LogDebug() << "Index Bytes per entry: " << header.entrySize;
    LogDebug() << "Current Index Bytes: " << currentIndexBytes;
    size_t requiredIndexSpace = header.entrySize * entriesToBeWritten;
    LogDebug() << "Required Index space: " << requiredIndexSpace;

    if(!allocIndexMemory()) return false;
    if(!file.setPosition(header.rootOffset)) return false;
    size_t entriesWritten = writeIndexTree(archiveIndex.index);

    return entriesToBeWritten == entriesWritten;
}

bool VDFSArchive::allocIndexMemory()
{
    size_t entriesToBeWritten = archiveIndex.index.countChildsAndElements();
    size_t newIndexByteSize = header.entrySize * entriesToBeWritten;
    size_t availableIndexMemory = 0;

    do
    {
        VdfsEntry* firstEntry = nullptr;
        if(!getFirstStoredEntry(firstEntry)) return false;
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
    if(tmpData == nullptr) return false;
    success = file.setPosition(entry->vdfs_offset);
    if(success) success = file.readBytes(tmpData, entry->vdfs_size);
    if(success) success = file.setPosition(file.getSize());
    size_t newOffset = file.getPosition();
    if(success) success = file.writeBytes(const_cast<const char*>(tmpData), entry->vdfs_size);
    if(success)
    {
        entry->vdfs_offset = newOffset;
    }
    delete[] tmpData;
    return success;
}

bool VDFSArchive::getFirstStoredEntry(VdfsEntry*& entry)
{
    entry = nullptr;
    for(auto& treeElement : archiveIndex.index)
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
            archiveIndex.currentStoredSize += file.getPosition() - beforeEntryRead;
            //Remove whitespace and CR as it would disturb a lookup on linunx.
            entry.vdfs_name = entry.vdfs_name.trim();

            //Regarding the VDFS file format:
            //First all directories get listed. Files afterwards.
            //After that the subdirectory contents, follow.
            if (entry.vdfs_type & Type::DIRECTORY)
            {
                LogDebug() << "Directory dump.";
                LogDebug() << "Offset: " << entry.vdfs_offset;
                LogDebug() << "Size: " << entry.vdfs_size;
                LogDebug() << "Attribute: " << entry.vdfs_attribute;
                //Recursivly add next directory stage.
                //LogDebug() << "Directory " << entry.vdfs_name;
                tree.addSubtree(entry.vdfs_name);
            }
            else //File entry
            {
                LogDebug() << "Add file to tree: " << entry.vdfs_name;
                entry.path = entry.vdfs_name;
                entry.size = entry.vdfs_size;
                entry.exists = true;
                tree.addElement(entry.path, entry);
            }

            if (entry.vdfs_type & Type::LAST)
            {
                //LogDebug() << "Last element of stage.";
                for( auto& subtree : tree.getSubtrees())
                {
                    //LogDebug() << "Join subtree with key: " << subtree.first;
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

size_t VDFSArchive::writeIndexTree(Tree<String, VdfsEntry>& tree)
{
    bool writeSuccess = true;

    for(auto& child : tree.childs)
    {
        if(writeSuccess) writeSuccess = file.writeString(child.first.fill(" ", EntryNameLength));

    }
}

std::unique_ptr<FileEntry> VDFSArchive::getFile(const Path& filepath)
{
    Path dir = filepath.getDirectory();
    Path file = filepath.getFilenameWithExt();

    bool found = false;
    auto* searchIndex = &archiveIndex.index;

    for (String stage : dir.split(Path("/")))
    {
        if (searchIndex->subtreeExist(stage))
        {
            searchIndex = &searchIndex->getSubtree(stage);
        }
        else
        {
            found = false;
            return std::unique_ptr<VdfsEntry>(new VdfsEntry(filepath, 0, 0, 0));
        }
    }
    if (searchIndex->elementExist(file))
    {
        found = true;
        return std::unique_ptr<VdfsEntry>( new VdfsEntry(searchIndex->getElement(file)));
    }
    found = false;
    return std::unique_ptr<VdfsEntry>(new VdfsEntry(filepath, 0, 0, 0));
}

bool VDFSArchive::readFile(const FileEntry& fileEntry, char* dest)
{
    const VdfsEntry* vdfsEntry = dynamic_cast<const VdfsEntry*>(&fileEntry);
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

bool VDFSArchive::readFile(const FileEntry& fileEntry, std::vector<char>& dest)
{
    const VdfsEntry* vdfsEntry = dynamic_cast<const VdfsEntry*>(&fileEntry);
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

bool VDFSArchive::readHeader(VDFSArchive::Header& header)
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

    if(result)
    {
        LogDebug() << header.toString();
    }

    return result;
}

bool VDFSArchive::writeHeader(const VDFSArchive::Header& header)
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

    if (result && header.signature.length() > SignatureLength)
    {
        LogWarn() << "Header signature too large (" << header.signature.length()
                  << ")! Cutted to max length (" << SignatureLength << ").";
        if (result) result = file.writeString(header.signature.substr(0, SignatureLength));
    }
    else  // Signature size ok
        if (result) result = file.writeString(header.signature.fill(" ", SignatureLength));

    if (result) result = file.write(header.entryCount);
    if (result) result = file.write(header.fileCount);
    if (result) result = file.write(header.creationTime);
    if (result) result = file.write(header.contentSize);
    if (result) result = file.write(header.rootOffset);
    if (result) result = file.write(header.entrySize);

    return result;
}
