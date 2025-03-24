#include <array>
#include <mutex>

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
    CircularBuffer() : head(0), tail(0)
    {};
    ~CircularBuffer() = default;

    void push(T& item)
    {
        // using a mutex to lock the push operation
        std::lock_guard<std::mutex> lock(bufferMutex);
        buffer[head] = std::move(item);
        head = (head + 1) % buffer.size();
    }

    T pop()
    {
        // using a mutex to lock the pop operation
        std::lock_guard<std::mutex> lock(bufferMutex);
        T item = std::move(buffer[tail]);
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

