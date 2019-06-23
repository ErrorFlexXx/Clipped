#pragma once

#include <stdint.h>

namespace Clipped
{
    /**
     * @brief The Entity struct is just an id, which is unique.
     */
    struct Entity
    {
        Entity() : id(0) {}

        Entity(uintmax_t id) : id(id) {}

        uintmax_t id;  //!< Unique id of the entity.
    };

    /**
     * @brief The EntityManager creates unique entity ids.
     */
    class EntityManager
    {
    public:
        /**
         * @brief getEntity returns a new and unique entity ID.
         * @return the Entity (ID)
         */
        static Entity getEntity();

    private:
        static uintmax_t nextId;  //!< Counter for new entity ids.
    };
}  // namespace Clipped
