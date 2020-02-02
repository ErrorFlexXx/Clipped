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

#include <list>
#include <map>
#include "cTextFile.h"
#include <ClippedUtils/cString.h>

namespace Clipped
{
    /**
     * @brief The ConfigFile class implements reading and writing config entries from text based files.
     */
    class ConfigFile : public TextFile
    {
    public:
        ConfigFile(const Path& filepath, const String& delim);

        /**
         * @brief readAll reads the entire config from a file.
         * @param delim to split keys and entries with.
         * @return true if all entries has been read successfully, false otherwise.
         */
        bool readAll();

        /**
         * @brief writeAll writes all entries back to file.
         * @return true, if written successfully.
         */
        bool writeAll();

        /**
         * @brief getEntries get reference to entries.
         *   You may modify it and call writeAll afterwards to write new entries or remove some.
         * @return a reference to the internal container of key value pairs.
         */
        std::list<std::pair<String, String>>& getEntries();

        /**
         * @brief getEntry writes the value of this overloaded type to target, if the key exists in the data set.
         * @param key to lookup
         * @param target to write the data to.
         * @return true, if the entry has been found.
         */
        bool getEntry(const String& key, int& target) const;

        /** @copydoc getEntry(const String&,int&)const */
        bool getEntry(const String& key, unsigned short& target) const;

        /** @copydoc getEntry(const String&,int&)const */
        bool getEntry(const String& key, uint32_t& target) const;

        /** @copydoc getEntry(const String&,int&)const */
        bool getEntry(const String& key, String& target) const;

        /** @copydoc getEntry(const String&,int&)const */
        bool getEntry(const String& key, bool& target) const;

    private:
        std::list<std::pair<String, String>> entries; //Config entries - used for writing entries back with blank lines and all.
        std::map<String, String> keyPairs; //Config entries - used for fast lookup of entries.
        String delim; //!< Delimiter to split keys and values with.

    }; //class ConfigFile
}  // namespace Clipped
