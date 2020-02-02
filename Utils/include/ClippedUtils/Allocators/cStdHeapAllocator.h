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

#include <memory>

/**
 * @brief StdHeapAllocator - first custom allocator for testing purposes.
 *   This allocator just allocates everythink with malloc.
 */
template <typename T = void>
struct StdHeapAllocator
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using pointer = T *;
    using const_pointer = const T *;
    using difference_type = typename std::pointer_traits<pointer>::difference_type;

    StdHeapAllocator() noexcept {}

    /**
     * @brief ~StdHeapAllocator - Destroys this allocator.
     */
    ~StdHeapAllocator() noexcept {}

    T *allocate(size_t size, const void * = 0)
    {
        if (size == 0)
        {
            return nullptr;
        }
        T *obj = static_cast<T *>(std::malloc(size * sizeof(T)));
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
        (void)size;
        if (!ptr)
        {
            return;
        }
        ptr->~T();  // Destruct the object.
        std::free(ptr);
    }

    template <typename U>
    struct rebind
    {
        typedef StdHeapAllocator<U> other;
    };

private:
};
