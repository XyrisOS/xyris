#pragma once
#include <lib/queue.hpp>
#include <sys/panic.hpp>

#define ARRAY_QUEUE_CAPACITY 8

template<typename T>
class ArrayQueue : public Queue<T>
{
    public:
        ArrayQueue<T>();
        ~ArrayQueue<T>();
        int size();
        int capacity();
        void enqueue(T i);
        T dequeue();
        T peek();
    private:
        int m_size;
        T m_items[ARRAY_QUEUE_CAPACITY];
        int m_head;
        int m_tail;
};

template<typename T>
ArrayQueue<T>::ArrayQueue() : m_size(0), m_head(0), m_tail(0) { }

template<typename T>
ArrayQueue<T>::~ArrayQueue() { }

template<typename T>
int ArrayQueue<T>::size()
{
    return m_size;
}

template<typename T>
int ArrayQueue<T>::capacity()
{
    return ARRAY_QUEUE_CAPACITY;
}

template<typename T>
void ArrayQueue<T>::enqueue(T i)
{
    if (size() == capacity())
        PANIC("Attempt to fill queue past it's capacity");
    m_items[m_head] = i;
    m_head = (m_head + 1) % ARRAY_QUEUE_CAPACITY;
    m_size++;
}

template<typename T>
T ArrayQueue<T>::dequeue()
{
    if (size() == 0)
        PANIC("Attempt to dequeue an empty queue");
    T item = m_items[m_tail];
    m_tail = (m_tail + 1) % ARRAY_QUEUE_CAPACITY;
    m_size--;
    return item;
}

template<typename T>
T ArrayQueue<T>::peek()
{
    return m_items[m_tail];
}
