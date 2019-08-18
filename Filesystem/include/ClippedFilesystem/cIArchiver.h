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

#include <ClippedUtils/cPath.h>
#include <ClippedUtils/cMemory.h>
#include <memory> //std::unique_ptr

namespace Clipped
{
    /**
     * @brief The class FileEntry represents an file object.
     *  With this FileEntry a user can query or add a file from or to an IArchiver implementation.
     */
    class FileEntry
    {
    public:
        FileEntry();

        FileEntry(const Path& path, const MemorySize size);

        virtual ~FileEntry() {}

        const Path& getPath() const; //!< Getter for the path.

        const MemorySize getSize() const; //!< Getter for the size.

        bool getExists() const; //!< Getter for the exists flag.

        bool getOverride() const; //!< Getter for the override flag.

        friend class VDFSArchive; //VDFS Archive needs to fill in data for an entry.

    private:
        Path path;      //!< The path of this entry - relative to the FileManager's basePath.
        MemorySize size;//!< The memory size of this entry in bytes.
    }; //class FileEntry

    /**
     * @brief The IArchiver class implements an interface to list/read/write/update file entries in a hierachical organized file storages.
     *   This hierachical file storage could be a directory stored in the filesystem of the operating system or
     *   inside a virtual filesystem like a zip file or any other archive.
     */
    class IArchiver
    {
    public:
        IArchiver(const Path& basePath);
        
        /**
         * @brief ~IArchiver the inheriting class dtor is called, if this instance gets destroyed.
         */
        virtual ~IArchiver();

        /**
         * @brief open checks if this Archiver can work with the given basePath.
         * @return true, if the initialization has been successfull.
         */
        virtual bool open() = 0;

        /**
         * @brief close closes this archives instance. Triggers closing tasks that might be required, by the specific archiver.
         * @return true, if closed successfully.
         */
        virtual bool close() = 0;

        /**
         * @brief finalize function to actually write contents to disk. To be overriden, if required by archiver.
         *   Required for some archivers, that can't change single entries, without updating an index or other data, too.
         * @return true, if written successfully.
         */
        virtual bool finalize();

        //TODO: Interface methods iterator based multifile access.

        //Interface methods (Single file access):
        /**
         * @brief getFile looks up a file in the file storage.
         * @param filepath the filepath of the requested file.
         * @return file handle or nullptr, if file doesn't exist.
         */
        virtual FileEntry* getFile(const Path& filepath) = 0;

        /**
         * @brief searchFile looks up a file in the file storage.
         *   Searches for it recursivly.
         * @param filename to look for.
         * @return a pointer to the entry or nullptr, if not found.
         */
        virtual FileEntry* searchFile(const Path& filename) = 0;

        /**
         * @brief createFile creates a file handle to add content or overwrite a file in the file storage.
         * @param filepath to the file.
         * @return a file handle - either an existing or a new created.
         */
        virtual FileEntry* createFile(const Path& filepath) = 0;

        /**
         * @brief readFile reads the file data to the given dest pointer.
         * @param fileEntry describing the file to read.
         * @param dest pointer to the memory to store the data at.
         * @return true, if the file has been read successfully.
         */
        virtual bool readFile(const FileEntry* fileEntry, uint8_t* dest) = 0;

        /**
         * @brief readFile reads the file data to the given data container.
         * @param fileEntry describing the file to read.
         * @param dest data container, to store the read data in.
         * @return true, if the file has been read successfully.
         */
        virtual bool readFile(const FileEntry* fileEntry, std::vector<uint8_t>& dest) = 0;

        /**
         * @brief writeFile writes given data to the file storage with a direct data pointer.
         * @param fileEntry describing the file to write.
         * @param src the data storage to be written.
         * @param length the amount of bytes from src data to write.
         * @return true, if the file has been written successfully.
         */
        virtual bool writeFile(FileEntry* fileEntry, const uint8_t* src, const size_t length) = 0;

        /**
         * @brief writeFile writes given data to the file storage from a std::vector containing the data.
         * @param fileEntry describing the file to write.
         * @param src the data storage to be written.
         * @return true, if the file has been written successfully.
         */
        virtual bool writeFile(FileEntry* fileEntry, const std::vector<uint8_t>& src) = 0;

        /**
         * @brief removeFile removes a file from the archive.
         * @param fileEntry to remove.
         * @return true, if removed successfully.
         */
        virtual bool removeFile(FileEntry* fileEntry) = 0;

        /**
         * @brief getBasePath returns the basePath of this instance.
         *   This can be the directory path or the full path to the archive file, this implementation
         *   works with.
         * @return the basePath.
         */
        const Path& getBasePath() const;

    protected:
        Path basePath; //!< Path to archive file or directory to work with.
    }; //class IArchiver

} //namespace Clipped
