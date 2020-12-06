#pragma once

template<typename T>
class Queue
{
    public:
        Queue<T>() { };
        virtual ~Queue<T>() = 0;
        virtual int size() = 0;
        virtual int capacity() = 0;
        virtual void enqueue(T i) = 0;
        virtual T dequeue() = 0;
        virtual T peek() = 0;
};

template<typename T> Queue<T>::~Queue() { }
