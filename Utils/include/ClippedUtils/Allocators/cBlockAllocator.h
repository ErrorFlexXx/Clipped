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

#include <ClippedUtils/cLogger.h>
#include <ClippedUtils/cMemory.h>
#include <assert.h>
#include <list>
#include <memory>

// Default block size in bytes:
#define DEFAULT_BLOCK_SIZE 10 * MEGA

/** \file blockAllocator.h a custom allocator.
 * A custom allocator reserving larger blocks to get more cache friendly clustered memory sections.
 *
 * Contents:
 * struct Block - A memory block, that allocates memory and manages free mem gaps.
 * struct BlockManager - A manager that allocates new memory blocks, if there is not enaugh memory
 *  in already registered blocks.
 * struct BlockAllocator final allocator struct, ready to be used e.g. in std containers.
 */

/**
 * @brief The Block struct represents an allocated memory block.
 */
template <typename T>
struct Block
{
    T *address;                              //!< Pointer to allocated memory.
    size_t elementCapacity;                  //!< Number of elements storable in this block.
    T *fillIndex;                            //!< Continous fill index
    std::list<std::pair<T *, size_t>> gaps;  //!< List of free memory gaps.

    /**
     * @brief Block initially no memory gets reserved. Call setup to make this block ready.
     */
    Block() : address(nullptr) {}

    /**
     * @brief setup reserves a memory block to work with.
     * @param size requested size of the memory block.
     */
    void setup(size_t size = static_cast<size_t>(DEFAULT_BLOCK_SIZE))
    {
        address = reinterpret_cast<T *>(std::malloc(size));
        fillIndex = address;
        if (address == nullptr)  // Out of memory ?
        {
            LogError() << "Out of memory! Bye bye.";  // Log
            assert(address != nullptr);               // And stop here!
        }
        elementCapacity = size / sizeof(T);  // Amount of T objects storeable.
    }

    bool empty() const { return address == fillIndex; }

    bool operator==(const Block &rhs) const { return this == &rhs; }

    /**
     * @brief ~Block destroys this memory block. Frees allocated memory, if present.
     */
    ~Block()
    {
        if (address)  // If memory has been allocated
        {
            std::free(address);  // Release it now.
        }
    }

    /**
     * @brief tryAlloc tries to get memory from this block.
     * @param count elements of type T requested.
     * @return pointer to memeory or nullptr, if not possible.
     */
    T *tryAlloc(size_t count)
    {
        for (auto &elementGap : gaps)  // Try to fill memory gaps.
        {
            if (elementGap.second >= count)  // Gap large enaugh
            {
                T *ret = elementGap.first;      // Gap selected. Assign memory.
                if (elementGap.second > count)  // Gap bigger than requested.
                {
                    // Update gap.
                    elementGap.first += count;   // Move index 'count' objs forward.
                    elementGap.second -= count;  // Decr. size of gap.
                }
                else  // Gap filled completely
                {
                    gaps.remove(elementGap);  // Remove gap from list.
                }
                return ret;  // Alloc successful
            }
        }
        // No gap found that is big enaugh.
        if (address + elementCapacity >= fillIndex + count)  // Rest of block big enaugh ?
        {
            T *ret = fillIndex;  // Assign memory
            fillIndex += count;  // Move fillindex
            return ret;          // Alloc successful
        }
        else  // Give up
        {
            return nullptr;  // Not enaugh memory in block!
        }
    }

    /**
     * @brief optimizeGaps chains neighbouring memory gaps if possible to maximize the size of the
     * biggest continous memory block.
     */
    void optimizeGaps()
    {
        if (gaps.empty()) return;   // Done.
        auto left = gaps.begin();   // Previous list element.
        auto right = gaps.begin();  // Current check element (for left check).
        right++;

        while (left != gaps.end())  // Iterate over all gaps.
        {
            if (left->first + left->second == fillIndex)  // Gap located at the right end ?
            {
                fillIndex -= left->second;  // Add memory to fillIndex
                gaps.remove(*left);         // Remove this gap.
                return optimizeGaps();      // Rerun optimization.
            }
            if (right != gaps.end() &&
                left->first + left->second == right->first)  // Left gap touching this gap ?
            {
                left->second += right->second;  // Group
                gaps.remove(*right);            // And remove additional gap.
                return optimizeGaps();          // Rerun optimization.
            }
            left++;   // Next gap
            right++;  // Next gap
        }
    }

    /**
     * @brief dealloc releases reserved memory inside the block and optimizes the memory
     * @param ptr to release.
     * @param size of objects to free (currently unused parameter).
     */
    void dealloc(T *ptr, size_t size)
    {
        gaps.push_back(std::make_pair(ptr, size));  // Add gap to gap list
        optimizeGaps();                             // Try to group gaps.
    }
};

/**
 * @brief The BlockManager manages multiple memory blocks and redirects alloc calls to
 * the referenced block.
 */
template <typename T>
struct BlockManager
{
    /**
     * @brief allocate tries to get the memory from already present blocks.
     *   If unsuccessful, this manager allocates a new block with at least
     *   the requetsed amound of space.
     * @param count count elements are requested.
     * @return pointer to memory.
     */
    T *allocate(size_t count, const void * = 0)
    {
        T *ret;
        for (Block<T> &block : blocks)  // Search in all registered blocks.
        {
            ret = block.tryAlloc(count);  // Try to get the amount of memory
            if (ret) return ret;          // Return if successful.
        }

        // Not enaugh memory in registered blocks.
        // Create a new one:
        size_t requestedSize = sizeof(T) * count;  // Amoung of bytes requested.
        if (requestedSize <= DEFAULT_BLOCK_SIZE)   // If it's less than the default block size
            requestedSize =
                static_cast<size_t>(DEFAULT_BLOCK_SIZE);  // Allocate the default block size
        blocks.push_back(Block<T>());                     // Create new block.
        blocks.back().setup(requestedSize);               // Setup block size
        return blocks.back().tryAlloc(count);             // And finally alloc
    }

    /**
     * @brief deallocate redirects the request to the corresponding memory block.
     * @param ptr address to reserved memory
     * @param count element count that were reserved.
     */
    void deallocate(T *ptr, size_t count)
    {
        for (Block<T> &block : blocks)  // Search for corresponding block
        {
            if (block.address <= ptr && ptr < block.address + block.elementCapacity)  // In Block ?
            {
                block.dealloc(ptr, count);  // Handover deallocation call.
                if (block.empty()) blocks.remove(block);
                return;  // Found the block. Stop searching.
            }
        }
    }

    std::list<Block<T>> blocks;  //!< Managed memory blocks.
};

/**
 * @brief BlockAllocator - custom allocator allocating larger memory blocks to get a more cache
 * friendly storage.
 */
template <typename T = void>
struct BlockAllocator
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using pointer = T *;
    using const_pointer = const T *;
    using difference_type = typename std::pointer_traits<pointer>::difference_type;

    BlockAllocator() noexcept {}

    /**
     * @brief ~StdHeapAllocator - Destroys this allocator.
     */
    ~BlockAllocator() noexcept {}

    T *allocate(size_t count, const void * = 0)
    {
        if (count == 0)
        {
            return nullptr;
        }
        T *obj = static_cast<T *>(blockManager.allocate(count));
        new (obj) T();  // Call constructor with allocated memory.
        return obj;
    }

    /**
     * @brief deallocate frees the allocated memory.
     * @param ptr that points to allocated memory.
     * @param size not evaluated, just satisfying the interface.
     */
    void deallocate(T *ptr, size_t size)
    {
        (void)size;          // Ignore
        if (ptr == nullptr)  // Do not deallocate nullptrs.
        {
            return;
        }
        ptr->~T();  // Destruct the object.
        blockManager.deallocate(ptr, size);
    }

    template <typename U>
    struct rebind
    {
        typedef BlockAllocator<U> other;
    };

private:
    BlockManager<T> blockManager;  //!< Block manager that creates memory blocks.
};
