
#include <mutex>
#include <condition_variable>

class Event
{
private:
    std::mutex mtx;
    std::condition_variable cv;

public:
    Event() = default;
    ~Event() = default;

    void fired()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.notify_all();
    }

    void wait()
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock);
    }
};
