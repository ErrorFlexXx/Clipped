#pragma once

#include <thread>
#include <atomic>

namespace Clipped
{
class Thread
{
public:
    /**
     * @brief The Priority enum contains windows values for priorities.
     * Note: for Posix these values are converted to values between posix min and max thread prio values.
     */
    enum Priority : int
    {
        LOWEST = -2,
        LOW,
        NORMAL,
        HIGH,
        HIGHEST
    };

    Thread();
    virtual ~Thread();

    /**
     * @brief start starts the thread.
     */
    void start();

    /**
     * @brief stop set the stop condition for the thread.
     *  The threadFunction shall check the flag and quit itself.
     */
    void stop();

    /**
     * @brief setPriority sets the scheduler priority for the thread.
     *  Note: Might require elevated rights. Program should'nt rely on priorities.
     * @param prio the priority to run at.
     * @return true, if the prio has been updated successfully.
     */
    bool setPriority(Priority prio);

    /**
     * @brief isRunning query wether the thread is currently running.
     * @return true, if the thread is currently running.
     */
    bool isRunning() const;

    /**
     * @brief threadFunction the function, that shall be executed in a thread.
     */
    virtual void threadFunction() = 0;

protected:
    std::atomic_bool runflag;   //!< Flag to keep the thread running.
    std::atomic_bool isStopped; //!< Flag indicating the real run state.

private:
    std::thread thread;         //!< The thread object.
    Priority prio;              //!< Priority the thread shall run at.
}; //class Thread

} //namespace Clipped
