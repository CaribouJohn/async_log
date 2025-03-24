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

    // struct LogTask
    // {
    //     std::string format;
    //     std::tuple<> args; // Tuple of strings

    //     // Constructor
    //     LogTask() = default;

    //     template <typename... Args>
    //     LogTask(int log_level, const char* fmt, Args&&... arguments)
    //         : format()
    //     {
    //         // Get thread ID
    //         auto thread_id = std::this_thread::get_id();

    //         // Get current time
    //         auto now = std::chrono::system_clock::now();
    //         auto now_time_t = std::chrono::system_clock::to_time_t(now);
    //         char time_buffer[20];
    //         std::tm local_time;
    //         localtime_s(&local_time, &now_time_t);
    //         std::strftime(time_buffer, sizeof(time_buffer), "%Y-%m-%d %H:%M:%S", &local_time);

    //         // Create metadata string
    //         std::ostringstream metadata;
    //         metadata << "[" << time_buffer << "] [Thread " << thread_id << "] [" << log_level << "] ";

    //         // Combine metadata with the user-provided format
    //         format = metadata.str() + fmt;
    //     }

    //     // Move constructor
    //     LogTask(LogTask&& other) noexcept
    //         : format(std::move(other.format)), args(std::move(other.args))
    //     {}

    //     // Move assignment operator
    //     LogTask& operator=(LogTask&& other) noexcept
    //     {
    //         if (this != &other)
    //         {
    //             format = std::move(other.format);
    //             args = std::move(other.args);
    //         }
    //         return *this;
    //     }

    //     // Delete copy constructor and copy assignment operator to avoid accidental copies
    //     LogTask(const LogTask&) = delete;
    //     LogTask& operator=(const LogTask&) = delete;

    //     // Convert to string for logging
    //     std::string to_string()
    //     {
    //         std::ostringstream oss;
    //         oss << format;

    //         std::apply(
    //             [&oss](const auto&... tupleArgs)
    //             {
    //                 ((oss << " " << tupleArgs), ...); // Append each tuple element to the stream
    //             },
    //             args);

    //         return oss.str();
    //     }

    // private:
    //     // Helper to convert arguments to a tuple of strings
    //     template <typename... Args>
    //     static std::tuple<std::string> convert_to_tuple(Args&&... arguments)
    //     {
    //         return std::make_tuple(to_string(std::forward<Args>(arguments))...);
    //     }

    //     // Helper to convert a single argument to a string
    //     template <typename T>
    //     static std::string to_string(T&& value)
    //     {
    //         if constexpr (std::is_arithmetic_v<std::decay_t<T>>)
    //         {
    //             return std::to_string(value);
    //         }
    //         else
    //         {
    //             std::ostringstream oss;
    //             oss << std::forward<T>(value);
    //             return oss.str();
    //         }
    //     }
    // };

    class Logger
    {
    private:
        // logger has a consumer that logs the messages
        std::thread consumer;
        bool shutdown = false;

        // queue to store messages
        CircularBuffer<std::array<char,4096>,100> queue;
        Event queueEmpty;

    public:
        Logger() = default;
        ~Logger() = default;

        void start_logger();
        void stop_logger();
        template <typename... Args>
        void log(int log_level, char const* fmt, Args &&...args)
        {
            std::array<char, 4096> buffer;
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
            snprintf(buffer.data(), size, format.c_str(), args...);
            queue.push(buffer);
        }

    private:
        void consumer_thread();

    };

}
