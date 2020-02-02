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
