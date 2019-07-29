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

#include "Archives/cVdfsFile.h"
#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cPath.h>

using namespace Clipped;

const String VDFSFile::CommentFillChar = "\x1A";
const size_t VDFSFile::CommentLength = 256;
const size_t VDFSFile::SignatureLength = 16;
const size_t VDFSFile::EntryNameLength = 64;

VDFSFile::VDFSFile(const Path& filepath, bool ignoreCase)
    : BinFile(filepath), ignoreCase(ignoreCase)
{
}

bool VDFSFile::open(const FileAccessMode& accessMode)
{
    bool result = this->BinFile::open(accessMode);
    if (!result)
    {
        LogError() << "File cannot be opened!";
        return result;
    }
    result = readHeader(header);
    if (!result)
    {
        LogError() << "VDFS Header corrupt!";
        return result;
    }
    result = readVDFSIndex();
    if (!result)
    {
        LogError() << "VDFS Index corrupt!";
        return result;
    }
    return result;
}

bool VDFSFile::readVDFSIndex()
{
    setPosition(header.rootOffset);
    return createIndexTree(index);
}

bool VDFSFile::createIndexTree(Tree<String, Entry>& tree)
{
    bool result = true;

    for (size_t i = 0; i < header.entryCount; i++)
    {
        Entry entry;

        if (result) result = BinFile::ReadString(entry.name, EntryNameLength);
        if (result) result = BinFile::Read(entry.offset);
        if (result) result = BinFile::Read(entry.size);
        if (result) result = BinFile::Read(entry.type);
        if (result) result = BinFile::Read(entry.attribute);
        entry.name = entry.name.trim();
        if (ignoreCase) entry.name = entry.name.toUpper();

        if (result && (entry.type & Type::DIRECTORY))
        {
            if (result) result = createIndexTree(tree.addSubtree(entry.name));
        }

        if (result && (entry.type & Type::LAST))
        {
            return result;
        }

        if (result)  // Regular file entry
        {
            result = tree.addElement(entry.name, entry);
        }
    }
    return result;
}

VDFSFile::Entry VDFSFile::getEntry(const Path& vdfsPath, bool& found)
{
    Path dir = vdfsPath.getDirectory();
    Path file = vdfsPath.getFilenameWithExt();
    if (ignoreCase)
    {
        dir = dir.toUpper();
        file = file.toUpper();
    }

    found = false;
    auto* searchIndex = &index;

    for (String stage : dir.split(Path("/")))
    {
        if (searchIndex->subtreeExist(stage))
            searchIndex = &searchIndex->getSubtree(stage);
        else
        {
            found = false;
            return VDFSFile::Entry();
        }
    }
    if (searchIndex->elementExist(file))
    {
        found = true;
        return searchIndex->getElement(file);
    }
    found = false;
    return VDFSFile::Entry();
}

std::vector<VDFSFile::Entry> VDFSFile::getEntries(const Path& extension, const Path& vdfsPath,
                                                  bool recursive)
{
    Path dir = vdfsPath;
    Path ext = extension;
    std::vector<VDFSFile::Entry> entries;

    if (ignoreCase)
    {
        dir = dir.toUpper();
        ext = ext.toUpper();
    }

    auto* searchIndex = &index;

    for (String stage : dir.split(Path("/")))
    {
        if (searchIndex->subtreeExist(stage))
            searchIndex = &searchIndex->getSubtree(stage);
        else
        {
            return entries;
        }
    }

    return findExtEntries(entries, ext, searchIndex, recursive);
}

bool VDFSFile::Read(std::vector<char>& data, const VDFSFile::Entry& entry)
{
    setPosition(entry.offset);

    if (!ReadBytes(data, entry.size))
    {
        LogError() << "Error while reading from file.";
        return false;
    }
    return true;
}

std::vector<VDFSFile::Entry>& VDFSFile::findExtEntries(std::vector<VDFSFile::Entry>& entries,
                                                       const Path& ext,
                                                       Tree<String, VDFSFile::Entry>* searchIndex,
                                                       bool recursive)
{
    const std::map<String, Entry>& dirElements = searchIndex->getElements();
    for (const auto& dirEntry : dirElements)
    {
        Path tmp = dirEntry.first;
        if (tmp.getExtension() == ext)
        {
            entries.push_back(dirEntry.second);
        }
    }
    if (recursive)
    {
        auto subtrees = searchIndex->getSubtrees();
        for (auto& subtree : subtrees)
        {
            findExtEntries(entries, ext, &subtree.second, recursive);
        }
    }
    return entries;
}

bool VDFSFile::readHeader(VDFSFile::Header& header)
{
    bool result = true;

    if (result) result = BinFile::ReadString(header.comment, 256);
    if (result) result = BinFile::ReadString(header.signature, 16);
    if (result) result = BinFile::Read(header.entryCount);
    if (result) result = BinFile::Read(header.fileCount);
    if (result) result = BinFile::Read(header.creationTime);
    if (result) result = BinFile::Read(header.contentSize);
    if (result) result = BinFile::Read(header.rootOffset);
    if (result) result = BinFile::Read(header.entrySize);

    header.comment = header.comment.trim(CommentFillChar);

    return result;
}

bool VDFSFile::writeHeader(const VDFSFile::Header& header)
{
    bool result = true;

    if (header.comment.length() > CommentLength)
    {
        LogWarn() << "Header comment too large (" << header.comment.length()
                  << ")! Cutted to max length (" << CommentLength << ").";
        if (result) result = WriteString(header.comment.substr(0, CommentLength));
    }
    else  // Comment size ok
        if (result)
        result = WriteString(header.comment.fill(CommentFillChar, CommentLength));

    if (result && header.signature.length() > SignatureLength)
    {
        LogWarn() << "Header signature too large (" << header.signature.length()
                  << ")! Cutted to max length (" << SignatureLength << ").";
        if (result) result = WriteString(header.signature.substr(0, SignatureLength));
    }
    else  // Signature size ok
        if (result)
        result = WriteString(header.signature.fill(" ", SignatureLength));
    if (result) result = Write(header.entryCount);
    if (result) result = Write(header.fileCount);
    if (result) result = Write(header.creationTime);
    if (result) result = Write(header.contentSize);
    if (result) result = Write(header.rootOffset);
    if (result) result = Write(header.entrySize);

    return result;
}

const Path& VDFSFile::getFilepath() const { return this->filepath; }
