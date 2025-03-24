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
        consumer = std::thread(&Logger::consumer_thread, this);
    }

    void Logger::stop_logger()
    {
        log(1, "Stopping logger, processing items in queue");
        queueEmpty.wait();
        log(1, "Queue Drained");
        shutdown = true;
        consumer.join();
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
