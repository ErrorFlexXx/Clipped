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

#include "cConfigFile.h"
#include <ClippedUtils/cLogger.h>

using namespace Clipped;

ConfigFile::ConfigFile(const Path& filepath, const String& delim = "=")
    : TextFile(filepath)
    , delim(delim)
{}

bool ConfigFile::readAll()
{
    if(!open(FileAccessMode::READ_ONLY))
        return false;

    entries.clear();
    keyPairs.clear();

    String line;
    while(readLine(line, '\n'))
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

bool ConfigFile::writeAll()
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
                success &= writeLine(pair.first + delim + pair.second);
            else //Keep commentaries or something else empty lines or something.
                success &= writeLine(pair.first + pair.second);

            if(!success)
                break; //Stop writing to file - heres a problem.
        }

        file.close();
    }
    return success;
}

std::list<std::pair<String, String>>& ConfigFile::getEntries()
{
    return entries;
}

bool ConfigFile::getEntry(const String& key, int& target) const
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

bool ConfigFile::getEntry(const String& key, unsigned short& target) const
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

bool ConfigFile::getEntry(const String& key, uint32_t& target) const
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

bool ConfigFile::getEntry(const String& key, String& target) const
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

bool ConfigFile::getEntry(const String& key, bool& target) const
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
