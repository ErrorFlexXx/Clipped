#pragma once

#include <list>
#include <ClippedUtils/cPath.h>
#include <ClippedUtils/cMemory.h>

namespace Clipped
{
    /**
     * @brief The Explorer class handles filesystem tasks.
     */
    class Explorer
    {
    public:
        Explorer();
        Explorer(const Path& cwd);

        /**
         * @brief getDriveAvailableMemory returns the available memory space on the current drive.
         * @return the available memory.
         */
        MemorySize getDriveAvailableMemory() const;

        /**
         * @brief getDriveCapacity retrurns the total capacity on the current drive.
         * @return the total capacity of the drive.
         */
        MemorySize getDriveCapacity() const;

        /**
         * @brief getCurrentPath returns the current path.
         * @return the current path.
         */
        Path getCurrentPath() const;

        /**
         * @brief changeDirectory changes the current directory.
         */
        void changeDirectory(const Path& gotoDirectory);

        /**
         * @brief searchFiles searchs for files.
         * @param searchString searchString, which may contain wildcards *.
         * @param recursive wether to recurse into subdirectories, or not.
         * @return a list with matching files.
         */
        std::list<Path> searchFiles(const String& searchString, bool recursive = true);

        /**
         * @brief searchDirs searchs for directories.
         * @param searchString searchString, which may contain wildcards *.
         * @param recursive wether to recurse into subdirectories, or not.
         * @return a list with matching dirs.
         */
        std::list<Path> searchDirs(const String& searchString, bool recursive = true);

        // Statics:
        /**
         * @brief CreateDir creates a directory on the filesystem.
         * @param directoryName name of the new directoy.
         * @return true, if the directory has been created.
         */
        static bool CreateDir(const Path& directoryName);

        /**
         * @brief Remove removes a directory from the filesystem.
         * @param directoryName directory to remove.
         * @param recursive removes recursivly, if true.
         * @return true, if it has been removed sucessfully.
         */
        static bool Remove(const Path& directoryName, bool recursive);

        /**
         * @brief Rename renames a file or directory.
         * @param from the file or directory to rename.
         * @param to the target name of file/directory.
         */
        static void Rename(const Path& from, const Path& to);

        /**
         * @brief Exists checks, if a file or directory exists.
         * @param path filepath to the object.
         * @return true, if the object exists. False otherwise.
         */
        static bool Exists(const Path& path);

        /**
         * @brief Copy copies files or directories from one location to another.
         * @param from path or directory to copy.
         * @param to destination to copy to.
         * @param recursive wether the copying shall be done recursivly or not.
         */
        static void Copy(const Path& from, const Path& to, bool recursive);

    private:
        Path currentDir; //!< Current directory

    }; //class Explorer
} //namespace Clipped
