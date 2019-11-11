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

/** \file cVdfsArchive
 * An archiver to store files in a virtual filesystem.
 * Implemented functionalities:
 *  - Access contents as directory tree with paths.
 *  - Add a file to the archive.
 *  - Remove a file from the archive.
 * Todo:
 * - Create a new VDFS Archive from scratch, without opening an existing.
 * - Iterate over files.
 */

#pragma once

#include <ClippedFilesystem/cIArchiver.h>
#include <ClippedFilesystem/cBinFile.h>
#include <ClippedUtils/cTime.h>
#include <ClippedUtils/DataStructures/cTree.h>
#include <sstream>
#include <list>

namespace Clipped
{
    /**
     * @brief The Type enum flags describing the type of an entry.
     */
    enum EntryType : uint32_t
    {
        BLANK       = 0,            //!< Not flagged..
        DIRECTORY   = 0x80000000u,  //!< Flags as directory.
        LAST        = 0x40000000u   //!< Flags as last entry of current tree hierachy.
    };

    /**
     * @brief The Attribute enum further attributes for entries (Windows file attributes like
     * Archive = 32).
     */
    enum EntryAttribute : uint32_t
    {
        NORMAL  = 0,    //!< The item is normal. That is, the item doesn't have any of the other values in the enumeration.
        ARCHIVE = 32u,  //!< The item is archived.
    };

    /**
     * @brief The VdfsEntry is a specialization of a regular FileEntry.
     */
    class VdfsEntry : public FileEntry
    {
        friend class VDFSArchive;
    public:
        VdfsEntry()
            : VdfsEntry(FileEntry("", 0))
        {}

        VdfsEntry(const Path& path, const MemorySize size)
            : VdfsEntry(FileEntry(path, size))
        {}

        VdfsEntry(const FileEntry& entry)
            : FileEntry(entry)
            , vdfs_name("")
            , vdfs_offset(0)
            , vdfs_size(0)
            , vdfs_type(EntryType::BLANK)
            , vdfs_attribute(EntryAttribute::ARCHIVE)
        {}

        static size_t getByteSize(const size_t vdfsNameSize)
        {
            size_t sum = vdfsNameSize;
            sum += sizeof(vdfs_offset);
            sum += sizeof(vdfs_size);
            sum += sizeof(vdfs_type);
            sum += sizeof(vdfs_attribute);
            return sum;
        }

        String vdfs_name;          //!< Name of this entry.

    private: /* Must not be modified by a user / programmer. VDFSArchiver will update this variables. */
        uint32_t vdfs_offset;      /** @brief vdfs_offset: Multipurpose field:
                                    * For files: Offset to the data of the entry inside the file.
                                    * For directories: Offset to first entry of the dir inside the index.
                                    */
        uint32_t vdfs_size;             //!< Size of the payload data.
        EntryType vdfs_type;            //!< Type of this entry.
        EntryAttribute vdfs_attribute;  //!< Attributes of this entry.
    };

    /**
     * @brief A MemoryBlock represents an memory area, identified by a position and size.
     */
    class MemoryBlock
    {
    public:
        MemoryBlock()
            : MemoryBlock(0, 0)
        {}

        MemoryBlock(const size_t offset, const size_t size)
            : offset(offset)
            , size(size)
        {}

        /**
         * @brief operator == comparison operator, which checks for equality.
         * @param rhs right hand side to check.
         * @return true, if the data is identical.
         */
        bool operator==(const MemoryBlock& rhs) const
        {
            return offset == rhs.offset && size == rhs.size;
        }

        /**
         * @brief operator != comparison operator, which checks for unequality.
         * @param rhs right hand side to check.
         * @return true, if the data isn't identical.
         */
        bool operator!=(const MemoryBlock& rhs) const
        {
            return !(*this == rhs);
        }

        size_t offset;  //!< Offset in file, where the block is located.
        size_t size;    //!< Amount of bytes.
    }; //class MemoryBlock

    /**
     * @brief The MemoryManager class handles the memory layout.
     *   It's used to store a memory map and takes care of free memory regions.
     *   handledBytes size will grow automatically, if memory in the outside region is requested.
     */
    class MemoryManager
    {
    public:
        /**
         * @brief MemoryManager creates a memory manager with an initial storage size of nothing.
         */
        MemoryManager()
            : handledBytes(0)
        {}

        /**
         * @brief MemoryManager creates a memory manager.
         * @param handledMemory the amount of bytes handled by this manager.
         */
        MemoryManager(const size_t handledMemory);

        /**
         * @brief alloc requests 'requestedBytes' amount of bytes.
         *   Memory location isn't requested. The caller gets some free storage location.
         * @param requestedBytes amount of bytes to allocate.
         * @param allocatedMemoryInfo informations about the given memory (pos/size).
         * @return true, if the memory has been allocated. False, if not.
         */
        bool alloc(const size_t requestedBytes, MemoryBlock& allocatedMemoryInfo);

        /**
         * @brief alloc requests 'requestedBytes' amount of bytes at given offset.
         * @param offset given location to mark the memory as used.
         * @param requestedBytes amount of bytes to allocate.
         * @return true, if the location is available and now allocated. false otherwise.
         */
        bool alloc(const size_t offset, const size_t requestedBytes);

        /**
         * @brief free frees the memory, specified by the freeMemoryInfo informations.
         *   Combines narrowing free memory sections to a bigger one to optimize memory gaps.
         * @param freeMemoryInfo infos about the memory to free.
         * @return true, if the memory has been freed successfully. False, if not.
         */
        bool free(const MemoryBlock& freeMemoryInfo);

        /**
         * @brief free frees the memory, specified by offset and length.
         * @param offset location of the memory.
         * @param length amount of bytes.
         * @return true, if the memory has been freed successfully. False, if not.
         */
        bool free(const size_t offset, const size_t length);

        /**
         * @brief optimizeFreeMemoryBlocks combines adjacent free memory regions to one big memory region.
         */
        void optimizeFreeMemoryBlocks();

        /**
         * @brief getHandledMemorySize getter for the amount of handled bytes.
         * @return the current amount of bytes handled by this manager.
         */
        size_t getHandledMemorySize() const
        {
            return handledBytes;
        }

        /**
         * @brief getDispersionRatio calculates the dispersion ratio.
         *  e.g. a totally compressed file without gaps has a dispersion ratio of 0.0.
         *  e.g. 50% free space gaps and 50% stored data will return 1.0.
         * @return the dispersion ratio (lower is better).
         */
        double getDispersionRatio() const
        {
            unsigned long long totalFreeBytes = 0;

            for(const auto& freeBlock : freeMemoryBlocks) //Sum up all freeMemory Blocks.
            {
                LogDebug() << "MemoryManager::getDispertionRatio Free Block: " << freeBlock.offset << " size: " << freeBlock.size;
                totalFreeBytes += freeBlock.size;
            }
            //If there are blocks and the last one is the end of handled memory
//            if(!freeMemoryBlocks.empty() &&
//               freeMemoryBlocks.back().offset + freeMemoryBlocks.back().size == handledBytes)
//            {
//                totalFreeBytes -= freeMemoryBlocks.back().size; //Ignore it - it doesn't cause dispersion.
//            }
//            else //The last free block isn't located at the end of handled memory.
//            {
//                //Nothing to do.
//            }
            LogDebug() << "Handled Bytes: " << handledBytes;
            return ((double)totalFreeBytes) / ((double)handledBytes);
        }

    private:
        std::list<MemoryBlock> freeMemoryBlocks;    //!< List of free memory blocks.
        MemorySize handledBytes;                    //!< Total size of the handled memory

        /**
         * @brief allocateInFreeBlock tries to alloc. with existing free blocks.
         * @param requestedBytes bytes to allocate.
         * @param allocatedMemoryInfo returned memory info.
         * @return true, if it has allocated. False otherwise.
         */
        bool allocateInFreeBlock(const size_t requestedBytes, MemoryBlock& allocatedMemoryInfo);

        /**
         * @brief allocateExpandLastFreeBlock enlarges the last free memory block and uses it to allocate.
         * @param requestedBytes bytes to allocate.
         * @param allocatedMemoryInfo returned memory info.
         * @return true, if it has allocated. False otherwise.
         */
        bool allocateExpandLastFreeBlock(const size_t requestedBytes, MemoryBlock& allocatedMemoryInfo);

        /**
         * @brief allocateWithNewMemory enlarges the total size of managed memory to alloc the requested memory.
         * @param requestedBytes bytes to allocate.
         * @param allocatedMemoryInfo
         * @return true, if it has allocated. False otherwise.
         */
        bool allocateWithNewMemory(const size_t requestedBytes, MemoryBlock& allocatedMemoryInfo);

    }; //class MemoryManager

    /**
     * @brief The VDFSArchive class implements the vdfs file protocol.
     */
    class VDFSArchive : public IArchiver
    {
        /**
         * @brief The VDFS File Header contains informations about the vdfs file.
         */
        class VDFSHeader
        {
            friend class VDFSArchive; //VDFSArchiver is allowed to change all header attributes.

        public:
            VDFSHeader()
                : comment("")
                , signature("")
                , entryCount(0)
                , fileCount(0)
                , creationTime((MSDOSTime32)Time())
                , contentSize(0)
                , rootOffset(0)
                , entrySize(0)
            {}

            static size_t getByteSize(const size_t commentLength, const size_t signatureLength)
            {
                size_t sum = commentLength + signatureLength;
                sum += sizeof(entryCount);
                sum += sizeof(fileCount);
                sum += sizeof(creationTime);
                sum += sizeof(contentSize);
                sum += sizeof(rootOffset);
                sum += sizeof(entrySize);
                return sum;
            }

            /**
             * @brief toString creates a textual representation of the VDFS Header.
             * @return a string describing the vdfs header contents.
             */
            String toString() const
            {
                String out = "VDFS Header: ";
                out += "\nComment:       " + comment;
                out += "\nSignature:     " + signature.trim() + " (Hex: " + signature.toHexString(false) + ")";
                out += "\nEntry Count:   " + String((int)entryCount);
                out += "\nFile Count:    " + String((int)fileCount);
                out += "\nCreation Time: " + Time(creationTime).toString();
                out += "\nContent Size:  " + String((int)contentSize);
                out += "\nRoot offset:   " + String((int)rootOffset);
                out += "\nEntry Size:    " + String((int)entrySize);
                return out;
            }

            String comment;            //!< Comment describing the file.
            String signature;          //!< A signature, e.g. a version indicator.

        private: /* Must not be changed by a user / programmer. VDFSArchiver will update this variables: */
            uint32_t entryCount;       //!< total count of entries inside this archive.
            uint32_t fileCount;        //!< count of files inside this archive.
            MSDOSTime32 creationTime;  //!< MSDOS 32 Bit time regarding the creation time.
            uint32_t contentSize;      //!< total size of this archive in Bytes.
            uint32_t rootOffset;       //!< Offset is where the index starts.
            int32_t entrySize;         //!< Size of the entry section.
        }; //class VDFSHeader

    public:
        /**
         * @brief VDFSFile constructs a vdfs file object with the specified path.
         * @param filepath
         */
        VDFSArchive(const Path& filepath);

        /**
         * @brief ~VDFSArchive destructs this archive. Calls finalize to ensure a clean index on file close.
         */
        virtual ~VDFSArchive();

        /**
         * @brief open checks if this Archiver can work with the given basePath.
         * @return true, if the initialization has been successfull.
         */
        virtual bool open() override;

        /**
         * @brief create starts the creation of a new vdfs archive with the given path.
         *   Note: It gets finally written on disk on a close or finalize call.
         * @return true, if the file has been created successfully. False otherwise.
         */
        bool create();

        /**
         * @brief close closes the vdfs archive. Updates the header / index on disk.
         * @return true, if closed successfully.
         */
        virtual bool close() override;

        /**
         * @brief finalize does closing tasks. Updates the header and vdfs index.
         * @return true, if successfully closed.
         */
        virtual bool finalize() override;

        /**
         * @brief getVdfsFile gets a vdfs file entry handle.
         * @param filepath the path inside the vdfs directory.
         * @param createIfNotFound creates the structure and file entry, if it doesn't exists.
         * @return a FileEntry handle.
         */
        FileEntry* getVdfsFile(const Path& filepath, bool createIfNotFound = false);

        /**
         * @brief getFile returns a FileEntry with informations about the stored file.
         * @param filename file to lookup.
         * @param create creates this entry in the index, if it doesn't exists.
         * @return FileEntry pointer or a nullptr, if not found.
         */
        virtual FileEntry* getFile(const Path& filepath) override;

        /**
         * @brief createFile gets a handle to a new or to be overriden file.
         * @param filepath of the new file.
         * @return FileEntry pointer in any case.
         */
        virtual FileEntry* createFile(const Path& filepath) override;

        /** \copydoc cIArchiver::readFile(const FileEntry&,char*) */
        virtual bool readFile(const FileEntry* fileEntry, char* dest) override;

        /** \copydoc cIArchiver::readFile(const FileEntry&,std::vector<char>&) */
        virtual bool readFile(const FileEntry* fileEntry, std::vector<char>& dest) override;

        /** \copydoc cIArchiver::writeFile(FileEntry&,const char*) */
        virtual bool writeFile(FileEntry* fileEntry, const char* src, const size_t length) override;

        /** \copydoc cIArchiver::writeFile(FileEntry&,const std::vector<char>&) */
        virtual bool writeFile(FileEntry* fileEntry, const std::vector<char>& src) override;

        virtual bool removeFile(FileEntry* fileEntry) override;

        double getDispersionRatio() const
        {
            return memoryManager.getDispersionRatio();
        }

    private:
        BinFile file;                   //!< File handle to actually read/write to a file.
        VDFSHeader header;              //!< Header of the vdfs file.
        uint32_t directoryOffsetCount;  //!< Counter for index writing. Offset to directory contents inside index.
        bool modified;                  //!< To be set if the index changes. finalize() will update it on archive closing.
        MemoryManager memoryManager;    //!< Memory manager, that keeps track of used/free memory blocks.

        /**
         * @brief The VDFSIndex struct contains attributes about the index section of a vdfs archive.
         */
        struct
        {
            MemorySize currentStoredSize = 0;   //!< Size of the index that is currently stored on the file in bytes.
            Tree<String, VdfsEntry> indexTree;  //!< Root stage of hierachical entry list.
        } vdfsIndex;                            //!< informations about the index and it's properties.

        //VDFS Archive specific properties:
        static const String CommentFillChar;    //!< Character used to fill up strings to target width.
        static const size_t CommentLength;      //!< The length of the comment section inside the header.
        static const size_t SignatureLength;    //!< The length of the signature section inside the header.
        static const size_t EntryNameLength;    //!< The length of an entries name.
        static const size_t HeaderLength;       //!< The length of the header area, after which the index starts.

        /**
         * @brief readHeader reads the vdfs header.
         * @param header data
         * @return true if successfully read, false otherwise.
         */
        bool readHeader(VDFSHeader& header);

        /**
         * @brief writeHeader writes the given header to the file.
         * @param header to be written.
         * @return true if successfully written, false otherwise.
         */
        bool writeHeader(const VDFSArchive::VDFSHeader& header);

        /**
         * @brief readVDFSIndex reads the file index for the vdfs file.
         * @return true, if index was successfully read.
         */
        bool readVDFSIndex();

        /**
         * @brief writeVDFSIndex writes the index for the vdfs file.
         * @return true, if index was successfully written.
         */
        bool writeVDFSIndex();

        /**
         * @brief readIndexTree reads all entries to a directory tree.
         *   Recursively called for subtrees.
         * @param tree to store objects in.
         * @return Counter of extracted elements in this stage.
         */
        size_t readIndexTree(Tree<String, VdfsEntry>& tree);

        /**
         * @brief writeIndexTree writes the local directory tree to the vdfs file.
         * @param tree to write.
         * @return true, if index tree/subtree has been successfully written.
         */
        bool writeIndexTree(Tree<String, VdfsEntry>& tree);

        /**
         * @brief allocIndexMemory assures, that the index has free space at the right position.
         *   Moves data to the end, if required, to free memeory for the index.
         * @return true, if the index has enaugh place now.
         */
        bool allocIndexMemory();

        /**
         * @brief getFirstStoredEntry gets the entry with the smallest offset from the index.
         * @param entry pointer to vdfsEntry with smallest offset.
         * @return true, if an entry has been found. false otherwise.
         */
        bool getFirstStoredEntry(VdfsEntry*& entry);

        /**
         * @brief moveEntryDataToTheEnd moves the data of an entry to the file end and updates the offset in the index.
         * @param entry to move.
         * @return true, if moved successfully.
         */
        bool moveEntryDataToTheEnd(VdfsEntry*& entry);

        /**
         * @brief checkFileEntryIsVdfsEntry
         * @param check pointer to FileEntry object to check.
         * @param target pointer reference to write the casted VdfsEntry pointer at.
         * @return true, if the FileEntry is an VdfsEntry..
         */
        bool checkFileEntryIsVdfsEntry(FileEntry* check, VdfsEntry*& target) const;

        /**
         * @brief getFreeMemoryOffset returns a file position offset with enaugh place to store the content.
         * @param requiredBytes amount of bytes to store.
         * @return an offset to write the content to.
         */
        size_t getFreeMemoryOffset(const size_t requiredBytes);
    }; //class VDFSArchive

}  // namespace Clipped
