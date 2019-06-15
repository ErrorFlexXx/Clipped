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

#include <fstream>
#include <limits>
#include <ClippedUtils/cMemory.h>
#include <ClippedUtils/cPath.h>
#include <ClippedUtils/cString.h>

namespace Clipped
{
    /**
     * @brief The FileAccessMode enum declares different access rights.
     */
    enum class FileAccessMode
    {
        READ_ONLY,   //!< Read only file access.
        READ_WRITE,  //!< Read write file access.
        TRUNC        //!< Read write file access and deletes existent file first.
    };

    /**
     * @brief The FileDataMode enum declares data modes to open a file with.
     */
    enum class FileDataMode
    {
        TEXT,   //!< Mode to read text.
        BINARY  //!< Mode to read binary data.
    };

    /**
     * @brief The File class interface to work with files.
     */
    class File
    {
    public:
        /**
         * @brief File creates a file object.
         * @param filepath
         */
        File(const Path& filepath);

        /**
         * @brief ~File destructs this handle.
         */
        ~File();

        /**
         * @brief open opens the file in the requested mode.
         * @param openMode to open the file with. Default ReadOnly.
         * @return true if successfull, false otherwise.
         */
        bool open(const FileAccessMode& accessMode, const FileDataMode& dataMode);

        /**
         * @brief close closes this file handle.
         */
        void close();

        /**
         * @brief exists checks, wether the file exists on the file system or not.
         * @return
         */
        bool exists() const;

        /**
         * @brief getSize gets the filesize of this file.
         * @return the filesize.
         */
        MemorySize getSize() const;

        /**
         * @brief remove removes the file from filesystem.
         * @return true if successfully removed, false otherwise.
         */
        bool remove();

        /**
         * @brief touch creates an empty file.
         * @param override whether an existing file shall be overriden, or not.
         * @return true if successfully created the file, false otherwise.
         */
        bool touch(const bool override = false);

        /**
         * @brief isOpen checks wether the file is currently opened.
         * @return true if opened, false otherwise.
         */
        bool isOpen() const;

        /**
         * @brief setPosition sets the current position inside the file.
         * @param pos to set the cursor at.
         * @return true, if set successfully, false otherwise.
         */
        bool setPosition(long pos);

        /**
         * @brief seek moves the cursor relative to the current cursors position.
         * @param count
         * @return true, if the cursor moved successfully, false otherwise.
         */
        bool seek(long delta);

        /**
         * @brief getFilepath returns the filepath of this file.
         * @return the path.
         */
        const Path& getFilepath() const;

    protected:
        Path filepath;              //!< Full filepath to file
        FileAccessMode accessMode;  //!< Access mode for this file instance.
        FileDataMode dataMode;      //!< Data mode for this file instance.
        std::fstream file;          //!< Actual filestream.
    };
}  // namespace Clipped
