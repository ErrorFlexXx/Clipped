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

namespace Clipped
{
    /**
     * @brief The class FileEntry represents an file object.
     *  With this FileEntry a user can query the file data from an FileManager implementation.
     */
    class FileEntry
    {
    public:
        FileEntry(const Path& path, const MemorySize size, bool exists, bool override);

        const Path& GetPath() const; //!< Getter for the path.

        const MemorySize GetSize() const; //!< Getter for the size.

        bool Exists() const; //!< Getter for the exists flag.

        bool Override() const; //!< Getter for the override flag.

    private:
        Path path;      //!< The path of this entry - relative to the FileManager's basePath.
        MemorySize size;//!< The memory size of this entry in bytes.
        bool exists;    //!< Flag stating, if the file exists.
        bool override;  //!< Flag stating, if the file shall be overriden.
    };



    /**
     * @brief The IFileManager class implements an interface to list/read/write file entries in a hierachical organized file storage.
     *   This hierachical file storage could be a directory stored in the filesystem of the operating system or
     *   inside a virtual filesystem like a zip file or any other archive.
     */
    class IFileManager
    {
    public:
        IFileManager(const Path& basePath);
        
        /**
         * @brief ~IFileManager the inheriting class dtor is called, if this instance gets destroyed.
         */
        virtual ~IFileManager();

        //TODO: Interface methods iterator based multifile access.

        //Interface methods (Single file access):
        /**
         * @brief GetFile looks up a file in the file storage.
         *   Note: A caller will always get a FileEntry object returned, that has e.g. the existing flag set.
         * @param filename the name of the requested file.
         * @param ignoreCase flag specifying, if the case shall be ignored, or not.
         * @return a file entry describing the file.
         */
        virtual FileEntry GetFile(const Path& filename, bool ignoreCase = false) const = 0;

        /**
         * @brief ReadFile reads the file data to the given dest pointer.
         * @param fileEntry describing the file to read.
         * @param dest pointer to the memory to store the data at.
         * @return true, if the file has been read successfully.
         */
        virtual bool ReadFile(const FileEntry& fileEntry, char* dest) const = 0;

        /**
         * @brief ReadFile reads the file data to the given data container.
         * @param fileEntry describing the file to read.
         * @param dest data container, to store the read data in.
         * @return true, if the file has been read successfully.
         */
        virtual bool ReadFile(const FileEntry& fileEntry, std::vector<char>& dest) = 0;

        /**
         * @brief WriteFile writes given data to the file storage with a direct data pointer.
         * @param fileEntry describing the file to write.
         * @param src the data storage to be written.
         * @return true, if the file has been written successfully.
         */
        virtual bool WriteFile(const FileEntry& fileEntry, const char* src) = 0;

        /**
         * @brief WriteFile writes given data to the file storage from a std::vector containing the data.
         * @param fileEntry describing the file to write.
         * @param src the data storage to be written.
         * @return true, if the file has been written successfully.
         */
        virtual bool WriteFile(const FileEntry& fileEntry, const std::vector<char>& src) = 0;

    protected:
        Path basePath; //!< Path to archive file or directory to work with.
    };

} //namespace Clipped
