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

#include "cEntityManager.h"
#include <ClippedUtils/Allocators/cBlockAllocator.h>
#include <ClippedUtils/Allocators/cStdHeapAllocator.h>
#include <map>

namespace Clipped
{
    /**
     * @brief The ComponentManager class is responsible for creating and linking components.
     */
    template <typename T>
    class ComponentManager
    {
    public:
        /**
         * @brief hasComponent checks if the given entity has a model component assigned.
         * @param entity to check
         * @return true if component is assigned.
         */
        bool hasComponent(Entity entity)
        {
            return components.find(entity.id) != components.end();
        }

        /**
         * @brief getComponent returns the existing or a new component for the Entity.
         * @param entity to return the component for.
         * @return pointer to component.
         */
        T* getComponent(Entity entity)
        {
            if (!hasComponent(entity))
            {
                T* newComponent = allocator.allocate(1);
                newComponent->id = entity.id;
                components[entity.id] = newComponent;
            }
            return components[entity.id];
        }

        /**
         * @brief removeComponent removes a component from specified Entity.
         * @param entity to remove the component from.
         */
        void removeComponent(Entity entity)
        {
            auto it = components.find(entity.id);
            if (it != components.end())
            {
                T* comp = *it;
                allocator.deallocate(comp, 1);
            }
        }

        /**
         * @brief search search a component compared on functor.
         * @param equalFunc functor to check for a match.
         * @param rhs Component reference to compare with.
         * @return pointer to component, if found, nullptr otherwise.
         */
        T* search(std::function<bool(T*, T&)> equalFunc, T& rhs)
        {
            for (auto it = components.begin(); it != components.end(); it++)
            {
                if (equalFunc(it->second, rhs)) return it->second;
            }
            return nullptr;
        }

    private:
        // StdHeapAllocator<T> allocator;  // For performance tests
        BlockAllocator<T> allocator;         //!< Used to allocate new components.
        std::map<uintmax_t, T*> components;  //!< Components managed by this component manager.
    };
}  // namespace Clipped
