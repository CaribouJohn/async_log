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

/**
 * @brief namespace containing the Logger class, and supporting classes
 */
namespace async_logger
{
    using LOG_TYPE = char;
    static size_t const MAX_LOG_SIZE = 4096; // max size of the log message
    static size_t const BUFFER_SIZE = 100; // buffer size for the circular buffer
    static size_t const TIME_FORMAT_BUFFER_SIZE = 256; // buffer size for time format, depends on actual format used. 

    /**
     * @brief Logger class that logs messages asynchronously
     * 
     * This class is the main interface to the logger, it uses a circular 
     * buffer to stor messages and has a consumer thread that actually 
     * handles the logging of the messages.
     * 
     * When a message is pushed into the circular buffer, the message
     * is constructed with the current time, thread id, and log level.
     * This is stored as an array of characters in the buffer. The consumer
     * thread then pops the message from the buffer and logs it to the console.
     * 
     * The messages are fixed length so they can be moved around, rather than copied. 
     * 
     * 
     */
    class Logger
    {
    private:
        // logger has a consumer that logs the messages
        std::thread consumer;
        bool shutdown = false;

        // queue to store messages
        CircularBuffer<std::array<LOG_TYPE, MAX_LOG_SIZE>, BUFFER_SIZE> queue;
        Event queueEmpty;

    public:
        Logger() = default;
        ~Logger() = default;

        /**
         * @brief start the logger consumer thread
         * 
         */
        void start_logger();

        /**
         * @brief stop the logger consumer thread
         */
        void stop_logger();


        /**
         * @brief Queue a log a message to be output by the consumer thread
         * 
         * This function will take a variable number of arguments and format them
         * into a char array. This array is then pushed into a circular buffer
         * to be consumed by the consumer thread. The message is prefixed with the
         * current time, thread id, and log level.
         * 
         * The message is formatted using snprintf, so the format specifiers are the same.
         * 
         * @tparam Args template parameter pack
         * @param log_level log level of the message
         * @param fmt format string (as per printf)
         * @param args arguments to be rendered into the format string
         * 
         * @note This function is thread safe, the message is constructed in a temporary buffer
         * and then moved into the circular buffer synchronised by a mutex.
         */
        template <typename... Args>
        void log(int log_level, char const* fmt, Args &&...args)
        {
            std::array<LOG_TYPE, MAX_LOG_SIZE> destinationBuffer; // temp buffer

            // Get thread ID
            auto thread_id = std::this_thread::get_id();

            // Get current time
            auto now = std::chrono::system_clock::now();
            auto duration = now.time_since_epoch();

            // Convert to seconds and nanoseconds
            auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
            auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration) - seconds;

            auto now_time_t = std::chrono::system_clock::to_time_t(now);
            char time_buffer[20];
            std::tm local_time;
            localtime_s(&local_time, &now_time_t);
            std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", &local_time);

            // Create metadata string
            std::ostringstream metadata;
            metadata << "[" << time_buffer << "." << nanoseconds.count() << "] [Thread " << thread_id << "] [" << log_level << "] ";

            // Combine metadata with the user-provided format
            auto format = metadata.str() + fmt;
            size_t size = snprintf(nullptr, 0, format.c_str(), args...) + 1;  
            if(size > MAX_LOG_SIZE)
            {
                log(1, "Log message too large, truncating");
                size = MAX_LOG_SIZE - 1;
            }
            snprintf(destinationBuffer.data(), size, format.c_str(), args...);
            queue.push(std::move(destinationBuffer)); // moves the buffer into the queue
        }

    private:
        /**
         * @brief  consumer thread that logs the messages
         * 
         * This function is the main consumer thread that logs the messages
         * from the circular buffer. It will wait for messages to be pushed
         * into the buffer and then log them to the console.
         * 
         * The consumer thread will exit when the shutdown flag is set to true
         * and the queue is empty.
         */
        void consumer_thread();

    };

}
