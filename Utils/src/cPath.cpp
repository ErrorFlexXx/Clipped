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

#include "cPath.h"
#include "cLogger.h"
#include "cOsDetect.h"

using namespace std;
using namespace Clipped;

#define DELIM               BasicPath<T>::fromAsci("/")
#define WIN_DELIM           BasicPath<T>::fromAsci("\\")
#define PATH_UP             BasicPath<T>::fromAsci("..")
#define PATH_CURRENT_DIR    BasicPath<T>::fromAsci(".")
#define FILE_EXT_DELIM      BasicPath<T>::fromAsci(".")

template <class T>
BasicPath<T>::BasicPath() : BasicString<T>()
{
}

template <class T>
BasicPath<T>::BasicPath(const BasicString<T>& str) : BasicString<T>(str)
{
}

template <class T>
BasicPath<T>::BasicPath(const T* str) : BasicString<T>(str)
{
}

template <class T>
BasicPath<T>::BasicPath(const BasicPath<T>& cpy) : BasicString<T>(cpy)
{
}

template <class T>
BasicPath<T> BasicPath<T>::normalize() const
{
    BasicPath<T> cpy = *this;
#ifdef WINDOWS
    cpy.replace(WIN_DELIM, DELIM);
#endif
    return cpy;
}

template <class T>
bool BasicPath<T>::isAbsolute() const
{
    BasicString<T> dir = getDirectory();
    return ((dir.find(PATH_CURRENT_DIR) == BasicString<T>::npos) &&
            (dir.find(PATH_UP) == BasicString<T>::npos));
}

template <class T>
bool BasicPath<T>::isRelative() const
{
    return !isAbsolute();
}

template <class T>
BasicString<T> BasicPath<T>::getDirectory() const
{
    auto it = this->normalize().find_last_of(DELIM);
    if (it != BasicPath<T>::npos)
        return this->normalize().substr(0, this->normalize().find_last_of(DELIM));
    return BasicString<T>();
}

template <class T>
BasicString<T> BasicPath<T>::getFilenameWithExt() const
{
    BasicString<T> filename;
    auto dirParts = this->normalize().split(DELIM);
    return dirParts.back();
}

template <class T>
BasicString<T> BasicPath<T>::getFilename() const
{
    BasicString<T> filename;
    auto filenameParts = getFilenameWithExt().split(FILE_EXT_DELIM);

    if (filenameParts.size() > 1)  // If there is an extension
    {
        for (size_t i = 0; i < filenameParts.size() - 1; i++)  // Copy filename, wo. ext.
        {
            filename += filenameParts[i];
        }
        return filename;
    }
    else  // if there is no extension
        return filenameParts.front();
}

template <class T>
BasicString<T> BasicPath<T>::getExtension() const
{
    vector<BasicString<T>> parts = getFilenameWithExt().split(FILE_EXT_DELIM);
    if(1 < parts.size())
    {
        return getFilenameWithExt().split(FILE_EXT_DELIM).back();
    }
    return "";
}

template <class T>
BasicPath<T> BasicPath<T>::up()
{
    return this->getDirectory();
}

template <class T>
BasicPath<T> BasicPath<T>::operator+(const BasicString<T>& rhs)
{
    BasicPath<T> newPath = *this;
    newPath.append(rhs);
    return newPath;
}

template <class T>
BasicPath<T> BasicPath<T>::operator+(const char* rhs)
{
    BasicPath<T> newPath = *this;
    newPath.append(rhs);
    return newPath;
}

template <class T>
BasicPath<T>& BasicPath<T>::setFilename(const BasicString<T>& rhs)
{
    BasicString<T> newPath;
    if(!getDirectory().empty())
        newPath += getDirectory() + DELIM;
    newPath += rhs;
    if(!getExtension().empty())
        newPath += FILE_EXT_DELIM + getExtension();

    *this = newPath;
    return *this;
}

template <class T>
bool BasicPath<T>::wildcardMatch(const BasicString<T>& pattern) const
{
    BasicString<T> match = *this; //Work copy to cut off matched parts.

    const auto parts = pattern.split('*');
    const bool endsWithWildcard = pattern.endsWith("*");

    for(const auto part : parts)
    {
        auto index = match.find(part);
        if(index != BasicPath<T>::npos)
        {
            match = match.substr(index + part.size()); //Keep rest of
        }
        else //Required part not found.
        {
            return false; //path doesn't match the pattern.
        }
    }
    return match.empty() | endsWithWildcard;
}

// Please compile template class for the following types:
namespace Clipped
{
    template class BasicPath<char>;
#ifdef CLIPPED_BUILD_WIDE
    template class BasicPath<wchar_t>;
#endif
}  // namespace Utils
