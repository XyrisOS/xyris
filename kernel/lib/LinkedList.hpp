/**
 * @file LinkedList.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Standard linked list library
 * @version 0.3
 * @date 2020-06-17
 *
 * @copyright Copyright the Xyris Contributors (c) 2021
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

namespace LinkedList {

template<typename T>
class LinkedListNode {
public:
    /**
     * @brief Construct a new Linked List Node object
     *
     */
    LinkedListNode()
        : data(0)
        , next(NULL)
        , prev(NULL)
    {
        // Default constructor
    }
    /**
     * @brief Construct a new Linked List Node object
     *
     * @param v Value to be stored
     */
    LinkedListNode(T v)
        : data(v)
        , next(NULL)
        , prev(NULL)
    {
        // Value constructor
    }
    /**
     * @brief Construct a new Linked List Node object
     *
     * @param n Next node in the list
     * @param p Previous node in the list
     * @param v Value to be stored
     */
    LinkedListNode(T v, LinkedListNode* n, LinkedListNode* p)
        : data(v)
        , next(n)
        , prev(p)
    {
        // Complete constructor
    }
    /**
     * @brief Return the data stored by the node
     *
     * @return T Stored data
     */
    T& Data()
    {
        return data;
    }
    /**
     * @brief Get the next node in the linked list
     *
     * @return LinkedListNode* Pointer to next node
     */
    LinkedListNode* Next()
    {
        return next;
    }
    /**
     * @brief Get the previous node in the linked list
     *
     * @return LinkedListNode* Pointer to the previous node
     */
    LinkedListNode* Previous()
    {
        return prev;
    }
    /**
     * @brief Set the node's data
     *
     */
    void SetData(T v)
    {
        data = v;
    }
    /**
     * @brief Set the node's next pointer
     *
     * @param n Pointer to next node
     */
    void SetNext(LinkedListNode* n)
    {
        next = n;
    }
    /**
     * @brief Set the node's previous pointer
     *
     * @param n Pointer to the previous node
     */
    void SetPrevious(LinkedListNode* n)
    {
        prev = n;
    }

private:
    T data;
    LinkedListNode* next;
    LinkedListNode* prev;
};

template<typename T>
class LinkedList {
public:
    LinkedList()
        : head(NULL)
        , tail(NULL)
        , count(0)
    {
        // Default constructor
    }
    LinkedList(T val)
        : LinkedList()
    {
        InsertFront(val);
    }
    ~LinkedList()
    {
        LinkedListNode<T>* back;
        while ((back = RemoveBack())) {
            delete back;
        }
    }
    void InsertFront(T val)
    {
        if (head) {
            InsertBefore(head, val);
        } else {
            head = new LinkedListNode<T>(val);
            tail = head;
            ++count;
        }
    }
    void InsertBack(T val)
    {
        if (tail) {
            InsertAfter(tail, val);
        } else {
            tail = new LinkedListNode<T>(val);
            head = tail;
            ++count;
        }
    }
    void InsertBefore(LinkedListNode<T>* next, T val)
    {
        if (!next)
            return;
        LinkedListNode<T>* newNode = new LinkedListNode<T>(val);
        newNode->SetPrevious(next->Previous());
        next->SetPrevious(newNode);
        newNode->SetNext(next);
        if (newNode->Previous()) {
            newNode->Previous()->SetNext(newNode);
        } else {
            head = newNode;
        }
        ++count;
    }
    void InsertAfter(LinkedListNode<T>* prev, T val)
    {
        if (!prev)
            return;
        LinkedListNode<T>* newNode = new LinkedListNode<T>(val);
        newNode->SetNext(prev->Next());
        prev->SetNext(newNode);
        newNode->SetPrevious(prev);
        if (newNode->Next()) {
            newNode->Next()->SetPrevious(newNode);
        } else {
            tail = newNode;
        }
        ++count;
    }
    void Remove(LinkedListNode<T>* del)
    {
        if (del == head)
            head = del->Next();
        if (del == tail)
            tail = del->Previous();
        if (del->Next())
            del->Next()->SetPrevious(del->Previous());
        if (del->Previous())
            del->Previous()->SetNext(del->Next());
        count--;
    }
    LinkedListNode<T>* RemoveFront()
    {
        if (!head)
            return NULL;
        LinkedListNode<T>* currHead = head;
        Remove(currHead);
        return currHead;
    }
    LinkedListNode<T>* RemoveBack()
    {
        if (!tail)
            return NULL;
        LinkedListNode<T>* currTail = tail;
        Remove(currTail);
        return currTail;
    }
    LinkedListNode<T>* RemoveBefore(LinkedListNode<T>* node)
    {
        if (!node)
            return NULL;
        LinkedListNode<T>* before = node->Previous();
        Remove(before);
        return before;
    }
    LinkedListNode<T>* RemoveAfter(LinkedListNode<T>* node)
    {
        if (!node)
            return NULL;
        LinkedListNode<T>* after = node->Next();
        Remove(after);
        return after;
    }
    /**
     * @brief Get pointer to the head node
     *
     * @return LinkedListNode* Pointer to head node
     */
    LinkedListNode<T>* Head()
    {
        return head;
    }
    /**
     * @brief Get pointer to the tail node
     *
     * @return LinkedListNode* Pointer to tail node
     */
    LinkedListNode<T>* Tail()
    {
        return tail;
    }
    /**
     * @brief Get the number of items in the linked list
     *
     * @return size_t Number of items in the linked list
     */
    size_t Count()
    {
        return count;
    }
    /**
     * @brief Check if the linked list is empty
     *
     * @return true The list is empty
     * @return false The list is not empty
     */
    bool IsEmpty()
    {
        return Count() == 0;
    }

private:
    LinkedListNode<T>* head;
    LinkedListNode<T>* tail;
    size_t count;
};

} // !namespace LinkedList
