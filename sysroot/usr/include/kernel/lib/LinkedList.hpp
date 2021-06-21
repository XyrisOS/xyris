/**
 * @file LinkedList.hpp
 * @author Keeton Feavel (keetonfeavel@cedarville.edu)
 * @brief Standard linked list library
 * @version 0.3
 * @date 2020-06-17
 *
 * @copyright Copyright the Panix Contributors (c) 2021
 */
#pragma once

#include <stdint.h>
#include <stddef.h>

namespace LinkedList {

template <typename T>
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
    LinkedListNode(LinkedListNode* n, LinkedListNode* p, T v)
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
    T GetData()
    {
        return data;
    }
    /**
     * @brief Get the next node in the linked list
     *
     * @return LinkedListNode* Pointer to next node
     */
    LinkedListNode* GetNext()
    {
        return next;
    }
    /**
     * @brief Get the previous node in the linked list
     *
     * @return LinkedListNode* Pointer to the previous node
     */
    LinkedListNode* GetPrevious()
    {
        return prev;
    }
    /**
     * @brief Set the node's data
     *
     */
    void SetData()
    {
        return data;
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
    // TODO: Add a size_t index value;
};

template <typename T>
class LinkedList {
public:
    LinkedList(T val)
    {
        head = LinkedListNode<T>(val);
    }
    void PushFront(T val)
    {
        LinkedListNode<T> next = head;
        LinkedListNode<T> newHead = new LinkedListNode<T>(next, NULL, val);
        head = newHead;
        ++count;
    }
    void PushBack(T val)
    {
        LinkedListNode<T>* newTail = new LinkedListNode<T>(NULL, tail, val);
        tail->SetNext(newTail);
        tail = newTail;
        ++count;
    }
    // TODO: Convert v into type T and make new node for the user
    void PushBefore(LinkedListNode<T>* n, LinkedListNode<T>* v)
    {
        // Get pointer to previous in line from n
        LinkedListNode<T>* prev = n->GetPrevious();
        // Set n's previous to be the new node
        n->SetPrevious(v);
        // Set v's previous pointer to be the former previous
        v->SetPrevious(prev);
        // Update the counter
        ++count;
    }
    // TODO: Convert v into type T and make new node for the user
    void PushAfter(LinkedListNode<T>* n, LinkedListNode<T>* v)
    {
        // Get pointer to the next in line from n
        LinkedListNode<T>* next = n->GetNext();
        // Set n's next to be the new node (v)
        n->SetNext(v);
        // Set v's next pointer to be the former next
        v->SetNext(next);
        // Insert node
        ++count;
    }
    LinkedListNode<T>* PopFront()
    {
        // Get pointer to the next in line
        LinkedListNode<T>* newHead = head.GetNext();
        // Copy the current head
        LinkedListNode<T>* currHead = head;
        // Reassign the pointer to the next in line
        head = newHead;
        newHead->SetPrevious(NULL);
        // Return copied (old) head
        --count;
        return currHead;
    }
    LinkedListNode<T>* PopBack()
    {
        // Get pointer to the next in line
        LinkedListNode<T>* newTail = tail->GetPrevious();
        // Copy the current head
        LinkedListNode<T> currTail = tail;
        // Reassign the pointer to the next in line
        tail = newTail;
        tail->SetNext(NULL);
        // Return copied (old) head
        --count;
        return currTail;
    }
    LinkedListNode<T>* PopBefore(LinkedListNode<T>* n)
    {
        // Grab the node before to n
        LinkedListNode<T>* before = n->SetPrevious();
        // Grab the node before the previous node (before's previous)
        LinkedListNode<T>* beforeBefore = before->SetPrevious();
        // Set n's previous to before's previous (skipping before)
        n->SetPrevious(beforeBefore);
        beforeBefore->SetNext(n);
        // Return before
        --count;
        return before;
    }
    LinkedListNode<T>* PopAfter(LinkedListNode<T>* n)
    {
        // Grab the next node (after n)
        LinkedListNode<T>* after = n->GetNext();
        // Grab the node following the next node (after's next)
        LinkedListNode<T>* afterAfter = after->GetNext();
        // Set n's next to after's next (skipping after)
        n->SetNext(afterAfter);
        afterAfter->SetPrevious(n);
        // Return after
        --count;
        return after;
    }
    /**
     * @brief Get pointer to the head node
     *
     * @return LinkedListNode* Pointer to head node
     */
    LinkedListNode<T>* GetHead()
    {
        return head;
    }
    /**
     * @brief Get pointer to the tail node
     *
     * @return LinkedListNode* Pointer to tail node
     */
    LinkedListNode<T>* GetTail()
    {
        return tail;
    }
    /**
     * @brief Get the number of items in the linked list
     *
     * @return size_t Number of items in the linked list
     */
    size_t GetCount()
    {
        return count;
    }

private:
    LinkedListNode<T>* head;
    LinkedListNode<T>* tail;
    size_t count;
};

};
