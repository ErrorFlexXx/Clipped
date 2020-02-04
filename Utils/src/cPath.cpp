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
    return getFilenameWithExt().split(FILE_EXT_DELIM).back();
}

template <class T>
BasicPath<T>& BasicPath<T>::up()
{
    *this = this->getDirectory();
    return *this;
}

template <class T>
BasicPath<T>& BasicPath<T>::operator+(const BasicString<T>& rhs)
{
    this->append(rhs);
    return *this;
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
