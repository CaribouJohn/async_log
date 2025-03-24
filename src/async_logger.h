//
// async logger header detailing the log functions and
// control functions for the async logger.
//

#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <mutex>
#include <condition_variable>

namespace async_logger
{
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

    // I need a lock free circular buffer to store messages
    template <typename T, int buffer_size>
    class CircularBuffer
    {
        static_assert(buffer_size > 0, "Size of CircularBuffer must be greater than 0");

    private:
        std::array<T, buffer_size> buffer;
        std::mutex bufferMutex;

        size_t head;
        size_t tail;

    public:
        CircularBuffer() : head(0), tail(0) {};
        ~CircularBuffer() = default;

        void push(T item)
        {
            // using a mutex to lock the push operation
            std::lock_guard<std::mutex> lock(bufferMutex);
            buffer[head] = item;
            head = (head + 1) % buffer.size();
        }

        T pop()
        {
            // using a mutex to lock the pop operation
            std::lock_guard<std::mutex> lock(bufferMutex);
            T item = buffer[tail];
            tail = (tail + 1) % buffer.size();
            return item;
        }

        bool empty()
        {
            return head == tail;
        }

        bool full()
        {
            return (head + 1) % buffer.size() == tail;
        }

        size_t buffer_size()
        {
            // calculate the number of items to dequeue
            // the number will depend on where head and tail
            // are in the buffer
            // if head is ahead of tail, then head - tail
            // if tail is ahead of head, then buffer_size - tail + head
            size_t queuedItems = 0;
            if (head >= tail)
            {
                queuedItems = head - tail;
            }
            else
            {
                queuedItems = buffer.size() - tail + head;
            }
            return queuedItems;
        }
    };

    class Logger
    {
    private:
        // logger has a consumer that logs the messages
        std::thread consumer;
        bool shutdown = false;

        // queue to store messages
        CircularBuffer<std::string, 1000> queue;
        Event queueEmpty;

    public:
        Logger() = default;
        ~Logger() = default;

        void start_logger()
        {
            consumer = std::thread(&Logger::consumer_thread, this);
        }

        void stop_logger()
        {
            log("Stopping logger, processing items in queue");
            queueEmpty.wait();
            log("Queue Drained");
            shutdown = true;
            consumer.join();
        }

        template <typename... Args>
        void log(char const *format, Args &&...args)
        {
            // format the message and push it to the queue
            // get the time to nano seconds, threadid, and the message
            auto now = std::chrono::system_clock::now();
            auto now_ns = std::chrono::time_point_cast<std::chrono::nanoseconds>(now);

            // use snprintf to format the message
            // and push it to the queue
            auto size = snprintf(nullptr, 0, format, args...) + 1;
            std::string message(size, '\0');
            snprintf(&message[0], size, format, args...);
            queue.push(message);
        }

    private:
        void consumer_thread()
        {
            while (!shutdown)
            {
                if (queue.empty())
                {
                    queueEmpty.fired();
                    std::this_thread::yield();
                }
                else
                {
                    std::cout << queue.pop() << std::endl;
                }
            }
        }
    };

}
