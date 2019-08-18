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
 * - Maintain memory gaplist to fill in new contents in gaps, if possible.
 * - Create a new VDFS Archive from scratch, without opening an existing.
 * - Iterate over files.
 */

#pragma once

#include <ClippedFilesystem/cIArchiver.h>
#include <ClippedFilesystem/cBinFile.h>
#include <ClippedUtils/cTime.h>
#include <ClippedUtils/DataStructures/cTree.h>
#include <sstream>

namespace Clipped
{
    /**
     * @brief The Type enum flags describing the type of an entry.
     */
    enum EntryType : uint32_t
    {
        BLANK = 0,               //!< Not flagged..
        DIRECTORY = 0x80000000,  //!< Flags as directory.
        LAST = 0x40000000        //!< Flags as last entry of current tree hierachy.
    };

    /**
     * @brief The Attribute enum further attributes for entries (Windows file attributes like
     * Archive = 32).
     */
    enum EntryAttribute : uint32_t
    {
        NORMAL = 0,  //!< The item is normal. That is, the item doesn't have any of the other values in the enumeration.
        ARCHIVE = 32 //!< The item is archived.
    };

    /**
     * @brief The Entry struct represents all stored properties of a vdfs entry.
     */
    class VdfsEntry : public FileEntry
    {
    public:
        VdfsEntry()
            : FileEntry("", 0)
        {}

        VdfsEntry(const FileEntry& entry)
            : FileEntry(entry)
        {}

        VdfsEntry(const Path& path, const MemorySize size)
            : FileEntry(path, size)
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
        uint32_t vdfs_offset;      /** @brief vdfs_offset: Multipurpose field:
                                    * For files: Offset to the data of the entry inside the file.
                                    * For directories: Offset to first entry of the dir inside the index.
                                    */
        uint32_t vdfs_size;        //!< Size of the payload data.
        EntryType vdfs_type;            //!< Type of this entry.
        EntryAttribute vdfs_attribute;  //!< Attributes of this entry.
    };

    /**
     * @brief The VDFSArchive class implements the vdfs file protocol.
     */
    class VDFSArchive : public IArchiver
    {
        /**
         * @brief The VDFS File Header contains informations about the vdfs file.
         */
        struct VDFSHeader
        {
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
            uint32_t entryCount;       //!< total count of entries inside this archive.
            uint32_t fileCount;        //!< count of files inside this archive.
            MSDOSTime32 creationTime;  //!< MSDOS 32 Bit time regarding the creation time.
            uint32_t contentSize;      //!< total size of this archive in Bytes.
            uint32_t rootOffset;       //!< Offset is where the index starts.
            int32_t entrySize;         //!< Size of the entry section.
        };

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
         * @brief searchVdfsFile search for a file recursivly in the index.
         * @param filename to look for.
         * @param tree to look at.
         * @return a FileEntry, if found or a nullptr otherwise.
         */
        FileEntry* searchVdfsFile(const Path& filename, Tree<String, VdfsEntry>& tree);

        /**
         * @brief getFile returns a FileEntry with informations about the stored file.
         * @param filename file to lookup.
         * @param create creates this entry in the index, if it doesn't exists.
         * @return FileEntry pointer or a nullptr, if not found.
         */
        virtual FileEntry* getFile(const Path& filepath) override;

        /** \copydoc cIArchiver::searchFile */
        virtual FileEntry* searchFile(const Path& filename) override;

        /**
         * @brief createFile gets a handle to a new or to be overriden file.
         * @param filepath of the new file.
         * @return FileEntry pointer in any case.
         */
        virtual FileEntry* createFile(const Path& filepath) override;

        /** \copydoc cIArchiver::readFile(const FileEntry&,char*) */
        virtual bool readFile(const FileEntry* fileEntry, uint8_t* dest) override;

        /** \copydoc cIArchiver::readFile(const FileEntry&,std::vector<char>&) */
        virtual bool readFile(const FileEntry* fileEntry, std::vector<uint8_t>& dest) override;

        /** \copydoc cIArchiver::writeFile(FileEntry&,const char*) */
        virtual bool writeFile(FileEntry* fileEntry, const uint8_t* src, const size_t length) override;

        /** \copydoc cIArchiver::writeFile(FileEntry&,const std::vector<char>&) */
        virtual bool writeFile(FileEntry* fileEntry, const std::vector<uint8_t>& src) override;

        virtual bool removeFile(FileEntry* fileEntry) override;

    private:
        BinFile file;                   //!< File handle to actually read/write to a file.
        VDFSHeader header;              //!< Header of the vdfs file.
        uint32_t directoryOffsetCount;  //!< Counter for index writing. Offset to directory contents inside index.
        bool modified;                  //!< To be set if the index changes. finalize() will update it on archive closing.

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
        static const size_t SignatureLength;    //! The length of the signature section inside the header.
        static const size_t EntryNameLength;    //!< The length of an entries name.

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
         * @return true, if an entry has been found (is false if the index is empty).
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
        size_t getFreeMemoryOffset(const size_t requiredBytes) const;
    }; //class VDFSArchive

}  // namespace Clipped
