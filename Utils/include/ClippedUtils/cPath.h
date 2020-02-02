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

#include <vector>
#include "cString.h"

namespace Clipped
{
    template <class T>
    class BasicPath : public BasicString<T>
    {
    public:
        /**
         * @brief BasicPath creates an empty path object.
         */
        BasicPath();

        /**
         * @brief BasicPath creates a copy of a path object.
         * @param cpy object to copy.
         */
        BasicPath(const BasicPath<T>& cpy);

        /**
         * @brief BasicPath creates a new path from a string object.
         * @param str to initialize with.
         */
        BasicPath(const BasicString<T>& str);

        /**
         * @brief BasicPath creates a new path from a cstring object.
         * @param str cstring to initialize with.
         */
        BasicPath(const T* str);

        /**
         * @brief normalize normalizes a path (e.g. under windows \ -> /)
         */
        BasicPath<T> normalize() const;

        /**
         * @brief isAbsolute checks if this path is an absolute one.
         * @return true, if absolute.
         */
        bool isAbsolute() const;

        /**
         * @brief isRelative checks if this path is a relative one.
         * @return true, if relative.
         */
        bool isRelative() const;

        /**
         * @brief getDirectory returns the directory part of this path.
         *   Note: Does only work, if this path contains a file at the end.
         * @return The directory part without leading delim.
         */
        BasicString<T> getDirectory() const;

        /**
         * @brief getFilenameWithExt returns the filename with extension of this path.
         * @return The filename with extension as string copy.
         */
        BasicString<T> getFilenameWithExt() const;

        /**
         * @brief getFilename returns the filename without extension of this path.
         * @return The filename without extension as string copy.
         */
        BasicString<T> getFilename() const;

        /**
         * @brief getExtension returns the extension of this path.
         * @return The extension as string copy.
         */
        BasicString<T> getExtension() const;

        /**
         * @brief up goes one directory up.
         * Note: The path has to be a directory path only.
         * @return reference to this object.
         */
        BasicPath<T>& up();

        /**
         * @brief operator + appends a string to the path.
         * @param rhs string to append.
         * @return reference to this object.
         */
        BasicPath<T>& operator+(const BasicString<T>& rhs);

        /**
         * @brief setFilename sets or overwrites the filename of this path.
         *   The directory and file externsion part are kept.
         * @param rhs the filename to set.
         * @return reference to this object.
         */
        BasicPath<T>& setFilename(const BasicString<T>& rhs);
    };                                               // class BasicPath

    using Path = BasicPath<char>;
    using WPath = BasicPath<wchar_t>;
    // TODO: Implement BasicString<T> toUtf16 and toUtf32 and corresponsing fromAscii methods
    //      to instanciate unicode variants U16Path and U32Path, if needed.

    //Tell the compiler what template instanciations are compiled (fixes -Wundefined-func-template)
    extern template class BasicPath<char>;
#ifdef CLIPPED_BUILD_WIDE
    extern template class BasicPath<wchar_t>;
#endif

}  // namespace Clipped
