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
         * @brief ReadAll reads the entire config from a file.
         * @param delim to split keys and entries with.
         * @return true if all entries has been read successfully, false otherwise.
         */
        bool ReadAll();

        /**
         * @brief WriteAll writes all entries back to file.
         * @return true, if written successfully.
         */
        bool WriteAll();

        /**
         * @brief GetEntries get reference to entries.
         *   You may modify it and call writeAll afterwards to write new entries or remove some.
         * @return a reference to the internal container of key value pairs.
         */
        std::list<std::pair<String, String>>& GetEntries();

        /**
         * @brief GetEntry writes the value of this overloaded type to target, if the key exists in the data set.
         * @param key to lookup
         * @param target to write the data to.
         * @return true, if the entry has been found.
         */
        bool GetEntry(const String& key, int& target) const;

        /** @copydoc GetEntry(const String&,int&)const */
        bool GetEntry(const String& key, unsigned short& target) const;

        /** @copydoc GetEntry(const String&,int&)const */
        bool GetEntry(const String& key, String& target) const;

        /** @copydoc GetEntry(const String&,int&)const */
        bool GetEntry(const String& key, size_t& target) const;

        /** @copydoc GetEntry(const String&,int&)const */
        bool GetEntry(const String& key, bool& target) const;

    private:
        std::list<std::pair<String, String>> entries; //Config entries - used for writing entries back with blank lines and all.
        std::map<String, String> keyPairs; //Config entries - used for fast lookup of entries.
        String delim; //!< Delimiter to split keys and values with.

    }; //class ConfigFile
}  // namespace Clipped
