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

#include "cEventManager.h"
#include "cEntityManager.h"

using namespace Clipped;

uintmax_t EventManager::nextEventId = 0;

std::map<uintmax_t, std::list<std::function<void(const Entity&)>>>
    EventManager::globalBindings;  //!< Event, callbacks relation.

Event EventManager::registerEvent() { return Event(nextEventId++); }

void EventManager::removeEvent(Event event) { bindings.erase(event.eventId); }

void EventManager::fireEvent(const Entity& entity, Event event)
{
    auto& handlers = bindings[event.eventId];  // Get attached event handlers for event id
    for (auto& func : handlers)
    {
        func(entity);  // Execute all event handlers.
    }
}

void EventManager::fireGlobalEvent(const Entity& entity, Event event)
{
    auto& handlers = globalBindings[event.eventId];  // Get attached event handlers for event id
    for (auto& func : handlers)
    {
        func(entity);  // Execute all event handlers.
    }
}

void EventManager::attachEventHandler(Event event,
                                      const std::function<void(const Entity&)>& callback)
{
    auto& handlers = bindings[event.eventId];
    handlers.insert(handlers.end(), callback);
}

void EventManager::attachGlobalEventHandler(Event event,
                                            const std::function<void(const Entity&)>& callback)
{
    auto& handlers = globalBindings[event.eventId];
    handlers.insert(handlers.end(), callback);
}

void EventManager::removeEventHandler(Event event, std::function<void(const Entity&)>& callback)
{
    // FIXME: event handler removing not possible via callback comparison.
    // Needs investigation!
    //    std::list<std::function<void(const Entity&)>>& handlers = bindings[event.eventId];
    //    handlers.remove(callback);
}
