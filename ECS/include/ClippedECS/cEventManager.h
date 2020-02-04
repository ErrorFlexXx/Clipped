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

#include <stdint.h>
#include <functional>
#include <list>
#include <map>

namespace Clipped
{
    /**
     * @brief The Event struct contains the event id assigned by an Event Manager.
     */
    struct Event
    {
        /**
         * @brief Event constructor for instant id setup.
         * @param eventId to set.
         */
        Event(uintmax_t eventId) : eventId(eventId) {}

        uintmax_t eventId;  //!< Application wide unique event id.
    };

    struct Entity;  // Forward decl.

    /**
     * @brief The EventManager is responsible for Event id assignment and event handlers.
     */
    class EventManager
    {
    public:
        /**
         * @brief registerEvent registers a new event.
         * @return An event identifier.
         */
        static Event registerEvent();

        /**
         * @brief removeEvent deletes an registered event entirely. Including all handlers.
         * @param event to be removed.
         */
        void removeEvent(Event event);

        /**
         * @brief attachEventHandler adds a callback for an event.
         * @param event to attach to.
         * @param callback that gets called, if the event is fired.
         */
        void attachEventHandler(Event event, const std::function<void(const Entity&)>& callback);

        /**
         * @brief attachGlobalEventHandler attaches a global event.
         * Global events are used to fire events without an explicit event handler instance.
         * Necessary e.g. to link events from global callbacks from libraries.
         * @param event
         * @param callback
         */
        static void attachGlobalEventHandler(Event event,
                                             const std::function<void(const Entity&)>& callback);

        /**
         * @brief removeEventHandler removes the callback.
         * @param callback that won't get called any longer.
         */
        void removeEventHandler(Event event, std::function<void(const Entity&)>& callback);

        /**
         * @brief fireEvent fires an event for a given entity. All callbacks will be called.
         * @param entity that has an event.
         * @param event that is fired.
         */
        void fireEvent(const Entity& entity, Event event);

        /**
         * @brief fireGlobalEvent fires a static event without a event handler instance.
         * @param entity that belongs to the event.
         * @param event that occured.
         */
        static void fireGlobalEvent(const Entity& entity, Event event);

    private:
        static uintmax_t nextEventId;  //!< Unique event id counter.
        std::map<uintmax_t, std::list<std::function<void(const Entity&)>>>
            bindings;  //!< Event, callbacks relation for a specific EventManager instance..
        static std::map<uintmax_t, std::list<std::function<void(const Entity&)>>>
            globalBindings;  //!< Event, callbacks relation for the global EventManager.
    };
}  // namespace Clipped
