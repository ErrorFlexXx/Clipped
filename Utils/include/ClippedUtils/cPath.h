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
        BasicPath<T> up();

        /**
         * @brief operator + appends a string to the path.
         * @param rhs string to append.
         * @return reference to this object.
         */
        BasicPath<T> operator+(const BasicString<T>& rhs);

        /**
         * @brief operator + appends a cstring to the path.
         * @param rhs the cstring.
         * @return reference to this object.
         */
        BasicPath<T> operator+(const char* rhs);

        /**
         * @brief setFilename sets or overwrites the filename of this path.
         *   The directory and file externsion part are kept.
         * @param rhs the filename to set.
         * @return reference to this object.
         */
        BasicPath<T>& setFilename(const BasicString<T>& rhs);

        /**
         * @brief wildcardMatch matchs path with a wildcard pattern string.
         * @param pattern wildcard string with asterisk as wildcard.
         * @return true, if the pattern matchs with this path.
         */
        bool wildcardMatch(const BasicString<T>& pattern) const;

    }; // class BasicPath

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
