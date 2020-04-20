#include "cThread.h"
#include "cSystem.h"
#include <ClippedUtils/cOsDetect.h>
#include <ClippedUtils/cLogger.h>

#if defined(WINDOWS)
    #include <windows.h>
#elif defined(LINUX)
    #include <pthread.h>
#endif

using namespace Clipped;

Thread::Thread()
    : thread()
    , runflag(false)
    , isStopped(true)
    , prio(NORMAL)
{}

Thread::~Thread()
{
    runflag = false;
}

void Thread::start()
{
    this->prio = prio;
    runflag = true;
    isStopped = false;
    thread = std::thread(&Thread::threadFunction, this);
    setPriority(prio);
    thread.detach(); //Let the tread run on it's own.
}

void Thread::stop()
{
    runflag = false; //Note, the threadFunction has to take care to handle this flag and quit itself.
}

bool Thread::setPriority(Priority prio)
{
    this->prio = prio;
#if defined(WINDOWS)
    if(0 == ::SetThreadPriority(thread.native_handle(), prio))
    {
        LogError() << System::getSystemErrorText();
        return false;
    }
    return true;
#elif defined(LINUX)
    sched_param sch;
    int policy;
    pthread_getschedparam(thread.native_handle(), &policy, &sch);
    int maxPrio = sched_get_priority_max(policy);
    int minPrio = sched_get_priority_min(policy);

    if(0 == maxPrio && maxPrio == minPrio)
    {
        LogWarn() << "Can't set priorities. Insufficient permissions or unsupported on this system.";
        return false;
    }
    int posixPrio = minPrio + static_cast<int>((maxPrio - minPrio) * ((prio + 2) / static_cast<float>(HIGHEST-LOWEST)));
    sch.sched_priority = posixPrio;

    LogDebug() << minPrio << " / " << maxPrio << ": " << posixPrio;
    int result = pthread_setschedparam(thread.native_handle(), SCHED_FIFO, &sch);
    if(0 != result)
    {
        switch(result)
        {
        case ENOTSUP:
            LogError() << "Change of thread priority isn't supported on this system!";
            break;
        case ESRCH:
            LogError() << "Selected thread doesn't exist!";
            break;
        case EINVAL:
            LogError() << "Priority setting doesn't make sense!";
            break;
        case EPERM:
            LogError() << "Missing privileges to change thread priorities!";
            break;
        }

        return false; //Error setting thread priority.
    }
    return true; //Successfully set new thread priority.
#endif
}

bool Thread::isRunning() const
{
    return !isStopped;
}
