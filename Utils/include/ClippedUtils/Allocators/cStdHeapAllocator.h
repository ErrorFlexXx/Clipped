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
