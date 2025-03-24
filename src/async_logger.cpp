//
//
//

#include "async_logger.h"
#include <iostream>
#include <cstdarg>
#include <sstream>

namespace async_logger
{
    void Logger::start_logger()
    {
        // start things up.
        consumer = std::thread(&Logger::consumer_thread, this);
    }

    void Logger::stop_logger()
    {
        // can reference it's self. 
        log(1, "Stopping logger, processing items in queue");
        queueEmpty.wait(); //wait until everything is processed
        log(1, "Queue Drained"); // log that the queue is drained
        shutdown = true; // allow shutdown after all remaining items are processed
        consumer.join(); // wait on thread shutdown.
    }
    
    void Logger::consumer_thread()
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
                std::array<char, 4096> logMessage = queue.pop();
                std::cout << logMessage.data() << std::endl;
            }
        }
    }
}
