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

#include "cConfigFile.h"
#include <ClippedUtils/cLogger.h>

using namespace Clipped;

ConfigFile::ConfigFile(const Path& filepath, const String& delim = "=")
    : TextFile(filepath)
    , delim(delim)
{}

bool ConfigFile::ReadAll()
{
    if(!open(FileAccessMode::READ_ONLY))
        return false;

    entries.clear();
    keyPairs.clear();

    String line;
    while(ReadLine(line, '\n'))
    {
        String key;
        String value;
        auto delimPos = line.find(delim);

        if(delimPos != String::npos)
        {
            key = line.substr(0, delimPos);
            value = line.substr(delimPos + delim.length());
            value.replace("\r", ""); //Remove possible windows line termination (left over from TextFile ReadLine).
            if(!key.empty())
            {
                keyPairs[key.toLower()] = value;
            }
        }
        entries.insert(entries.end(), std::make_pair(key, value));
    }

    file.close();
    return true;
}

bool ConfigFile::WriteAll()
{
    bool success = true;

    if(!open(FileAccessMode::TRUNC))
    {
        success = false;
    }
    else
    {
        for(const auto& pair : entries)
        {
            if(!pair.first.empty())
                success &= WriteLine(pair.first + delim + pair.second);
            else //Keep commentaries or something else empty lines or something.
                success &= WriteLine(pair.first + pair.second);

            if(!success)
                break; //Stop writing to file - heres a problem.
        }

        file.close();
    }
    return success;
}

std::list<std::pair<String, String>>& ConfigFile::GetEntries()
{
    return entries;
}

bool ConfigFile::GetEntry(const String& key, int& target) const
{
    auto it = keyPairs.find(key.toLower());
    if(it != keyPairs.end())
    {
        target = it->second.toInt();
        return true;
    }
    //else key not found
    return false;
}

bool ConfigFile::GetEntry(const String& key, unsigned short& target) const
{
    auto it = keyPairs.find(key.toLower());
    if(it != keyPairs.end())
    {
        int test = it->second.toInt();
        if(0 <= test)
        {
            target = (unsigned short)it->second.toInt();
            return true;
        }
        else //Negative numbers not allowed!
        {
            LogWarn() << "Invalid value range for config key \"" << key << "\" value: " << test << " must not be negative!";
            return false;
        }
    }
    //else key not found
    return false;
}

bool ConfigFile::GetEntry(const String& key, uint32_t& target) const
{
    auto it = keyPairs.find(key.toLower());
    if(it != keyPairs.end())
    {
        int test = it->second.toInt();
        if(0 <= test)
        {
            target = (uint32_t)it->second.toInt();
            return true;
        }
        else //Negative numbers not allowed!
        {
            LogWarn() << "Invalid value range for config key \"" << key << "\" value: " << test << " must not be negative!";
            return false;
        }
    }
    //else key not found
    return false;
}

bool ConfigFile::GetEntry(const String& key, String& target) const
{
    auto it = keyPairs.find(key.toLower());
    if(it != keyPairs.end())
    {
        target = it->second;
        return true;
    }
    //else key not found
    return false;
}

bool ConfigFile::GetEntry(const String& key, size_t& target) const
{
    auto it = keyPairs.find(key.toLower());
    if(it != keyPairs.end())
    {
        int test = it->second.toInt();
        if(0 <= test)
        {
            target = (size_t)it->second.toInt();
            return true;
        }
        else //Negative numbers not allowed!
        {
            LogWarn() << "Invalid value range for config key \"" << key << "\" value: " << test << " must not be negative!";
            return false;
        }
    }
    //else key not found
    return false;
}

bool ConfigFile::GetEntry(const String& key, bool& target) const
{
    auto it = keyPairs.find(key.toLower());
    if(it != keyPairs.end())
    {
        String value = it->second.toLower().trim();

        if(value.equals("1")    ||
           value.equals("true") ||
           value.equals("yes")  ||
           value.equals("on"))
        {
            target = true;
            return true;
        }
        else if(value.equals("0")     ||
                value.equals("false") ||
                value.equals("no")    ||
                value.equals("off"))
        {
            target = false;
            return true;
        }
        else
        {
            LogWarn() << "Unrecognized boolean value (try: 1, 0, true, false, yes, no, on or off) for key: \"" << key << "\" Value: \"" << value;
            return false;
        }
    }
    //else key not found
    return false;
}
