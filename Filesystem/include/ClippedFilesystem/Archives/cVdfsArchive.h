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
    enum Type : uint32_t
    {
        DIRECTORY = 0x80000000,  //!< Flags as directory.
        LAST = 0x40000000        //!< Flags as last entry of current tree hierachy.
    };

    /**
     * @brief The Attribute enum further attributes for entries (Windows file attributes like
     * Archive = 32).
     */
    enum Attribute : uint32_t
    {
    };

    /**
     * @brief The Entry struct represents all stored properties of a vdfs entry.
     */
    class VdfsEntry : public FileEntry
    {
    public:
        VdfsEntry()
            : FileEntry("", 0, false, false)
        {}

        VdfsEntry(const FileEntry& entry)
            : FileEntry(entry)
        {}

        VdfsEntry(const Path& path, const MemorySize size, bool exists, bool override)
            : FileEntry(path, size, exists, override)
        {}

        String vdfs_name;          //!< Name of this entry.
        uint32_t vdfs_offset;      //!< Offset specifying the position of the entry inside the file.
        uint32_t vdfs_size;        //!< Size of the payload data.
        Type vdfs_type;            //!< Type of this entry.
        Attribute vdfs_attribute;  //!< Attributes of this entry.
    };

    /**
     * @brief The VDFSArchive class implements the vdfs file protocol.
     */
    class VDFSArchive : public IArchiver
    {
        /**
         * @brief The VDFS File Header contains informations about the vdfs file.
         */
        struct Header
        {
            String comment;            //!< Comment describing the file.
            String signature;          //!< A signature, e.g. a version indicator.
            uint32_t entryCount;       //!< total count of entries inside this archive.
            uint32_t fileCount;        //!< count of files inside this archive.
            MSDOSTime32 creationTime;  //!< MSDOS 32 Bit time regarding the creation time.
            uint32_t contentSize;      //!< total size of this archive in Bytes.
            uint32_t rootOffset;       //!< Offset to first entry in the archive.
            int32_t entrySize;         //!< Size of the entry section.
        };

    public:
        /**
         * @brief VDFSFile constructs a vdfs file object with the specified path.
         * @param filepath
         */
        VDFSArchive(const Path& filepath);

        /**
         * @brief open checks if this Archiver can work with the given basePath.
         * @return true, if the initialization has been successfull.
         */
        virtual bool open() override;

        /**
         * @brief getFile returns a FileEntry with informations about the stored file.
         * @param filename file to lookup.
         * @return a FileEntry as unique ptr (necessary - polymorphic object (VDFSFileEntry)).
         */
        virtual std::unique_ptr<FileEntry> getFile(const Path& filepath) override;

        /** \copydoc cIArchiver::readFile(const FileEntry&,char*) */
        virtual bool readFile(const FileEntry& fileEntry, char* dest) override;

        /** \copydoc cIArchiver::readFile(const FileEntry&,std::vector<char>&) */
        virtual bool readFile(const FileEntry& fileEntry, std::vector<char>& dest) override;

        /**
         * @brief getEntries returns entries with matching file extension
         * @param extension to search for
         * @param vdfsPath optionally only in a particular directory
         * @param recursive by default recursive searching from given vdfsPath origin
         * @return a container of Entries.
         */
        std::vector<VdfsEntry> getEntries(const Path& extension, const Path& vdfsPath = "",
                                      bool recursive = true);

    private:
        BinFile file;   //!< File handle to actually read/write to a file.
        Header header;  //!< Header of the vdfs file.

        /**
         * @brief The VDFSIndex struct contains attributes about the index section of a vdfs archive.
         */
        struct VDFSIndex
        {
            VDFSIndex() : size(0) {}

            MemorySize size;                //!< Size of the index in bytes.
            Tree<String, VdfsEntry> index;  //!< Root stage of hierachical entry list.
        } archiveIndex; //!< informations about the index and it's properties.

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
        bool readHeader(Header& header);

        /**
         * @brief writeHeader writes the given header to the file.
         * @param header to be written.
         * @return true if successfully written, false otherwise.
         */
        bool writeHeader(const VDFSArchive::Header& header);

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
         * @brief createIndex reads all entries to a directory tree.
         *   Recursively called for subtrees.
         * @param tree to store objects in.
         * @return Counter of extracted elements in this stage.
         */
        size_t createIndexTree(Tree<String, VdfsEntry>& tree);

    }; //class VDFSArchive

}  // namespace Clipped
