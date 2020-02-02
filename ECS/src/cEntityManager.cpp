#include "cEntityManager.h"

using namespace Clipped;

uintmax_t EntityManager::nextId = 0;

Entity EntityManager::getEntity() { return Entity(nextId++); }
