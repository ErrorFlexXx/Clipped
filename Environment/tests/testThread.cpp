#include <ClippedUtils/cLogger.h>
#include <ClippedEnvironment/cThread.h>
#include <ClippedEnvironment/cSystem.h>
#include <atomic>

using namespace Clipped;

class TestThread : public Thread
{
public:
    TestThread()
        : counter(0)
    {}

    virtual void threadFunction() override
    {
        while(runflag)
        {
            counter++;
        }
        isStopped = true;
    }
    uintmax_t counter;
};

int main()
{
    Logger() << Logger::MessageType::Debug;
    TestThread t1;
    t1.start();

    TestThread t2;
    t2.start();

    System::mSleep(500);
    t1.stop();
    t2.stop();
    while(t1.isRunning() || t2.isRunning())
    {
        System::mSleep(100);
    }
    LogInfo() << "T1 reached: " << t1.counter;
    LogInfo() << "T2 reached: " << t2.counter;
    return 0;
}
