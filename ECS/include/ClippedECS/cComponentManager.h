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
