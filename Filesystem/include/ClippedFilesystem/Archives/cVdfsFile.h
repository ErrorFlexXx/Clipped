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

#include <sstream>
#include <ClippedFilesystem/cBinFile.h>
#include <ClippedUtils/cTime.h>
#include <ClippedUtils/DataStructures/cTree.h>

namespace Clipped
{
    /**
     * @brief The VDFSFile class implements the vdfs file protocol.
     */
    class VDFSFile : BinFile
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
        struct Entry
        {
            String name;          //!< Name of this entry.
            uint32_t offset;      //!< Offset specifying the position of the entry inside the file.
            uint32_t size;        //!< Size of the payload data.
            Type type;            //!< Type of this entry.
            Attribute attribute;  //!< Attributes of this entry.
        };

        /**
         * @brief VDFSFile constructs a vdfs file object with the specified path.
         * @param filepath
         */
        VDFSFile(const Path& filepath, bool ignoreCase = true);

        /**
         * @brief open opens the archive in specified mode.
         * @param accessMode permissions to open the file with.
         * @return true if opened successfully, false otherwise.
         */
        bool open(const FileAccessMode& accessMode);

        /**
         * @brief close closes the file handle.
         */
        void close() { this->BinFile::close(); }

        /**
         * @brief exists checks if the file exists.
         * @return true if the file exists on the fs, false otherwise.
         */
        bool exists() const { return this->BinFile::exists(); }

        /**
         * @brief getSize returns the size on fs.
         * @return the FileSize.
         */
        MemorySize getSize() const { return this->BinFile::getSize(); }

        /**
         * @brief remove deletes this vdfs file on the fs.
         */
        bool remove() { return this->BinFile::remove(); }

        /**
         * @brief getEntry search for the given entry path inside the index.
         * @param vdfsPath vdfs path to entry
         * @param found [out] flags if the entry has been found.
         * @param ignoreCase ignores the letter case or not.
         * @return filled Entry, if found.
         */
        Entry getEntry(const Path& vdfsPath, bool& found);

        /**
         * @brief getEntries returns entries with matching file extension
         * @param extension to search for
         * @param vdfsPath optionally only in a particular directory
         * @param recursive by default recursive searching from given vdfsPath origin
         * @return a container of Entries.
         */
        std::vector<Entry> getEntries(const Path& extension, const Path& vdfsPath = "",
                                      bool recursive = true);

        /**
         * @brief Read reads a given entry into a vector.
         * @param data to store the data in.
         * @param entry to store.
         * @return true on success, false otherwise.
         */
        bool Read(std::vector<char>& data, const Entry& entry);

        /**
         * @brief getFilepath returns the filepath of this instance.
         * @return the path object.
         */
        const Path& getFilepath() const;

    private:
        Header header;  //!< Header of the vdfs file.

        Tree<String, Entry> index;  //!< Root stage of hierachical entry list.

        bool ignoreCase;  //!< If set, you don't have to worry about the character case.

        static const String
            CommentFillChar;  //!< Character used to fill up strings to target width.

        static const size_t
            CommentLength;  //!< The length of the comment section inside the header.

        static const size_t
            SignatureLength;  //! The length of the signature section inside the header.

        static const size_t EntryNameLength;  //!< The length of an entries name.

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
        bool writeHeader(const VDFSFile::Header& header);

        /**
         * @brief readVDFSIndex creates the file index for the vdfs file.
         * @return true if index was successfully read.
         */
        bool readVDFSIndex();

        /**
         * @brief createIndex reads all entries to a directory tree.
         * @param tree to store objects in.
         * @return true if successfully read
         */
        bool createIndexTree(Tree<String, Entry>& tree);

        /**
         * @brief findExtEntries search for entries with matching extension.
         * @param entries storage for found entries.
         * @param ext to search for.
         * @param searchIndex to search in
         * @param recursive wether to recurse further into subtrees or not.
         * @return
         */
        std::vector<Entry>& findExtEntries(std::vector<Entry>& entries, const Path& ext,
                                           Tree<String, Entry>* searchIndex, bool recursive);
    };

}  // namespace Clipped
