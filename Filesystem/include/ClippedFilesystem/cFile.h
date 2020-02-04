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
         * @brief copy creates a copy of this file.
         * @param destination filepath of the new file.
         * @return true, if the copy has been created successfully.
         */
        bool copy(const Path& destination);

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
        bool setPosition(size_t pos);

        /**
         * @brief setPostionToFileEnd sets the file pointer to the end of the file.
         * @return true, if positioned successfully.
         */
        bool setPostionToFileEnd();
        /**
         * @brief getPosition gets the current cursor position in file.
         * @return the absolute cursor position.
         */
        size_t getPosition();

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
