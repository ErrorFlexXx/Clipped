/*
** Clipped -- a Multipurpose C++ Library.
**
** Copyright (C) 2019-2020 Christian Löpke. All rights reserved.
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**
** [ MIT license: http://www.opensource.org/licenses/mit-license.php ]
*/

#include "Archives/cVdfsArchive.h"
#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cPath.h>

using namespace Clipped;

const String VDFSArchive::CommentFillChar = "\x1A";
const size_t VDFSArchive::CommentLength = 256;
const size_t VDFSArchive::SignatureLength = 16;
const size_t VDFSArchive::EntryNameLength = 64;
const size_t VDFSArchive::HeaderLength = 296;

/* ========================================================= */

MemoryManager::MemoryManager(const size_t handledMemory)
    : handledBytes(handledMemory)
{
    //Initially one big free block:
    freeMemoryBlocks.push_back(MemoryBlock(0, handledMemory));
}

bool MemoryManager::alloc(const size_t requestedBytes, MemoryBlock& allocatedMemoryInfo)
{
    bool hasAllocated = false;

    if(false == allocateInFreeBlock(requestedBytes, allocatedMemoryInfo))
    {
        if(false == allocateExpandLastFreeBlock(requestedBytes, allocatedMemoryInfo))
        {
            if(false == allocateWithNewMemory(requestedBytes, allocatedMemoryInfo))
            {
                LogError() << "Allocate failed!"; //Should never happen.
                hasAllocated = false;
            }
            else
            {
                hasAllocated = true; //Allocated by adding new memory to the manager.
            }
        }
        else
        {
            hasAllocated = true; //Allocated by expansion of last free memory block.
        }
    }
    else
    {
        hasAllocated = true; //Allocated by using a free block.
    }

    return hasAllocated; //Return the final result of allocation.
}

bool MemoryManager::alloc(const size_t offset, const size_t requestedBytes)
{
    if(0 == requestedBytes)
    {
        return true; //Successfully allocated nothing. You're welcome :)
    }

    if(offset < handledBytes) //Memory already managed
    {
        //Search for memory block containing the storage segment.
        bool foundBlock = false;

        for(std::list<MemoryBlock>::iterator freeBlock = freeMemoryBlocks.begin(); freeBlock != freeMemoryBlocks.end(); freeBlock++)
        {
            if(freeBlock->offset <= offset && (offset + requestedBytes) <= freeBlock->offset + freeBlock->size)
            {
                const size_t freeBefore = offset - freeBlock->offset;
                const size_t freeAfter = (freeBlock->offset + freeBlock->size) - (offset + requestedBytes);
                if(0 == freeBefore && 0 == freeAfter) //Block used entirely
                {
                    freeMemoryBlocks.erase(freeBlock); //Remove free block
                }
                else if((0 == freeBefore) ^ (0 == freeAfter)) //Only one side has free memory left.
                {
                    if(0 != freeBefore) //Memory located at the beginning
                    {
                        freeBlock->size = freeBefore;
                    }
                    else //Memory located at the end
                    {
                        freeBlock->offset = freeBlock->offset + freeBlock->size - freeAfter; //Update offset
                        freeBlock->size = freeAfter;
                    }
                }
                else //Both sides has free memory left.
                {
                    MemoryBlock freeBeforeBlock = *freeBlock; //Copy current freeBlock.
                    freeBeforeBlock.size = freeBefore;

                    MemoryBlock freeAfterBlock;
                    freeAfterBlock.offset = freeBlock->offset + freeBlock->size - freeAfter;
                    freeAfterBlock.size = freeAfter;

                    *freeBlock = freeAfterBlock; //Current freeBlock is the new freeAfterBlock.
                    freeMemoryBlocks.insert(freeBlock, freeBeforeBlock); //FreeBefore block gets inserted in front.
                }
                foundBlock = true;
                break; //Stop searching - We found the block.
            }
            else //Memory not contained in this block.
            {
                //Nothing to do - Go on with searching.
            }
        }
        return foundBlock;
    }
    else //Memory currently unmanaged - Expand handledBytes area
    {
        const size_t freeBefore = offset - handledBytes;
        if(0 < freeBefore) //If there is free memory in front
        {
            freeMemoryBlocks.push_back(MemoryBlock(handledBytes, freeBefore)); //Add it to freeMemBlocks.
        }
        else //No free memory in front
        {
            //Nothing to do.
        }
        handledBytes += freeBefore;
        handledBytes += requestedBytes;
        return true; //Has allocated.
    }
    return false; //Should never be reached.
}

bool MemoryManager::allocateInFreeBlock(const size_t requestedBytes, MemoryBlock& allocatedMemoryInfo)
{
    bool hasAllocated = false;

    for(auto& freeBlock : freeMemoryBlocks) //Search for a block with enaugh free memory.
    {
        if(freeBlock.size >= requestedBytes) //Enaugh size to store the element.
        {
            allocatedMemoryInfo.offset = freeBlock.offset;
            allocatedMemoryInfo.size = requestedBytes;
            if(freeBlock.size > requestedBytes) //Left over free memory ?
            {
                //Adjust offset and size.
                freeBlock.offset += requestedBytes;
                freeBlock.size -= requestedBytes;
            }
            else //Free block completely filled up now.
            {
                freeMemoryBlocks.remove(freeBlock); //Remove it from the list of free blocks.
            }
            hasAllocated = true; //Memory allocated now.
            break; //Stop searching for free memory block.
        }
        else //This free block is to small.
        {
            //Nothing to do - continue searching for matching free block.
        }
    } //End for search free block

    return hasAllocated;
}

bool MemoryManager::allocateExpandLastFreeBlock(const size_t requestedBytes, MemoryBlock& allocatedMemoryInfo)
{
    bool hasAllocated = false;

    //Extend last block of free memory to make it fit.
    if(!freeMemoryBlocks.empty()) //If there is at least one block with free memory.
    {
        MemoryBlock& lastFreeMemoryBlock = freeMemoryBlocks.back(); //The last block of free memory
        size_t missingBytes = requestedBytes - lastFreeMemoryBlock.size;
        lastFreeMemoryBlock.size += missingBytes; //gets now extended by the missing amount of bytes
        handledBytes += missingBytes; //the manager gets it's counter of total handled bytes updated
        allocatedMemoryInfo.offset = lastFreeMemoryBlock.offset; //The allocated memory gets returned.
        allocatedMemoryInfo.size = lastFreeMemoryBlock.size;
        freeMemoryBlocks.remove(lastFreeMemoryBlock); //And the now completely used block gets removed from the free mem list.
        hasAllocated = true;
    }
    else //No free memory block left to expand.
    {
        hasAllocated = false;
    }

    return hasAllocated;
}

bool MemoryManager::allocateWithNewMemory(const size_t requestedBytes, MemoryBlock& allocatedMemoryInfo)
{
    bool hasAllocated = false;

    allocatedMemoryInfo.offset = handledBytes;
    allocatedMemoryInfo.size = requestedBytes;
    handledBytes += requestedBytes;
    hasAllocated = true;

    return hasAllocated;
}

bool MemoryManager::free(const MemoryBlock& freeMemoryInfo)
{
    bool hasFreed = false;

    if(freeMemoryInfo.offset + freeMemoryInfo.size <= handledBytes) //In range of the manager ?
    {
        size_t freeMemoryEndPosition = freeMemoryInfo.offset + freeMemoryInfo.size;

        //Search for right hand side free block.
        for(auto it = freeMemoryBlocks.begin(); it != freeMemoryBlocks.end(); it++)
        {
            if(freeMemoryEndPosition <= it->offset) //Are we in front of following free block ?
            {
                freeMemoryBlocks.insert(it, freeMemoryInfo); //Insert in front of right hand side free block.
                hasFreed = true;
                break; //Stop searching.
            }
            else
            {
                //Nothing to do -- Free block isn't located on the right side. Go on with searching.
            }
        } //forsearch right hand side free memory block.
        if(false == hasFreed) //No right hand side free block found ?
        {
            //Append new free memory block to the end of the list.
            freeMemoryBlocks.push_back(freeMemoryInfo);
            hasFreed = true;
        }
        optimizeFreeMemoryBlocks(); //Combine adjacent free memory blocks, if possible.
    }
    else //Should never happen. Memory, which shall be freed, isn't in range of this manager.
    {
        hasFreed = false;
        LogError() << "Bug. Cannot free memory I'm not responsible for!";
    }
    return hasFreed;
}

bool MemoryManager::free(const size_t offset, const size_t length)
{
    return this->free(MemoryBlock(offset, length));
}

void MemoryManager::optimizeFreeMemoryBlocks()
{
    auto leftIt = freeMemoryBlocks.begin();

    if(leftIt != freeMemoryBlocks.end())
    {
        auto rightIt = freeMemoryBlocks.begin();
        rightIt++;
        while(rightIt != freeMemoryBlocks.end()) //Optimize the whole list of free memory.
        {
            if(leftIt->offset + leftIt->size == rightIt->offset) //Adjacent blocks ?
            {
                leftIt->size += rightIt->size;      //Combine free memory blocks.
                freeMemoryBlocks.erase(rightIt);    //Remove right one.
                continue; //Skip iterator incrementing. Recheck current left with new right one.
            }
            else
            {
                //Nothing to do -- No adjacent blocks. Blocks aren't 'touching' each other.
            }
            //Move check scope to the next free blocks.
            leftIt++;
            rightIt++;
        } //while iterate over all free blocks.
    }
    else
    {
        //Nothing to do -- No free memory blocks available.
    }
}

/* =================== Memory Manager =================== */

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

bool VDFSArchive::create()
{
    bool result = file.open(FileAccessMode::TRUNC);
    header.rootOffset = static_cast<uint32_t>(VDFSHeader::getByteSize(CommentLength, SignatureLength));
    header.entrySize = 80;
    memoryManager.alloc(0, header.rootOffset); //Mark header region as used.
    modified = true;
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
    memoryManager.alloc(header.rootOffset, file.getPosition() - header.rootOffset); //Mark index area as used.
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
            return true; //No elements found, that may collide with the header (Blank archive).
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
    for(auto& treeElement : vdfsIndex.indexTree) //Search for the very first entry in the data storage.
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
                if(!memoryManager.alloc(entry.vdfs_offset, entry.vdfs_size)) //Mark storage as used.
                {
                    LogWarn() << "VDFS Index corrupt! File: " << entry.vdfs_name << " offset: " << entry.vdfs_offset << " already used!";
                }
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

    size_t i = 1; //Written entries in this stage.
    const size_t entriesOfStage = tree.countLocalElements() + tree.countLocalSubtrees();
    directoryOffsetCount += entriesOfStage;  //Add entries of this stage to global counter.

    size_t subdirectoryOffsetCount = directoryOffsetCount; //Total entries of this stage
    for(auto& child : tree.childs) //Write directories.
    {
        uint32_t entryType = EntryType::DIRECTORY;
        if(writeSuccess) writeSuccess = file.writeString(child.first.fill(" ", EntryNameLength));
        if(writeSuccess) writeSuccess = file.write((uint32_t) subdirectoryOffsetCount);
        if(writeSuccess) writeSuccess = file.write((uint32_t) 0); //Size
        if(i++ == entriesOfStage) //Last element of this stage ?
            entryType |= EntryType::LAST;
        if(writeSuccess) writeSuccess = file.write((uint32_t) entryType);
        if(writeSuccess) writeSuccess = file.write((uint32_t) 0); //Attribute

        subdirectoryOffsetCount += child.second.countChildsAndElements();
    }

    for(auto& element : tree.elements) //Write local elements.
    {
        uint32_t entryType = EntryType::BLANK;
        if(i++ == entriesOfStage) //Last element of this stage ?
        {
            entryType |= EntryType::LAST;
        }
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
    bool removed = false;           //Variable stating if the entry has been removed.
    VdfsEntry* vdfsEntry = nullptr; //The Vdfs object instance, if casted successfully.

    if(false != checkFileEntryIsVdfsEntry(fileEntry, vdfsEntry))
    {
        const size_t offset = vdfsEntry->vdfs_offset;
        const size_t sizeOfFile = vdfsEntry->vdfs_size;

        removed = vdfsIndex.indexTree.removeElement(fileEntry->getPath(), vdfsEntry);
        if(removed) //File removed -- update index
        {
            //Update header:
            modified = true; //Update index on disk, if archive gets closed.
            header.fileCount--;
            header.contentSize -= static_cast<uint32_t>(sizeOfFile);
            header.entryCount--;
            memoryManager.free(offset, sizeOfFile);
        }
        else //Entry wasn't removed.
        {
            //Nothing to do. Index doen't need to be updated.
        }
    }
    else //Given FileEntry isn't a vdfs entry!
    {
        LogError() << "Handle given, that wasn't created by an VDFSArchive instance!";
        removed = false;
    }
    return removed;
}

bool VDFSArchive::readHeader(VDFSArchive::VDFSHeader& header)
{
    bool result = true;
    header.comment.clear();
    header.signature.clear();

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

    memoryManager.alloc(0, header.rootOffset); //Mark memory as used for the header region.

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

size_t VDFSArchive::getFreeMemoryOffset(const size_t requiredBytes)
{
    MemoryBlock storage;
    if(memoryManager.alloc(requiredBytes, storage))
    {
        return storage.offset;
    }
    else
    {
        LogError() << "Bug! MemoryManager alloc should never return false!";
        memoryManager.alloc(file.getSize(), requiredBytes);
        return file.getSize(); //Fallback: append to file.
    }
}
