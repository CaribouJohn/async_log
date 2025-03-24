//
// async logger header detailing the log functions and
// control functions for the async logger.
//

#pragma once
#include <iostream>
#include <array>
#include <vector>
#include <sstream>
#include <variant>

#include "circular_buffer.h"
#include "event.h"

namespace async_logger
{
    static size_t const MAX_LOG_SIZE = 4096;

    class Logger
    {
    private:
        // logger has a consumer that logs the messages
        std::thread consumer;
        bool shutdown = false;
        std::array<char, MAX_LOG_SIZE> m_tempBuffer; // temp buffer

        // queue to store messages
        CircularBuffer<std::array<char, MAX_LOG_SIZE>, 100> queue;
        Event queueEmpty;

    public:
        Logger() = default;
        ~Logger() = default;

        void start_logger();
        void stop_logger();
        template <typename... Args>
        void log(int log_level, char const* fmt, Args &&...args)
        {
            m_tempBuffer.fill(0); // clear the buffer

            // Get thread ID
            auto thread_id = std::this_thread::get_id();

            // Get current time
            auto now = std::chrono::system_clock::now();
            auto now_time_t = std::chrono::system_clock::to_time_t(now);
            char time_buffer[20];
            std::tm local_time;
            localtime_s(&local_time, &now_time_t);
            std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", &local_time);

            // Create metadata string
            std::ostringstream metadata;
            metadata << "[" << time_buffer << "] [Thread " << thread_id << "] [" << log_level << "] ";

            // Combine metadata with the user-provided format
            auto format = metadata.str() + fmt;
            size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;  
            if(size > MAX_LOG_SIZE)
            {
                log(1, "Log message too large, truncating");
                size = MAX_LOG_SIZE - 1;
            }
            snprintf(m_tempBuffer.data(), size, format.c_str(), args...);
            queue.push(m_tempBuffer); // moves the buffer into the queue
        }

    private:
        void consumer_thread();

    };

}
