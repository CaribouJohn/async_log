#include <array>
#include <mutex>


/**
 * @brief Circular buffer class
 * 
 * This class implements a circular buffer with a fixed size. The buffer
 * is a template class that can store any type of data. The buffer is
 * thread safe and uses a mutex to synchronise access to the buffer.
 * 
 * @note The buffer size must be greater than 0, and the type T must be
 * move constructible.
 * 
 */
template <typename T, int buffer_size>
class CircularBuffer
{
    static_assert(buffer_size > 0, "Size of CircularBuffer must be greater than 0");
    

private:
    std::array<T, buffer_size> buffer; // buffer to store items
    std::mutex bufferMutex; // mutex to lock the buffer

    size_t head; // current write position
    size_t tail; // current read position

public:
    /**
     * @brief Construct a new Circular Buffer object
     */
    CircularBuffer() : head(0), tail(0)
    {};

    ~CircularBuffer() = default;

    /**
     * @brief Push an item onto the buffer
     * 
     * This function will push an item onto the buffer. The item is moved
     * into the buffer, so the original item is no longer valid.
     * 
     * @param item item to be MOVED onto the buffer
     * 
     */
    void push(T&& item)
    {
        // using a mutex to lock the push operation
        std::lock_guard<std::mutex> lock(bufferMutex);
        buffer[head] = std::move(item);
        head = (head + 1) % buffer.size();
    }

    /**
     * @brief Pop an item from the buffer
     * 
     * This function will pop an item from the buffer. The item is moved
     * out of the buffer, so the original item is no longer valid. It will
     * be overwritten in the buffer if another item is pushed onto the buffer 
     * in that slot later.
     * 
     * @return T item that was popped from the buffer
     * 
     */
    T pop()
    {
        // using a mutex to lock the pop operation
        std::lock_guard<std::mutex> lock(bufferMutex);
        T item = std::move(buffer[tail]);
        tail = (tail + 1) % buffer.size();
        return item;
    }

    /**
     * @brief Check if the buffer is empty
     */
    bool empty()
    {
        return head == tail;
    }

    /**
     * @brief Check if the buffer is full
     */
    bool full()
    {
        return (head + 1) % buffer.size() == tail;
    }

    /**
     * @brief Get the number of items in the buffer
     * 
     * @return size_t number of items in the buffer
     * 
     * This function will return the number of items in the buffer. This
     * is calculated by checking the head and tail positions in the buffer.
     * 
     * If the head is ahead of the tail, then the number of items is head - tail.
     * If the tail is ahead of the head, then the number of items is buffer_size - tail + head.
     * 
     */
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

