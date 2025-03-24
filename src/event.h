
#include <mutex>
#include <condition_variable>

/**
 * @brief class that encapsulates a condition variable
 * 
 * Creating an instance of this class allows a nicer 
 * interface to the condition variable.
 * 
 * @example 
 * 
 * Event event;
 * event.wait(); // will block until event.fired() is called
 * 
 * //another thread
 * event.fired(); // will unblock the waiting thread
 * 
 */
class Event
{
private:
    std::mutex mtx;
    std::condition_variable cv;

public:
    Event() = default;
    ~Event() = default;

    /**
     * @brief Notify all waiting threads
     * 
     * This function will notify all waiting threads
     * that the event has been fired. It will release
     * them allowing processing to continue.
     * 
     * @note the mutex is locked before notifying
     * the condition variable. then the mutex is unlocked
     * after the notification. The usual pattern for
     * condition variables.
     */
    void fired()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.notify_all();
    }

    /**
     * @brief Wait for the event to be fired
     * 
     * The thread calling this will block in the 
     * wait state until the event is fired. the 
     * mutex is unlocked while it waits. Usual 
     * condition variable pattern.
     */
    void wait()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock);
    }
};
